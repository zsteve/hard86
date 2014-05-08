#include "window.h"
#include "global.h"

namespace nsObjWin32{

namespace nsGUI{

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
				pWindow->m_hWnd=hWnd;
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
		return 0;
	}

	HWND Window::Create(DWORD dwExStyle,
				LPCTSTR lpWindowName,
				DWORD dwStyle,
				int x, int y,
				int w, int h,
				HWND hwndParent,
				HMENU hMenu)
	{
		m_width=w;
		m_height=h;
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

	inline int Window::WindowWidth(){ return nsGUI::WindowWidth(m_hWnd); }
	inline int Window::WindowHeight(){ return nsGUI::WindowHeight(m_hWnd); }
	inline int Window::ClientWidth(){ return nsGUI::ClientWidth(m_hWnd); }
	inline int Window::ClientHeight(){ return nsGUI::ClientHeight(m_hWnd); }
	inline int Window::XPos(){ return nsGUI::WindowX(m_hWnd); }
	inline int Window::YPos(){ return nsGUI::WindowY(m_hWnd); }
	inline int Window::SetSize(int w, int h){ return (m_style & WS_CHILD) ? nsGUI::SetChildWindowSize(m_hWnd, w, h) : nsGUI::SetWindowSize(m_hWnd, w, h); }
	inline int Window::SetWidth(int w){ return SetSize(w, WindowWidth()); }
	inline int Window::SetHeight(int h){ return SetSize(WindowHeight(), h); }
	inline int Window::SetXY(int x, int y){ return nsGUI::SetWindowXY(m_hWnd, x, y); }
	inline int Window::SetX(int x){ return SetXY(x, YPos()); }
	inline int Window::SetY(int y){ return SetXY(XPos(), y); }
}

}

