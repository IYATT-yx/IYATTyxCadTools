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
#include "Common.hpp"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (GenericPairEditDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(GenericPairEditDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, &GenericPairEditDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &GenericPairEditDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &GenericPairEditDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &GenericPairEditDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &GenericPairEditDlg::OnBnClickedCheck2)
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

	// 获取当前默认字体为常规字体
	CFont* pDefaultFont = GetFont();
	LOGFONT lf;
	if (pDefaultFont != nullptr)
	{
		pDefaultFont->GetLogFont(&lf);
		// 有旧字体则删除
		if (this->fontNormal.GetSafeHandle() != nullptr)
		{
            this->fontNormal.DeleteObject();
		}
		this->fontNormal.CreateFontIndirectW(&lf);
	}

	// 创建 GDT 字体
	lf.lfCharSet = ANSI_CHARSET;
	wcscpy(lf.lfFaceName, Common::CharMap::font);
	lf.lfItalic = false;
	lf.lfUnderline = false;
	if (lf.lfHeight < 0)
	{
		lf.lfHeight -= 6; // 负值越小，字号越大
	}
	else
	{
		lf.lfHeight += 6;
	}
	if (this->fontGDT.GetSafeHandle() != nullptr)
	{
        this->fontGDT.DeleteObject();
	}
	this->fontGDT.CreateFontIndirectW(&lf);

	// 初始化对话框
	this->SetWindowTextW(title);
	this->staticText1.SetWindowTextW(label1);
    this->staticText2.SetWindowTextW(label2);
	this->editControl1.SetWindowTextW(L"");
	this->editControl2.SetWindowTextW(L"");

	// 单输入框模式隐藏第 2 组
	if (this->singleMode)
	{
		this->staticText2.ShowWindow(SW_HIDE);
        this->editControl2.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK2)->ShowWindow(SW_HIDE);
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

	this->GdtCheckedStatus[0] = IsDlgButtonChecked(IDC_CHECK1);
    this->GdtCheckedStatus[1] = IsDlgButtonChecked(IDC_CHECK2);

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

void GenericPairEditDlg::OnBnClickedButton1()
{
	Common::startCharMapWithGDT();
}

void GenericPairEditDlg::OnBnClickedCheck1()
{
	int state = IsDlgButtonChecked(IDC_CHECK1);

	if (state == BST_CHECKED)
	{
		this->editControl1.SetFont(&this->fontGDT);
	}
	else
	{
		this->editControl1.SetFont(&this->fontNormal);
	}
	this->editControl1.Invalidate();
}

void GenericPairEditDlg::OnBnClickedCheck2()
{
	int state = IsDlgButtonChecked(IDC_CHECK2);
	if (state == BST_CHECKED)
	{
		this->editControl2.SetFont(&this->fontGDT);
	}
	else
	{
		this->editControl2.SetFont(&this->fontNormal);
	}
	this->editControl1.Invalidate();
}

bool GenericPairEditDlg::getGdtCheckStatus(int idx)
{
	if (idx < 0 || idx > 1)
	{
		AfxMessageBox(L"获取 GDT 复选框状态错误，指定下标的复选框不存在", MB_OK | MB_ICONERROR); // 针对开发者查错
		return false;
	}

	return this->GdtCheckedStatus[idx];
}

BOOL GenericPairEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// 拦截 Tab 键按下消息
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
	{
		CWnd* pFocus = GetFocus();

		// 如果当前焦点在第一个编辑框
		if (pFocus == &editControl1)
		{
			// 如果不是单输入模式，跳到第二个编辑框
			if (!this->singleMode)
			{
				editControl2.SetFocus();
				editControl2.SetSel(0, -1);
			}
			// 如果是单输入模式，保持在 editControl1（或跳回自己）
			else
			{
				editControl1.SetFocus();
				editControl1.SetSel(0, -1);
			}
			return TRUE; // 表示消息已处理，不再向下传递
		}
		// 如果当前焦点在第二个编辑框
		else if (pFocus == &editControl2)
		{
			editControl1.SetFocus();
			editControl1.SetSel(0, -1);
			return TRUE;
		}
	}

	return CAcUiDialog::PreTranslateMessage(pMsg);
}