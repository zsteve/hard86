#ifndef DIALOG_H
#define DIALOG_H

#include <Windows.h>
#include "window.h"
#include "global.h"

using namespace nsObjWin32::nsGlobal;

namespace nsObjWin32{

namespace nsGUI{

	class DLLEXPORT Dialog{
	public:

		Dialog()
		{
			m_hWnd=0;
			m_resId=0;
		}

		virtual ~Dialog(){}

		static INT_PTR CALLBACK Base_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual HWND Create(HWND hwndParent=NULL);

		virtual INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)=0;

		HWND GetHWND(){ return m_hWnd; }

	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;

		DWORD m_resId;
	private:
	};

}

}

#endif