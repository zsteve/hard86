#ifndef DIALOG_H
#define DIALOG_H

#include <Windows.h>
#include "window.h"
#include "global.h"

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

		void Dialog::CreateNoReturn(HWND hwndParent=NULL);

		virtual INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)=0;
	
		LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		HWND GetHWND(){ return m_hWnd; }

		// Exactly the same as in Window

		int GetText(wchar_t lpBuffer[]) const{
			return GetWindowText(m_hWnd, lpBuffer, 80);
		}

		int SetText(const wchar_t* lpName) const{
			return SetWindowText(m_hWnd, lpName);
		}

		int GetTextLength() const{
			return GetWindowTextLength(m_hWnd);
		}

		std::wstring GetText() const{
			wchar_t* buffer=new wchar_t[GetTextLength()];
			GetText(buffer);
			std::wstring retv(buffer);
			delete[] buffer;
			return retv;
		}

		inline int WindowWidth(){ return nsGUI::WindowWidth(m_hWnd); }
		inline int WindowHeight(){ return nsGUI::WindowHeight(m_hWnd); }
		inline int ClientWidth(){ return nsGUI::ClientWidth(m_hWnd); }
		inline int ClientHeight(){ return nsGUI::ClientHeight(m_hWnd); }
		inline int XPos(){ return nsGUI::WindowX(m_hWnd); }
		inline int YPos(){ return nsGUI::WindowY(m_hWnd); }
		inline int SetSize(int w, int h){ return SetWindowSize(m_hWnd, w, h); }
		inline int SetWidth(int w){ return SetSize(w, WindowWidth()); }
		inline int SetHeight(int h){ return SetSize(WindowHeight(), h); }
		inline int SetXY(int x, int y){ return nsGUI::SetWindowXY(m_hWnd, x, y); }
		inline int SetX(int x){ return SetXY(x, YPos()); }
		inline int SetY(int y){ return SetXY(XPos(), y); }

	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;

		DWORD m_resId;
	private:
	};

}

}

#endif