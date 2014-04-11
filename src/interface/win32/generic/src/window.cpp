#include "window.h"
#include "global.h"

namespace nsObjWin32{

namespace nsWindows{

	HINSTANCE Window::m_hInstance;
	
	void Window::SetHInstance(HINSTANCE hInstance){
		m_hInstance=hInstance;
	}
	
	bool Window::Show(int nCmdShow){
		BOOL canShow=ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);
		return canShow;
	}

	LRESULT Window::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){
		return ::SendMessageW(m_hWnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK Window::Base_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_NCCREATE:
			{
				CREATESTRUCT* cs=(CREATESTRUCT*)lParam;
				Window* pWindow=(Window*)cs->lpCreateParams;
				if(pWindow==nullptr){
					ALERT_ERR("pWindow is a NULL pointer");
					abort();
				}
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)pWindow);
				return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
			}
		default:
			{
				Window* pWindow=(Window*)GetWindowLongPtr(hWnd, GWL_USERDATA);
				if(pWindow==nullptr){
					return DefWindowProc(hWnd, uMsg, wParam, lParam);
				}
				return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
			}
		}
		return NULL;
	}

}

}

