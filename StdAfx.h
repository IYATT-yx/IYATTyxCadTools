// (C) Copyright 2002-2012 by Autodesk, Inc.
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
//- StdAfx.h : include file for standard system include files,
//-      or project specific include files that are used frequently,
//-      but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once
#define CADTOOLS_MODULE

/*
#ifndef _ALLOW_RTCc_IN_STL
#define _ALLOW_RTCc_IN_STL
#endif
*/

#pragma pack(push, 8)
#pragma warning(disable : 4786 4996)
    // #pragma warning(disable: 4098)

    //-----------------------------------------------------------------------------
#define STRICT

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN //- Exclude rarely-used stuff from Windows headers
#endif

//- Modify the following defines if you have to target a platform prior to the ones specified below.
//- Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER        //- Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0601 //- Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

    
        //- ObjectARX and OMF headers needs this
#include <map>

    //-----------------------------------------------------------------------------
#include <afxwin.h> //- MFC core and standard components
#include <afxext.h> //- MFC extensions
#include <afxcmn.h> //- MFC support for Windows Common Controls

                //-----------------------------------------------------------------------------
    //- Include ObjectDBX/ObjectARX headers
    //- Uncomment one of the following lines to bring a given library in your project.
    // #define _BREP_SUPPORT_					//- Support for the BRep API
    // #define _HLR_SUPPORT_						//- Support for the Hidden Line Removal API
    // #define _AMODELER_SUPPORT_				//- Support for the AModeler API
        // #define _ASE_SUPPORT_							//- Support for the ASI/ASE API
    // #define _RENDER_SUPPORT_					//- Support for the AutoCAD Render API
        // #define _ARX_CUSTOM_DRAG_N_DROP_	//- Support for the ObjectARX Drag'n Drop API
    // #define _INC_LEAGACY_HEADERS_			//- Include legacy headers in this project
#include "arxHeaders.h"

// AutoCAD Mechanical SDK
///////////////////////////////////
#ifdef ACMSYMBB_CRX_DLLSHARE
#undef ACMSYMBB_CRX_DLLSHARE
#endif
#define ACMSYMBB_CRX_DLLSHARE __declspec(dllimport)

#ifdef ACMSYMBB_CRX_DLLSHARE_DATA
#undef ACMSYMBB_CRX_DLLSHARE_DATA
#endif
#define ACMSYMBB_CRX_DLLSHARE_DATA __declspec(dllimport)

// 必须重新定义这个宏，否则 AcmCObjArray 内部会报错
#undef  DBREF_DECLARE
#define DBREF_DECLARE(AcDbType)

#include <afxtempl.h>
#include "acdb.h"
#include "aced.h"
#include "adslib.h"
#include "rxobject.h"
#include "rxregsvc.h"
#include "actrans.h"
#include "adesk.h"
#include "adeskabb.h"
#include "ol_errno.h"
#include "acgi.h"
#include "acestext.h"

// DBR files
#include "dbid.h"
#include "dbintar.h"
#include "dbptrar.h"
#include "dbidar.h"
#include "dbidmap.h"
#include "dbsymtb.h"
#include "dbents.h"
#include "dbhatch.h"
#include "dbsol3d.h"
#include "dbbody.h"
#include "dbmtext.h"
#include "dbproxy.h"
#include "dbcurve.h"
#include "dbdict.h"

// Ge Lib
#include "geassign.h"
#include "gegbl.h"
#include "gedblar.h"
#include "gearc3d.h"
#include "gelnsg3d.h"
#include "gepnt3d.h"
#include "gept3dar.h"
#include "gemat3d.h"
#include "gevec3d.h"

#include "mistatus.h"
#include "miarray.h"
#include "mibase.h"
#include "acm.h"
#include "acmdef.h"
#include "symbol.h"
#include "cauxent.h" 
#include "symstd.h"
#include "fcframe.h"
#include "FCFSymLib.h"
///////////////////////
// AutoCAD Mechanical SDK

        
//-----------------------------------------------------------------------------
#include "DocData.h" //- Your document specific data class holder

    //- Declare it as an extern here so that it becomes available in all modules
    extern AcApDataManager<CDocData>
        DocVars;

                #pragma pack(pop)

    