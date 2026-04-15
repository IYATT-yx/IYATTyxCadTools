module;
#include "stdafx.h"
#include "resource.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

module ImeAutoSwitcher;
import Common;

namespace ImeAutoSwitcher
{
    HHOOK g_hKeyboardHook = nullptr;
    DWORD g_lastSwitchTime = 0;
    DWORD g_intervalMs = ImeAutoSwitcher::defaultIntervalMs;

    bool isCommandRunning()
    {
        struct resbuf rb;
        if (acedGetVar(ImeAutoSwitcher::cmdActive, &rb) == RTNORM)
        {
            return (rb.resval.rint > 0);
        }
        return false;
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
                    acutPrintf(L"\n%s\n", Common::loadString(IDS_ImeAutoSwitchPrompt));
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
                acutPrintf(Common::loadString(IDS_ImeAutoSwitchStartPrompt_FMT), g_intervalMs);
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
            acutPrintf(L"\n%s", Common::loadString(IDS_ImeAutoSwitchStopPrompt));
        }
    }

    bool saveSettings(bool bAutoStart, int nIntervalMS)
    {
        HKEY hKey;
        const wchar_t* pProductRoot = acdbHostApplicationServices()->getMachineRegistryProductRootKey();

        if (pProductRoot == nullptr)
        {
            return false;
        }

        AcString sFullKey;
        sFullKey.format(L"%s%s", pProductRoot, ImeAutoSwitcher::regSettingsPath);

        LSTATUS ls = RegCreateKeyEx(
            HKEY_CURRENT_USER,
            sFullKey.constPtr(),
            0,
            nullptr,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE,
            nullptr,
            &hKey,
            nullptr
        );

        if (ls != ERROR_SUCCESS)
        {
            return false;
        }

        DWORD dwAuto = bAutoStart ? 1 : 0;
        DWORD dwInterval = static_cast<DWORD>(nIntervalMS);

        RegSetValueEx(hKey, ImeAutoSwitcher::regAutoStartKey, 0, REG_DWORD, reinterpret_cast<BYTE*>(&dwAuto), sizeof(DWORD));
        RegSetValueEx(hKey, ImeAutoSwitcher::regIntervalKey, 0, REG_DWORD, reinterpret_cast<BYTE*>(&dwInterval), sizeof(DWORD));

        RegCloseKey(hKey);
        return true;
    }

    bool loadSettings(bool& bAutoStart, int& nIntervalMS)
    {
        HKEY hKey;
        const wchar_t* pProductRoot = acdbHostApplicationServices()->getMachineRegistryProductRootKey();

        if (pProductRoot == nullptr)
        {
            bAutoStart = false;
            nIntervalMS = ImeAutoSwitcher::defaultIntervalMs;
            return false;
        }

        AcString sFullKey;
        sFullKey.format(L"%s%s", pProductRoot, ImeAutoSwitcher::regSettingsPath);

        LSTATUS ls = RegOpenKeyEx(HKEY_CURRENT_USER, sFullKey.constPtr(), 0, KEY_READ, &hKey);

        if (ls != ERROR_SUCCESS)
        {
            bAutoStart = false;
            nIntervalMS = ImeAutoSwitcher::defaultIntervalMs;
            return false;
        }

        DWORD dwAuto = 0;
        DWORD dwInterval = ImeAutoSwitcher::defaultIntervalMs;
        DWORD dwSize = sizeof(DWORD);

        RegQueryValueEx(hKey, ImeAutoSwitcher::regAutoStartKey, nullptr, nullptr, reinterpret_cast<BYTE*>(&dwAuto), &dwSize);

        dwSize = sizeof(DWORD);
        RegQueryValueEx(hKey, ImeAutoSwitcher::regIntervalKey, nullptr, nullptr, reinterpret_cast<BYTE*>(&dwInterval), &dwSize);

        bAutoStart = (dwAuto != 0);
        nIntervalMS = static_cast<int>(dwInterval);

        RegCloseKey(hKey);
        return true;
    }
}