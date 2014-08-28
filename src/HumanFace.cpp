#include <uRobotics/hal/sensors/HumanFace.h>
#include <uRobotics/Exception.h>

using namespace Robotics::HAL::Sensors;

const double HumanFace::MaximumPan = 1.05;
const double HumanFace::MaximumTilt = 1.05;
const double HumanFace::MinimumPan = -1.05;
const double HumanFace::MinimumTilt = -1.05;
const boost::regex HumanFace::RxNameValidator("^[A-Za-z_][\\w\\s]*$");

HumanFace::HumanFace() : name("Unknown"), pan(0), tilt(0), confidence(0) {}

HumanFace::HumanFace(const std::string& name, const double& pan, const double& tilt, const double& confidence) {
	this->setName(name);
	this->setPan(pan);
	this->setTilt(tilt);
	this->setConfidence(confidence);
}

std::string HumanFace::getName() const {
	return this->name;
}

void HumanFace::setName(const std::string& value) {
	if (value.empty())
		throw ArgumentNullException();
	if (!boost::regex_match(value, RxNameValidator))
		throw Exception("Invalid input string (name)");
	this->name = value;
}

double HumanFace::getPan() const {
	return this->pan;
}

void HumanFace::setPan(const double& value) {
	if ((value < -1.57) || (value > 1.57))
		throw ArgumentOutOfRangeException();
	this->pan = value;
}

double HumanFace::getTilt() const {
	return this->tilt;
}

void HumanFace::setTilt(const double& value) {
	if ((value < -1.57) || (value > 1.57))
		throw ArgumentOutOfRangeException();
	this->tilt = value;
}

double HumanFace::getConfidence() const {
	return this->confidence;
}

void HumanFace::setConfidence(const double& value) {
	if ((value > 1) || (value < 0))
		throw ArgumentOutOfRangeException();
	this->confidence = value;
}