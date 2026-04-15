module;
#include "stdafx.h"
#include "resource.h"

module Commands;
import Common;

namespace Commands
{
	void registerYxCmds(Commands::CommandInfoList& cil)
	{
		CAcModuleResourceOverride resOverride;
		CString cmdGroup;
		cmdGroup.LoadStringW(IDS_CommandGroup);
		for (Commands::CommandInfo commandInfo : cil)
		{
			// ×¢²áÃüÁîÈ«Ãû
			acedRegCmds->addCommand(cmdGroup, commandInfo.commandName.constPtr(), commandInfo.commandName.constPtr(), commandInfo.flags, commandInfo.proc);
			// ×¢²áÃüÁî¼ò³Æ
			AcString shortCommandName = commandInfo.getShortCommandName();
			acedRegCmds->addCommand(cmdGroup, shortCommandName.constPtr(), shortCommandName.constPtr(), commandInfo.flags, commandInfo.proc);
		}
	}
}