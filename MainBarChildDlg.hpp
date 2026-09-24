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
//----- MainBarChildDlg.h : Declaration of the MainBarChildDlg
//-----------------------------------------------------------------------------
/**
 * @file      MainBarChildDlg.hpp
 * @brief     主停靠控制条子对话框
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"

import Commands;

//-----------------------------------------------------------------------------
class MainBarChildDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (MainBarChildDlg)

public:
	MainBarChildDlg (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_MAINBAR};

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam) ;

protected:
	afx_msg LRESULT OnAcadKeepFocus (WPARAM wParam, LPARAM lParam) ;
public:	
	afx_msg void OnSize (UINT nType, int cx, int cy) ;

protected:
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl commandListControl;
	CEdit searchEditControl;                 // 搜索输入框控件
	const Commands::CommandInfoList* pAllCommands = nullptr;  // 全量命令数据引用（用于实时筛选）
public:
	virtual BOOL OnInitDialog();
	/**
	 * @brief 向命令报表插入命令
	 * @param commandInfoList 命令列表
	 */
	void insertCommands(Commands::CommandInfoList& commandInfoList);
private:
	/**
	 * @brief 根据筛选文本更新列表
	 * @param filterText 过滤字符串
	 */
	void updateListByFilter(const CString& filterText);
	/**
	 * @brief 搜索框文本变更事件
	 */
	afx_msg void OnEnChangeEditSearch();

	/**
	 * @brief 命令报表双击事件
	 * @param pNMHDR
	 * @param pResult 
	 */
	void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
} ;
