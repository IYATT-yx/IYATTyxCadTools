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
//----- GenericPairEditDlg.cpp : Implementation of GenericPairEditDlg
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "GenericPairEditDlg.hpp"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (GenericPairEditDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(GenericPairEditDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, &GenericPairEditDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &GenericPairEditDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
GenericPairEditDlg::GenericPairEditDlg (CString title, CString label1, CString label2, bool singleMode, CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (GenericPairEditDlg::IDD, pParent, hInstance)
, title(title), label1(label1), label2(label2), singleMode(singleMode)
{

}

//-----------------------------------------------------------------------------
void GenericPairEditDlg::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABEL1, staticText1);
	DDX_Control(pDX, IDC_LABEL2, staticText2);
	DDX_Control(pDX, IDC_EDIT1, editControl1);
	DDX_Control(pDX, IDC_EDIT2, editControl2);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT GenericPairEditDlg::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}

BOOL GenericPairEditDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();

	// 初始化对话框
	this->SetWindowTextW(title);
	this->staticText1.SetWindowTextW(label1);
    this->staticText2.SetWindowTextW(label2);
	this->editControl1.SetWindowTextW(L"");
	this->editControl2.SetWindowTextW(L"");

	// 修改确定和取消按钮文字
	SetDlgItemTextW(IDOK, L"确定");
	SetDlgItemTextW(IDCANCEL, L"取消");

	// 单输入框模式隐藏第 2 组
	if (this->singleMode)
	{
		this->staticText2.ShowWindow(SW_HIDE);
        this->editControl2.ShowWindow(SW_HIDE);
	}

	// 第 1 个编辑框获取焦点
	this->editControl1.SetFocus();
	this->editControl1.SetSel(0, -1);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void GenericPairEditDlg::OnBnClickedOk()
{
	this->editControl1.GetWindowTextW(edit1Result);

	if (this->singleMode)
	{
		edit2Result = L"";
	}
	else
	{
        this->editControl2.GetWindowTextW(edit2Result);
	}

	CAcUiDialog::OnOK();
}

void GenericPairEditDlg::OnBnClickedCancel()
{
	edit1Result = L"";
    edit2Result = L"";
	CAcUiDialog::OnCancel();
}

CString GenericPairEditDlg::getEdit1Result()
{
	return this->edit1Result;
}

CString GenericPairEditDlg::getEdit2Result()
{
	return this->edit2Result;
}
