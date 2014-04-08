#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>

namespace nsWin32Component{

namespace nsWindows{
	
	class Window{
	public:

		Window(){};

		virtual ~Window(){};

		virtual bool Show(int nCmdShow);

		/**
		 * pure virtual wndproc function that will be called with
		 * a this pointer
		 */
		virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)=0;

		static LRESULT CALLBACK Base_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual HWND GetHWND(){return m_hWnd;}

	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;

		virtual HWND Create(DWORD dwExStyle,
					LPCTSTR lpWindowName,
					DWORD dwStyle,
					int x, int y,
					int w, int h,
					HWND hwndParent,
					HMENU hMenu)=0;

	private:
	};

}

}


#endif