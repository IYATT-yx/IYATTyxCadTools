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
