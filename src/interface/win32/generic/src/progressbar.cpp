#include <windows.h>
#include "progressbar.h"
#include "global.h"

namespace nsWin32Component{

namespace nsWindows{

	using namespace nsGlobal;

	HWND ProgressBar::Create(HWND hwndParent, int w, int h, int x, int y){
		return Create(NULL, L"Frame Window",
			WS_CHILD,
			x, y, w, h, hwndParent, NULL);
	}

	ATOM ProgressBar::Register(){
		WNDCLASSEX wcx;
		wcx.cbSize=sizeof(WNDCLASSEX);
		wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wcx.lpfnWndProc=&Base_WndProc;
		wcx.cbClsExtra=NULL;
		wcx.cbWndExtra=NULL;
		wcx.hInstance=m_hInstance;
		wcx.hIcon=NULL;
		wcx.hCursor=NULL;
		wcx.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName=m_className;
		wcx.hIconSm=NULL;
		return (m_classAtom=RegisterClassEx(&wcx));
	}

	LRESULT CALLBACK ProgressBar::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_PAINT:
			OnPaint(hWnd, wParam, lParam);
			break;
		case WM_CLOSE:
			PostQuitMessage(WM_QUIT);
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	HWND ProgressBar::Create(DWORD dwExStyle,
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

	// Message handlers

	int ProgressBar::OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hDC=BeginPaint(hWnd, &ps);

		RECT rc;
		GetWindowRect(hWnd, &rc);
		
		int w=RectWidth(rc);
		int h=RectHeight(rc);

		HPEN hPen=CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
		HPEN hOldPen=(HPEN)SelectObject(hDC, (HGDIOBJ)hPen);

		MoveToEx(hDC, 0, 0, NULL);
		LineTo(hDC, (w-1), 0);
		LineTo(hDC, (w-1), (h-1));
		LineTo(hDC, 0, (h-1));
		LineTo(hDC, 0, 0);

		SelectObject(hDC, (HGDIOBJ)hOldPen);

		DeleteObject(hPen);

		EndPaint(hWnd, &ps);
		return 0;
	}
}

}