/* 
 * File:   SumCommandExecuter.cpp
 * Author: Mauricio Matamoros
 *
 * There are two kinds of command executers: SyncCommandExecuter and AsyncCommandExecuter.
 * SyncCommandExecuter: those are executed within the CommandManager's main thread context, they do not allow communications and shall return in a few milliseconds
 * AsyncCommandExecuter: those are executed in a separate thread, allow communications and may take long periods of time to execute.
 */

#ifndef __MODULE_H__
#define	__MODULE_H__

#pragma warning(disable: 4996)

#include <string>
// Include uRobotics libraries

// Class that manages connection with Blackboard
#include <uRobotics/api/SimpleConnectionManager.h>
// Class that manages commands and shared variables
#include <uRobotics/api/SimpleCommandManager.h>
// Class that encapsulates a command
#include <uRobotics/api/Command.h>
// Class that encapsulates a response
#include <uRobotics/api/Response.h>

class Module{
private:
	/**
	* A command manager to handle commands and shared variables
	*/
	SimpleCommandManager cmdMan;
	/**
	* A connection manager to handle connections from Blackboard
	*/
	SimpleConnectionManager cnnMan;

	/**
	* Configures the command and connection managers, then bind the connection manager to the command manager
	*/
	void setupManagers();
	/**
	* Initializes all the command executers and adds them to the Command Manager
	*/
	void setupCommandExecuters();
	/**
	* Start the command and connection managers, then wait for the blackboard to connect
	*/
	void startManagers();

public:
	/**
	* Initializes a module
	* @param port The port where the blackboard will connect to the module.
	*/
	Module(const int& port);
	/**
	* Keeps running the module application till exit
	*/
	void run();
	/**
	* Sends a command, waits for the number of seconds specified in timeout and returns a response object
	* @param command A command object which will be sent to the blackboard for execution
	* @param timeout The amount of time to wait for a response in milliseconds
	* @param response A pointer to a response object which contains the response received if the command was executed
	* @return true if command was sent and executed, false otherwise
	*/
	bool sendAndWait(const Command& command, const size_t& timeout, Response*& response);

	/**
	* Sends a command, waits for the number of seconds specified in timeout and returns a response object
	* @param command A string that represents a command which will be sent to the blackboard for execution
	* @param timeout The amount of time to wait for a response in milliseconds
	* @param response A string that represents the response received (if the command was executed)
	* @return true if command was sent and executed, false otherwise
	*/
	bool sendAndWait(const std::string& command, const size_t& timeout, std::string& response);

	/**
	* Initializes a shared variable wrapper
	* @param variableName The name of the shared variable to initialize
	* @param wrapper A SharedVariable<U> derived object which wraps around the shared variable in the blackboard
	*/
	template<typename T>
	void setupSharedVariable(const std::string& variableName, T*& wrapper);

	/**
	* Starts the module application
	*/
	void start();
};

template<typename T>
void Module::setupSharedVariable(const std::string& variableName, T*& wrapper){
	// To add a new shared variable, it is necessary to check if the variable already exists. If does exists, the local wrapper 
	// is pointed, otherwise, a new variable is created and added to the Command Manager
	wrapper = NULL;
	if(cmdMan.getSharedVariables().contains(variableName))
		wrapper = (T*)cmdMan.getSharedVariables()[variableName];
	else{
		wrapper = new T(variableName);
		cmdMan.getSharedVariables().add(wrapper);
	}
}

#endif // __MODULE_H__