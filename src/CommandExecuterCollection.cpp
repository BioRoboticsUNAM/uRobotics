#include "uRobotics/api/CommandExecuterCollection.h"
#include "uRobotics/Exception.h"

using namespace Robotics;

CommandExecuterCollection::CommandExecuterCollection(SimpleCommandManager& cmdMan) :
cmdMan(cmdMan){ }

//CommandExecuterCollection::CommandExecuterCollection(const CommandExecuterCollection& other){}

CommandExecuterCollection::~CommandExecuterCollection(void)
{
}

SimpleCommandManager& CommandExecuterCollection::getCommandManager() const{
	return this->cmdMan;
}

void CommandExecuterCollection::add(CommandExecuter* commandExecuter){
	if(commandExecuter == NULL)
		throw ArgumentNullException("Argument cannot be null");
	scoped_lock(commandExecuterListMutex);
	if(contains(commandExecuter->getCommandName()))
		throw Exception("The provided CommandExecuter already exists in the collection");
	this->commandExecuterList[commandExecuter->getCommandName()] = commandExecuter;
	commandExecuter->setCmdMan(&(this->cmdMan));
}

void CommandExecuterCollection::clear() {
	scoped_lock(commandExecuterListMutex);
	this->commandExecuterList.erase(this->commandExecuterList.begin(), this->commandExecuterList.end());
}

bool CommandExecuterCollection::contains(const std::string& commandName) const {
	scoped_lock(commandExecuterListMutex);
	return this->commandExecuterList.count(commandName) > 0;
}

int CommandExecuterCollection::count(){
	scoped_lock(commandExecuterListMutex);
	return this->commandExecuterList.size();
}

bool CommandExecuterCollection::remove(const std::string& commandName) {
	scoped_lock(commandExecuterListMutex);
	if(!contains(commandName))
		return false;
	this->commandExecuterList.erase(commandName);
	return true;
}

CommandExecuter* CommandExecuterCollection::operator[](const std::string& commandName){
	scoped_lock(commandExecuterListMutex);
	if(!contains(commandName))
		return NULL;
	return commandExecuterList[commandName];
}
