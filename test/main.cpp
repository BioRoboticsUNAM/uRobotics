/* 
 * File:   main.cpp
 * Author: Mauricio Matamoros
 *
 */
#pragma warning(disable: 4996)

// Include standard libraries
#include <cstdlib>
#include <iostream>
#include "Module.h"
// Class that manages shared variable updates
#include <uRobotics/api/SharedVariableSubscriptionReport.h>
// Wrappers for shared variables
#include <uRobotics/api/psv/IntSharedVariable.h>
#include <uRobotics/api/psv/StringSharedVariable.h>

// A wrapper for a shared variable that stores an int
IntSharedVariable *ivar = NULL;

/**
* The function that will handle when a shared variable (int_var_test) is written
* @param report Contains the information of the shared variable which information has been written
*/
void ivarWritten(const SharedVariableSubscriptionReport<int>& report);

int main(int argc, char* argv[])
{
	// Configure the module to run on port 2011
	Module module(2011);
	// Start the module logic
	module.start();

	// Send Example Commands: Sending a command using Command and response objects
	Response *rspPtr;
	if(!module.sendAndWait(Command("connected", "ACT-PLN", 1), 2000, rspPtr))
		std::cout << "No response received for command: 'connected \"ACT-PLN\"'"  << std::endl;
	else{
		std::cout << "Received: " << *rspPtr << std::endl;
		delete rspPtr;
	}

	// Send Example Commands: Sending a command using strings
	std::string rspStr;
	if(!module.sendAndWait("connected \"ACT-PLN\" @1", 2000, rspStr))
		std::cout << "No response received for command: 'connected \"ACT-PLN\"'"  << std::endl;
	else
		std::cout << "Received: " << rspStr << std::endl;

	// Send Example Commands: Sending a command using strings to this module
	if(!module.sendAndWait("sum \"2.4 3.6\" @1", 2000, rspStr))
		std::cout << "No response received for command: 'sum \"2.4 3.6\"'"  << std::endl;
	else
		std::cout << "Received: " << rspStr << std::endl;

	// Setup of a shared variable (int) with subscription
	// First the shared variable is initialized
	module.setupSharedVariable<IntSharedVariable>("int_var_test", ivar);
	
	// After initialized, the shared variable may be read or written.
	// However, no notification on updates will be received since there is no subscription
	ivar->tryWrite(100);
	std::cout << "int_var_test: "; std::cout << ivar->read(); std::cout << std::endl;

	// Second: set the function that will handle the write notification
	ivar->writeNotification.connect(ivarWritten);
	// Third to subscribe to the shared variable. This subscription will request for the content of the variable no matter who's the writer.
	((SharedVariable*)ivar)->subscribe(SharedVariableReportType::SendContent, SharedVariableSubscriptionType::WriteAny);

	// After subscription, a notification will be received with every update
	ivar->tryWrite(10);

	// That's it. Keep the module running
	module.run();

	return 0;
}

void ivarWritten(const SharedVariableSubscriptionReport<int>& report){
	int value = report.getValue();
	std::cout << "Shared variable '"<< report.getVariable()->getName() <<"' = ";
	std::cout << value;
	std::cout << std::endl;
	if(value > 0)
		ivar->tryWrite(value -1);

}