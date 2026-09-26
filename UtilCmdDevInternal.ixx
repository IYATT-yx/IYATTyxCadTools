/**
 * @file      UtilCmdDevInternal.ixx
 * @brief     开发工具命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilCmdDevInternal;
import std;
import UiUniversalPicker;
import FrameworkCommands;
import UtilEntity;
import FrameworkTranslator;
import FrameworkLifecycleInterface;

import FrameworkActivityInsightsManager;

namespace
{
    void cmdPrintClassHierarchy()
    {
        UiUniversalPicker::run(nullptr, UtilEntity::printClassHierarchy, _(L"打印类层次结构"), UiUniversalPicker::SelectMode::Immediate, true);
    }

    void cmdUnloadApp()
    {
        const wchar_t* appName = acedGetAppName();
        FrameworkCommands::CommandList pszCmdList =
        {
            L"ARX",
            L"U",
            appName
        };
        FrameworkCommands::executeCommand(pszCmdList);
    }

    void cmdRestartApp()
    {
        std::wstring lispPath = std::filesystem::path(acedGetAppName()).generic_wstring();
        FrameworkLifecycleInterface::unload();
        std::wstring lispCmd = L"(progn (arxunload \"" + lispPath + L"\" nil) (arxload \"" + lispPath + L"\")(princ))";
        FrameworkCommands::CommandList pszCmdList =
        {
            lispCmd.c_str()
        };
        FrameworkCommands::executeCommand(pszCmdList, false);
    }

    void test()
    {
        FrameworkActivityInsightsManager manager;
        std::vector<CadHistory::ActivityItem> vecRawHistory = manager.fetchHistory(CadHistory::OpFilter::OpAll);

        if (vecRawHistory.empty())
        {
            acutPrintf(L"\n[IYATT-yx] 未检测到任何有效的文件活动日志。\n");
            return;
        }

        // 同路径去重处理
        std::vector<CadHistory::ActivityItem> vecUniqueHistory;
        std::set<std::wstring> setProcessedPaths;

        for (const auto& item : vecRawHistory)
        {
            std::wstring wstrUpperPath = item.wstrDstPath;
            std::transform(wstrUpperPath.begin(), wstrUpperPath.end(), wstrUpperPath.begin(), ::towupper);

            if (setProcessedPaths.find(wstrUpperPath) == setProcessedPaths.end())
            {
                vecUniqueHistory.push_back(item);
                setProcessedPaths.insert(wstrUpperPath);
            }
        }

        acutPrintf(L"\n[IYATT-yx] ===== 开始打印图纸活动日志 =====");

        for (const auto& item : vecUniqueHistory)
        {
            // 直接调用结构体封装的方法获取时间字符串！干净纯粹！
            std::wstring wstrDisplayTime = item.toTimeString();

            acutPrintf(
                L"\n[%s] 动作: %-12s | 产品: %-25s \n  路径: %s",
                wstrDisplayTime.c_str(),
                item.wstrOp.c_str(),
                item.wstrProduct.c_str(),
                item.wstrDstPath.c_str()
            );
        }

        acutPrintf(L"\n[IYATT-yx] ===== 图纸日志打印结束 (共 %d 项) =====\n", vecUniqueHistory.size());
    }

    FrameworkCommands::AutoRegister ar =
    {
        { L"yxPrintClassHierarchy", []() { return _(L"打印类层次结构"); }, FrameworkCommands::CommandFlags::Base, cmdPrintClassHierarchy },
        { L"yxRestart", []() { return _(L"重启本插件"); }, FrameworkCommands::CommandFlags::Base, cmdRestartApp },
        { L"yxUnload", []() { return _(L"关闭本插件"); }, FrameworkCommands::CommandFlags::Base, cmdUnloadApp },
        { L"yxTest", []() { return _(L"测试"); }, FrameworkCommands::CommandFlags::Base, test },
    };
}