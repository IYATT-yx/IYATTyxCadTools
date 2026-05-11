// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//- MainBar.cpp : Implementation of MainBarChildDlg
//-----------------------------------------------------------------------------
/**
 * @file      MainBarChildDlg.cpp
 * @brief     主停靠控制条子对话框实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
#include "StdAfx.h"

#include "resource.h"
#include "MainBarChildDlg.hpp"

import Commands;
import Common;

//-----------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (MainBarChildDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(MainBarChildDlg, CAcUiDialog)
	//{{AFX_MSG_MAP(MainBarChildDlg)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)    // Needed for modeless dialog.
	//}}AFX_MSG_MAP

	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &MainBarChildDlg::OnNMDblclkList1)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
MainBarChildDlg::MainBarChildDlg (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (MainBarChildDlg::IDD, pParent, hInstance) {
	//{{AFX_DATA_INIT(MainBarChildDlg)
	//}}AFX_DATA_INIT
}

//-----------------------------------------------------------------------------
void MainBarChildDlg::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MainBarChildDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST1, commandListControl);
}

//-----------------------------------------------------------------------------
//- Needed for modeless dialogs to keep focus.
//- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT MainBarChildDlg::OnAcadKeepFocus (WPARAM wParam, LPARAM lParam) {
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//- As this dialog is a child dialog we need to disable ok and cancel
BOOL MainBarChildDlg::OnCommand (WPARAM wParam, LPARAM lParam) {
	switch ( wParam ) {
		case IDOK:
		case IDCANCEL:
			return (FALSE) ;
	}	
	return (CAcUiDialog::OnCommand (wParam, lParam)) ;
}

//-----------------------------------------------------------------------------
void MainBarChildDlg::OnSize (UINT nType, int cx, int cy)
{
	CAcUiDialog::OnSize (nType, cx, cy) ;

	if (this->commandListControl.GetSafeHwnd())
	{
		this->commandListControl.MoveWindow(0, 0, cx, cy); // 填满窗口

		// 最后一列自动拉伸填满
		CRect rect;
		this->commandListControl.GetClientRect(&rect);
		int width0 = this->commandListControl.GetColumnWidth(0);
		int width1 = this->commandListControl.GetColumnWidth(1);
		int remainingWidth = rect.Width() - width0 - width1;
		if (remainingWidth > 0)
		{
			this->commandListControl.SetColumnWidth(2, remainingWidth);
		}
	}
}

BOOL MainBarChildDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	CAcModuleResourceOverride resOverride;

	this->commandListControl.SetExtendedStyle(this->commandListControl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//std::vector<CString> columnNames = {Common::loadString(IDS_LBL_CommandDescription), Common::loadString(IDS_LBL_CommandName), Common::loadString(IDS_LBL_ShortCommandName)};
	//for (auto columnName : columnNames)
	//{
 //       this->commandListControl.InsertColumn(0, columnName, LVCFMT_LEFT, 100);
	//}
	this->commandListControl.InsertColumn(0, Common::loadString(IDS_LBL_CommandDescription), LVCFMT_LEFT, 100);
	this->commandListControl.InsertColumn(0, Common::loadString(IDS_LBL_CommandName), LVCFMT_LEFT, 100);
	this->commandListControl.InsertColumn(0, Common::loadString(IDS_LBL_ShortCommandName), LVCFMT_LEFT, 80);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void MainBarChildDlg::insertCommands(Commands::CommandInfoList& commandInfoList)
{
	int idx = 0;
	for (Commands::CommandInfo ci : commandInfoList)
	{
		AcString shortCommandName = ci.getShortCommandName();
		this->commandListControl.InsertItem(idx, shortCommandName.constPtr());
        this->commandListControl.SetItemText(idx, 1, ci.commandName.constPtr());
		this->commandListControl.SetItemText(idx, 2, ci.commandDescription.constPtr());
		++idx;
	}
}

void MainBarChildDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nIndex = pNMItemActivate->iItem;

	if (nIndex != -1)
	{
		CString strFullCmd = this->commandListControl.GetItemText(nIndex, 1);
		if (!strFullCmd.IsEmpty())
		{
			Commands::CommandList pszCmdList = { strFullCmd.GetString() };
			Commands::executeCommand(pszCmdList);
		}
	}
	*pResult = 0;
}

