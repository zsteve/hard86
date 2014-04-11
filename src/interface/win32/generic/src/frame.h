#ifndef FRAME_H
#define FRAME_H

#include <Windows.h>
#include "userwindow.h"

namespace nsObjWin32{

namespace nsWindows{
	
	class Frame : public UserWindow{
		
	public:

		Frame(){
			m_className=L"Frame_Class";
		}

		virtual ~Frame(){
			DestroyWindow(m_hWnd);
		}

		virtual HWND Create(int w, int h, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT);

		ATOM Register();

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:

		virtual HWND Create(DWORD dwExStyle,
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