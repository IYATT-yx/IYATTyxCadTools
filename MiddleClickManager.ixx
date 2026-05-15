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
import ConfigManager;

export class MiddleClickManager
{
public:
    /**
     * @brief 获取单例实例
     * @return MiddleClickManager& 引用
     */
    static MiddleClickManager& getInstance();

    void startUnifiedMiddleClickProc(ConfigItems::MiddleClickManagerSettings& settings);
    void stopUnifiedMiddleClickProc();

private:
    MiddleClickManager();
    ~MiddleClickManager();

    // 禁用拷贝与移动
    MiddleClickManager(const MiddleClickManager&) = delete;
    MiddleClickManager& operator=(const MiddleClickManager&) = delete;

    static LRESULT CALLBACK unifiedMiddleClickProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HHOOK mhUnifiedMiddleClickHook;
    DWORD mdwLastMButtonDownTime = 0;
    DWORD mdCmdMiddleClickDownUpInterval;
    bool mbEnabledDialogOk = false;
    bool mbEnabledCmdEnter = false;
};
