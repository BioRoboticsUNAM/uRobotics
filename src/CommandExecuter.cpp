/* 
 * File:   CommandExecuter.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include "uRobotics/api/SimpleCommandManager.h"
#include "uRobotics/api/CommandExecuter.h"

CommandExecuter::CommandExecuter(const std::string& commandName) :
	commandName(commandName), cmdMan(NULL){}

CommandExecuter::CommandExecuter(const std::string& commandName, SimpleCommandManager *cmdMan) :
	commandName(commandName), cmdMan(cmdMan){}

CommandExecuter::~CommandExecuter(void)
{
}

std::string CommandExecuter::getCommandName() const{
	return this->commandName;
}

SimpleCommandManager* CommandExecuter::getCmdMan() const{
	return this->cmdMan;
}

void CommandExecuter::setCmdMan(SimpleCommandManager* cmdMan){
	this->cmdMan = cmdMan;
}

bool CommandExecuter::sendCommand(const Command& command) const{
	if(this->cmdMan == NULL)
		return false;
	return this->cmdMan->send(command);
}

bool CommandExecuter::sendResponse(const Response& response) const{
	if(this->cmdMan == NULL)
		return false;
	return this->cmdMan->send(response);
}

bool CommandExecuter::sendResponse(const Command& command, const bool& success) const{
	if(this->cmdMan == NULL)
		return false;
	Response *rsp = Response::createFromCommand(command, success);
	bool result = this->cmdMan->send(*rsp);
	delete rsp;
	return result;
}
