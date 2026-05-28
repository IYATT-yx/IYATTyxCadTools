/**
 * @file      DocCloseInterceptor.ixx
 * @brief     文档关闭拦截模块，负责在仅发生平移缩放时静默关闭图纸，防止弹出原生保存提示。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module DocCloseInterceptor;

export class DocCloseInterceptor : public AcEditorReactor
{
public:
    // 获取全局唯一单例接口
    static DocCloseInterceptor& getInstance();

    // 显式删除拷贝与移动构造，确保全局唯一性
    DocCloseInterceptor(const DocCloseInterceptor&) = delete;
    DocCloseInterceptor& operator=(const DocCloseInterceptor&) = delete;
    DocCloseInterceptor(DocCloseInterceptor&&) = delete;
    DocCloseInterceptor& operator=(DocCloseInterceptor&&) = delete;

    // 启动与停止拦截器
    void start();
    void stop();

    // 重写 AcEditorReactor 事件
    virtual void commandWillStart(const wchar_t* pCmdStr) override;
    virtual void commandEnded(const wchar_t* pCmdStr) override;
    virtual void commandCancelled(const wchar_t* pCmdStr) override;

private:
    // 隐藏构造与析构函数，防止外部破坏单例
    DocCloseInterceptor();
    virtual ~DocCloseInterceptor() override;

    // Win32 CBT 钩子回调
    static LRESULT CALLBACK cbtFilterHook(int code, WPARAM wParam, LPARAM lParam);

private:
    HHOOK mhCbtHook;
    bool mbIntercepting;

    // 全局单例指针，供静态钩子函数访问
    static DocCloseInterceptor* spInstance;
};