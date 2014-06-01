#include "dialog.h"

namespace nsObjWin32{

namespace nsGUI{

	INT_PTR CALLBACK Dialog::Base_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_SETFONT:
			return 0;
		case WM_INITDIALOG:
			{
				Dialog* pDialog=(Dialog*)lParam;
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)pDialog);
				if(pDialog==nullptr){
					return 0;
				}
				return pDialog->DlgProc(hWnd, uMsg, wParam, lParam);
			}
		default:
			{
				Dialog* pDialog=(Dialog*)GetWindowLong(hWnd, GWL_USERDATA);
				if(pDialog==nullptr){
					return 0;
				}
				return pDialog->DlgProc(hWnd, uMsg, wParam, lParam);
			}
		}
		return 0;
	}

	HWND Dialog::Create(HWND hwndParent)
	{
		m_hInstance=Window::m_hInstance;
		return m_hWnd=CreateDialogParam(m_hInstance, MAKEINTRESOURCE(m_resId), hwndParent, Dialog::Base_DlgProc, (LPARAM)this);
	}

	void Dialog::CreateNoReturn(HWND hwndParent){
		m_hInstance=Window::m_hInstance;
		DialogBoxParam(m_hInstance, MAKEINTRESOURCE(m_resId), hwndParent, Dialog::Base_DlgProc, (LPARAM)this);
	}

	LRESULT Dialog::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){
		return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
	}

}

}