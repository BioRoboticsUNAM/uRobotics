#include "uRobotics/Exception.h"

using namespace Robotics;

Exception::Exception(void) throw() : msg("Unknown exception"){ }

Exception::Exception(const std::string& message): msg(message){}

Exception::Exception(const char *message) : msg(message){}

Exception::~Exception(void) throw(){}

const char* Exception::what() const throw(){
	return msg.c_str();
}

ArgumentNullException::ArgumentNullException(void):
Exception("Argument cannot be NULL"){}

ArgumentNullException::ArgumentNullException(const std::string& parameterName):
	Exception(std::string("Parameter '") + parameterName + std::string(" cannot be NULL")){}

ArgumentOutOfRangeException::ArgumentOutOfRangeException(void):
Exception("Argument is out of range"){}

ArgumentOutOfRangeException::ArgumentOutOfRangeException(const std::string& parameterName):
Exception(std::string("Parameter '") + parameterName + std::string(" is out of range")){}