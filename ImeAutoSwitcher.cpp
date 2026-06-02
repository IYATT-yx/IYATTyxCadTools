/**
 * @file      ImeAutoSwitcher.cpp
 * @brief     ImeAutoSwitcher 模块的具体逻辑实现。
 * @details   通过设置全局低级键盘钩子 (WH_KEYBOARD_LL) 监控按键，并结合 AutoCAD 的 CMDACTIVE
 *            系统变量，实现当 CAD 处于空闲状态且按下字母键时，自动强制切换为英文输入模式。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

module ImeAutoSwitcher;
import Common;
import ConfigManager;
import Translator;
import AcadVarUtil;

namespace ImeAutoSwitcher
{
    HHOOK g_hKeyboardHook = nullptr;
    DWORD g_lastSwitchTime = 0;
    const ConfigItems::ImeSettings defaultConfig;
    DWORD g_intervalMs = defaultConfig.iIntervalMs;

    bool isCommandRunning()
    {
        int cmdActive = 0;
        if (!AcadVarUtil::getVar(L"CMDACTIVE", cmdActive))
        {
            return false;
        }
        return (cmdActive > 0);
    }

    void ForceEnglishMode(HWND hWnd)
    {
        HIMC hIMC = ImmGetContext(hWnd);
        if (hIMC)
        {
            DWORD dwConv, dwSent;
            if (ImmGetConversionStatus(hIMC, &dwConv, &dwSent))
            {
                // 使用位掩码判断是否处于非英文模式
                // 只要不是纯粹的 ALPHANUMERIC，或者处于 NATIVE (中文) 模式，就触发切换
                if ((dwConv & IME_CMODE_NATIVE) != 0 || dwConv != IME_CMODE_ALPHANUMERIC)
                {
                    INPUT inputs[2] = {};
                    inputs[0].type = INPUT_KEYBOARD;
                    inputs[0].ki.wVk = VK_SHIFT;
                    inputs[1].type = INPUT_KEYBOARD;
                    inputs[1].ki.wVk = VK_SHIFT;
                    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(2, inputs, sizeof(INPUT));
                    CAcModuleResourceOverride resOverride;
                    acutPrintf(_(L"\n已自动切换输入法语言\n"));
                }
            }
            ImmReleaseContext(hWnd, hIMC);
        }
    }

    LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
        {
            KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

            // 先过滤按键范围，减少后续昂贵 API 的调用
            if (pKey->vkCode >= 'A' && pKey->vkCode <= 'Z')
            {
                // 检查修饰键
                bool bControl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                bool bAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
                bool bWin = (GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0;

                if (bControl || bAlt || bWin)
                {
                    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
                }

                DWORD currentTime = GetTickCount();
                if (currentTime - g_lastSwitchTime > g_intervalMs)
                {
                    HWND hAcadWnd = adsw_acadMainWnd();
                    // 仅在 CAD 是当前活动窗口时才进一步检查变量
                    if (GetForegroundWindow() == hAcadWnd)
                    {
                        if (!ImeAutoSwitcher::isCommandRunning())
                        {
                            ForceEnglishMode(hAcadWnd);
                            g_lastSwitchTime = currentTime;
                        }
                    }
                }
            }
        }
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
    }

    void start(DWORD intervlMs)
    {
        if (g_hKeyboardHook == nullptr)
        {
            g_intervalMs = intervlMs;
            g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
            if (g_hKeyboardHook)
            {
                CAcModuleResourceOverride resOverride;
                acutPrintf(_(L"\n启动输入法语言自动切换，切换间隔：%dms\n"), g_intervalMs);
            }
        }
    }

    void stop()
    {
        if (g_hKeyboardHook != nullptr)
        {
            UnhookWindowsHookEx(g_hKeyboardHook);
            g_hKeyboardHook = nullptr;
            CAcModuleResourceOverride resOverride;
            acutPrintf(_(L"\n停止输入法语言自动切换\n"));
        }
    }
}