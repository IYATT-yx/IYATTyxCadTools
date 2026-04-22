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
//----- MainBar.cpp : Implementation of MainBar
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "MainBar.hpp"
#include "acuidock.h"
#include "acui.h"
#include "adui.h"

import Common;

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (MainBar, CAcUiDockControlBar)

BEGIN_MESSAGE_MAP(MainBar, CAcUiDockControlBar)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- MainBar *pInstance = new MainBar;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsMainBar = {0xeab78c04, 0x2194, 0x47ad, {0xa4, 0xf2, 0xad, 0xca, 0x3e, 0x3b, 0xb6, 0x3c}} ;


//-----------------------------------------------------------------------------
MainBar::MainBar () : CAcUiDockControlBar() {
}

//-----------------------------------------------------------------------------
MainBar::~MainBar () {

}

//-----------------------------------------------------------------------------
#ifdef _DEBUG
//- Please uncomment the 2 following lines to avoid linker error when compiling
//- in release mode. But make sure to uncomment these lines only once per project
//- if you derive multiple times from CAdUiDockControlBar/CAcUiDockControlBar
//- classes.

//void CAdUiDockControlBar::AssertValid () const {
//}
#endif

//-----------------------------------------------------------------------------
BOOL MainBar::Create (CWnd *pParent, LPCTSTR lpszTitle) {
    LPCTSTR strWndClass = AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW)) ;
	CRect rect (0, 0, 250, 200) ;
	if (!CAcUiDockControlBar::Create (
			strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
			rect, pParent, 0
		)
	)
		return (FALSE) ;

	SetToolID (&clsMainBar) ;

	// TODO: Add your code here
	
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be 
//----- created by calling the Create or CreateEx member function
int MainBar::OnCreate (LPCREATESTRUCT lpCreateStruct) {
	if ( CAcUiDockControlBar::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;

	//----- Point to our resource
	CAcModuleResourceOverride resourceOverride; 	
	//----- Create it and set the parent as the dockctrl bar
	mChildDlg.Create (IDD_MAINBAR, this) ;
	//----- Move the window over so we can see the control lines
	mChildDlg.MoveWindow (0, 0, 100, 100, TRUE) ;
	return (0) ;
}

//-----------------------------------------------------------------------------
void MainBar::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) {
	// If valid
	if (::IsWindow (mChildDlg.GetSafeHwnd ())) 
	{
		//----- Always point to our resource to be safe
		CAcModuleResourceOverride resourceOverride ;
		//----- Then update its window size relatively
		mChildDlg.SetWindowPos (this, lpRect->left + 4, lpRect->top + 4, lpRect->Width (), lpRect->Height (), SWP_NOZORDER) ;
	}
}

//-----------------------------------------------------------------------------
//-----  Function called when user selects a command from Control menu or when user 
//----- selects the Maximize or the Minimize button.
void MainBar::OnSysCommand (UINT nID, LPARAM lParam) {
	CAcUiDockControlBar::OnSysCommand (nID, lParam) ;
}

//-----------------------------------------------------------------------------
//----- The framework calls this member function after the window's size has changed
void MainBar::OnSize (UINT nType, int cx, int cy) {
	CAcUiDockControlBar::OnSize (nType, cx, cy) ;
	// If valid
	if (::IsWindow (mChildDlg.GetSafeHwnd ())) 
	{
		//----- Always point to our resource to be safe
		CAcModuleResourceOverride resourceOverride ;
		//----- then update its window position relatively
		mChildDlg.MoveWindow (0, 0, cx, cy) ;
	}
}

MainBar* MainBar::gpMainBar = nullptr;

void MainBar::showBar(Commands::CommandInfoList& commandInfoList)
{
	// 如果窗口已经创建，则只负责切换“显示/隐藏”
	if (gpMainBar != nullptr && ::IsWindow(gpMainBar->m_hWnd))
	{
		if (gpMainBar->IsWindowVisible())
		{
			acedGetAcadFrame()->ShowControlBar(gpMainBar, FALSE, FALSE);
		}
		else
		{
			acedGetAcadFrame()->ShowControlBar(gpMainBar, TRUE, FALSE);
			gpMainBar->RestoreControlBar();
			gpMainBar->SendMessage(WM_NCPAINT);
		}
		return; // 这里直接返回，不再执行后续插入逻辑
	}

	// 首次创建
	if (gpMainBar == nullptr)
	{
		gpMainBar = new MainBar();
	}

	{
		CAcModuleResourceOverride resOverride;
		CWnd* pAcadWnd = acedGetAcadFrame();

		if (!gpMainBar->Create(pAcadWnd, Common::loadString(IDS_VAL_LocaleProjectName)))
		{
			delete gpMainBar;
			gpMainBar = nullptr;
			return;
		}
	}
	
	gpMainBar->EnableDocking(CBRS_ALIGN_ANY);
	gpMainBar->RestoreControlBar();
	acedGetAcadFrame()->ShowControlBar(gpMainBar, TRUE, FALSE);

	gpMainBar->insertCommands(commandInfoList);
}
void MainBar::insertCommands(Commands::CommandInfoList& commandInfoList)
{
    this->mChildDlg.insertCommands(commandInfoList);
}

void MainBar::terminateBar()
{
	if (gpMainBar == nullptr)
	{
		return;
	}

	CAcModuleResourceOverride resourceOverride;

	if (::IsWindow(gpMainBar->m_hWnd))
	{
		// 强制失去焦点
		::SetFocus(acedGetAcadFrame()->GetSafeHwnd());

		// 销毁窗口
		gpMainBar->DestroyWindow();
	}

	delete gpMainBar;
	gpMainBar = nullptr;
}