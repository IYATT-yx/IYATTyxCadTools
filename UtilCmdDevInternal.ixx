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
import UtilConstants;
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
    
    void cmdExportCommandTreeToClipboard()
    {
        // 遍历收集命令，构建树状文本
        // key: 命令组名 (对应插件或模块标识), value: 该组下的所有命令列表
        std::map<std::wstring, std::vector<std::wstring>> groupMap;

        // 获取全局命令栈迭代器
        AcEdCommandIterator* iter = acedRegCmds->iterator();
        for (; iter && !iter->done(); iter->next())
        {
            const AcEdCommand* pCmd = iter->command();
            if (pCmd != nullptr)
            {
                const wchar_t* globalNamePtr = pCmd->globalName();
                const wchar_t* groupNamePtr = iter->commandGroup(); // 获取命令所属的组（插件标识）

                std::wstring gName = (groupNamePtr != nullptr) ? groupNamePtr : L"UNKNOWN_GROUP";
                std::wstring cName = (globalNamePtr != nullptr) ? globalNamePtr : L"UNKNOWN_CMD";

                groupMap[gName].push_back(cName);
            }
        }
        delete iter;

        // 使用内存字符串流组装清晰的树状结构文本
        std::wstringstream wss;
        wss << L"========================================\r\n";
        wss << _(L"     AutoCAD 已加载插件与命令树状图     \r\n");
        wss << L"========================================\r\n\r\n";

        wss << _(L"总计命令组数: ") << groupMap.size() << L"\r\n\r\n";

        size_t groupCount = groupMap.size();
        size_t groupIndex = 0;

        for (const auto& pair : groupMap)
        {
            groupIndex++;
            bool isLastGroup = (groupIndex == groupCount);

            std::wstring groupPrefix = isLastGroup ? L"└── " : L"├── ";

            wss << groupPrefix << _(L"[命令组/模块] ") << pair.first
                << _(L" (包含命令数: ") << pair.second.size() << L")\r\n";

            const auto& cmdList = pair.second;
            for (size_t i = 0; i < cmdList.size(); ++i)
            {
                bool isLastCmd = (i == cmdList.size() - 1);
                std::wstring cmdBranchPrefix = isLastGroup ? L"    " : L"│   ";
                std::wstring cmdPrefix = isLastCmd ? L"└── " : L"├── ";

                wss << cmdBranchPrefix << cmdPrefix << _(L"命令: ") << cmdList[i] << L"\r\n";
            }
        }

        std::wstring finalString = wss.str();

        // 将生成的文本直接写入 Windows 剪切板
        if (OpenClipboard(nullptr))
        {
            EmptyClipboard();

            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (finalString.length() + 1) * sizeof(wchar_t));
            if (hMem != nullptr)
            {
                void* pMem = GlobalLock(hMem);
                if (pMem != nullptr)
                {
                    memcpy(pMem, finalString.c_str(), (finalString.length() + 1) * sizeof(wchar_t));
                    GlobalUnlock(hMem);
                    SetClipboardData(CF_UNICODETEXT, hMem);
                }
            }
            CloseClipboard();

            acutPrintf(_(L"\n命令树状图已成功复制到剪切板！请直接按 Ctrl+V 粘贴查看。"));
        }
        else
        {
            acutPrintf(_(L"\n错误: 无法打开系统剪切板！"));
        }
    }

    FrameworkCommands::AutoRegister ar =
    {
        { L"yxClass", []() { return _(L"打印类层次结构"); }, FrameworkCommands::CommandFlags::Base, cmdPrintClassHierarchy },
        { L"yxRestart", []() { return _(L"重启本插件"); }, FrameworkCommands::CommandFlags::Base, cmdRestartApp },
        { L"yxUnload", []() { return _(L"关闭本插件"); }, FrameworkCommands::CommandFlags::Base, cmdUnloadApp },
        { L"yxTest", []() { return _(L"测试"); }, FrameworkCommands::CommandFlags::Base, test },
        { L"yxTree", []() { return _(L"导出已注册命令树状图到剪贴板"); } , FrameworkCommands::CommandFlags::Base, cmdExportCommandTreeToClipboard},
    };
}