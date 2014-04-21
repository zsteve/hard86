#ifndef WINDOW_H
#define WINDOW_H

#define DLLIMPORT __declspec(dllimport)
#define DLLEXPORT __declspec(dllexport)

#include <Windows.h>

namespace nsObjWin32{

namespace nsGUI{

	class DLLEXPORT Window{
	public:

		static void SetHInstance(HINSTANCE hInstance);

		Window(){
			m_height=m_width=0;
			m_hWnd=0;
			m_style=m_exStyle=NULL;
			m_className=L"";
		};

		Window(const wchar_t* className, DWORD style=0, DWORD exStyle=0){
			m_className=(wchar_t*)className;
			m_style=style;
			m_exStyle=exStyle;
		}

		virtual ~Window(){}

		virtual bool Show(int nCmdShow=SW_SHOWDEFAULT);

		/**
		 * pure virtual wndproc function that will be called with
		 * a this pointer
		 */
		virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)=0;

		static LRESULT CALLBACK Base_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual HWND GetHWND() const { return m_hWnd; }

		LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		BOOL SetEnabled(bool enabled){ return EnableWindow(m_hWnd, enabled); }

		void SetExStyle(DWORD dwStyle){ m_exStyle=dwStyle; }

		void SetStyle(DWORD dwStyle){ m_style=dwStyle; }

		virtual BOOL Close(){ return DestroyWindow(m_hWnd); }

		int GetText(wchar_t lpBuffer[]){
			return GetWindowText(m_hWnd, lpBuffer, 80);
		}

		int SetText(wchar_t* lpName){
			return SetWindowText(m_hWnd, lpName);
		}

		int GetTextLength(){
			return GetWindowTextLength(m_hWnd);
		}

	protected:

		friend class Dialog;

		wchar_t* m_className;

		HWND m_hWnd;
		static HINSTANCE m_hInstance;

		virtual HWND Create(DWORD dwExStyle,
					LPCTSTR lpWindowName,
					DWORD dwStyle,
					int x, int y,
					int w, int h,
					HWND hwndParent,
					HMENU hMenu);

		int m_height;
		int m_width;

		DWORD m_style;
		DWORD m_exStyle;

		HFONT m_defFont;

	private:
	};
}

}


#endif