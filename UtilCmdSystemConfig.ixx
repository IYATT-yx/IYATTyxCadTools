/**
 * @file      UtilCmdSystemConfig.ixx
 * @brief     配置命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"

export module UtilCmdSystemConfig;
import std;
import FrameworkLifecycleInterface;
import FrameworkCommands;
import FrameworkTranslator;
import FrameworkMiddleClickManager;
import FrameworkConfigManager;
import FrameworkImeAutoSwitcher;
import UiFileDialog;
import UtilCommon;
import FrameworkDocCloseInterceptor;

namespace
{
    void yx()
    {
        FrameworkLifecycleInterface::cmdYx();
    }

    void cmdDialogMiddleClickToOk()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置对话框中鼠标中键映射到确定按钮");
        UiGenericPairEditDlg dlg(title, _(L"启用1/0"), L"", true, true, true);

        CString edit1Result;
        auto& manager = FrameworkConfigManager::getInstance();
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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled = (edit1Result == L"1");
        auto& middleClickManager = FrameworkMiddleClickManager::getInstance();
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
        UiGenericPairEditDlg dlg(title, _(L"启用1/0"), _(L"间隔(ms)"), false, true, true);

        CString edit1Result, edit2Result;
        auto& manager = FrameworkConfigManager::getInstance();
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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled = (edit1Result == L"1");
        FrameworkMiddleClickManager::getInstance().stopUnifiedMiddleClickProc();
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            // 保存失败，还原状态
            config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled = bEnabled;
            config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval = dCmdMiddleClickDownUpInterval;
        }
        FrameworkMiddleClickManager::getInstance().startUnifiedMiddleClickProc(config.middleClickManagerSettings);
    }

    void cmdImeAutoSwitch()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置输入法自动切换");
        UiGenericPairEditDlg dlg(title, _(L"启用1/0"), _(L"启用1/0"), false, true, true);

        CString edit1Result, edit2Result;
        auto& manager = FrameworkConfigManager::getInstance();
        auto& config = manager.getConfig();
        bool bEnabled = config.imeSettings.bEnabled;
        unsigned long iInterval = config.imeSettings.iIntervalMs;
        const ConfigItems::ImeSettings defaultConfig;
        edit1Result.Format(L"%d", config.imeSettings.bEnabled);
        edit2Result.Format(L"%d", config.imeSettings.iIntervalMs);
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
                    config.imeSettings.iIntervalMs = std::stoi(value2.GetString(), &pos);
                    if (pos != value2.GetLength())
                    {
                        throw std::exception();
                    }
                    if (config.imeSettings.iIntervalMs < defaultConfig.iIntervalMs)
                    {
                        throw std::exception();
                    }
                }
                catch (...)
                {
                    CString csInvalidInterval;
                    csInvalidInterval.Format(_(L"切换间隔必须为不小于 %d 的整数"), defaultConfig.iIntervalMs);
                    return csInvalidInterval;
                }

                edit1Result = value1;
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.imeSettings.bEnabled = (edit1Result == L"1");
        FrameworkImeAutoSwitcher::stop();
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            // 保存失败，还原状态
            config.imeSettings.bEnabled = bEnabled;
            config.imeSettings.iIntervalMs = iInterval;
        }
        if (config.imeSettings.bEnabled)
        {
            FrameworkImeAutoSwitcher::start(config.imeSettings.iIntervalMs);
        }
    }

    void cmdLocateDrawing()
    {
        CString drawingPath = UtilCommon::getCurrPath();
        if (drawingPath.IsEmpty())
        {
            AfxMessageBox(_(L"图纸未保存"), MB_OK | MB_ICONERROR);
            return;
        }

        UiFileDialog::locateFileInExplorer(drawingPath);
    }

    void cmdLocateSelf()
    {
        const wchar_t* appName = acedGetAppName();
        UiFileDialog::locateFileInExplorer(appName);
    }

    void cmdOpenConfigFile()
    {
        auto& manager = FrameworkConfigManager::getInstance();
        std::wstring configFilename = manager.getConfigFilename();
        UiFileDialog::locateFileInExplorer(configFilename.c_str());
    }

    void cmdSetLanguage()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置语言");
        UiGenericPairEditDlg dlg(title, _(L"语言代码"), L"提示", false, true, true);

        auto& manager = FrameworkConfigManager::getInstance();
        auto& config = manager.getConfig();
        std::wstring languageCode = config.languageSettings.languageCode;
        dlg.modifyEditControl(languageCode.c_str(), _(L"无匹配语言代码的翻译文件时，默认显示中文"));

        dlg.setValidatorAndParser([&](const CString& value1, const CString& _2) -> CString
            {
                if (value1.IsEmpty())
                {
                    return _(L"必须输入语言代码");
                }
                config.languageSettings.languageCode = value1.GetString();
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            config.languageSettings.languageCode = languageCode;
        }
        AfxMessageBox(_(L"重启插件刷新语言设置"), MB_OK | MB_ICONINFORMATION);
    }

    void cmdClosePrompt()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置跳过仅视图修改时的文件保存提示");
        UiGenericPairEditDlg dlg(title, _(L"启用(1/0)"), L"提示", false, true, true);

        auto& manager = FrameworkConfigManager::getInstance();
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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        FrameworkDocCloseInterceptor::getInstance().stop();
        config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled = (edit1Result == L"1");
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled = bSkipSavePromptOnViewChangesEnabled;
        }
        if (config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled)
        {
            FrameworkDocCloseInterceptor::getInstance().start();
        }
    }

    FrameworkCommands::AutoRegister ar =
    {
        { L"yx", []() { return _(L"显示或隐藏命令菜单"); }, FrameworkCommands::CommandFlags::Base, yx },
        { L"yxDialogMiddleClickToOk", []() { return _(L"设置对话框中鼠标中键映射到确定按钮"); }, FrameworkCommands::CommandFlags::Base, cmdDialogMiddleClickToOk },
        { L"yxCmdMiddleClickToEnter", []() { return _(L"设置命令执行状态下鼠标中键映射回车键"); }, FrameworkCommands::CommandFlags::Base, cmdCmdMiddleClickToEnter },
        { L"yxImeAutoSwitch", []() { return _(L"设置输入法自动切换"); }, FrameworkCommands::CommandFlags::Base, cmdImeAutoSwitch },
        { L"yxLocateDrawing", []() { return _(L"打开图纸路径"); }, FrameworkCommands::CommandFlags::Base, cmdLocateDrawing },
        { L"yxLocateSelf", []() { return _(L"打开本工具路径"); }, FrameworkCommands::CommandFlags::Base, cmdLocateSelf },
        { L"yxOpenConfigFile", []() { return _(L"打开配置路径"); }, FrameworkCommands::CommandFlags::Base, cmdOpenConfigFile },
        { L"yxSetLanguage", []() { return _(L"设置语言"); }, FrameworkCommands::CommandFlags::Base, cmdSetLanguage },
        { L"yxSkipSavePromptOnViewChangesEnabled", []() { return _(L"设置跳过仅视图修改时的文件保存提示"); }, FrameworkCommands::CommandFlags::Base, cmdClosePrompt },
    };
}