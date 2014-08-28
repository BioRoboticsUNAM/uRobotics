#include <uRobotics/hal/sensors/RecognizedSpeechAlternate.h>
#include <uRobotics/Exception.h>

using namespace Robotics;
using namespace Robotics::HAL::Sensors;

RecognizedSpeechAlternate::RecognizedSpeechAlternate(const std::string& text, const float& confidence) {
	this->setText(text);
	this->setConfidence(confidence);
}

float RecognizedSpeechAlternate::getConfidence() const {
	return this->confidence;
}

std::string RecognizedSpeechAlternate::getText() const {
	return this->text;
}

int RecognizedSpeechAlternate::compareTo(const RecognizedSpeechAlternate& other) {
	if (this->text.compare(other.text) == 0)
		return 0;
	return ((int)other.getConfidence() - (int)this->confidence);
}

void RecognizedSpeechAlternate::setConfidence(const float& value) {
	if ((value < 0) || (value > 1))
		throw Exception("Value must be between 0 and 1");
	this->confidence = value;
}

void RecognizedSpeechAlternate::setText(const std::string& value) {
	if(value.empty())
		throw ArgumentNullException("String cannot be null nor empty");
	this->text = std::string(value);
}