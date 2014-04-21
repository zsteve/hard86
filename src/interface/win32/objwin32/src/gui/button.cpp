#include "button.h"
#include <windows.h>

namespace nsObjWin32{

namespace nsGUI{

	HWND Button::Create(DWORD dwExStyle,
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
		int e=GetLastError();
		return m_hWnd;
	}

	// PushButton

	HWND PushButton::Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Button::Create(m_exStyle, lpName, m_style, x, y, w, h, hwndParent, (HMENU)ID);	}

	// Checkbox

	HWND CheckBox::Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Button::Create(m_exStyle, lpName, m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

}

}