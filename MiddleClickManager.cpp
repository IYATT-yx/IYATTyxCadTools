/**
 * @file      MiddleClickManager.cpp
 * @brief     MiddleClickManager 模块的逻辑实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "resource.h"

module MiddleClickManager;
import Common;
import Commands;

MiddleClickManager* MiddleClickManager::mpInstance = nullptr;
inline constexpr const wchar_t* kWinStandardDialogClassName = L"#32770";

MiddleClickManager::MiddleClickManager() : mhDialogMiddleClickToOkHook(nullptr)
{
}

MiddleClickManager::~MiddleClickManager()
{
    this->stopDialogMiddleClickToOk();
    this->stopCmdMiddleClickToEnter();
}

MiddleClickManager& MiddleClickManager::getInstance()
{
    static MiddleClickManager instance;
    return instance;
}

void MiddleClickManager::startDialogMiddleClickToOk()
{
    if (this->mhDialogMiddleClickToOkHook == nullptr)
    {
        this->mhDialogMiddleClickToOkHook = SetWindowsHookEx(WH_MOUSE_LL, this->dialogMiddleClickToOkProc, GetModuleHandle(nullptr), 0);
        acutPrintf(Common::loadString(IDS_MSG_DialogMiddleClickToOkStarted));
    }
}

void MiddleClickManager::stopDialogMiddleClickToOk()
{
    if (this->mhDialogMiddleClickToOkHook != nullptr)
    {
        UnhookWindowsHookEx(this->mhDialogMiddleClickToOkHook);
        this->mhDialogMiddleClickToOkHook = nullptr;
        acutPrintf(Common::loadString(IDS_MSG_DialogMiddleClickToOkStopped));
    }
}

void MiddleClickManager::startCmdMiddleClickToEnter(DWORD dCmdMiddleClickDownUpInterval)
{
    if (this->mhCmdMiddleClickToEnterHook == nullptr)
    {
        this->mhCmdMiddleClickToEnterHook = SetWindowsHookEx(WH_MOUSE_LL, this->cmdMiddleClickToEnterProc, GetModuleHandle(nullptr), 0);
        this->mdCmdMiddleClickDownUpInterval = dCmdMiddleClickDownUpInterval;
        acutPrintf(Common::loadString(IDS_MSG_CmdMiddleClickToEnterStarted_FMT), dCmdMiddleClickDownUpInterval);
    }
}

void MiddleClickManager::stopCmdMiddleClickToEnter()
{
    if (this->mhCmdMiddleClickToEnterHook != nullptr)
    {
        UnhookWindowsHookEx(this->mhCmdMiddleClickToEnterHook);
        this->mhCmdMiddleClickToEnterHook = nullptr;
        acutPrintf(Common::loadString(IDS_MSG_CmdMiddleClickToEnterStopped));
    }
}

bool MiddleClickManager::isDialogMiddleClickToOkRunning() const
{
    if (this->mhDialogMiddleClickToOkHook != nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}

LRESULT CALLBACK MiddleClickManager::dialogMiddleClickToOkProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_MBUTTONDOWN)
        {
            // 探测当前前台窗口
            HWND hWndActive = GetForegroundWindow();

            if (hWndActive != nullptr)
            {
                wchar_t szClassName[256] = { 0 };
                GetClassName(hWndActive, szClassName, 256);

                // 判断是否为标准对话框类名
                if (wcscmp(szClassName, kWinStandardDialogClassName) == 0)
                {
                    // 尝试获取该对话框的确认按钮 (IDOK = 1)
                    HWND hOkButton = GetDlgItem(hWndActive, IDOK);

                    if (hOkButton != nullptr)
                    {
                        // 投递点击消息给对话框
                        PostMessage(hWndActive, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hOkButton);
                        CAcModuleResourceOverride resOverride;
                        acutPrintf(Common::loadString(IDS_MSG_DialogMiddleClickToOkDone));
                        // 返回 1 表示拦截此消息，不继续分发（防止 CAD 触发平移操作）
                        return 1;
                    }
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK MiddleClickManager::cmdMiddleClickToEnterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        static DWORD dwMouseDownTime = 0;
        if (wParam == WM_MBUTTONDOWN)
        {
            dwMouseDownTime = GetTickCount();
        }

        if (wParam == WM_MBUTTONUP)
        {
            DWORD dwDuration = GetTickCount() - dwMouseDownTime;

            struct resbuf rb = { 0 };
            int cmdActive = 0;

            if (acedGetVar(L"CMDACTIVE", &rb) == RTNORM)
            {
                cmdActive = rb.resval.rint;
            }

            if (cmdActive > 0 && dwDuration < MiddleClickManager::getInstance().mdCmdMiddleClickDownUpInterval)
            {
                // 发送一个空命令（回车）
                static Commands::CommandList space = { L"" };
                Commands::executeCommand(space, false);
                CAcModuleResourceOverride resOverride;
                acutPrintf(Common::loadString(IDS_MSG_CmdMiddleClickToEnterDone));
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}