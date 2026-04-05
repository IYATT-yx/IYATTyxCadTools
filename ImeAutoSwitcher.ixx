module;
#include "StdAfx.h"
//#include <windows.h>

export module ImeAutoSwitcher;

export namespace ImeAutoSwitcher
{
    /**
     * @brief 启动输入法语言监控
     * @param intervlMs 监控间隔时间，单位毫秒。设置低于 200 ms时还是按 200ms 执行，防止系统卡顿。
     */
    void start(DWORD intervlMs = 200);

    /**
     * @brief 停止输入法语言监控
     */
    void stop();

    /**
     * @brief 保存配置到注册表
     * * @param bAutoStart 是否自启动
     * @param nIntervalMS 监控时间间隔
     * @return true 保存成功
     * @return false 保存失败
     */
    bool saveSettings(bool bAutoStart, int nIntervalMS);

    /**
     * @brief 从注册表加载配置
     * * @param bAutoStart [out] 输出自启动状态
     * @param nIntervalMS [out] 输出间隔时间
     * @return true 加载成功
     * @return false 加载失败，返回默认值
     */
    bool loadSettings(bool& bAutoStart, int& nIntervalMS);
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