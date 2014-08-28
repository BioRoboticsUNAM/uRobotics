#include <uRobotics/api/psv/DoubleSharedVariable.h>

DoubleSharedVariable::DoubleSharedVariable(const std::string& name):
SharedVariableGeneric<double>(name){}

DoubleSharedVariable::DoubleSharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric<double>(commandManager, variableName, initialize){}

const std::string DoubleSharedVariable::getTypeName() const{
	return std::string("double");
}

const bool DoubleSharedVariable::isArray() const{
	return false;
}

const int DoubleSharedVariable::getLength() const{
	return -1;
}

bool DoubleSharedVariable::serialize(const double& value, std::string& serializedData) const{
	std::stringstream ss;
	ss << value;
	serializedData = std::string(ss.str());
	return true;
}

bool DoubleSharedVariable::deserialize(const std::string& serializedData, double& value) const{
	try{
		std::stringstream(serializedData) >> value;
		return true;
	}catch(...){ return false; }
}