#include "CDesktopWindow.h"

//#define MEMORY_DETECT
//#define USE_ATL_MODULE

#ifdef MEMORY_DETECT
	#include "vld.h"
#endif

#ifdef USE_ATL_MODULE
CComModule _Module;
#else
CAppModule _Module;
#endif

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR szCmdLine, int nCmdShow)
{
	// Init GDI++
	ULONG_PTR token_;
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output = { 0, 0};
	Gdiplus::Status ret = Gdiplus::GdiplusStartup(&token_, &input, &output);
	if (ret != Gdiplus::Ok) {
		MessageBox(NULL, _T("Fail to init GDI++!"), _T("I am sorry~"), MB_OK);
		return 0;
	}

#ifdef MEMORY_DETECT
	VLDEnable();
#endif

	_Module.Init(NULL, hInst);

	CDesktopWindow desktopWnd;

	// Create & show our main window
	if (NULL == desktopWnd.Create(NULL, CWindow::rcDefault, _T("Desktop Window"))) {
		MessageBox(NULL, _T("Fail to create desktop window!"), _T("I am sorry~"), MB_OK);
		return 1;
	}

	desktopWnd.ShowWindow(nCmdShow);
	desktopWnd.UpdateWindow();

	// Window message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	_Module.Term();

	// Shut down GDI++
	Gdiplus::GdiplusShutdown(token_);

	return (int)(msg.wParam);
}
