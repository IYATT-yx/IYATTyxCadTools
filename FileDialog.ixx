module;
#include "stdafx.h"

export module FileDialog;

export namespace FileDialog
{
	/**
	 * @brief 弹出保存文件对话框
	 * @param title 对话框标题，默认 "保存文件"
	 * @param defExt 默认扩展名，默认 "csv"
	 * @param filter 文件类型过滤器，格式示例: "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"
	 * @return 用户选择的完整文件路径，如果取消返回空字符串
	 */
	CString ShowSaveFileDialog(const CString& title = L"保存文件", const CString& defaultName = L"标注数据.csv", const CString& defExt = L"csv", const CString& filter = L"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
};