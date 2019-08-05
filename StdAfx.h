// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER	// Allow use of features specific to Windows XP or later.
	#define WINVER	0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINNT	// Allow use of features specific to Windows XP or later.
	#define _WIN32_WINNT	0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINDOWS	// Allow use of features specific to Windows 98 or later.
	#define _WIN32_WINDOWS	0x0410	// Change this to the appropriate value to target Windows Me or later.
#endif
#ifndef _WIN32_IE	// Allow use of features specific to IE 6.0 or later.
	#define _WIN32_IE	0x0600	// Change this to the appropriate value to target other versions of IE.
#endif
#define STRICT
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// ATL/WTL Header Files
#include <atlbase.h>	// 基本的ATL类
#include <atlapp.h> // 基本的WTL类
#include <atlwin.h> // ATL 窗口类
#include <atlframe.h>	// WTL 主框架窗口类
#include <atlmisc.h>	// WTL 实用工具类，例如：CString
#include <atlcrack.h>	// WTL 增强的消息宏
#include <atlstr.h>
#include <atlcoll.h>	// CAtlArray
#include <atlctrls.h>	// CImageList

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifdef USE_ATL_MODULE
extern CComModule _Module;	// 全局_Module
#else
extern CAppModule _Module;	// WTL 派生的CComModule版本
#endif

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
