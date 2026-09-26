/**
 * @file      FrameworkCommands.cpp
 * @brief     命令模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module FrameworkCommands;

import UtilConstants;

namespace FrameworkCommands
{
	void registerYxCmds()
	{
		CAcModuleResourceOverride resOverride;

		commandInfoList.clear();
		const auto& specs = CommandRegistry::getInstance().getSpecs();

		// 建立指针列表
		std::vector<const CommandSpec*> sortedSpecs;
		sortedSpecs.reserve(specs.size());
		for (const auto& spec : specs)
		{
			sortedSpecs.push_back(&spec);
		}

		// 对指针列表进行排序
		std::sort(sortedSpecs.begin(), sortedSpecs.end(), [](const CommandSpec* a, const CommandSpec* b) {
			return wcscmp(a->commandName.constPtr(), b->commandName.constPtr()) < 0;
			});

		for (const auto* pSpec : sortedSpecs)
		{
			const auto& spec = *pSpec;
			CommandInfo info;
			info.commandName = spec.commandName;
			// 触发 Lambda 执行翻译，此时 FrameworkTranslator 已经成功 initialize
			info.commandDescription = spec.getLocalizedDescription ? spec.getLocalizedDescription() : spec.commandName;
			info.flags = spec.flags;
			info.proc = spec.proc;

			commandInfoList.push_back(info);

			// 注册命令全名
			acedRegCmds->addCommand(UtilConstants::cmdGroup, info.commandName.constPtr(), info.commandName.constPtr(), info.flags, info.proc);

			// 注册命令简称
			AcString shortCommandName = info.getShortCommandName();
			acedRegCmds->addCommand(UtilConstants::cmdGroup, shortCommandName.constPtr(), shortCommandName.constPtr(), info.flags, info.proc);
		}
	}

	void executeCommand(const FrameworkCommands::CommandList& cmdList, bool usePrefix, AcApDocument* pDoc)
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