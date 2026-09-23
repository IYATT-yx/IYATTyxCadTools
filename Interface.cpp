/**
 * @file      Interface.cpp
 * @brief     接口模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "GenericPairEditDlg.hpp"
#include "MainBar.hpp"

module Interface;
import BuildingTime;
import Common;
import ConfigManager;
import DocCloseInterceptor;
import Translator;
import ImeAutoSwitcher;
import MiddleClickManager;

void Interface::init()
{
    // 读取配置文件
    auto& manager = ConfigManager::getInstance();
    auto appPath = Common::getAppSubFolder();
    if (!appPath.has_value())
    {
        AfxMessageBox(L"Failed to get application data directory", MB_OK | MB_ICONERROR);
        return;
    }
    std::filesystem::path configPathObj = appPath.value() / Common::Config::configName;
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
    auto& translator = Translator::getInstance();
    if (!translator.initialize(localesDir, config.languageSettings.languageCode))
    {
        AfxMessageBox(L"Initialize translator failed", MB_OK | MB_ICONERROR);
        return;
    }

    Interface::info();

    // 注册命令
    Commands::registerYxCmds();

    // 输入法语言自动切换
    if (config.imeSettings.bEnabled)
    {
        ImeAutoSwitcher::start(config.imeSettings.iIntervalMs);
    }

    // 中键处理
    MiddleClickManager::getInstance().startUnifiedMiddleClickProc(config.middleClickManagerSettings);

    // 文件关闭提示拦截
    if (config.closePromptSettings.bSkipSavePromptOnViewChangesEnabled)
    {
        DocCloseInterceptor::getInstance().start();
    }

    // 显示命令报表悬浮窗
    MainBar::showBar(Commands::commandInfoList);
}

void Interface::info()
{
    
    acutPrintf(L"\n%s %s_%s | %s: IYATT-yx | %s: MIT | %s: https://github.com/IYATT-yx/IYATTyxCadTools\n",
        Common::getLocalProjectName(),
        BuildingTime::WDATE, BuildingTime::WTIME,
        _(L"作者"),
        _(L"开源协议"),
        _(L"项目地址")
        );
}

void Interface::cmdYx()
{
    Interface::info();
    MainBar::showBar(Commands::commandInfoList);
}

void Interface::unload()
{
    // 关闭输入法自动切换
    ImeAutoSwitcher::stop();
    // 关闭中键处理
    MiddleClickManager::getInstance().stopUnifiedMiddleClickProc();
    // 停止文件关闭提示拦截
    DocCloseInterceptor::getInstance().stop();
    // 关闭命令菜单
    MainBar::terminateBar();
    // 卸载命令
    acedRegCmds->removeGroup(Common::cmdGroup);
    acutPrintf(_(L"\n已卸载 %s"), Common::getLocalProjectName());
}
