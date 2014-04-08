#ifndef FRAME_H
#define FRAME_H

#include <Windows.h>
#include "userwindow.h"

namespace nsWin32Component{

namespace nsWindows{
	
	class Frame : public UserWindow{
		
	public:

		Frame(HINSTANCE hInstance){
			m_hInstance=hInstance;
			m_className=L"Frame_Class";
		}

		virtual ~Frame(){}

		HWND Create(int w, int h, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT);

		ATOM Register();

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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