/**
 * @file      MiddleClickManager.cpp
 * @brief     MiddleClickManager 模块的逻辑实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "GenericPairEditDlg.hpp"

module MiddleClickManager;
import Common;
import Commands;
import Translator;
import AcadVarUtil;
import Translator;

inline constexpr const wchar_t* kWinStandardDialogClassName = L"#32770"; // 标准对话框类名
inline constexpr const wchar_t* kAcadDialogClassName = L"adesk_dlg0000"; // AutoCAD 对话框类名

MiddleClickManager::MiddleClickManager() : mhUnifiedMiddleClickHook(nullptr)
{
}

MiddleClickManager::~MiddleClickManager()
{
    this->stopUnifiedMiddleClickProc();
}

MiddleClickManager& MiddleClickManager::getInstance()
{
    static MiddleClickManager instance;
    return instance;
}

void MiddleClickManager::startUnifiedMiddleClickProc(ConfigItems::MiddleClickManagerSettings& settings)
{
    if (this->mhUnifiedMiddleClickHook == nullptr)
    {
        this->mbEnabledDialogOk = settings.bDialogMiddleClickToOkEnabled;
        this->mbEnabledCmdEnter = settings.bCmdMiddleClickToEnterEnabled;
        this->mdCmdMiddleClickDownUpInterval = settings.dCmdMiddleClickDownUpInterval;
        HMODULE hCurrentModule = nullptr;
        const wchar_t* pszAppPath = ::acedGetAppName();

        if (pszAppPath != nullptr)
        {
            {
                std::filesystem::path fullPath(pszAppPath);
                std::wstring moduleName = fullPath.filename().wstring();
                hCurrentModule = ::GetModuleHandleW(moduleName.c_str());
            }
        }
        this->mhUnifiedMiddleClickHook = SetWindowsHookEx(WH_MOUSE_LL, this->unifiedMiddleClickProc, hCurrentModule, 0);
        if (this->mbEnabledDialogOk)
        {
            acutPrintf(_(L"\n已启用对话框中鼠标中键映射确定按钮\n"));
        }
        if (this->mbEnabledCmdEnter)
        {
            acutPrintf(_(L"\n已启用命令中鼠标中键映射回车键，按下释放间隔阈值：%dms\n"), this->mdCmdMiddleClickDownUpInterval);
        }
    }
}

void MiddleClickManager::stopUnifiedMiddleClickProc()
{
    if (this->mhUnifiedMiddleClickHook != nullptr)
    {
        UnhookWindowsHookEx(this->mhUnifiedMiddleClickHook);
        this->mhUnifiedMiddleClickHook = nullptr;
        acutPrintf(_(L"\n已停止对话框中鼠标中键映射确定按钮\n"));
        acutPrintf(_(L"\n已停用命令中鼠标中键映射回车键\n"));
    }
}

LRESULT CALLBACK MiddleClickManager::unifiedMiddleClickProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        auto& instance = MiddleClickManager::getInstance();
        static bool bWasDialogHandled = false;
        static DWORD dwMouseDownTime = 0;

        if (wParam == WM_MBUTTONDOWN)
        {
            bWasDialogHandled = false;
            dwMouseDownTime = GetTickCount();

            if (instance.mbEnabledDialogOk)
            {
                HWND hWndActive = GetForegroundWindow();
                if (hWndActive != nullptr)
                {
                    wchar_t szClassName[256] = { 0 };
                    GetClassName(hWndActive, szClassName, 256);

                    if (wcscmp(szClassName, kWinStandardDialogClassName) == 0 || wcscmp(szClassName, kAcadDialogClassName) == 0)
                    {
                        HWND hOkButton = GetDlgItem(hWndActive, IDOK);
                        if (hOkButton != nullptr)
                        {
                            PostMessage(hWndActive, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hOkButton);
                            bWasDialogHandled = true; // 记录本次点击已分配给对话框
                            return 1; // 拦截按下消息
                        }
                    }
                }
            }
        }

        if (wParam == WM_MBUTTONUP)
        {
            // 如果按下时已被对话框逻辑消耗，抬起时必须拦截，防止污染命令行逻辑
            if (bWasDialogHandled)
            {
                bWasDialogHandled = false;
                return 1;
            }

            if (instance.mbEnabledCmdEnter)
            {
                DWORD dwDuration = GetTickCount() - dwMouseDownTime;
                int cmdActive = 0;

                AcadVarUtil::getVar(L"CMDACTIVE", cmdActive);
                if (cmdActive > 0 && dwDuration < instance.mdCmdMiddleClickDownUpInterval)
                {
                    static Commands::CommandList space = { L"" };
                    Commands::executeCommand(space, false);

                    CAcModuleResourceOverride resOverride;
                    acutPrintf(_(L"\n检测到命令状态鼠标中键点击，已触发回车按键\n"));
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

namespace
{
    void cmdDialogMiddleClickToOk()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置对话框中鼠标中键映射到确定按钮");
        GenericPairEditDlg dlg(title, _(L"启用1/0"), L"", true, true, true);

        CString edit1Result;
        auto& manager = ConfigManager::getInstance();
        auto& config = manager.getConfig();
        bool bDialogMiddleClickToOkEnabled = config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled;
        edit1Result.Format(L"%d", config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled);
        dlg.modifyEditControl(edit1Result);

        dlg.setValidatorAndParser([&](const CString& value1, const CString& _2) -> CString
            {
                if (value1.IsEmpty())
                {
                    return _(L"必须输入1或0设置是否启用中键映射确定按钮");
                }
                if (value1.SpanIncluding(L"01") != value1)
                {
                    return _(L"必须输入1或0设置是否启用中键映射确定按钮");
                }
                edit1Result = value1;
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled = (edit1Result == L"1");
        auto& middleClickManager = MiddleClickManager::getInstance();
        middleClickManager.stopUnifiedMiddleClickProc();
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            // 保存失败，还原状态
            config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled = bDialogMiddleClickToOkEnabled;
        }
        middleClickManager.startUnifiedMiddleClickProc(config.middleClickManagerSettings);
    }

    void cmdCmdMiddleClickToEnter()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置命令执行状态下鼠标中键映射回车键");
        GenericPairEditDlg dlg(title, _(L"启用1/0"), _(L"间隔(ms)"), false, true, true);

        CString edit1Result, edit2Result;
        auto& manager = ConfigManager::getInstance();
        auto& config = manager.getConfig();
        bool bEnabled = config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled;
        unsigned long dCmdMiddleClickDownUpInterval = config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval;
        edit1Result.Format(L"%d", bEnabled);
        edit2Result.Format(L"%d", dCmdMiddleClickDownUpInterval);
        const ConfigItems::MiddleClickManagerSettings defaultConfig;
        dlg.modifyEditControl(edit1Result, edit2Result);

        dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
            {
                if (value1.IsEmpty() || value2.IsEmpty())
                {
                    return _(L"必须输入自启动状态和切换间隔时间");
                }
                if (value1.SpanIncluding(L"01") != value1)
                {
                    return _(L"自启动状态必须为 0 或 1，1表示自启动，0 表示不自启动");
                }
                try
                {
                    size_t pos;
                    config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval = std::stoi(value2.GetString(), &pos);
                    if (pos != value2.GetLength())
                    {
                        throw std::exception();
                    }
                    if (config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval < defaultConfig.dCmdMiddleClickDownUpInterval)
                    {
                        throw std::exception();
                    }
                }
                catch (...)
                {
                    CString csInvalidInterval;
                    csInvalidInterval.Format(_(L"切换间隔必须为不小于 %d 的整数"), defaultConfig.dCmdMiddleClickDownUpInterval);
                    return csInvalidInterval;
                }

                edit1Result = value1;
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled = (edit1Result == L"1");
        MiddleClickManager::getInstance().stopUnifiedMiddleClickProc();
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            // 保存失败，还原状态
            config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled = bEnabled;
            config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval = dCmdMiddleClickDownUpInterval;
        }
        MiddleClickManager::getInstance().startUnifiedMiddleClickProc(config.middleClickManagerSettings);
    }

    Commands::AutoRegister ar =
    {
        { L"yxDialogMiddleClickToOk", []() { return _(L"设置对话框中鼠标中键映射到确定按钮"); }, Commands::CommandFlags::Base, cmdDialogMiddleClickToOk },
        { L"yxCmdMiddleClickToEnter", []() { return _(L"设置命令执行状态下鼠标中键映射回车键"); }, Commands::CommandFlags::Base, cmdCmdMiddleClickToEnter },
    };
}