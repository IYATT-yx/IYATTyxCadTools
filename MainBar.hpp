/**
 * @file      MainBar.hpp
 * @brief     主停靠控制条
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "MainBarChildDlg.hpp"

//-----------------------------------------------------------------------------
class MainBar : public CAcUiDockControlBar {
	DECLARE_DYNAMIC (MainBar)

private:
	//----- Child dialog which will use the resource id supplied
	MainBarChildDlg mChildDlg ;

public:
	MainBar ();
	virtual ~MainBar ();

public:
	static MainBar* gpMainBar;
	static void showBar(Commands::CommandInfoList& commandInfoList);
	static void terminateBar();
	void insertCommands(Commands::CommandInfoList& commandInfoList);

protected:
	virtual BOOL Create (CWnd *pParent, LPCTSTR lpszTitle) ;
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;

	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct) ;
	afx_msg void OnSysCommand (UINT nID, LPARAM lParam) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;

	DECLARE_MESSAGE_MAP()
} ;
