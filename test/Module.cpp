#include "Module.h"

// Include standard libraries
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <boost/bind.hpp> 
#include <boost/cstdint.hpp>

// Include uRobotics libraries
// Exception handling
#include <uRobotics/Exception.h>

// Local command executers used to execute commands received from blackboard
#include "SumCommandExecuter.h"

Module::Module(const int& port): cnnMan(port){}

void Module::run(){
	// Nothing else to do.
	while(true)
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
}

void Module::start(){
	try {
		setupManagers();
		setupCommandExecuters();
		startManagers();
		// Shared variables are loaded.
		cmdMan.getSharedVariables().loadFromBlackboard();
		// When the module is ready, it is necessary to inform the Blackboard
		cmdMan.setReady(true);
	}
	catch (Exception& e) { std::cerr << "Exception: " << e.what() << std::endl; }
	catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; }
}

void Module::setupManagers(){
	// First step: Create a Command Manager and a Connection Manager and set the connection port (2011).
	// The Connection Manager must be bound to the command manager so they can communicate with each other.
	std::cout << "Starting uRobotics" << std::endl;
	cmdMan.bindConnectionManager(cnnMan);
}

void Module::setupCommandExecuters(){
	// Command executers execute the commands. One instance of each must be created and added to the Command Manager
	cmdMan.getCommandExecuters().add(new SumCommandExecuter());
}

void Module::startManagers(){
	// Start the Command Manager, it will start also the Connection Manager
	std::cout << "\tStarting Command Manager...";
	cmdMan.start();
	std::cout << "\r\tCommand Manager started     " << std::endl;

	// Other initialization code can be written here

	// Before loading the SharedVariables and add new ones, the Connection Manager must be connected to the Blackboard
	// Therefore, program shall wait.
	std::cout << "uRobotics started!\t\t" << std::endl;
	cnnMan.waitClientConnected();
}

bool Module::sendAndWait(const Command& command, const size_t& timeout, Response*& response){
	return cmdMan.sendAndWait(command, timeout, response);
}

bool Module::sendAndWait(const std::string& command, const size_t& timeout, std::string& response){
	Command* cmdPtr = Command::parse(command);
	Response *rspPtr = NULL;
	
	if(!cmdPtr) return false;
	bool result = cmdMan.sendAndWait(*cmdPtr, timeout, rspPtr);
	delete cmdPtr;
	if(rspPtr != NULL){
		response = std::string(rspPtr->getStringToSend());
		delete rspPtr;
	}
	return result;
}
