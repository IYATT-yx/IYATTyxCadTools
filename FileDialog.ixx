module;
#include "stdafx.h"

export module FileDialog;

export namespace FileDialog
{
	/**
	 * @brief 弹出保存文件对话框
	 * @param title 对话框标题
	 * @param defExt 默认扩展名，默认 "csv"
	 * @param filter 文件类型过滤器，格式示例: "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"
	 * @return 用户选择的完整文件路径，如果取消返回空字符串
	 */
	CString ShowSaveFileDialog(const CString& title, const CString& defaultName = L"data.csv", const CString& defExt = L"csv", const CString& filter = L"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");

	/**
	 * @brief 弹出打开文件对话框
	 * @param title 对话框标题
	 * @param defExt 默认扩展名，默认 "csv"
	 * @param filter 文件类型过滤器
	 * @return 用户选择的完整文件路径，如果取消返回空字符串
	 */
	CString ShowOpenFileDialog(const CString& title, const CString& defExt = L"csv", const CString& filter = L"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
};

export namespace FileDialog
{
    class FileDialogFilterBuilder
    {
        public:
            FileDialogFilterBuilder()
            {
            }

            /**
             * @brief 添加过滤选项
             * @param label 显示给用户看的文字，如 "Excel 文件"
             * @param extensions 扩展名列表，如 { "*.xls", "*.xlsx" }
             */
            FileDialogFilterBuilder& addFilter(const CString& label, const std::vector<CString>& extensions);

            /**
             * @brief 构建最终的 CString 字符串
             */
            CString build() const;

        private:
            std::vector<CString> mfilters;
    };
};