/**
 * @file      ImeAutoSwitcher.ixx
 * @brief     ImeAutoSwitcher 模块的接口定义。
 * @details   定义了用于 AutoCAD 环境下自动切换输入法语言的配置常量、启动与停止接口。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module ImeAutoSwitcher;

export namespace ImeAutoSwitcher
{
    /**
     * @brief 启动输入法自动切换
     * @param intervlMs 语言切换间隔时间，单位毫秒。
     */
    void start(DWORD intervlMs = 200);

    /**
     * @brief 停止输入法自动切换
     */
    void stop();
}

namespace ImeAutoSwitcher
{
    /**
     * @brief 键盘钩子回调函数
     * @param nCode 消息码
     * @param wParam 消息参数
     * @param lParam 消息参数
     * @return 
     */
    LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    /**
     * @brief 使用系统变量判断 CAD 是否处于空闲状态
     */
    bool isCommandRunning();

    /**
     * @brief 执行强制切换逻辑
     * @param hWnd 窗口句柄
     */
    void ForceEnglishMode(HWND hWnd);
};