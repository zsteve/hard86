#include "window.h"
#include "global.h"

namespace nsWin32Component{

namespace nsWindows{
	
	bool Window::Show(int nCmdShow){
		BOOL canShow=ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);
		return canShow;
	}

	LRESULT CALLBACK Window::Base_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_GETMINMAXINFO:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_NCCREATE:
			{
				CREATESTRUCT* cs=(CREATESTRUCT*)lParam;
				Window* pWindow=(Window*)cs->lpCreateParams;
				if(pWindow==nullptr){
					ALERT_ERR("Error : pWindow is a NULL pointer");
				}
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)pWindow);	// set this pointer
				return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
			}
		default:
			{
				Window* pWindow=(Window*)GetWindowLongPtr(hWnd, GWL_USERDATA);
				if(pWindow==nullptr){
					ALERT_ERR("Error : pWindow is a NULL pointer");
				}
				return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
			}
		}
		return NULL;
	}

}

}

