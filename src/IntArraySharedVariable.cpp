#include <uRobotics/api/psv/IntArraySharedVariable.h>
#include <uRobotics/utilities/Utils.h>

using namespace Robotics::Utilities;

IntArraySharedVariable::IntArraySharedVariable(const std::string& name):
SharedVariableGeneric< std::vector<boost::int32_t> >(name){}

IntArraySharedVariable::IntArraySharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric< std::vector<boost::int32_t> >(commandManager, variableName, initialize){}

const std::string IntArraySharedVariable::getTypeName() const{
	return std::string("int");
}

const bool IntArraySharedVariable::isArray() const{
	return true;
}

const int IntArraySharedVariable::getLength() const{
	return -1;
}

bool IntArraySharedVariable::serialize(const std::vector<boost::int32_t>& values, std::string& serializedData) const{
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

bool IntArraySharedVariable::deserialize(const std::string& serializedData, std::vector<boost::int32_t>& values) const{
	std::vector<std::string> parts;
	boost::int32_t data;

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
