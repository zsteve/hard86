#include "static.h"

namespace nsObjWin32{

namespace nsWindows{

	HWND Static::Create(DWORD dwExStyle,
					LPCTSTR lpWindowName,
					DWORD dwStyle,
					int x, int y,
					int w, int h,
					HWND hwndParent,
					HMENU hMenu)
	{
		m_hWnd=CreateWindowEx(dwExStyle,
							m_className,
							lpWindowName,
							dwStyle,
							x, y,
							w, h,
							hwndParent, hMenu,
							m_hInstance, this);
		return m_hWnd;
	}

	// TextStatic
	HWND TextStatic::Create(LPCTSTR lpWindowName, int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Static::Create(m_exStyle, lpWindowName, m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

	// BmpStatic
	HWND BmpStatic::Create(LPCTSTR lpBmpResId, int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Static::Create(m_exStyle, lpBmpResId, m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

	// FrameStatic
	HWND FrameStatic::Create(LPCTSTR lpWindowName, int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Static::Create(m_exStyle, lpWindowName, m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

}

}