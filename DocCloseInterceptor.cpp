/**
 * @file      DocCloseInterceptor.cpp
 * @brief     文档关闭拦截模块的实现，通过挂载 Win32 CBT 钩子拦截并处理关闭时的标准对话框消息。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module DocCloseInterceptor;
import Translator;

// 初始化静态成员
DocCloseInterceptor* DocCloseInterceptor::spInstance = nullptr;

DocCloseInterceptor& DocCloseInterceptor::getInstance()
{
    static DocCloseInterceptor instance;
    return instance;
}

DocCloseInterceptor::DocCloseInterceptor()
{
    mhCbtHook = nullptr;
    mbIntercepting = false;
    spInstance = this;
}

DocCloseInterceptor::~DocCloseInterceptor()
{
    stop();
    spInstance = nullptr;
}

void DocCloseInterceptor::start()
{
    if (acedEditor != nullptr)
    {
        acedEditor->addReactor(this);
        acutPrintf(_(L"\n启用跳过仅视图修改时的文件保存提示"));
    }
}

void DocCloseInterceptor::stop()
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

void DocCloseInterceptor::commandWillStart(const wchar_t* pCmdStr)
{
    if (pCmdStr != nullptr)
    {
        if (_wcsicmp(pCmdStr, L"CLOSE") == 0 ||
            _wcsicmp(pCmdStr, L"QUIT") == 0 ||
            _wcsicmp(pCmdStr, L"CLOSEALL") == 0)
        {
            resbuf rb = { 0 };
            if (acedGetVar(L"DBMOD", &rb) == RTNORM)
            {
                int nDbmod = rb.resval.rint;

                // 升级安全判断逻辑（包含新图纸默认的1，平移缩放的16，以及两者叠加的17）
                if (nDbmod == 0 || nDbmod == 1 || nDbmod == 16 || nDbmod == 17)
                {
                    if (mhCbtHook == nullptr)
                    {
                        mbIntercepting = true;
                        // 挂载当前线程的 CBT 钩子
                        mhCbtHook = SetWindowsHookEx(WH_CBT, cbtFilterHook, nullptr, GetCurrentThreadId());
                    }
                }
            }
        }
    }
}

void DocCloseInterceptor::commandEnded(const wchar_t* pCmdStr)
{
    if (mhCbtHook != nullptr)
    {
        UnhookWindowsHookEx(mhCbtHook);
        mhCbtHook = nullptr;
        mbIntercepting = false;
    }
}

void DocCloseInterceptor::commandCancelled(const wchar_t* pCmdStr)
{
    if (mhCbtHook != nullptr)
    {
        UnhookWindowsHookEx(mhCbtHook);
        mhCbtHook = nullptr;
        mbIntercepting = false;
    }
}

LRESULT CALLBACK DocCloseInterceptor::cbtFilterHook(int code, WPARAM wParam, LPARAM lParam)
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