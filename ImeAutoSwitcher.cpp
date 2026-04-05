module;
#include "stdafx.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

module ImeAutoSwitcher;

namespace ImeAutoSwitcher
{
    HHOOK g_hKeyboardHook = nullptr;
    DWORD g_lastSwitchTime = 0;
    DWORD g_intervalMs = 200;

    bool isCommandRunning()
    {
        struct resbuf rb;
        // 获取 CMDACTIVE 变量：0 为空闲，>0 为有命令或交互
        if (acedGetVar(_T("CMDACTIVE"), &rb) == RTNORM)
        {
            return (rb.resval.rint > 0);
        }
        return false;
    }

    void ForceEnglishMode(HWND hWnd)
    {
        // 1. 尝试通过 IMM 接口切换
        HIMC hIMC = ImmGetContext(hWnd);
        if (hIMC)
        {
            DWORD dwConv, dwSent;
            if (ImmGetConversionStatus(hIMC, &dwConv, &dwSent))
            {
                if (dwConv != IME_CMODE_ALPHANUMERIC)
                {
                    ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
                    acutPrintf(L"\n自动切换到英文输入法\n");
                }
            }
            ImmReleaseContext(hWnd, hIMC);
        }

        // 2. 模拟 Shift 键：针对不听 Windows 话的国产输入法（如搜狗/微软拼音中英切换）
        // 只有当 IMM 切换后图标没变时，物理模拟才最有效
        keybd_event(VK_SHIFT, 0, 0, 0);
        keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    }

    LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
        {
            KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

            // 过滤：仅字母 A-Z
            if (pKey->vkCode >= 'A' && pKey->vkCode <= 'Z')
            {
                DWORD currentTime = GetTickCount();

                // 控制周期：300ms 冷却，解决卡顿
                if (currentTime - g_lastSwitchTime > 300)
                {
                    HWND hAcadWnd = adsw_acadMainWnd();

                    // 只有 CAD 在前台时处理
                    if (GetForegroundWindow() == hAcadWnd)
                    {
                        // 使用你提供的 acedGetVar 方案，增加异常保护
                        __try
                        {
                            if (!isCommandRunning())
                            {
                                ForceEnglishMode(hAcadWnd);
                                g_lastSwitchTime = currentTime;
                            }
                        }
                        __except (EXCEPTION_EXECUTE_HANDLER)
                        {
                            // 防止某些特殊时刻读取变量导致异常
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
                acutPrintf(L"\n启动输入法语言状态监控，扫描周期：%dms", g_intervalMs);
            }
        }
    }

    void stop()
    {
        if (g_hKeyboardHook != nullptr)
        {
            UnhookWindowsHookEx(g_hKeyboardHook);
            g_hKeyboardHook = nullptr;
            acutPrintf(L"\n停止输入法语言状态监控");
        }
    }

    bool saveSettings(bool bAutoStart, int nIntervalMS)
    {
        HKEY hKey;
        const ACHAR* pProductRoot = acdbHostApplicationServices()->getMachineRegistryProductRootKey();

        if (pProductRoot == nullptr)
        {
            return false;
        }

        AcString sFullKey;
        sFullKey.format(_T("%s\\Applications\\IYATTyxCadTools\\Settings"), pProductRoot);

        LSTATUS ls = RegCreateKeyEx(
            HKEY_CURRENT_USER,
            sFullKey.constPtr(),
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE,
            NULL,
            &hKey,
            NULL
        );

        if (ls != ERROR_SUCCESS)
        {
            return false;
        }

        DWORD dwAuto = bAutoStart ? 1 : 0;
        DWORD dwInterval = static_cast<DWORD>(nIntervalMS);

        RegSetValueEx(hKey, _T("AutoMonitor"), 0, REG_DWORD, reinterpret_cast<BYTE*>(&dwAuto), sizeof(DWORD));
        RegSetValueEx(hKey, _T("MonitorInterval"), 0, REG_DWORD, reinterpret_cast<BYTE*>(&dwInterval), sizeof(DWORD));

        RegCloseKey(hKey);
        return true;
    }

    bool loadSettings(bool& bAutoStart, int& nIntervalMS)
    {
        HKEY hKey;
        const ACHAR* pProductRoot = acdbHostApplicationServices()->getMachineRegistryProductRootKey();

        if (pProductRoot == nullptr)
        {
            bAutoStart = false;
            nIntervalMS = 200;
            return false;
        }

        AcString sFullKey;
        sFullKey.format(_T("%s\\Applications\\IYATTyxCadTools\\Settings"), pProductRoot);

        LSTATUS ls = RegOpenKeyEx(HKEY_CURRENT_USER, sFullKey.constPtr(), 0, KEY_READ, &hKey);

        if (ls != ERROR_SUCCESS)
        {
            bAutoStart = false;
            nIntervalMS = 200;
            return false;
        }

        DWORD dwAuto = 0;
        DWORD dwInterval = 0;
        DWORD dwSize = sizeof(DWORD);

        RegQueryValueEx(hKey, _T("AutoMonitor"), NULL, NULL, reinterpret_cast<BYTE*>(&dwAuto), &dwSize);

        dwSize = sizeof(DWORD);
        RegQueryValueEx(hKey, _T("MonitorInterval"), NULL, NULL, reinterpret_cast<BYTE*>(&dwInterval), &dwSize);

        bAutoStart = (dwAuto != 0);
        nIntervalMS = static_cast<int>(dwInterval);

        RegCloseKey(hKey);
        return true;
    }
}