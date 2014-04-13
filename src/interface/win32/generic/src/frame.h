#ifndef FRAME_H
#define FRAME_H

#include <Windows.h>
#include "userwindow.h"

namespace nsObjWin32{

namespace nsWindows{
	
	class Frame : public UserWindow, public HasText{
		
	public:

		Frame(){
			m_className=L"Frame_Class";
			m_style|=WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;
		}

		virtual ~Frame(){}

		virtual HWND Create(int w, int h, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT);

		ATOM Register();

	protected:

		HWND Create(DWORD dwExStyle,
					LPCTSTR lpWindowName,
					DWORD dwStyle,
					int x, int y,
					int w, int h,
					HWND hwndParent,
					HMENU hMenu);
	};

}

}

#endif