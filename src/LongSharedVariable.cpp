#include <uRobotics/api/psv/LongSharedVariable.h>
#include <uRobotics/utilities/Utils.h>

LongSharedVariable::LongSharedVariable(const std::string& name):
SharedVariableGeneric<boost::int64_t>(name){}

LongSharedVariable::LongSharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric<boost::int64_t>(commandManager, variableName, initialize){}

const std::string LongSharedVariable::getTypeName() const{
	return std::string("long");
}

const bool LongSharedVariable::isArray() const{
	return false;
}

const int LongSharedVariable::getLength() const{
	return -1;
}

bool LongSharedVariable::serialize(const boost::int64_t& value, std::string& serializedData) const{
	std::stringstream ss;
	ss << value;
	serializedData = std::string(ss.str());
	return true;
}

bool LongSharedVariable::deserialize(const std::string& serializedData, boost::int64_t& value) const{
	return Utils::convert(serializedData, value);
}
