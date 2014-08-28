#include <uRobotics/api/psv/ByteSharedVariable.h>

ByteSharedVariable::ByteSharedVariable(const std::string& name):
SharedVariableGeneric<boost::int8_t>(name){}


ByteSharedVariable::ByteSharedVariable(SimpleCommandManager *commandManager, const std::string& name, const bool& initialize) :
SharedVariableGeneric<boost::int8_t>(commandManager, name, initialize){}

const std::string ByteSharedVariable::getTypeName() const{
	return std::string("byte");
}

const bool ByteSharedVariable::isArray() const{
	return false;
}

const int ByteSharedVariable::getLength() const{
	return -1;
}

bool ByteSharedVariable::serialize(const boost::int8_t& value, std::string& serializedData) const{
	std::stringstream ss;
	ss << value;
	serializedData = std::string(ss.str());
	return true;
}

bool ByteSharedVariable::deserialize(const std::string& serializedData, boost::int8_t& value) const{
	try{
		int iValue;
		std::stringstream(serializedData) >> iValue;
		if((iValue & 0x00FF) != 0)
			return false;
		value = (boost::int8_t)iValue;
		return true;
	}catch(...){ return false; }
}