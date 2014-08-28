#include <uRobotics/api/SharedVariable.h>
#include <uRobotics/api/SimpleCommandManager.h>
#include <uRobotics/api/Parser.h>

SharedVariable::SharedVariable(void) : commandManager(NULL){
}

SharedVariable::~SharedVariable(void){
}

const SimpleCommandManager* SharedVariable::getCommandManager() const{
	return this->commandManager;
}

void SharedVariable::setCommandManager(SimpleCommandManager* value){
	this->commandManager = value;
}

/**
* Request the blackboard to notify each time the shared variable is written
*/
void SharedVariable::subscribe()
{
	subscribe(SharedVariableReportType::Notify, SharedVariableSubscriptionType::WriteOthers);
}

/**
* Request the blackboard to notify each time the shared variable is written
* @param reportType	The type of report. This affects the event that will be raised when the variable is written
*/
void SharedVariable::subscribe(const SharedVariableReportType& reportType)
{
	subscribe(reportType, SharedVariableSubscriptionType::WriteOthers);
}

/**
* Request the blackboard to notify each time the shared variable is written
* @param subscriptionType	The type of subscription
*/
void SharedVariable::subscribe(const SharedVariableSubscriptionType& subscriptionType)
{
	subscribe(SharedVariableReportType::Notify, subscriptionType);
}

/**
* Request the blackboard to notify each time the shared variable is written
* @param reportType	The type of report. This affects the event that will be raised when the variable is written
* @param subscriptionType	The type of subscription
*/
void SharedVariable::subscribe(const SharedVariableReportType& reportType, const SharedVariableSubscriptionType& subscriptionType)
{
	subscribe(reportType, subscriptionType, 500, 3);
}

/**
* Request the blackboard to stop notifying each time the shared variable is written
*/
void SharedVariable::unsubscribe()
{
	unsubscribe(300, 3);
}

/**
* Request the blackboard to stop notifying each time the shared variable is written
* @param timeout	The timeout for each unsubscription attempt (of 3) in milliseconds
*/
void SharedVariable::unsubscribe(const int& timeout)
{
	unsubscribe(timeout, 3);
}

/**
* Returns the string representation of the shared variable
* @return The string representation of the shared variable
*/
const std::string SharedVariable::toString() const{
	std::string s(this->getTypeName());
	if (this->isArray()) s += "[]";
	s += " " + this->getName();
	return s;
}

/**
* Performs a read operation and updates the data stored in cache
* @return true if the the data acquisition operation succeeded, false otherwise
*/
bool SharedVariable::updateBufferedData()
{
	return updateBufferedData(300);
}

/**
* Queries the Blackboard for updated information (writers and subscriptions) about the Shared Variable
*/
void SharedVariable::updateInfo()
{
	Exception ex;
	if (!updateInfo(500, ex))
		throw ex;
}

/**
* Updates the SharedVariable object with data provided from the blackboard due to a subscription
* @param variableType	The type of the variable specified by blackboard
* @param variableName	The name of the variable specified by blackboard
* @param isArray	Value that indicates if the variable specified by blackboard is an array
* @param arraySize	The size of the variable specified by blackboard if it is an array
* @param sData	The serialized data contained in the report
* @param reportType	The type of report
* @param subscriptionType	The type of subscription
* @param writer	The name of the module which performed the write/create operation
* @param ex	When this method returns contains null if the variable was updated successfully, or the exception to be thrown if the update failed
* @return true if variable was updated successfully, false otherwise
*/
bool SharedVariable::update(const std::string& variableType, const bool& isArray, const int& arraySize, const std::string& variableName,
	const std::string& sData, const SharedVariableReportType& reportType,
	const SharedVariableSubscriptionType& subscriptionType, const std::string& writer, Exception& ex)
{
	SharedVariableInfo variableInfo(variableType, variableName, isArray, arraySize);
	SharedVariableReport report(&variableInfo, sData, reportType, subscriptionType, writer);

	return update(report, ex);
}

/**
* Updates the SharedVariable object with data provided from the blackboard
* @param variableType	The type of the variable specified by blackboard
* @param variableName	The name of the variable specified by blackboard
* @param isArray	Value that indicates if the variable specified by blackboard is an array
* @param arraySize	The size of the variable specified by blackboard if it is an array
* @param data	The serialized data stored in blackboard
* @param ex	When this method returns contains null if the variable was updated successfully, or the exception to be thrown if the update failed
* @return true if variable was updated successfully, false otherwise
*/
bool SharedVariable::update(const std::string& variableType, const bool& isArray, const int& arraySize, const std::string& variableName, const std::string& data, Exception& ex)
{
	if (!isValidUpdateData(variableType, isArray, arraySize, variableName, ex))
		return false;

	return updateValue(data, ex);
}

/**
* Gets a value indicating if the data provided from the blackboard for an update is valid
* @param variableType	The type of the variable specified by blackboard
* @param variableName	The name of the variable specified by blackboard
* @param isArray	Value that indicates if the variable specified by blackboard is an array
* @param arraySize	The size of the variable specified by blackboard if it is an array
* @param ex	When this method returns contains null if the variable was updated successfully, or the exception to be thrown if the update failed
* @return true if provided data is suitable for update the variable, false otherwise
*/
bool SharedVariable::isValidUpdateData(const std::string& variableType, const bool& isArray, const int& arraySize, const std::string& variableName, Exception& ex)
{
	ex = Exception();
	if (getTypeName().compare(variableType) != 0)
	{
		ex = Exception("Incompatible types");
		return false;
	}
	if (this->isArray() != isArray)
	{
		ex = Exception("Incompatible types");
		return false;
	}
	if (getName().compare(variableName) != 0)
	{
		ex = Exception("Incompatible names");
		return false;
	}
	return true;
}

/**
* Deserializes and updates the value of the shared variable
* @param serializedData	The serialized data containing the new value for the shared variable
* @param ex	When this method returns contains null if the variable was updated successfully, or the exception to be thrown if the update failed
* @return true if variable was updated successfully, false otherwise
*/
bool SharedVariable::updateValue(const std::string& serializedData, Exception& ex)
{
	std::string s;
	return updateValue(serializedData, s, ex);
}

/**
* Queries the Blackboard for updated information (writers and subscriptions) about the Shared Variable
* @param timeout	The amout of time to wait for a stat confirmation from blackboard
* @param ex	When this method returns contains null if the variable information was updated successfully, or the exception to be thrown if the update failed
* @return true if the information was updated successfully, false otherwise
*/
// internal
bool SharedVariable::updateInfo(const int& timeout, Exception& ex)
{

	SharedVariableInfo varInfo;

	ex = Exception();
	if (this->commandManager == NULL)
	{
		ex = Exception("Cannot communicate with blackboard");
		return false;
	}

	Command cmdStatVar("stat_var", this->getName());
	Response *rspStatVar;
	if (!this->commandManager->sendAndWait(cmdStatVar, timeout, rspStatVar))
	{
		ex = Exception("No response from blackboard");
		return false;
	}
	if (!rspStatVar->success)
	{
		ex = Exception("Unsupported command" + getName());
		delete rspStatVar;
		return false;
	}

	if (!SharedVariableInfo::deserialize(rspStatVar->parameters, varInfo, ex)){
		delete rspStatVar;
		return false;
	}

	if ( (getName().compare(varInfo.getName()) != 0) ||
		(getTypeName().compare(varInfo.getTypeName()) != 0))
	{
		ex = Exception("Invalid response. Variable type/name mismatch.");
		delete rspStatVar;
		return false;
	}

	//this.creationTime = varInfo.CreationTime;
	//this.allowedWriters = varInfo.AllowedWriters;
	//this.subscriptions = varInfo.Subscriptions;
	delete rspStatVar;
	return true;
}

bool SharedVariable::parseResponse(const Response& response, std::string& variableType, bool& isArray, int& arrayLength, std::string& variableName, std::string& variableData){
	return Parser::parseSharedVariable(response.parameters, variableType, isArray, arrayLength, variableName, variableData);
}

void SharedVariable::onReportReceived(const SharedVariableReport& report){
	reportReceived(*this, report);
}

void SharedVariable::onUpdated(){
	updated(*this);
}