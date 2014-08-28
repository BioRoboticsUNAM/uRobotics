#include <uRobotics/api/psv/IntSharedVariable.h>

IntSharedVariable::IntSharedVariable(const std::string& name):
SharedVariableGeneric<boost::int32_t>(name){}


IntSharedVariable::IntSharedVariable(SimpleCommandManager *commandManager, const std::string& name, const bool& initialize) :
SharedVariableGeneric<boost::int32_t>(commandManager, name, initialize){}

const std::string IntSharedVariable::getTypeName() const{
	return std::string("int");
}

const bool IntSharedVariable::isArray() const{
	return false;
}

const int IntSharedVariable::getLength() const{
	return -1;
}

bool IntSharedVariable::serialize(const boost::int32_t& value, std::string& serializedData) const{
	std::stringstream ss;
	ss << value;
	serializedData = std::string(ss.str());
	return true;
}

bool IntSharedVariable::deserialize(const std::string& serializedData, boost::int32_t& value) const{
	try{
		std::stringstream(serializedData) >> value;
		return true;
	}catch(...){ return false; }
}