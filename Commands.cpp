module;
#include "stdafx.h"
#include "resource.h"

module Commands;
import Common;

namespace Commands
{
	void registerYxCmds(Commands::CommandInfoList& cil)
	{
		CString cmdGroup;
		cmdGroup.LoadStringW(IDS_CommandGroup);
		for (Commands::CommandInfo commandInfo : cil)
		{
			// ×¢²áÃüÁîÈ«Ãû
			acedRegCmds->addCommand(cmdGroup, commandInfo.commandName, commandInfo.commandName, commandInfo.flags, commandInfo.proc); 
			// ×¢²áÃüÁî¼ò³Æ
			AcString shortCommandName = commandInfo.getShortCommandName();
			acedRegCmds->addCommand(cmdGroup, shortCommandName.kACharPtr(), shortCommandName.kACharPtr(), commandInfo.flags, commandInfo.proc);
		}
	}
}