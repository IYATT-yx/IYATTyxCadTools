/**
 * @file      MiddleClickManager.ixx
 * @brief     全局鼠标中键确认管理模块，实现中键映射到确定按钮。
 * @details   本模块灵感来源于 Siemens NX 软件的高效交互体验：在 Siemens NX 中，点击鼠标中键（MB2）
 *            等同于点击对话框的“确定”或“应用”按钮（上一次点击过的按钮），极大地提升了参数化建模过程中的操作流畅度。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include <Windows.h>

export module MiddleClickManager;

export class MiddleClickManager
{
public:
    /**
     * @brief 获取单例实例
     * @return MiddleClickManager& 引用
     */
    static MiddleClickManager& getInstance();

    void startDialogMiddleClickToOk();
    void stopDialogMiddleClickToOk();
    void startCmdMiddleClickToEnter(DWORD dCmdMiddleClickDownUpInterval);
    void stopCmdMiddleClickToEnter();

    /**
     * @brief 查询当前运行状态
     * @return bool 是否正在监听
     */
    bool isDialogMiddleClickToOkRunning() const;

private:
    MiddleClickManager();
    ~MiddleClickManager();

    // 禁用拷贝与移动
    MiddleClickManager(const MiddleClickManager&) = delete;
    MiddleClickManager& operator=(const MiddleClickManager&) = delete;

    /**
     * @brief 低级鼠标钩子回调函数
     * 对话框打开状态下，将鼠标中键点击映射为对话框的“确定”按钮点击
     */
    static LRESULT CALLBACK dialogMiddleClickToOkProc(int nCode, WPARAM wParam, LPARAM lParam);

    /**
     * @brief 低级鼠标钩子回调函数
     * 命令行状态下，将鼠标中键点击映射为命令的“Enter”键按下
     */
    static LRESULT CALLBACK cmdMiddleClickToEnterProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HHOOK mhDialogMiddleClickToOkHook;
    HHOOK mhCmdMiddleClickToEnterHook;
    DWORD mdwLastMButtonDownTime = 0;
    DWORD mdCmdMiddleClickDownUpInterval;
    static MiddleClickManager* mpInstance;
};
