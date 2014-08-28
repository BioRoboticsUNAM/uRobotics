#include <uRobotics/api/psv/ByteArraySharedVariable.h>
#include <iostream>
#include <iomanip>

ByteArraySharedVariable::ByteArraySharedVariable(const std::string& name):
SharedVariableGeneric< std::vector<boost::int8_t> >(name){}

ByteArraySharedVariable::ByteArraySharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric< std::vector<boost::int8_t> >(commandManager, variableName, initialize){}

const std::string ByteArraySharedVariable::getTypeName() const{
	return std::string("byte");
}

const bool ByteArraySharedVariable::isArray() const{
	return true;
}

const int ByteArraySharedVariable::getLength() const{
	return -1;
}

bool ByteArraySharedVariable::serialize(const std::vector<boost::int8_t>& value, std::string& serializedData) const{

	std::stringstream ss;
	ss << "0x";
	for(size_t i = 0; i < value.size(); ++i)
		ss << std::setfill('0') << std::hex << std::setw(2) << (int)value[i];
	serializedData = std::string(ss.str());
	return true;
}

bool ByteArraySharedVariable::deserialize(const std::string& serializedData, std::vector<boost::int8_t>& value) const{
	
	size_t cc;
	value.clear();
	boost::int8_t data;
	std::stringstream ss;

	// Eat white spaces
	cc = 0;
	while ((cc < serializedData.size()) && ((serializedData[cc] == 32) || (serializedData[cc] == '\t')))
		++cc;

	// No data. Return.
	if ((serializedData.size() - cc) < 2)
		return false;

	// Skip prefix 0x and 0X
	if ((serializedData[cc] == '0') && ((serializedData[cc+1] == 'X') || (serializedData[cc+1] == 'x')))
		cc += 2;

	// Parse data
	// 0x 48 60 28 68 64 57 63 2a
	while (cc < serializedData.size() - 1)
	{
		try{
			boost::uint32_t iData = 0;
			ss.clear();
			ss << std::hex << serializedData.substr(cc, 2);
			ss >> iData;
			data = (boost::int8_t)iData;
		}
		catch(...){ 
			value.clear();
			return false;
		}
		value.push_back(data);
		cc += 2;
	}
	return true;
}
