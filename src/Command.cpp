/* 
 * File:   Command.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include <uRobotics/api/Command.h>
#include <uRobotics/api/Response.h>
#include <uRobotics/api/Parser.h>
#include <uRobotics/utilities/Utils.h>

using namespace Robotics::Utilities;

// boost::regex Command::commandParser("^(?<command>[A-Za-z_]+)(\\s+\"(?<params>((\\\\.)|[^\"])*)\")?(\\s+@(?<id>\\d+))?$");

Command::Command(const Command& command) :
	MessageBase(command.commandName, command.parameters, command.id){}

Command::Command(const std::string& commandName) :
	MessageBase(commandName){}

	Command::Command(const std::string& commandName, const std::string& parameters) :
	MessageBase(commandName, parameters, -1){}

Command::Command(const std::string& commandName, const std::string& parameters, const int& id) :
	MessageBase(commandName, parameters, id){}

Command::~Command(void){
}

std::string Command::getStringToSend() const{
	std::string s;
	s.append( this->commandName);
	if(this->parameters.length() > 0){
		s.append(" \"");
		s.append(this->getEscapedParameters());
		s.append("\"");
	}
	if((this->id >= 0) && (this->id <= 255)){
		s.append(" @");
		std::stringstream stream;
		stream << this->id;
		s.append(stream.str());
	}
	return s;
}

Command* Command::parse(const std::string& s) {
	size_t cc = 0;
	Command *command = NULL;
	std::string commandName;
	std::string parameters;
	int result = 0;
	int id = 0;
	
	// boost::cmatch match;
	// if(!boost::regex_match(s.c_str(), match, Command::commandParser))
	// 	return NULL;
	// 
	// commandName.append(match.str("command"));
	// parameters.append(match.str("params"));
	// if(match.str("id").length() > 0)
	// {
	// 	std::stringstream stream(match.str("id"));
	// 	stream >> id;
	// }
	// else id = -1;

	if(!Parser::xtractCommandName(s, cc, commandName))
		return NULL;
	Parser::skipSpaces(s, cc);
	if(Parser::xtractCommandParams(s, cc, parameters))
		Parser::skipSpaces(s, cc);
	if(Parser::xtractResult(s, cc, result))
		return NULL;
	if(!Parser::xtractId(s, cc, id))
		id = -1;
	Utils::replace(parameters, "\\\"", "\"");
	command = new Command(commandName, parameters, id);
	return command;
}

bool Command::isMatch(const Response& response) const {
	bool matchId = ((this->id != -1) && (response.id != -1)) ? (this->id == response.id) : true;
	return (this->commandName.compare(response.commandName) == 0) && matchId;
}

std::ostream& operator<<(std::ostream& os, const Command& command){
	return os << command.getStringToSend();
}

std::ostream& operator<<(std::ostream& os, const Command*& command){
	if(command != NULL)
		os << command->getStringToSend();
	return os;
}