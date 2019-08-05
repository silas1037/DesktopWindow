#pragma once

#include "StdAfx.h"
#include "CBackGround.h"

#include <shobjidl.h>
#include <ShellAPI.h>
#undef ExtractIcon
using namespace Gdiplus;

static IContextMenu2*  g_pIContext2 = NULL;
static IContextMenu3*  g_pIContext3 = NULL;

typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, WS_EX_APPWINDOW> CDesktopWindowTraits;

class CDesktopWindow :
	public CWindowImpl<CDesktopWindow, CWindow, CDesktopWindowTraits>,
	public CBackGround<CDesktopWindow, RGB(0, 0, 255)> {
public:
	DECLARE_WND_CLASS(_T("Desktop Window Class"))
	//////////////////////////////////////////////////////////////////////////
	typedef CBackGround<CDesktopWindow, RGB (0, 0, 255)> CBackGroundBase;

	BEGIN_MSG_MAP_EX(CDesktopWindow)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MSG_WM_NCACTIVATE(OnNCActivate)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLeftButtonDblClk)
	MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRightButtonDown)
	MESSAGE_HANDLER(WM_RBUTTONUP, OnRightButtonUp)
	CHAIN_MSG_MAP(CBackGroundBase)
	END_MSG_MAP()
	//////////////////////////////////////////////////////////////////////////
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow(GetDesktopWindow());

		iconSpacingWidth = 0;
		iconSpacingHeight = 0;

		if (!InitShellFolder()) {
			MessageBox(L"Fail to init shell folder!", L"I am sorry!", MB_OK);
		}

		SetShellItemPosition();

		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DestroyWindow();
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UINT nType = (UINT) wParam;
		CSize size(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (nType != SIZE_MINIMIZED) {
			SetShellItemPosition();
		}

		return 0;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DrawShellFolder(m_hWnd);

		return 0;
	}

	LRESULT OnRightButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		int index = -1;
		IsHitShellItem(point, index);
		return 0;
	}

	LRESULT OnLeftButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		int index = -1;
		if (IsHitShellItem(point, index)) {
			XShellItem&	 item = itemArray[index];
			item.hit = TRUE;
		}

		return 0;
	}

	LRESULT OnLeftButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		int index = -1;
		if (IsHitShellItem(point, index)) {
			XShellItem&	 item = itemArray[index];
			RunShellItem(item.itemId);
		}

		return 0;
	}

	LRESULT OnRightButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		int index = -1;
		if (IsHitShellItem(point, index)) {
			XShellItem&	 item = itemArray[index];
			RightMenu(item.itemId);
		}
		else {
			DesktopMenu();
		}

		return 0;
	}

	// 判断是否选择了某个图标
	BOOL IsHitShellItem(const WTL::CPoint& pt, int& index)
	{
		index = -1;

		int cItems = (int)itemArray.GetCount();
		for (int i = 0; i < cItems; i++) {
			XShellItem&	 item = itemArray[i];
			BOOL isHit = FALSE;
			if (
				(pt.x >= item.x && pt.x <= (item.x + item.w) && pt.y >= item.y && pt.y <= (item.y + item.h)) ||
				(
					pt.x >= item.nameX && pt.x <= (item.nameX + item.nameW) && pt.y >= item.nameY && pt.y <=
						(item.nameY + item.nameH)
				)) {
				index = i;
				isHit = TRUE;
			}

			if (isHit != item.hit) {
				item.hit = isHit;

				WTL::CRect rect(item.x, item.y, item.x + item.w, item.y + item.h);
				WTL::CRect rectName(item.nameX, item.nameY, item.nameX + item.nameW, item.nameY + item.nameH);
				InvalidateRect(&rect, TRUE);
				InvalidateRect(&rectName, TRUE);
			}
		}

		return index != -1 ? TRUE : FALSE;
	}

	BOOL OnNCActivate(BOOL bActive)
	{
		if (!bActive) {
			WTL::CRect rect;
			GetClientRect(&rect);
			InvalidateRect(&rect, TRUE);
		}

		SetMsgHandled(false);
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
private:
	struct XShellItem {
		ITEMIDLIST*	 itemId;

		int x;
		int y;
		int w;
		int h;

		int nameX;
		int nameY;
		int nameW;
		int nameH;

		BOOL hit;

		CStringW name;
		Bitmap*	 icon;
		Bitmap*	 nameIcon;

		XShellItem()
		:
		itemId(NULL),
		x(0),
		y(0),
		w(0),
		h(0),
		nameX(0),
		nameY(0),
		nameW(0),
		nameH(0),
		name(L""),
		hit(FALSE),
		icon(NULL),
		nameIcon(NULL) {
		}
		~XShellItem() {
		}
	};

	CComPtr<IShellFolder> folder;
	CAtlArray<XShellItem> itemArray;

	static const int OFFSET_WIDTH = 0;
	static const int OFFSET_HEIGHT = 4;
	static const int NAME_WIDTH = 72;
	static const int NAME_HEIGHT = 32;
	static const int ICON_WIDTH = 32;
	static const int ICON_HEIGHT = 32;

	int iconSpacingWidth;
	int iconSpacingHeight;

	//////////////////////////////////////////////////////////////////////////
	// 获取桌面图标的相关数据
	BOOL InitShellFolder()
	{
		HRESULT hRslt = SHGetDesktopFolder(&folder);
		if (FAILED(hRslt)) {
			return FALSE;
		}

		CComPtr<IEnumIDList> ids;
		hRslt = folder->EnumObjects(0, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ids);
		if (FAILED(hRslt)) {
			return FALSE;
		}

		CAtlList<XShellItem> items;
		for (;;) {
			ITEMIDLIST*	 id = 0;
			ULONG cIds = 0;

			hRslt = ids->Next(1, &id, &cIds);
			if (hRslt != S_OK) {
				break;
			}

			CStringW name;
			STRRET str = { 0};
			hRslt = folder->GetDisplayNameOf(id, SHGDN_NORMAL | SHGDN_INFOLDER, &str);
			if (SUCCEEDED(hRslt)) {
				LPWSTR pname = 0;
				StrRetToStrW(&str, id, &pname);
				name = pname;
				CoTaskMemFree(pname);
			}

			XShellItem item;

			item.itemId = id;
			item.name = name;
			items.AddTail(item);
		}

		SIZE_T iItem = 0;
		SIZE_T cItems = items.GetCount();

		itemArray.SetCount(cItems);

		POSITION pos = items.GetHeadPosition();
		while (pos != 0) {
			XShellItem&	 si = items.GetNext(pos);
			itemArray[iItem] = si;
			iItem++;
		}

		HDC hDC = CreateCompatibleDC(0);

		Graphics g(hDC);
		g.Clear(Color(0, 0, 0, 0));

		ICONMETRICS im = { 0};
		im.cbSize = sizeof(im);
		SystemParametersInfo(SPI_GETICONMETRICS, sizeof(im), &im, 0);

		SolidBrush br_t(Color(255, 255, 255));
		Font font_i(hDC, &(im.lfFont));
		float fcy = font_i.GetHeight(&g) * 2 + 2;
		DeleteDC(hDC);

		Gdiplus::StringFormat sf(Gdiplus::StringFormat::GenericTypographic());
		sf.SetAlignment(Gdiplus::StringAlignmentCenter);
		sf.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);

		iconSpacingWidth = im.iHorzSpacing + OFFSET_WIDTH;
		iconSpacingHeight = im.iVertSpacing + OFFSET_HEIGHT;

		int iconWidth = GetSystemMetrics(SM_CXICON);
		int iconHeight = GetSystemMetrics(SM_CYICON);

		for (SIZE_T i = 0; i < cItems; i++) {
			XShellItem&	 item = itemArray[i];

			// SHGetFileInfo
			HICON hIcon = 0;
			HIMAGELIST hImgList;
			SHFILEINFO stSHFileInfo;
			CImageList cImgList;

			// 获取图标
			hImgList = (HIMAGELIST)::SHGetFileInfo(
					(LPCWSTR) item.itemId,
					0,
					&stSHFileInfo,
					sizeof(SHFILEINFO),
					SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SYSICONINDEX);

			// DIBSection 8bit
			BITMAPINFO bmi;
			BITMAPINFOHEADER&  bmih = bmi.bmiHeader;
			bmih.biSize = sizeof(bmih);
			bmih.biWidth = ICON_WIDTH;
			bmih.biHeight = -ICON_HEIGHT;	// BMP反转
			bmih.biPlanes = 1;
			bmih.biBitCount = 32;
			bmih.biCompression = BI_RGB;
			bmih.biSizeImage = 0;
			bmih.biXPelsPerMeter = 0;
			bmih.biYPelsPerMeter = 0;
			bmih.biClrUsed = 0;
			bmih.biClrImportant = 0;

			HDC memDC = CreateCompatibleDC(0);
			void*  pDib = 0;
			HBITMAP hBmp = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pDib, 0, 0);
			GdiFlush();

			HGDIOBJ old = SelectObject(memDC, hBmp);

			// ImageList_Draw WindowsXP
			ImageList_SetBkColor(hImgList, 0x0);
			ImageList_Draw(hImgList, stSHFileInfo.iIcon, memDC, 0, 0, ILD_NORMAL);
			SelectObject(memDC, old);
			DeleteDC(memDC);

			cImgList.Attach(hImgList);
			hIcon = cImgList.ExtractIcon(stSHFileInfo.iIcon);
			cImgList.Detach();

			if (hIcon != 0) {

				// Bitmap::FromHICON 0~255
				item.icon = Bitmap::FromHICON(hIcon);
				item.w = iconWidth;
				item.h = iconHeight;

				Gdiplus::RectF rc(float(2), float(2), float(iconSpacingWidth - 4), fcy);

				Gdiplus::Bitmap * nameIcon = new Bitmap(NAME_WIDTH, NAME_HEIGHT, &g);
				Gdiplus::Graphics * g2 = Gdiplus::Graphics::FromImage(nameIcon);
				g2->Clear(Gdiplus::Color(Gdiplus::ARGB(0)));

				g2->DrawString(item.name, item.name.GetLength(), &font_i, rc, &sf, &br_t);

				item.nameIcon = nameIcon;
				item.nameW = NAME_WIDTH;
				item.nameH = NAME_HEIGHT;

				delete g2;
			}

			DestroyIcon(hIcon);
			DeleteObject(hBmp);
			DestroyIcon(stSHFileInfo.hIcon);
		}

		return TRUE;
	}

	// 根据窗口大小设置图标位置
	void SetShellItemPosition()
	{
		int iconWidth = GetSystemMetrics(SM_CXICON);
		int iconHeight = GetSystemMetrics(SM_CYICON);
		static const int OFFSET_Y = 20;
		int x = 0;
		int y = OFFSET_Y;
		SIZE_T cItems = itemArray.GetCount();
		for (SIZE_T i = 0; i < cItems; i++) {
			XShellItem&	 item = itemArray[i];
			if (item.icon) {
				item.x = x + (iconSpacingWidth - iconWidth) / 2;
				item.y = y;
			}

			if (item.nameIcon) {
				item.nameX = x;
				item.nameY = y + iconHeight + 2;
			}

			WTL::CRect rect;
			GetClientRect(&rect);
			y += iconSpacingHeight;
			if (y + iconSpacingHeight >= rect.bottom) {
				x += iconSpacingWidth;
				y = OFFSET_Y;
			}
		}
	}

	// 描绘图标
	void DrawShellFolder(HWND Hwnd)
	{
		WTL::CPaintDC dc(Hwnd);
		Gdiplus::Graphics g(dc);

		dc.SetBkMode(TRANSPARENT);

		SIZE_T cItems = itemArray.GetCount();
		for (SIZE_T i = 0; i < cItems; i++) {
			const XShellItem&  item = itemArray[i];

			if (item.icon) {
				DrawImage(&dc, item.icon, item.hit, item.x, item.y, item.w, item.h);
			}

			if (item.nameIcon) {
				if (item.hit) {
					DrawImage(&dc, item.nameIcon, item.hit, item.nameX, item.nameY, item.nameW, item.nameH);
				}
				else {
					DrawImage(&g, item.nameIcon, item.nameX, item.nameY, item.nameW, item.nameH);
				}
			}
		}
	}

	void RunShellItem(ITEMIDLIST* pIID)
	{
		SHELLEXECUTEINFO info;
		info.cbSize = sizeof(SHELLEXECUTEINFO);
		info.fMask = SEE_MASK_INVOKEIDLIST;
		info.hwnd = m_hWnd;
		info.lpVerb = NULL;
		info.lpFile = NULL;
		info.lpParameters = NULL;
		info.lpDirectory = NULL;
		info.nShow = SW_SHOWNORMAL;
		info.hInstApp = NULL;
		info.lpIDList = pIID;
		ShellExecuteEx(&info);
	}

	// 桌面菜单
	void DesktopMenu()
	{
		HWND program = FindWindowEx(0, 0, _T("Progman"), _T("Program Manager"));
		HWND view = FindWindowEx(program, 0, _T("SHELLDLL_DefView"), 0);

		//HWND list = FindWindowEx(view, 0, _T("SysListView32"), 0);
		::SetForegroundWindow(view);

		POINT pt;
		GetCursorPos(&pt);

		LPARAM lp = pt.y << 16 | (pt.x - 32);
		::PostMessage(view, WM_LBUTTONDOWN, 0, lp);
		::PostMessage(view, WM_RBUTTONUP, 0, lp);
	}

	// 图标菜单
	void RightMenu(ITEMIDLIST* pIID)
	{
		HWND hwnd = m_hWnd;

		LPCONTEXTMENU pContextMenu = NULL;
		LPCONTEXTMENU pCtxMenuTemp = NULL;

		g_pIContext2 = NULL;
		g_pIContext3 = NULL;

		int menuType = 0;

		HRESULT hRslt = folder->GetUIObjectOf(
				hwnd,
				1,
				(LPCITEMIDLIST*) &(pIID),
				IID_IContextMenu,
				0,
				(void**) &pCtxMenuTemp);
		if (FAILED(hRslt)) {
			return;
		}

		POINT pt;
		GetCursorPos(&pt);

		if (pCtxMenuTemp->QueryInterface(IID_IContextMenu3, (void**) &pContextMenu) == NOERROR) {
			menuType = 3;
		}
		else if (pCtxMenuTemp->QueryInterface(IID_IContextMenu2, (void**) &pContextMenu) == NOERROR) {
			menuType = 2;
		}

		if (pContextMenu) {
			pCtxMenuTemp->Release();
		}
		else {
			pContextMenu = pCtxMenuTemp;
			menuType = 1;
		}

		if (menuType == 0) {
			return;
		}

		HMENU hMenu = CreatePopupMenu();
		hRslt = pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);
		if (FAILED(hRslt)) {
			return;
		}

#ifndef _WIN64
	#pragma warning(disable : 4244 4311)
#endif

		// subclass window
		WNDPROC oldWndProc = NULL;
		if (menuType > 1) {

			// only subclass if it is IID_IContextMenu2 or IID_IContextMenu3
			oldWndProc = (WNDPROC) SetWindowLongPtr(GWL_WNDPROC, (LONG) HookWndProc);
			if (menuType == 2) {
				g_pIContext2 = (LPCONTEXTMENU2) pContextMenu;
			}
			else {
				g_pIContext3 = (LPCONTEXTMENU3) pContextMenu;
			}
		}
		else {
			oldWndProc = NULL;
		}

		int cmd = ::TrackPopupMenu(
				hMenu,
				TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON,
				pt.x,
				pt.y,
				0,
				hwnd,
				0);

		// unsubclass
		if (oldWndProc) {
			SetWindowLongPtr(GWL_WNDPROC, (LONG) oldWndProc);
		}

#ifndef _WIN64
	#pragma warning(default : 4244 4311)
#endif
		if (cmd != 0) {
			CMINVOKECOMMANDINFO ci = { 0};
			ci.cbSize = sizeof(CMINVOKECOMMANDINFO);
			ci.hwnd = hwnd;
			ci.lpVerb = (LPCSTR) MAKEINTRESOURCE(cmd - 1);
			ci.nShow = SW_SHOWNORMAL;

			pContextMenu->InvokeCommand(&ci);
		}

		pContextMenu->Release();
		g_pIContext2 = NULL;
		g_pIContext3 = NULL;
		::DestroyMenu(hMenu);
	}

	static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message) {
		case WM_MENUCHAR:	// only supported by IContextMenu3
			if (g_pIContext3) {
				LRESULT lResult = 0;
				g_pIContext3->HandleMenuMsg2(message, wParam, lParam, &lResult);
				return(lResult);
			}
			break;
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
			if (wParam) {
				break;	// if wParam != 0 then the message is not menu-related
			}

		case WM_INITMENUPOPUP:
			if (g_pIContext2) {
				g_pIContext2->HandleMenuMsg(message, wParam, lParam);
			}
			else {
				g_pIContext3->HandleMenuMsg(message, wParam, lParam);
			}

			return(message == WM_INITMENUPOPUP ? 0 : TRUE);
			break;
		default:
			break;
		}

		return ::CallWindowProc((WNDPROC) GetProp(hWnd, TEXT("oldWndProc")), hWnd, message, wParam, lParam);
	}

	void DrawImage(Gdiplus::Graphics * g, Gdiplus::Bitmap * bmp, int x, int y, int w, int h)
	{
		Gdiplus::ImageAttributes ia;
		g->DrawImage(bmp, Gdiplus::Rect(x, y, w, h), 0, 0, w, h, Gdiplus::UnitPixel, &ia);
	}

	void DrawImage(WTL::CPaintDC * pDc, Gdiplus::Bitmap * bmp, BOOL isSelected, int x, int y, int w, int h)
	{
		if (isSelected) {
			HBITMAP hBmp;
			bmp->GetHBITMAP(RGB(49, 106, 108), &hBmp);
			pDc->DrawState(WTL::CPoint(x, y), WTL::CSize(w, h), hBmp, DSS_NORMAL | DST_BITMAP, NULL);
		}
		else {
			HICON hIcon;
			bmp->GetHICON(&hIcon);
			pDc->DrawIcon(WTL::CPoint(x, y), hIcon);
		}
	}
};
