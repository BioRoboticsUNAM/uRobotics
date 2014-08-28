#include <uRobotics/api/psv/LongArraySharedVariable.h>

LongArraySharedVariable::LongArraySharedVariable(const std::string& name):
SharedVariableGeneric< std::vector<boost::int64_t> >(name){}

LongArraySharedVariable::LongArraySharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric< std::vector<boost::int64_t> >(commandManager, variableName, initialize){}

const std::string LongArraySharedVariable::getTypeName() const{
	return std::string("long");
}

const bool LongArraySharedVariable::isArray() const{
	return true;
}

const int LongArraySharedVariable::getLength() const{
	return -1;
}

bool LongArraySharedVariable::serialize(const std::vector<boost::int64_t>& values, std::string& serializedData) const{
	serializedData.clear();
	if (values.size() < 1)
		return true;

	std::stringstream ss;
	ss << values[0];
	for (size_t i = 1; i < values.size(); ++i)
		ss << " " << values[i];
	serializedData = std::string(ss.str());

	return true;
}

bool LongArraySharedVariable::deserialize(const std::string& serializedData, std::vector<boost::int64_t>& values) const{
	std::vector<std::string> parts;
	boost::int64_t data;

	values.clear();
	boost::split(parts, serializedData, boost::is_any_of(" \t\r\n"));

	for (size_t i = 0; i < parts.size(); ++i)
	{
		if (!Utils::convert(parts[i], data)){
			values.clear();
			return false;
		}
		values.push_back(data);
	}
	return true;
}
