module;
#include "stdafx.h"

module FileDialog;

namespace FileDialog
{
	CString ShowSaveFileDialog(const CString& title, const CString& defaultName, const CString& defExt, const CString& filter)
	{
		CFileDialog dlg(FALSE, defExt, defaultName, OFN_HIDEREADONLY, filter, AfxGetMainWnd());
		dlg.m_ofn.lpstrTitle = title;

		if (dlg.DoModal() == IDOK)
		{
			return dlg.GetPathName();
		}

		return L"";
	}

	CString ShowOpenFileDialog(const CString& title, const CString& defExt, const CString& filter)
	{
		// 第一个参数为 TRUE 表示 Open 对话框
		CFileDialog dlg(TRUE, defExt, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, filter, AfxGetMainWnd());

		// 设置对话框标题
		dlg.m_ofn.lpstrTitle = title;

		if (dlg.DoModal() == IDOK)
		{
			return dlg.GetPathName();
		}

		return L"";
	}
};

namespace FileDialog
{
	FileDialogFilterBuilder& FileDialogFilterBuilder::addFilter(const CString& label, const std::vector<CString>& extensions)
	{
		CString rule;
		CString displayExt;

		for (size_t i = 0; i < extensions.size(); ++i)
		{
			rule += extensions[i];
			displayExt += extensions[i];

			if (i < extensions.size() - 1)
			{
				rule += L";";
				displayExt += L"; ";
			}
		}

		// 格式：描述文本 (*.ext)|*.ext
		CString entry;
		entry.Format(L"%s (%s)|%s", label.GetString(), displayExt.GetString(), rule.GetString());

		mfilters.push_back(entry);
		return *this;
	}

	CString FileDialogFilterBuilder::build() const
	{
		CString finalFilter = L"";

		for (const auto& f : mfilters)
		{
			finalFilter += f;
			finalFilter += L"|";
		}

		// 以双管道结尾
		finalFilter += L"|";
		return finalFilter;
	}
};