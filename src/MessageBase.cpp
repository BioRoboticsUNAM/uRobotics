/* 
 * File:   MessageBase.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include "uRobotics/api/MessageBase.h"

MessageBase::MessageBase(const std::string& commandName) :
	commandName(commandName),
	id(-1){}

MessageBase::MessageBase(const std::string& commandName, const std::string& parameters, const int& id) :
	commandName(commandName),
	parameters(parameters),
	id(id){}

MessageBase::~MessageBase(void)
{
}

bool MessageBase::hasParams() const{
	return this->parameters.length() > 0;
}

std::string MessageBase::getEscapedParameters() const{
	char c;
	std::stringstream ss;
	for(std::string::size_type i = 0; i < this->parameters.size(); ++i){
		c = this->parameters[i];
		if (c == '\\')
		{
			ss << c;
			++i;
			if (i >= this->parameters.size())
			{
				ss << "\\";
				break;
			}
			ss << this->parameters[i];
			continue;
		}
		else if (c == '"')
			ss << "\\\"";
		else
			ss << c;
	}
	return ss.str();
}

void MessageBase::setEscapedParameters(const std::string& p){
	char c;
	std::stringstream ss;
	for(std::string::size_type i = 0; i < p.size(); ++i){
		c = p[i];
		if (c != '\\') 
		{
			ss << c;
			continue;
		}
		if( (i+1) >= p.size() )
			continue;
		++i;
		c = p[i];
		if( (c == '"') || (c == '\\'))
			ss << c;
		else
			ss << "\\" << c;	
	}
	this->parameters = ss.str();
}