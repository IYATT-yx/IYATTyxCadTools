/**
 * @file      Commands.cpp
 * @brief     命令模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
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
			// 注册命令全名
			acedRegCmds->addCommand(cmdGroup, commandInfo.commandName.constPtr(), commandInfo.commandName.constPtr(), commandInfo.flags, commandInfo.proc);
			// 注册命令简称
			AcString shortCommandName = commandInfo.getShortCommandName();
			acedRegCmds->addCommand(cmdGroup, shortCommandName.constPtr(), shortCommandName.constPtr(), commandInfo.flags, commandInfo.proc);
		}
	}

	void executeCommand(const Commands::CommandList& cmdList, bool usePrefix, AcApDocument* pDoc)
	{
        if (cmdList.empty())
        {
            return;
        }

        if (pDoc == nullptr)
        {
            pDoc = acDocManager->curDocument();
        }

        if (pDoc == nullptr)
        {
            return;
        }

        AcString finalCmd;
        if (usePrefix)
        {
            finalCmd.format(L"._%s\n", cmdList[0]);
        }
        else
        {
            finalCmd.format(L"%s\n", cmdList[0]);
        }
        for (size_t i = 1; i < cmdList.size(); ++i)
        {
            finalCmd.append(cmdList[i]);
            finalCmd.append(L"\n");

        }
        acDocManager->sendStringToExecute(pDoc, finalCmd.constPtr(), false, true, true);
	}
}