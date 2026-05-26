/**
 * @file      MiddleClickManager.cpp
 * @brief     MiddleClickManager 模块的逻辑实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module MiddleClickManager;
import Common;
import Commands;
import Translator;

inline constexpr const wchar_t* kWinStandardDialogClassName = L"#32770";

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

                    if (wcscmp(szClassName, kWinStandardDialogClassName) == 0)
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
                struct resbuf rb = { 0 };
                int cmdActive = 0;

                if (acedGetVar(L"CMDACTIVE", &rb) == RTNORM)
                {
                    cmdActive = rb.resval.rint;
                }

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