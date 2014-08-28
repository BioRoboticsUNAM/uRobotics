/* 
 * File:   SumCommandExecuter.cpp
 * Author: Mauricio Matamoros
 *
 * There are two kinds of command executers: SyncCommandExecuter and AsyncCommandExecuter.
 * SyncCommandExecuter: those are executed within the CommandManager's main thread context, they do not allow communications and shall return in a few milliseconds
 * AsyncCommandExecuter: those are executed in a separate thread, allow communications and may take long periods of time to execute.
 */

#ifndef __SUM_COMMAND_EXECUTER_H__
#define	__SUM_COMMAND_EXECUTER_H__

#include "uRobotics/api/Command.h"
#include "uRobotics/api/Response.h"
#include "uRobotics/api/SyncCommandExecuter.h"

class SumCommandExecuter : public SyncCommandExecuter
{
public:
	SumCommandExecuter(void);
	~SumCommandExecuter(void);
	// This method indicates if the command executer requires parameters or not. It is recommended to be overloaded
	virtual bool parametersRequired() const;
	// This method executes the command. It must be overloaded
	virtual Response* SyncTask(const Command& command);
};



#endif // __SUM_COMMAND_EXECUTER_H__