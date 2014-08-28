#include <algorithm>
#include <uRobotics/hal/sensors/RecognizedSpeech.h>

using namespace Robotics;
using namespace Robotics::HAL::Sensors;

RecognizedSpeech::RecognizedSpeech(){}

float RecognizedSpeech::getConfidence() const {
	return (alternates.size() > 0) ? alternates[0]->getConfidence() : 0;
}

int RecognizedSpeech::count() const {
	return alternates.size(); 
}

bool RecognizedSpeech::isReadOnly() const {
	return false;
}

std::string RecognizedSpeech::getText() const {
	return (alternates.size() > 0) ? alternates[0]->getText() : std::string();
}

RecognizedSpeechAlternate* RecognizedSpeech::operator[](int index) {
	return this->alternates[index];
}

void RecognizedSpeech::add(RecognizedSpeechAlternate *item){
	this->alternates.push_back(item);
}

void RecognizedSpeech::add(std::string text, float confidence){
	this->alternates.push_back(new RecognizedSpeechAlternate(text, confidence));
	//this->alternates.Sort();
}

void RecognizedSpeech::clear() {
	this->alternates.clear();
}

bool RecognizedSpeech::contains(RecognizedSpeechAlternate *item) {
	return std::find(this->alternates.begin(), this->alternates.end(), item)!= this->alternates.end();
}

bool RecognizedSpeech::remove(RecognizedSpeechAlternate *item) {
	std::vector<RecognizedSpeechAlternate*>::iterator index = std::find(this->alternates.begin(), this->alternates.end(), item);
	if(index == this->alternates.end()) return false;
	this->alternates.erase(index);
	return true;
}

void RecognizedSpeech::sort() {
	//this->alternates.Sort();
}