
#include <Windows.h>
#include "frame.h"

namespace nsObjWin32{

namespace nsWindows{

	HWND Frame::Create(int w, int h, int x, int y){
		return Create(m_exStyle, L"Frame Window",
						m_style,
						x, y, w, h, NULL, NULL);
	}

	ATOM Frame::Register(){
		WNDCLASSEX wcx;
		wcx.cbSize=sizeof(WNDCLASSEX);
		wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wcx.lpfnWndProc=&Base_WndProc;
		wcx.cbClsExtra=NULL;
		wcx.cbWndExtra=NULL;
		wcx.hInstance=m_hInstance;
		wcx.hIcon=NULL;
		wcx.hCursor=NULL;
		wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName=m_className;
		wcx.hIconSm=NULL;
		return (m_classAtom=RegisterClassEx(&wcx));
	}

	HWND Frame::Create(DWORD dwExStyle,
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
}

}
