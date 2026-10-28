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
import FrameworkTranslator;

namespace FrameworkCommands
{
	bool registerYxCmds()
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

		// 用于检测重复命令的集合（AutoCAD 命令不区分大小写，统一转换为大写进行比对）
		std::set<std::wstring> registeredCommands;

		for (const auto* pSpec : sortedSpecs)
		{
			const auto& spec = *pSpec;

			// 复制一份命令名并转换为大写，用于大小写不敏感的查重
			std::wstring upperName = spec.commandName.constPtr();
			for (auto& c : upperName)
			{
				c = static_cast<wchar_t>(towupper(c));
			}

			// 检查是否重复
			if (registeredCommands.find(upperName) != registeredCommands.end())
			{
				// 构造警告弹窗提示信息
				AcString errMsg;
				errMsg.format(_(L"[FrameworkCommands 警告] 发现重复注册的命令: \"%s\" !\n请检查代码中是否有多次定义该命令。"), spec.commandName.constPtr());

				// 弹出 AutoCAD 原生警告对话框
				acedAlert(errMsg.constPtr());

				return false; // 终止注册
			}

			// 记录已注册的命令
			registeredCommands.insert(upperName);

			CommandInfo info;
			info.commandName = spec.commandName;
			// 触发 Lambda 执行翻译，此时 FrameworkTranslator 已经成功 initialize
			info.commandDescription = spec.getLocalizedDescription ? spec.getLocalizedDescription() : spec.commandName;
			info.flags = spec.flags;
			info.proc = spec.proc;

			commandInfoList.push_back(info);

			// 注册命令全名
			acedRegCmds->addCommand(UtilConstants::cmdGroup, info.commandName.constPtr(), info.commandName.constPtr(), info.flags, info.proc);
		}
		return true;
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