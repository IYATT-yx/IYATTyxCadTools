/**
 * @file      FrameworkDocCloseInterceptor.cpp
 * @brief     文档关闭拦截模块的实现，通过挂载 Win32 CBT 钩子拦截并处理关闭时的标准对话框消息。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"

module FrameworkDocCloseInterceptor;
import FrameworkTranslator;
import UtilAcadVar;
import FrameworkConfigManager;
import FrameworkCommands;

// 初始化静态成员
FrameworkDocCloseInterceptor* FrameworkDocCloseInterceptor::spInstance = nullptr;

FrameworkDocCloseInterceptor& FrameworkDocCloseInterceptor::getInstance()
{
    static FrameworkDocCloseInterceptor instance;
    return instance;
}

FrameworkDocCloseInterceptor::FrameworkDocCloseInterceptor()
{
    mhCbtHook = nullptr;
    mbIntercepting = false;
    spInstance = this;
}

FrameworkDocCloseInterceptor::~FrameworkDocCloseInterceptor()
{
    stop();
    spInstance = nullptr;
}

void FrameworkDocCloseInterceptor::start()
{
    if (acedEditor != nullptr)
    {
        acedEditor->addReactor(this);
        acutPrintf(_(L"\n启用跳过仅视图修改时的文件保存提示"));
    }
}

void FrameworkDocCloseInterceptor::stop()
{
    if (acedEditor != nullptr)
    {
        acedEditor->removeReactor(this);
    }

    if (mhCbtHook != nullptr)
    {
        UnhookWindowsHookEx(mhCbtHook);
        mhCbtHook = nullptr;
    }
    mbIntercepting = false;
    acutPrintf(_(L"\n停用跳过仅视图修改时的文件保存提示"));
}

void FrameworkDocCloseInterceptor::commandWillStart(const wchar_t* pCmdStr)
{
    if (pCmdStr != nullptr)
    {
        if (_wcsicmp(pCmdStr, L"CLOSE") == 0 || _wcsicmp(pCmdStr, L"QUIT") == 0 || _wcsicmp(pCmdStr, L"CLOSEALL") == 0)
        {
            int nDbmod;
            UtilAcadVar::getVar(L"DBMOD", nDbmod);
            if (nDbmod == 16)
            {
                if (mhCbtHook == nullptr)
                {
                    mhCbtHook = SetWindowsHookEx(WH_CBT, cbtFilterHook, nullptr, GetCurrentThreadId());
                }
            }
        }
    }
}

void FrameworkDocCloseInterceptor::commandEnded(const wchar_t* pCmdStr)
{
    if (mhCbtHook != nullptr)
    {
        UnhookWindowsHookEx(mhCbtHook);
        mhCbtHook = nullptr;
        mbIntercepting = false;
    }
}

void FrameworkDocCloseInterceptor::commandCancelled(const wchar_t* pCmdStr)
{
    if (mhCbtHook != nullptr)
    {
        UnhookWindowsHookEx(mhCbtHook);
        mhCbtHook = nullptr;
        mbIntercepting = false;
    }
}

LRESULT CALLBACK FrameworkDocCloseInterceptor::cbtFilterHook(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HCBT_ACTIVATE)
    {
        HWND hWnd = reinterpret_cast<HWND>(wParam);
        wchar_t szClassName[256] = { 0 };

        if (GetClassName(hWnd, szClassName, 256) > 0)
        {
            if (_wcsicmp(szClassName, L"#32770") == 0)
            {
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDNO, BN_CLICKED), 0);
                return 0;
            }
        }
    }

    HHOOK hHook = (spInstance != nullptr) ? spInstance->mhCbtHook : nullptr;
    return CallNextHookEx(hHook, code, wParam, lParam);
}
