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

MiddleClickManager* MiddleClickManager::mpInstance = nullptr;
inline constexpr const wchar_t* kWinStandardDialogClassName = L"#32770";

MiddleClickManager::MiddleClickManager() : mhDialogMiddleClickToOkHook(nullptr)
{
}

MiddleClickManager::~MiddleClickManager()
{
    stop();
}

MiddleClickManager& MiddleClickManager::getInstance()
{
    static MiddleClickManager instance;
    return instance;
}

void MiddleClickManager::start()
{
    if (this->mhDialogMiddleClickToOkHook == nullptr)
    {
        this->mhDialogMiddleClickToOkHook = SetWindowsHookEx(WH_MOUSE_LL, this->dialogMiddleClickToOkProc, GetModuleHandle(nullptr), 0);
        acutPrintf(Common::loadString(IDS_MSG_MiddleClickToOkStarted));
    }
}

void MiddleClickManager::stop()
{
    if (mhDialogMiddleClickToOkHook != nullptr)
    {
        UnhookWindowsHookEx(this->mhDialogMiddleClickToOkHook);
        this->mhDialogMiddleClickToOkHook = nullptr;
        acutPrintf(Common::loadString(IDS_MSG_MiddleClickToOkStopped));
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
                        acutPrintf(Common::loadString(IDS_MSG_MiddleClickToOkDone));
                        // 返回 1 表示拦截此消息，不继续分发（防止 CAD 触发平移操作）
                        return 1;
                    }
                }
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}