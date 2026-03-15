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
//----- GenericPairEditDlg.h : Declaration of the GenericPairEditDlg
//-----------------------------------------------------------------------------
// 通用双编辑对话框
#pragma once
//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"

//-----------------------------------------------------------------------------
class GenericPairEditDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (GenericPairEditDlg)

public:
	GenericPairEditDlg (CString title = L"通用双编辑对话框", CString label1 = L"编辑框1：", CString label2 = L"编辑框2：", bool singleMode = false, CWnd* pParent = NULL, HINSTANCE hInstance = NULL);

	enum { IDD = IDD_GENERICPAIREDITDLG} ;

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CString title;
	CString label1;
    CString label2;
	bool singleMode;

	CStatic staticText1; // 标签1变量
	CStatic staticText2; // 标签2变量
	CEdit editControl1; // 编辑框1变量
	CEdit editControl2; // 编辑框2变量
	CString edit1Result; // 编辑框1传出结果
    CString edit2Result; // 编辑框2传出结果
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString getEdit1Result();
	CString getEdit2Result();
} ;
