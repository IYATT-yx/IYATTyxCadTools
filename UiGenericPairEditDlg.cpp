/**
 * @file      UiGenericPairEditDlg.hpp
 * @brief     通用双编辑对话框实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"

import UtilCommon;
import FrameworkTranslator;

const CString UiGenericPairEditDlg::ValidatorOk = L""; // 验证通过的标志

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (UiGenericPairEditDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(UiGenericPairEditDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDOK, &UiGenericPairEditDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &UiGenericPairEditDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &UiGenericPairEditDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &UiGenericPairEditDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &UiGenericPairEditDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
UiGenericPairEditDlg::UiGenericPairEditDlg (CString title, CString label1, CString label2, bool singleMode, bool disableGdt, bool trim, CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (UiGenericPairEditDlg::IDD, pParent, hInstance)
, title(title), label1(label1), label2(label2), singleMode(singleMode), disableGdt(disableGdt), bTrim(trim)
{

}

//-----------------------------------------------------------------------------
void UiGenericPairEditDlg::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABEL1, staticText1);
	DDX_Control(pDX, IDC_LABEL2, staticText2);
	DDX_Control(pDX, IDC_EDIT1, editControl1);
	DDX_Control(pDX, IDC_EDIT2, editControl2);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT UiGenericPairEditDlg::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}

BOOL UiGenericPairEditDlg::OnInitDialog()
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
	wcscpy(lf.lfFaceName, UtilCommon::CharMap::font);
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

	// 禁用 GDT 选项
	if (this->disableGdt)
	{
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_CHECK2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_HIDE);
	}

	// 设置编辑框默认值
	this->editControl1.SetWindowTextW(this->csEdit1Input);
    this->editControl2.SetWindowTextW(this->csEdit2Input);

	// 第 1 个编辑框获取焦点
	this->editControl1.SetFocus();
	this->editControl1.SetSel(0, -1);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void UiGenericPairEditDlg::OnBnClickedOk()
{
	this->editControl1.GetWindowTextW(this->edit1Result);
	if (this->bTrim)
	{
		this->edit1Result.Trim();
	}

	if (this->singleMode)
	{
		this->edit2Result = L"";
	}
	else
	{
        this->editControl2.GetWindowTextW(this->edit2Result);

		if (this->bTrim)
		{
			this->edit2Result.Trim();
		}
	}

	if (this->validator != nullptr)
	{
		CString errorMessage = this->validator(this->edit1Result, this->edit2Result);
		if (errorMessage != UiGenericPairEditDlg::ValidatorOk)
		{
			AfxMessageBox(errorMessage, MB_OK | MB_ICONWARNING);
			return;
		}
	}

	this->GdtCheckedStatus[0] = IsDlgButtonChecked(IDC_CHECK1);
    this->GdtCheckedStatus[1] = IsDlgButtonChecked(IDC_CHECK2);

	CAcUiDialog::OnOK();
}

void UiGenericPairEditDlg::OnBnClickedCancel()
{
	edit1Result = L"";
    edit2Result = L"";
	CAcUiDialog::OnCancel();
}

void UiGenericPairEditDlg::OnBnClickedButton1()
{
	UtilCommon::startCharMapWithGDT();
}

void UiGenericPairEditDlg::OnBnClickedCheck1()
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

void UiGenericPairEditDlg::OnBnClickedCheck2()
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

bool UiGenericPairEditDlg::getGdtCheckStatus(const int& idx)
{
	if (idx < 0 || idx > 1)
	{
		AfxMessageBox(_(L"获取 GDT 复选框状态错误，指定下标的复选框不存在"), MB_OK | MB_ICONERROR); // 针对开发者查错
		return false;
	}

	return this->GdtCheckedStatus[idx];
}

BOOL UiGenericPairEditDlg::PreTranslateMessage(MSG* pMsg)
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
void UiGenericPairEditDlg::modifyEditControl(const CString& edit1Value, const CString& edit2Value)
{
	this->csEdit1Input = edit1Value;
    this->csEdit2Input = edit2Value;
}

void UiGenericPairEditDlg::setValidatorAndParser(UiGenericPairEditDlg::Validator validator)
{
	this->validator = std::move(validator);
}