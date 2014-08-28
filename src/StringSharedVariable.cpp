#include <algorithm>
#include <uRobotics/utilities/Utils.h>
#include <uRobotics/api/psv/StringSharedVariable.h>

using namespace Robotics::Utilities;

StringSharedVariable::StringSharedVariable(const std::string& name):
SharedVariableGeneric<std::string>(name){}

StringSharedVariable::StringSharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric<std::string>(commandManager, variableName, initialize){}

const std::string StringSharedVariable::getTypeName() const{
	return std::string("string");
}

const bool StringSharedVariable::isArray() const{
	return false;
}

const int StringSharedVariable::getLength() const{
	return -1;
}

bool StringSharedVariable::serialize(const std::string& value, std::string& serializedData) const{
	if (value.empty())
		serializedData = std::string("null");

	serializedData = std::string(value);
	Utils::replace(serializedData, "\\", "\\\\");
	Utils::replace(serializedData, "\\\"", "\\\"");
	serializedData = std::string("\"" + serializedData + "\"");

	return true;
}

bool StringSharedVariable::deserialize(const std::string& serializedData, std::string& value) const{
	if (serializedData.empty() || (serializedData.compare("null") == 0)){
		value = std::string("null");
		return true;
	}

	size_t start, end;
	value.clear();
	start = serializedData.find("\"");
	end = serializedData.rfind("\"") - 1;
	if(end == start) return true;
	if ((start < 0) || (end < 1) || (end <= start))
		return false;
	value = serializedData.substr(start + 1, end - start);
	Utils::replace(value, "\\\"", "\\\"");
	Utils::replace(value, "\\\\", "\\");

	return true;
}
