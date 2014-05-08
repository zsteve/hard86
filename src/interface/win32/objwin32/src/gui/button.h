#ifndef BUTTON_H
#define BUTTON_H

#include "common_control.h"
#include <Windows.h>
#include <WindowsX.h>

namespace nsObjWin32{

namespace nsGUI{

	/**
	 * Button base class
	 */
	class DLLEXPORT Button : public CommonControl
	{
	public:

		Button(){
			m_className=L"BUTTON";
		}

		virtual ~Button(){}

		LRESULT GetState(){ return Button_GetState(m_hWnd); }

		LRESULT SetState(BOOL state){ return Button_SetState(m_hWnd, state); }

		void SetStyle(DWORD dwStyle, BOOL fRedraw=FALSE){ return Button_SetStyle(m_hWnd, dwStyle, fRedraw); }

		static const int DEF_H=20;
		static const int DEF_W=84;

	protected:

		HWND Create(DWORD dwExStyle,
					LPCTSTR lpWindowName,
					DWORD dwStyle,
					int x, int y,
					int w, int h,
					HWND hwndParent,
					HMENU hMenu);

	private:
	};

	class DLLEXPORT PushButton : public Button
	{
	public:

		PushButton(DWORD style=BS_PUSHBUTTON, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~PushButton(){}

		HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID);

	protected:

	private:
	};

	class DLLEXPORT CheckBox : public Button
	{
	public:

		CheckBox(DWORD style=BS_CHECKBOX, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~CheckBox(){}

		HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID);

		void SetCheck(int check){ Button_SetCheck(m_hWnd, check); }

		LRESULT GetCheck(){ return Button_GetCheck(m_hWnd); }
	};

	class DLLEXPORT BmpPushButton : public PushButton
	{
	public:

		BmpPushButton(DWORD style, DWORD exStyle=0){
			m_style|=style|BS_BITMAP;
			m_exStyle|=exStyle;
		}

		virtual ~BmpPushButton(){}

		HBITMAP SetImage(HBITMAP hBmp){
			return (HBITMAP)SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
		}
	};
}

}

#endif