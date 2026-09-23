/**
 * @file      DocCloseInterceptor.cpp
 * @brief     文档关闭拦截模块的实现，通过挂载 Win32 CBT 钩子拦截并处理关闭时的标准对话框消息。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "GenericPairEditDlg.hpp"

module DocCloseInterceptor;
import Translator;
import AcadVarUtil;
import ConfigManager;
import Commands;

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
        if (_wcsicmp(pCmdStr, L"CLOSE") == 0 || _wcsicmp(pCmdStr, L"QUIT") == 0 || _wcsicmp(pCmdStr, L"CLOSEALL") == 0)
        {
            int nDbmod;
            AcadVarUtil::getVar(L"DBMOD", nDbmod);
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

namespace
{
    void cmdClosePrompt()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置跳过仅视图修改时的文件保存提示");
        GenericPairEditDlg dlg(title, _(L"启用(1/0)"), L"提示", false, true, true);

        auto& manager = ConfigManager::getInstance();
        auto& config = manager.getConfig();
        bool bSkipSavePromptOnViewChangesEnabled = config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled;
        CString edit1Result;
        edit1Result.Format(L"%d", bSkipSavePromptOnViewChangesEnabled);
        dlg.modifyEditControl(edit1Result, _(L"启用后。如果图纸仅发生平移和缩放，关闭图纸时不会提示保存文件。"));

        dlg.setValidatorAndParser([&](const CString& value1, const CString& _2) -> CString
            {
                if (value1.IsEmpty())
                {
                    return _(L"必须输入启用状态");
                }
                if (value1.SpanIncluding(L"01") != value1)
                {
                    return _(L"启用状态必须为 0 或 1，1表示启用，0 表示不启用");
                }
                edit1Result = value1;
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        DocCloseInterceptor::getInstance().stop();
        config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled = (edit1Result == L"1");
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled = bSkipSavePromptOnViewChangesEnabled;
        }
        if (config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled)
        {
            DocCloseInterceptor::getInstance().start();
        }
    }

    Commands::AutoRegister ar =
    {
        { L"yxSkipSavePromptOnViewChangesEnabled", []() { return _(L"设置跳过仅视图修改时的文件保存提示"); }, Commands::CommandFlags::Base, cmdClosePrompt },
    };
}