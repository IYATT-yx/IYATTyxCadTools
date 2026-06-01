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

                bool bIsSafeToIntercept = false;

                // 类别 1：完全没有修改对象数据库（最低位为 0）
                // 完美涵盖：0(绝对干净)、16(纯视野移动)、8(纯窗口操作)、12(变量+窗口) 等
                if ((nDbmod & 1) == 0)
                {
                    bIsSafeToIntercept = true;
                }
                // 类别 2：虽然触发了位 1，但符合纯打印(37)、视野打印(53)或模板初始化(33/49)的特征
                // 特征：包含了位 32 (Field modified)，且没有发生常规的非打印变量污染
                else
                {
                    if (nDbmod == 33 || nDbmod == 37 || nDbmod == 49 || nDbmod == 53)
                    {
                        bIsSafeToIntercept = true;
                    }
                }

                // 只有判定为安全的非图形修改状态，才挂钩拦截器
                if (bIsSafeToIntercept)
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