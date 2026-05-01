module;
#include "stdafx.h"
#include "resource.h"

module FileDialog;
import Common;

namespace FileDialog
{
	CString ShowSaveFileDialog(const CString& title, const CString& defaultName, const CString& defExt, const CString& filter, const CString& initialDir)
	{
		CFileDialog dlg(FALSE, defExt, defaultName, OFN_HIDEREADONLY, filter, AfxGetMainWnd());
		dlg.m_ofn.lpstrTitle = title;

		if (!initialDir.IsEmpty())
		{
            dlg.m_ofn.lpstrInitialDir = initialDir;
		}

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

	void locateFileInExplorer(const CString& filename)
	{
		CString args;
		args.Format(L"/select,\"%s\"", filename);
		HINSTANCE inst = ShellExecuteW(
			nullptr,
			L"open",
			L"explorer.exe",
			args,
			nullptr,
			SW_SHOWNORMAL
		);

		INT_PTR result = reinterpret_cast<INT_PTR>(inst);
		if (result <= 32)
		{
			CString strErr;
			if (result == SE_ERR_FNF || result == SE_ERR_PNF)
			{
				strErr = Common::loadString(IDS_ERR_FileNotFound);
			}
			else if (result == SE_ERR_ACCESSDENIED)
			{
				strErr = Common::loadString(IDS_ERR_AccessDenied);
			}
			else
			{
				strErr.Format(Common::loadString(IDS_ERR_Unknown_FMT), result);
			}
			AfxMessageBox(strErr, MB_OK | MB_ICONERROR);
		}
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