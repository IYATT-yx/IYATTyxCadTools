/**
 * @file      UiMainBarChildDlg.cpp
 * @brief     主停靠控制条子对话框实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
#include "StdAfx.h"

#include "resource.h"
#include "UiMainBarChildDlg.hpp"

import FrameworkCommands;
import FrameworkTranslator;

//-----------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (UiMainBarChildDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(UiMainBarChildDlg, CAcUiDialog)
	//{{AFX_MSG_MAP(UiMainBarChildDlg)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)    // Needed for modeless dialog.
	//}}AFX_MSG_MAP

	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &UiMainBarChildDlg::OnNMDblclkList1)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &UiMainBarChildDlg::OnEnChangeEditSearch)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &UiMainBarChildDlg::OnBnClickedBtnClear)			// 绑定清空按钮事件
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
UiMainBarChildDlg::UiMainBarChildDlg (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (UiMainBarChildDlg::IDD, pParent, hInstance) {
	//{{AFX_DATA_INIT(UiMainBarChildDlg)
	//}}AFX_DATA_INIT
}

//-----------------------------------------------------------------------------
void UiMainBarChildDlg::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UiMainBarChildDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_SEARCH, this->searchEditControl);
	DDX_Control(pDX, IDC_LIST1, commandListControl);
}

//-----------------------------------------------------------------------------
//- Needed for modeless dialogs to keep focus.
//- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT UiMainBarChildDlg::OnAcadKeepFocus (WPARAM wParam, LPARAM lParam) {
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//- As this dialog is a child dialog we need to disable ok and cancel
BOOL UiMainBarChildDlg::OnCommand (WPARAM wParam, LPARAM lParam) {
	switch ( wParam ) {
		case IDOK:
		case IDCANCEL:
			return (FALSE) ;
	}	
	return (CAcUiDialog::OnCommand (wParam, lParam)) ;
}

//-----------------------------------------------------------------------------
void UiMainBarChildDlg::OnSize(UINT nType, int cx, int cy)
{
	CAcUiDialog::OnSize(nType, cx, cy);

	const int editHeight = 22; // 控件统一高度
	const int margin = 2;     // 控件间距
	const int labelWidth = 40; // “筛选：”标签宽度
	const int btnWidth = 50;   // “清空”按钮宽度

	// 定位左侧标签“筛选：”（固定宽高）
	if (this->searchLabelControl.GetSafeHwnd())
	{
		this->searchLabelControl.MoveWindow(0, 0, labelWidth, editHeight);
	}

	// 定位右侧“清空”按钮（固定宽高，贴紧右侧）
	if (this->clearButtonControl.GetSafeHwnd())
	{
		int btnLeft = cx - btnWidth;
		if (btnLeft < labelWidth) btnLeft = labelWidth;
		this->clearButtonControl.MoveWindow(btnLeft, 0, btnWidth, editHeight);
	}

	// 定位中间的搜索编辑框（宽度随窗口拉伸变大）
	if (this->searchEditControl.GetSafeHwnd())
	{
		int editLeft = labelWidth + margin;
		int editRight = (this->clearButtonControl.GetSafeHwnd()) ? (cx - btnWidth - margin) : cx;
		int editWidth = (editRight > editLeft) ? (editRight - editLeft) : 0;
		this->searchEditControl.MoveWindow(editLeft, 0, editWidth, editHeight);
	}

	// 定位列表控件（起点在搜索框下方：editHeight + margin）
	if (this->commandListControl.GetSafeHwnd())
	{
		int listTop = editHeight + margin;
		int listHeight = (cy > listTop) ? (cy - listTop) : 0;
		this->commandListControl.MoveWindow(0, listTop, cx, listHeight);

		// 最后一列自动拉伸填满
		CRect rect;
		this->commandListControl.GetClientRect(&rect);
		int width0 = this->commandListControl.GetColumnWidth(0);
		int remainingWidth = rect.Width() - width0;
		if (remainingWidth > 0)
		{
			this->commandListControl.SetColumnWidth(1, remainingWidth);
		}
	}
}

BOOL UiMainBarChildDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	CAcModuleResourceOverride resOverride;

	this->searchLabelControl.Create(_(L" 筛选："), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, CRect(0, 0, 0, 0), this, IDC_STATIC);
	this->searchLabelControl.SetFont(this->GetFont());

	this->clearButtonControl.Create(_(L"清空"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_BTN_CLEAR);
	this->clearButtonControl.SetFont(this->GetFont());

	this->commandListControl.SetExtendedStyle(this->commandListControl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	this->commandListControl.InsertColumn(0, _(L"命令描述"), LVCFMT_LEFT, 100);
	this->commandListControl.InsertColumn(0, _(L"命令名"), LVCFMT_LEFT, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void UiMainBarChildDlg::insertCommands(FrameworkCommands::CommandInfoList& commandInfoList)
{
	this->pAllCommands = &commandInfoList;

	// 读取当前搜索框文字并触发筛选更新
	CString filterText;
	if (this->searchEditControl.GetSafeHwnd())
	{
		this->searchEditControl.GetWindowText(filterText);
	}
	this->updateListByFilter(filterText);
}

void UiMainBarChildDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nIndex = pNMItemActivate->iItem;

	if (nIndex != -1)
	{
		CString strFullCmd = this->commandListControl.GetItemText(nIndex, 0);
		if (!strFullCmd.IsEmpty())
		{
			FrameworkCommands::CommandList pszCmdList = { strFullCmd.GetString() };
			FrameworkCommands::executeCommand(pszCmdList);
		}
	}
	*pResult = 0;
}

void UiMainBarChildDlg::updateListByFilter(const CString& filterText)
{
	this->commandListControl.SetRedraw(FALSE);
	this->commandListControl.DeleteAllItems();

	if (this->pAllCommands == nullptr)
	{
		this->commandListControl.SetRedraw(TRUE);
		return;
	}

	CString keyword = filterText;
	keyword.Trim();
	keyword.MakeLower();

	int idx = 0;
	for (const auto& ci : *(this->pAllCommands)) // 解引用遍历，全过程为 const 引用访问
	{
		CString cmdName(ci.commandName.constPtr());
		CString cmdDesc(ci.commandDescription.constPtr());

		CString lowerName = cmdName;
		CString lowerDesc = cmdDesc;
		lowerName.MakeLower();
		lowerDesc.MakeLower();

		if (keyword.IsEmpty() || lowerName.Find(keyword) != -1 || lowerDesc.Find(keyword) != -1)
		{
			int row = this->commandListControl.InsertItem(idx, cmdName);
			this->commandListControl.SetItemText(row, 1, cmdDesc);
			++idx;
		}
	}

	this->commandListControl.SetRedraw(TRUE);
	this->commandListControl.Invalidate();
}

void UiMainBarChildDlg::OnEnChangeEditSearch()
{
	CString filterText;
	this->searchEditControl.GetWindowText(filterText);
	this->updateListByFilter(filterText);
}

void UiMainBarChildDlg::OnBnClickedBtnClear()
{
	if (this->searchEditControl.GetSafeHwnd())
	{
		this->searchEditControl.SetWindowText(L""); // 清空编辑框文字
		this->searchEditControl.SetFocus();            // 保持焦点在搜索框
	}
}