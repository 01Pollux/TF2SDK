#pragma once

#include "../interfaces/InterfacesSys.hpp"

SG_NAMESPACE_BEGIN;

/**
 * 
 * 
 */

class IConCommand;

class IConCommandManager : public IInterface
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name=""></param>
	virtual void Register(IPlugin* pPlugin, IConCommand* command) abstract;
};


SG_NAMESPACE_END;
/*

// my_command -arg0 val0 -arg1 val1 val2
IConCommand cmd(
"my_command",
R"(my command description)",
R"(command output on failure)",
[](ICommandArgs* pArgs)
{
	int arg = pArgs->GetArg<int>("arg1", 3 // default value<);
});

CmdManager->Register(me, cmd);



*/