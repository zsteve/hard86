#include "dialog.h"

using namespace nsObjWin32::nsGlobal;

namespace nsObjWin32{

namespace nsWindows{

	INT_PTR CALLBACK Dialog::Base_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_SETFONT:
			return 0;
		case WM_INITDIALOG:
			{
				Dialog* pDialog=(Dialog*)lParam;
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)pDialog);
				if(pDialog==nullptr){
					ALERT_ERR("pDialog is a NULL pointer");
					abort();
				}
				return pDialog->DlgProc(hWnd, uMsg, wParam, lParam);
			}
		default:
			{
				Dialog* pDialog=(Dialog*)GetWindowLong(hWnd, GWL_USERDATA);
				if(pDialog==nullptr){
					
				}
				return pDialog->DlgProc(hWnd, uMsg, wParam, lParam);
			}
		}
		return 0;
	}

	HWND Dialog::Create(HWND hwndParent)
	{
		return CreateDialogParam(m_hInstance, MAKEINTRESOURCE(m_resId), hwndParent, Dialog::Base_DlgProc, (LPARAM)this);
	}

}

}