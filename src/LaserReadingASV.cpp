/*
#include <uRobotics/api/msv/LaserReadingASV.h>
#include <uRobotics/utilities/Utils.h>

using namespace Robotics::Utilities;

LaserReadingASV::LaserReadingASV(const std::string& name):
SharedVariableGeneric< std::vector<LaserReading> >(name){}

LaserReadingASV::LaserReadingASV(SimpleCommandManager *commandManager, const std::string& variableName, const bool& initialize):
SharedVariableGeneric< std::vector<LaserReading> >(commandManager, variableName, initialize){}

const std::string LaserReadingASV::getTypeName() const{
	return std::string("LaserReadingArray");
}

const bool LaserReadingASV::isArray() const{
	return false;
}

const int LaserReadingASV::getLength() const{
	return -1;
}

bool LaserReadingASV::serialize(const std::vector<LaserReading>& values, std::string& serializedData) const{
	serializedData.clear();
	return false;
}

bool LaserReadingASV::deserialize(const std::string& serializedData, std::vector<LaserReading>& values) const{
	values.clear();
	return false;
}
*/
