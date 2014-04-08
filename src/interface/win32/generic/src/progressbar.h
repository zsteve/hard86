#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <Windows.h>
#include "userwindow.h"

namespace nsWin32Component{

namespace nsWindows{

	class ProgressBar : public UserWindow{
	public:

		ProgressBar(HINSTANCE hInstance){
			m_hInstance=hInstance;
			m_className=L"ProgressBar_Class";
		}

		virtual ~ProgressBar(){}

		HWND Create(HWND hwndParent, int w, int h, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT);

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		ATOM Register();

		// Message handlers
		int OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);

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