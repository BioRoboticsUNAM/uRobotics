#include <uRobotics/utilities/Utils.h>
using namespace Robotics::Utilities;

void Utils::trim(std::string& s){
	size_t p = s.find_first_not_of(" \t\r\n");
		s.erase(0, p);
	p = s.find_last_not_of(" \t\r\n");
	if (std::string::npos != p)
		s.erase(p+1);
}


unsigned int Utils::hashString(const std::string& s){
	unsigned int a = 54059; // A prime
	unsigned int b = 76963; // Another prime
	unsigned int h = 31; // also prime
	for(size_t i = 0; i < s.size(); ++i) {
		h = (h * a) ^ (s[i] * b);
	}
	return h;
}

unsigned int Utils::hashString(const char* s){
	unsigned int a = 54059; // A prime
	unsigned int b = 76963; // Another prime
	unsigned int h = 31; // also prime
	while (*s) {
		h = (h * a) ^ (*s * b);
		s++;
	}
	return h;
}


bool Utils::convert(const std::string& input, boost::int8_t& value){
	boost::int32_t iValue;
	try{
		std::stringstream(input) >> iValue;
		value = (boost::int8_t)iValue;
		return true;
	}catch(...){ return false; }
}

bool Utils::convert(const std::string& input, boost::int16_t& value){
	std::stringstream ss(input);
	try{
		ss >> value;
		return true;
	}catch(...){ return false; }
}

bool Utils::convert(const std::string& input, boost::int32_t& value){
	try{
		std::stringstream(input) >> value;
		return true;
	}catch(...){ return false; }
}

bool Utils::convert(const std::string& input, boost::int64_t& value){
	try{
		std::stringstream(input) >> value;
		return true;
	}catch(...){ return false; }
}

bool Utils::convert(const std::string& input, float& value){
	try{
		std::stringstream(input) >> value;
		return true;
	}catch(...){ return false; }
}

bool Utils::convert(const std::string& input, double& value){
	try{
		std::stringstream(input) >> value;
		return true;
	}catch(...){ return false; }
}

void Utils::replace(std::string& str, const std::string& oldString, const std::string& newString){
	if(str.empty() || oldString.empty())
		return;
	size_t start = 0;
	while((start = str.find(oldString, start)) != std::string::npos) {
		str.replace(start, oldString.length(), newString);
		start += newString.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
