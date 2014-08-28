/* 
 * File:   SimpleCommandManager.cpp
 * Author: Mauricio Matamoros
 *
 * Created on Aug 12, 2013 de 2013, 10:53 AM
 */

#include <climits>
#include "uRobotics/api/SimpleCommandManager.h"
#include "uRobotics/api/CommandExecuter.h"
#include "uRobotics/utilities/Stopwatch.h"
#include "uRobotics/api/SharedVariableReport.h"

SimpleCommandManager::SimpleCommandManager() :
	busy(false), ready(false), running(false), executers(*this), sharedVariables(*this)
{
	this->mainThread = NULL;
	this->cnnMan = NULL;
}

SimpleCommandManager::~SimpleCommandManager(){ this->stop(); }

bool SimpleCommandManager::isRunning()
{
	return this->running;
}

bool SimpleCommandManager::getBusy(){
	return this->busy;
}

void SimpleCommandManager::setBusy(bool busy){
	if(this->busy == busy)
		return;
	this->busy = busy;
	Response r("busy", busy);
	send(r);
}

bool SimpleCommandManager::getReady(){
	return this->ready;
}

void SimpleCommandManager::setReady(bool ready){
	if(this->ready == ready)
		return;
	this->ready = ready;
	Response r("ready", ready);
	send(r);
}

CommandExecuterCollection& SimpleCommandManager::getCommandExecuters(){
	return this->executers;
}

SharedVariableList& SimpleCommandManager::getSharedVariables(){
	return this->sharedVariables;
}

void SimpleCommandManager::bindConnectionManager(SimpleConnectionManager& cnnMan)
{
	this->cnnMan = &cnnMan;
}

SimpleConnectionManager& SimpleCommandManager::getConnectionManager(){
	return *(this->cnnMan);
}

void SimpleCommandManager::cleanBuffers(){
	commandsReceived.clear();
	responsesReceived.clear();
	{
		scoped_lock lock(unpairedResponsesMutex);
		while (!unpairedResponses.empty()){
			delete unpairedResponses[unpairedResponses.size()-1];
			unpairedResponses.pop_back();
		}
	}
	// parallelSendAndWaitQueue.Clear();
}

void SimpleCommandManager::start()
{
	if(this->running)
		return;

	if(this->cnnMan == NULL)
		return;
	if(!this->cnnMan->isRunning())
		this->cnnMan->start();
	running = true;
	this->mainThread = new boost::thread(boost::bind(&SimpleCommandManager::mainThreadTask, this));
}

void SimpleCommandManager::stop()
{
	if(!this->running)
		return;
	
	this->running = false;
	if(this->cnnMan->isRunning())
		this->cnnMan->start();
	delete this->mainThread;
}

void SimpleCommandManager::mainThreadTask(){
	Command *cmd;
	ProducerConsumer<Command*>& pendingCommands = this->commandsReceived;
	std::vector<Response*>& unpairedResponses = this->unpairedResponses;

	this->parserThread = new boost::thread(boost::bind(&SimpleCommandManager::parserThreadTask, this));
	this->responseAnalizerThread = new boost::thread(boost::bind(&SimpleCommandManager::responseAnalizerThreadTask, this));

	while(running){
		if(pendingCommands.timedConsume(100, cmd) && (cmd != NULL)){
			executeCommand(*cmd);
			delete cmd;
		}
	}

	commandsReceived.clear();
	responsesReceived.clear();
}

bool SimpleCommandManager::manageSubscription(const Response& response){
	// 1. Get report info
	SharedVariableReport *report;
	Exception ex;
	if (!SharedVariableReport::createFromResponse(response, report))
		return false;
	const ISharedVariableInfo* info = report->getVariableInfo();
	std::string name(info->getName());

	// 2. Update variable
	if (!sharedVariables.contains(name)){
		delete info;
		delete report;
		return true;
	}

	sharedVariables[name]->update(*report, ex);
	delete info;
	delete report;
	return true;
}

void SimpleCommandManager::parserThreadTask(){
	ProducerConsumer<TcpPacket*>& packetsReceived = this->cnnMan->getPacketsReceived();

	TcpPacket *packet;
	while(running){
		if(!packetsReceived.timedConsume(100, packet) || (packet == NULL))
			continue;
		parseTcpPacket(*packet);
		delete packet;
	}
}

void SimpleCommandManager::responseAnalizerThreadTask(){
	Response *response;
	while(running){
		if(!responsesReceived.timedConsume(100, response) || (response == NULL))
			continue;
		if (manageSubscription(*response)){
			delete response;
			continue;
		}
		//lock (unpairedResponses)
		{
			scoped_lock lock(unpairedResponsesMutex);
			unpairedResponses.push_back(response);
		}
	}
}

void SimpleCommandManager::parseTcpPacket(TcpPacket& packet)
{
	Command* cmd;
	Response *rsp;
	std::vector<std::string>& strings = packet.getDataStrings();
	for(size_t i = 0; i < strings.size(); ++i){
		rsp = Response::parse(strings[i]);
		if(rsp != NULL){
			responsesReceived.produce(rsp);
			continue;
		}
		cmd = Command::parse(strings[i]);
		if(cmd != NULL){
			commandsReceived.produce(cmd);
			continue;
		}
	}
}

void SimpleCommandManager::executeCommand(const Command& command)
{
	if(executeSystemCommand(command))
		return;

	if(this->executers.contains(command.commandName))
	{
		CommandExecuter* executer = executers[command.commandName];
		executer->execute(command);
	}
	// Response *rsp = Response::createFromCommand(command, false);
	// send(*rsp);
	// delete rsp;
}

bool SimpleCommandManager::executeSystemCommand(const Command& command){
	Response *rspPtr = NULL;
	bool result = false;

	if(!command.commandName.compare("ready")){
		rspPtr = Response::createFromCommand(command, this->ready);
		result = true;
	}
	else if(!command.commandName.compare("busy")){
		rspPtr = Response::createFromCommand(command, this->busy);
		result = true;
	}
	else if(!command.commandName.compare("alive")){
		if(this->busy)
			rspPtr = new Response("busy", "", this->busy, -1);
		else if(!this->ready)
			rspPtr = new Response("ready", "", this->ready, -1);
		else
			rspPtr = Response::createFromCommand(command, true);
		result = true;
	}

	if(rspPtr != NULL){
		send(*rspPtr);
		delete rspPtr;
	}
	return result;
}

bool SimpleCommandManager::send(const Command& command){
	if(this->cnnMan == NULL)
		return false;
	return this->cnnMan->sendToAll(command.getStringToSend()) > 0;
}

bool SimpleCommandManager::send(const Response& response){
	if(this->cnnMan == NULL)
		return false;
	return this->cnnMan->sendToAll(response.getStringToSend()) > 0;
}

bool SimpleCommandManager::sendAndWait(const Command& command, const int& timeout, Response*& response)
{
	// Time elapsed for operation
	Stopwatch swElapsed;
	// Flag that indicates if a response has been found
	bool found = false;
	// Response candidate while looking for
	Response *candidate = NULL;

	// Initialize response
	response = NULL;
	int timeOut = (timeout <= 0) ? INT_MAX : timeout;

	// 1. Send the command. If command cannot be sent, return false
	if (!send(command))
		return false;

	// 2. Wait for response to arrival
	swElapsed.reset();
	swElapsed.start();
	while (running && (swElapsed.elapsedMilliseconds() < timeOut))
	{
		//lock (unpairedResponses)
		{
			scoped_lock lock(unpairedResponsesMutex);
			for (size_t i = 0; running && (i < unpairedResponses.size()); ++i)
			{
				candidate = unpairedResponses[i];
				if (command.isMatch(*candidate))
				{
					unpairedResponses.erase(unpairedResponses.begin() + i);
					found = true;
					break;
				}
			}
		}
		if (found) break;
		candidate = NULL;
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
	swElapsed.stop();

	// 3. No response arrival. Return false
	if (!found || (candidate == NULL))
		return false;

	// Success
	response = candidate;
	return true;
}

Response* SimpleCommandManager::waitForResponse(const Command& commandSent, const int& timeout)
{
	// Time elapsed for operation
	Stopwatch swElapsed;

	// Response candidate while looking for
	Response *candidate = NULL;

	// Wait for response to arrival
	swElapsed.reset();
	swElapsed.start();
	while (running && (swElapsed.elapsedMilliseconds() < timeout))
	{
		//lock (unpairedResponses)
		{
			scoped_lock lock(unpairedResponsesMutex);
			for (size_t i = 0; running && (i < unpairedResponses.size()); ++i)
			{
				candidate = unpairedResponses[i];
				if (commandSent.isMatch(*candidate))
				{
					unpairedResponses.erase(unpairedResponses.begin() + i);
					swElapsed.stop();
					return candidate;
				}
			}
		}
		candidate = NULL;
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}

	// Timed out
	swElapsed.stop();
	return NULL;
}
void SimpleCommandManager::onSharedVariablesLoaded(){
	sharedVariablesLoaded(this);
}