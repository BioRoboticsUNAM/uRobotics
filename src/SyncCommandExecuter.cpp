/* 
 * File:   SyncCommandExecuter.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include "uRobotics/api/SyncCommandExecuter.h"

SyncCommandExecuter::SyncCommandExecuter(const std::string& commandName) :
	CommandExecuter(commandName),
	busy(false), running(false){}

SyncCommandExecuter::SyncCommandExecuter(const std::string& commandName, SimpleCommandManager *cmdMan) :
	CommandExecuter(commandName, cmdMan),
	busy(false), running(false){}

SyncCommandExecuter::~SyncCommandExecuter(void)
{
}

bool SyncCommandExecuter::isBusy() const{
	return this->busy;
}

bool SyncCommandExecuter::isRunning() const{
	return this->running;
}

bool SyncCommandExecuter::parametersRequired() const{
	return false;
}

bool SyncCommandExecuter::abort(){
	return false;
}

void SyncCommandExecuter::execute(const Command& command){
	// 1. Check busy state and parameters
	if (isBusy()){
		//if (ResponseRequired)
			sendResponse(command, false);
		return;
	}
	if (parametersRequired() && !command.hasParams()){
		sendResponse(command, false);
		return;
	}

	// 2. Set execution flags
	this->busy = true;
	this->running = true;

	// 3. Create default failed response
	Response *response = Response::createFromCommand(command, false);

	// 4. Raise ExecutionStarted event
	// onExecutionStarted(command);

	// 5. Perform command execution
	response = SyncTask(command);

	// 6. Send the result of command execution
	//if (ResponseRequired)
	if(response != NULL){
		sendResponse(*response);
		delete response;
	}

	// 7. Reset execution flags
	this->busy = false;
	this->running = false;

	// 8. Raise ExecutionFinished event
	//onExecutionFinished(command, response);
}