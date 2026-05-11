/**
 * @file      FileDialog.cpp
 * @brief     文件对话框模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
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
		if (filename.IsEmpty())
		{
			return;
		}

		// 获取文件的 PIDL (Item ID List)
		// 这是实现“确保可见 (EnsureVisible)”最可靠的底层方式
		PIDLIST_ABSOLUTE pidl = nullptr;
		HRESULT hr = SHParseDisplayName(filename, nullptr, &pidl, 0, nullptr);

		if (SUCCEEDED(hr))
		{
			if (pidl != nullptr)
			{
				// 执行定位：此 API 会自动打开文件夹、选中文件并滚动到视野中
				hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);

				if (FAILED(hr))
				{
					// 如果 API 失败（某些极端受限环境），弹出未知错误
					CString strErr;
					strErr.Format(Common::loadString(IDS_ERR_Unknown_FMT), hr);
					AfxMessageBox(strErr, MB_OK | MB_ICONERROR);
				}

				// 必须手动释放 Shell 分配的内存
				CoTaskMemFree(pidl);
			}
		}
		else
		{
			CString strErr;
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				strErr = Common::loadString(IDS_ERR_FileNotFound);
			}
			else if (hr == E_ACCESSDENIED)
			{
				strErr = Common::loadString(IDS_ERR_AccessDenied);
			}
			else
			{
				strErr.Format(Common::loadString(IDS_ERR_Unknown_FMT), hr);
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