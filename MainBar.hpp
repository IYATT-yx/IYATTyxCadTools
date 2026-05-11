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
//----- MainBar.hpp : Declaration of the MainBar
//-----------------------------------------------------------------------------
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
