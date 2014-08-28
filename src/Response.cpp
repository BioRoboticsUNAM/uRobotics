/* 
 * File:   Response.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include <uRobotics/api/Response.h>
#include <uRobotics/api/Parser.h>
#include <uRobotics/utilities/Utils.h>

using namespace Robotics::Utilities;

// boost::regex Response::responseParser("^(?<command>[A-Za-z_]+)(\\s+\"(?<params>((\\\\.)|[^\"])*)\")?\\s+(?<success>[10])(\\s+@(?<id>\\d+))?$");

Response::Response(const Response& response) :
	MessageBase(response.commandName, response.parameters, response.id), success(response.success) {}

Response::Response(const std::string& commandName, const bool& success) :
	MessageBase(commandName), success(success){}

Response::Response(const std::string& commandName) :
	MessageBase(commandName), success(false){}

Response::Response(const std::string& commandName, const std::string& parameters, const bool& success, const int& id) :
	MessageBase(commandName, parameters, id), success(success){}

Response::~Response(void)
{
}

std::string Response::getStringToSend() const{
	std::string s;
	s.append( this->commandName);
	if(this->parameters.length() > 0){
		s.append(" \"");
		s.append(this->getEscapedParameters());
		s.append("\"");
	}

	s.append(success ? " 1" : " 0");

	if((this->id >= 0) && (this->id <= 255)){
		s.append(" @");
		std::stringstream stream;
		stream << this->id;
		s.append(stream.str());
	}
	return s;
}
Response* Response::parse(const std::string& s)
{
	size_t cc = 0;
	Response *response = NULL;
	std::string commandName;
	std::string parameters;
	int result;
	int id = 0;
	
	// boost::cmatch match;
	// if(!boost::regex_match(s.c_str(), match, Response::responseParser))
	//	return NULL;
	//
	// commandName.append(match.str("command"));
	// parameters.append(match.str("params"));
	// success = !match.str("success").compare("1");
	// std::stringstream stream(match.str("id"));
	// stream >> id;

	if(!Parser::xtractCommandName(s, cc, commandName))
		return NULL;
	Parser::skipSpaces(s, cc);
	if(Parser::xtractCommandParams(s, cc, parameters))
		Parser::skipSpaces(s, cc);
	if(!Parser::xtractResult(s, cc, result))
		return NULL;
	++cc;
	Parser::skipSpaces(s, cc);
	if(!Parser::xtractId(s, cc, id))
		id = -1;
	Utils::replace(parameters, "\\\"", "\"");
	response = new Response(commandName, parameters, result != 0, id);
	return response;
}

Response* Response::createFromCommand(const Command& command, const bool& success){
	Response *r = new Response(command.commandName, command.parameters, success, command.id);
	return r;
}

std::ostream& operator<<(std::ostream& os, const Response& response){
	return os << response.getStringToSend();
}

std::ostream& operator<<(std::ostream& os, const Response*& response){
	if(response != NULL)
		os << response->getStringToSend();
	return os;
}