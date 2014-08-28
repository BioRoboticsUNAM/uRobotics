#include <uRobotics/api/psv/DoubleArraySharedVariable.h>
#include <uRobotics/utilities/Utils.h>

using namespace Robotics::Utilities;

DoubleArraySharedVariable::DoubleArraySharedVariable(const std::string& name):
SharedVariableGeneric< std::vector<double> >(name){}

DoubleArraySharedVariable::DoubleArraySharedVariable(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric< std::vector<double> >(commandManager, variableName, initialize){}

const std::string DoubleArraySharedVariable::getTypeName() const{
	return std::string("double");
}

const bool DoubleArraySharedVariable::isArray() const{
	return true;
}

const int DoubleArraySharedVariable::getLength() const{
	return this->bufferedData.size();
}

bool DoubleArraySharedVariable::serialize(const std::vector<double>& values, std::string& serializedData) const{
	serializedData.clear();
	if (values.size() < 1)
		return true;

	std::stringstream ss;
	ss << values[0];
	for (size_t i = 1; i < values.size(); ++i)
	{
		if (values[i] != values[i])
			return false;
		ss << " " << values[i];
	}
	serializedData = std::string(ss.str());

	return true;
}

bool DoubleArraySharedVariable::deserialize(const std::string& serializedData, std::vector<double>& values) const{
	std::vector<std::string> parts;
	double data;

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
