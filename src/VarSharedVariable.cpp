#include <algorithm>
#include <uRobotics/api/psv/VarSharedVariable.h>

VarSharedVariable::VarSharedVariable(const std::string& name):
SharedVariableGeneric<std::string>(name){}

VarSharedVariable::VarSharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric<std::string>(commandManager, variableName, initialize){}

const std::string VarSharedVariable::getTypeName() const{
	return std::string("var");
}

const bool VarSharedVariable::isArray() const{
	return false;
}

const int VarSharedVariable::getLength() const{
	return -1;
}

bool VarSharedVariable::serialize(const std::string& value, std::string& serializedData) const{
	if (value.empty())
		serializedData = std::string("null");

	serializedData = std::string(value);
	return true;
}

bool VarSharedVariable::deserialize(const std::string& serializedData, std::string& value) const{
	value.clear();
	if (serializedData.empty() || (serializedData.compare("null") == 0))
		return true;

	value = std::string(serializedData);
	return true;
}
