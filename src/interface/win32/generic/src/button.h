#ifndef BUTTON_H
#define BUTTON_H

#include "common_control.h"
#include <Windows.h>
#include <WindowsX.h>

namespace nsObjWin32{

namespace nsWindows{

	/**
	 * Button base class
	 */
	class Button : virtual public CommonControl, virtual public HasText
	{
	public:

		Button(){
			m_className=L"BUTTON";
		}

		virtual ~Button(){}

		LRESULT GetState(){return Button_GetState(m_hWnd);}

		LRESULT SetState(BOOL state){return Button_SetState(m_hWnd, state);}

		void SetStyle(DWORD dwStyle, BOOL fRedraw=FALSE){return Button_SetStyle(m_hWnd, dwStyle, fRedraw);}

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

	class PushButton : virtual public Button
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

	class CheckBox : virtual public Button
	{
	public:

		CheckBox(DWORD style=BS_CHECKBOX, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~CheckBox(){}

		HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID);

		void SetCheck(int check){Button_SetCheck(m_hWnd, check);}

		LRESULT GetCheck(){return Button_GetCheck(m_hWnd);}
	};

	class BmpPushButton : public PushButton, public HasBitmap
	{
	public:

		BmpPushButton(DWORD style=0, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~BmpPushButton(){}
	};
}

}

#endif