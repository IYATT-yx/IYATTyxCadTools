/**
 * @file      FrameworkLifecycleInterface.cpp
 * @brief     接口模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"
#include "UiMainBar.hpp"

module FrameworkLifecycleInterface;
import FrameworkBuildingTime;
import UtilConstants;
import FrameworkConfigManager;
import FrameworkDocCloseInterceptor;
import FrameworkTranslator;
import FrameworkImeAutoSwitcher;
import FrameworkMiddleClickManager;
import UtilSystem;

void FrameworkLifecycleInterface::init()
{
    // 读取配置文件
    auto& manager = FrameworkConfigManager::getInstance();
    auto appPath = UtilSystem::getAppSubFolder();
    if (!appPath.has_value())
    {
        AfxMessageBox(L"Failed to get application data directory", MB_OK | MB_ICONERROR);
        return;
    }
    std::filesystem::path configPathObj = appPath.value() / UtilConstants::Config::configName;
    if (!manager.loadConfig(configPathObj.wstring()))
    {
        std::wstring err = manager.getLastError();
        AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
    }
    auto& config = manager.getConfig();

    // 初始化翻译器
    std::filesystem::path appName(acedGetAppName());
    std::filesystem::path appDir = appName.parent_path();
    std::filesystem::path localesDir = appDir / L"locales";
    auto& translator = FrameworkTranslator::getInstance();
    if (!translator.initialize(localesDir, config.languageSettings.languageCode))
    {
        AfxMessageBox(L"Initialize translator failed", MB_OK | MB_ICONERROR);
        return;
    }

    FrameworkLifecycleInterface::info();

    // 注册命令
    FrameworkCommands::registerYxCmds();

    // 输入法语言自动切换
    if (config.imeSettings.bEnabled)
    {
        FrameworkImeAutoSwitcher::start(config.imeSettings.iIntervalMs);
    }

    // 中键处理
    FrameworkMiddleClickManager::getInstance().startUnifiedMiddleClickProc(config.middleClickManagerSettings);

    // 文件关闭提示拦截
    if (config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled)
    {
        FrameworkDocCloseInterceptor::getInstance().start();
    }

    // 显示命令报表悬浮窗
    UiMainBar::showBar(FrameworkCommands::commandInfoList);
}

void FrameworkLifecycleInterface::info()
{
    
    acutPrintf(L"\n%s %s_%s | %s: IYATT-yx | %s: MIT | %s: https://github.com/IYATT-yx/IYATTyxCadTools\n",
        UtilConstants::getLocalProjectName(),
        FrameworkBuildingTime::WDATE, FrameworkBuildingTime::WTIME,
        _(L"作者"),
        _(L"开源协议"),
        _(L"项目地址")
        );
}

void FrameworkLifecycleInterface::cmdYx()
{
    FrameworkLifecycleInterface::info();
    UiMainBar::showBar(FrameworkCommands::commandInfoList);
}

void FrameworkLifecycleInterface::unload()
{
    // 关闭输入法自动切换
    FrameworkImeAutoSwitcher::stop();
    // 关闭中键处理
    FrameworkMiddleClickManager::getInstance().stopUnifiedMiddleClickProc();
    // 停止文件关闭提示拦截
    FrameworkDocCloseInterceptor::getInstance().stop();
    // 关闭命令菜单
    UiMainBar::terminateBar();
    // 卸载命令
    acedRegCmds->removeGroup(UtilConstants::cmdGroup);
    acutPrintf(_(L"\n已卸载 %s"), UtilConstants::getLocalProjectName());
}
