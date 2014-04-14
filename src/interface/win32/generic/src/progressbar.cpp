#include <windows.h>
#include "progressbar.h"
#include "global.h"

namespace nsObjWin32{

namespace nsWindows{

	using namespace nsGlobal;

	HWND RetroProgressBar::Create(HWND hwndParent, int w, int h, int x, int y){
		m_width=w;
		m_height=h;
		return Create(NULL, L"Frame Window",
			WS_CHILD,
			x, y, w, h, hwndParent, NULL);
	}

	ATOM RetroProgressBar::Register(){
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
		return (RegisterClassEx(&wcx));
	}

	LRESULT CALLBACK RetroProgressBar::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_PAINT:
			OnPaint(hWnd, wParam, lParam);
			break;
		case WM_CLOSE:
			PostQuitMessage(WM_QUIT);
			break;
		// Custom window messages
		case CPM_SETRANGE:
			{
				m_rangeMin=(int)wParam;
				m_rangeMax=(int)lParam;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case CPM_GETRANGE:
			{
				return (bool)wParam ? m_rangeMax : m_rangeMin;
			}
		case CPM_SETPOS:
			{
				m_value=(int)wParam;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case CPM_GETPOS:
			{
				return m_value;
			}
		case CPM_SETCOLOR:
			{
				m_color=(COLORREF)wParam;
				break;
			}
		case CPM_SETTEXT:
			{
				wchar_t* text=(wchar_t*)wParam;
				delete[] m_text;
				m_text=new wchar_t[wcslen(text)+1];
				wcscpy(m_text, text);
				break;
			}
		case CPM_SETTEXTCOLOR:
			{
				m_textColor=(COLORREF)wParam;
				break;
			}
		case CPM_SETBKCOLOR:
			{
				m_bkColor=(COLORREF)wParam;
				break;
			}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	HWND RetroProgressBar::Create(DWORD dwExStyle,
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

	int RetroProgressBar::OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hDC=BeginPaint(hWnd, &ps);

		RECT rc;
		GetWindowRect(hWnd, &rc);
		
		int w=RectWidth(rc);
		int h=RectHeight(rc);

		HPEN hPen=CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		HPEN hOldPen=(HPEN)SelectObject(hDC, (HGDIOBJ)hPen);

		HBRUSH hBrush=CreateSolidBrush(m_color);
		HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC, (HGDIOBJ)hBrush);

		SetBkColor(hDC, m_bkColor);

		MoveToEx(hDC, 0, 0, NULL);
		LineTo(hDC, (w-1), 0);
		LineTo(hDC, (w-1), (h-1));
		LineTo(hDC, 0, (h-1));
		LineTo(hDC, 0, 0);

		Rectangle(hDC, 2, 2, 2+(m_width-4)*((float)(m_value-m_rangeMin)/(m_rangeMax-m_rangeMin)), (h-2));

		{
			// Draw Text
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SetTextColor(hDC, m_textColor);
			TextOut(hDC, (Center(w)-(tm.tmAveCharWidth*wcslen(m_text)/2)), (Center(h)-(tm.tmHeight/2)), m_text, wcslen(m_text));
		}

		SelectObject(hDC, (HGDIOBJ)hOldPen);
		SelectObject(hDC, (HGDIOBJ)hOldBrush);

		DeleteObject(hPen);

		EndPaint(hWnd, &ps);
		return 0;
	}
}

}