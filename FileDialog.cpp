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
};