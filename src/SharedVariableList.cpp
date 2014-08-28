#include <uRobotics/api/SharedVariableList.h>
#include <uRobotics/api/SimpleCommandManager.h>
#include <uRobotics/api/SharedVariable.h>
#include <uRobotics/api/Parser.h>
#include <uRobotics/utilities/Utils.h>
#include <uRobotics/Exception.h>
#include <uRobotics/api/psv/ByteSharedVariable.h>
#include <uRobotics/api/psv/ByteArraySharedVariable.h>
#include <uRobotics/api/psv/DoubleSharedVariable.h>
#include <uRobotics/api/psv/DoubleArraySharedVariable.h>
#include <uRobotics/api/psv/IntSharedVariable.h>
#include <uRobotics/api/psv/IntArraySharedVariable.h>
#include <uRobotics/api/psv/LongSharedVariable.h>
#include <uRobotics/api/psv/LongArraySharedVariable.h>
#include <uRobotics/api/psv/StringSharedVariable.h>
#include <uRobotics/api/psv/VarSharedVariable.h>

SharedVariableList::SharedVariableList(SimpleCommandManager& commandManager):
	commandManager(commandManager){}

SharedVariableList::~SharedVariableList(void){
}

void SharedVariableList::add(SharedVariable* item)
{
	if (item == NULL)
		throw ArgumentNullException();
	
	scoped_lock(sharedVariableListMutex);

	if (contains(item->getName()))
		throw Exception("An SharedVariable object with the same name already exists in the collection");
	this->sharedVariableList[item->getName()] = item;
	item->setCommandManager(&(this->commandManager));
	item->initialize();
}

int SharedVariableList::loadFromBlackboard(){
	std::string message;
	return loadFromBlackboard(3000, message);
}

void SharedVariableList::clear() {
	scoped_lock(sharedVariableListMutex);
	this->sharedVariableList.erase(this->sharedVariableList.begin(), this->sharedVariableList.end());
}

int SharedVariableList::count() const{
	scoped_lock(sharedVariableListMutex);
	return this->sharedVariableList.size();
}

bool SharedVariableList::contains(const std::string& variableName) const {
	scoped_lock(sharedVariableListMutex);
	return this->sharedVariableList.count(variableName) > 0;
}

bool SharedVariableList::remove(const std::string& variableName) {
	scoped_lock(sharedVariableListMutex);
	if(!this->contains(variableName))
		return false;
	this->sharedVariableList.erase(variableName);
	return true;
}

void SharedVariableList::removeAt(const int& index){
	scoped_lock(sharedVariableListMutex);
	if(index >= (this->count()-1))
		return;
	std::map<std::string, SharedVariable*>::iterator it;
	std::advance(it, index);
	this->sharedVariableList.erase(it);
}

int SharedVariableList::updateFromBlackboard(const Response& response){
	std::string message;
	return updateFromBlackboard(response, message);
}

int SharedVariableList::updateFromBlackboard(const Response& response, std::string& message){
	int count = 0;
	std::string variableType;
	bool isArray;
	int arrayLength;
	std::string variableName;
	std::string variableData;

	count = 0;
	message.clear();
	std::vector<std::string> splitted = splitMultipleSharedVariables(response.parameters);
	for (std::vector<std::string>::iterator i = splitted.begin(); i != splitted.end(); ++i)
	{
		// 1. Fetch variable data
		if (!Parser::parseSharedVariable(*i, variableType, isArray, arrayLength, variableName, variableData))
			continue;

		// 2. Check if variable exists
		if(this->contains(variableName))
		{
			//++existing;
			continue;
		}

		// 3. Register variable
		if (registerVar(variableType, isArray, variableName, variableData))
			++count;
		else
			message += "Can not register variable " + variableType + " " + variableName + "\r\n";
	}

	return count;
}

std::vector<std::string> SharedVariableList::splitMultipleSharedVariables(const std::string& s)
 {
	 size_t cc, start, end;
	 int counter;
	 std::vector<std::string> vars;

	 cc = 0;
	 // Skip space at the beginning of the string
	 Parser::skipSpaces(s, cc);

	 do
	 {
		 // Shared variables must be enclosed within braces
		 if (!Scanner::readChar('{', s, cc))
			 break;
		 counter = 1;
		 start = cc - 1;

		 // Read until next close brace
		 while ((cc < s.size()) && (counter > 0))
		 {
			 switch (s[cc])
			 {
				 // Open braces increments counter
			 case '{':
				 ++counter;
				 break;

				 // Closed braces decrements counter
			 case '}':
				 --counter;
				 break;

				 // Skips escaped chars
			 case '\\':
				 ++cc;
				 break;
			 }
			 ++cc;
		 }
		 end = cc;
		 if ((end - start) > 0)
			 vars.push_back(s.substr(start, end - start));
	 } while (cc < s.size());

	 return vars;
 }

bool SharedVariableList::registerVar(std::string type, bool isArray, std::string name, std::string data)
{
	SimpleCommandManager* commandManager = &(this->commandManager);
	SharedVariable *variable = NULL;
	if(contains(name))
		return false;

	switch (Utils::hashString(type))
	{
		case 0x058A22F9:// "byte"
			if (isArray)
				variable = new ByteArraySharedVariable(commandManager, name, false);
			else
				variable = new ByteSharedVariable(commandManager, name, false);
			break;

		case 0xB8F7832A: // "double"
			if (isArray)
				variable = new DoubleArraySharedVariable(commandManager, name, false);
			else
				variable = new DoubleSharedVariable(commandManager, name, false);
			break;

		case 0x83B80BA5: // "HumanFace"
			// if(!isArray)
			// 	variable = new DetectedHumanFaces(this.commandManager, name, false);
			// break;
			return false;

		case 0x1BE6C88C: // "int"
			if (isArray)
				variable = new IntArraySharedVariable(commandManager, name, false);
			else
				variable = new IntSharedVariable(commandManager, name, false);
			break;

		case 0x06849CBB: // "LaserReadingArray"
			// if (!isArray)
			// 	variable = new LaserReadingArrayShV(commandManager, name, false);
			// break;
			return false;

		case 0x5189E03D: // "long"
			if (isArray)
				variable = new LongArraySharedVariable(commandManager, name, false);
			else
				variable = new LongSharedVariable(commandManager, name, false);
			break;

		case 0x121EB342: // "Matrix"
			// if (!isArray)
			// 	variable = new MatrixSharedVariable(commandManager, name, false);
			// break;
			return false;

		case 0x50B2F8D5: // "RecognizedSpeech"
			// if (!isArray)
			// 	variable = new RecognizedSpeechSharedVariable(commandManager, name, false);
			// break;
			return false;

		case 0x75DF4D9C: // "var"
			variable = new VarSharedVariable(commandManager, name, false);
			isArray = false;
			break;

		case 0xEFE5D43C: // "Vector"
			// if(!isArray)
			// 	variable = new VectorSharedVariable(commandManager, name, false);
			// break;

		case 0x09682B0E: // "string"
			if (!isArray)
				variable = new StringSharedVariable(commandManager, name, false);
			break;
		
		default: return false;
	}

	if (variable == NULL)
		return false;

	this->add(variable);

	Exception ex;
	variable->updateInfo(500, ex);
	//variable.Initialized = true;
	return variable->update(type, isArray, -1, name, data, ex);
}

int SharedVariableList::loadFromBlackboard(const int& timeout, std::string& message){
	Response *rspListVars;
	Response *rspReadVars;
	int count;

	//if (!Monitor.TryEnter(sharedVarLoadRequestLock, 100))
	//{
	//	message = "Another load operation is being performed";
	//	return 0;
	//}

	message.clear();
	Command cmdListVars("list_vars", "");
	if (!this->commandManager.sendAndWait(cmdListVars, timeout, rspListVars))
	{
		message = std::string("No response from blackboard while requesting variable list (timeout?)");
		return 0;
	}
	if (!rspListVars->success || !rspListVars->hasParams())
	{
		message = "Blackboard has not variables defined";
		delete rspListVars;
		return 0;
	}
	Command cmdReadVars("read_vars", rspListVars->parameters);
	if (!this->commandManager.sendAndWait(cmdReadVars, timeout, rspReadVars))
	{
		message = "No response from blackboard while reading variable list (timeout?)";
		delete rspListVars;
		return 0;
	}

	count = updateFromBlackboard(*rspReadVars);
	delete rspListVars;
	delete rspReadVars;
	return count;
}

SharedVariable* SharedVariableList::operator[](const std::string& variableName){
	scoped_lock(sharedVariableListMutex);
	return this->sharedVariableList[variableName];
}
