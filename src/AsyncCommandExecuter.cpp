/* 
 * File:   AsyncCommandExecuter.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include "uRobotics/api/AsyncCommandExecuter.h"

AsyncCommandExecuter::AsyncCommandExecuter(const std::string& commandName) :
	CommandExecuter(commandName),
	busy(false), running(false),
	asyncExecutionThread(NULL){}

AsyncCommandExecuter::AsyncCommandExecuter(const std::string& commandName, SimpleCommandManager *cmdMan) :
	CommandExecuter(commandName, cmdMan),
	busy(false), running(false),
	asyncExecutionThread(NULL){}

AsyncCommandExecuter::~AsyncCommandExecuter(void)
{
}

bool AsyncCommandExecuter::isBusy() const{
	return this->busy;
}

bool AsyncCommandExecuter::isRunning() const{
	return this->running;
}

bool AsyncCommandExecuter::parametersRequired() const{
	return true;
}

bool AsyncCommandExecuter::abort(){
	if(!running)
		return true;
	running = false;
	this->asyncExecutionThread->join();
	return true;
}

void AsyncCommandExecuter::execute(const Command& cmd){
	// 1. Check busy state and parameters
	if (isBusy()){
		//if (ResponseRequired)
			sendResponse(cmd, false);
		return;
	}
	if (parametersRequired() && !cmd.hasParams()){
		sendResponse(cmd, false);
		return;
	}

	// 2. Set execution flags and configure the thread
	this->busy = true;
	this->running = true;
	this->asyncExecutionThread = new boost::thread(
		boost::bind(&AsyncCommandExecuter::asyncExecutionThreadTask, this, cmd));

	// 3. See asyncExecutionThreadTask(Command& command)
}

void AsyncCommandExecuter::asyncExecutionThreadTask(const Command& command){
	try{
		// 4. Set execution flag
		//aborted = false;
		this->running = true;

		//5. Default failed response
		Response *response = Response::createFromCommand(command, false);

		// 6. Raise ExecutionStarted event
		//if (running)
		//	OnExecutionStarted(command);

		// 7. Perform command execution
		if (running)
			response = AsyncTask(command);

		// 8. Send the result of command execution (if any)
		if(response != NULL){
			sendResponse(*response);
			delete response;
		}

		// 9. Reset execution flags
		this->busy = false;
		this->running = false;
	}
	catch(...){
		// Reset execution flags to allow thread remain alive while executing Abort events
		this->busy = false;
		this->running = false;
	}
}