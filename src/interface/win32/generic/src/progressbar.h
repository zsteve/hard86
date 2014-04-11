#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <Windows.h>
#include "userwindow.h"
#include "global.h"

namespace nsObjWin32{

namespace nsWindows{

	/**
	 * Custom progress bar
	 * Window Messages
	 * ~~~~~~~~~~~~~~~
	 * CPM_SETRANGE - Sets range.
	 * wParam - Minimum range value
	 * lParam - Maximum range value
	 *
	 * CPM_SETPOS - Sets position
	 * wParam - position
	 * 
	 * CPM_GETRANGE - Returns range
	 * wParam - If true, return max. If false, return min.
	 *
	 * CPM_GETPOS - Returns position
	 *
	 * CPM_SETCOLOR - Sets color
	 * wParam - COLORREF value for color
	 *
	 * CPM_SETTEXT - Sets text
	 * wParam - LPWSTR pointer to text (will be copied)
	 *
	 * CPM_SETTEXTCOLOR - Sets text color
	 * wParam - COLORREF value for color
	 *
	 * CPM_SETBKCOLOR - Sets background color
	 * wParam - COLORREF value for color
	 */

	class RetroProgressBar : public UserWindow{
	public:

		// Custom window messages
		enum{
			CPM_SETRANGE=WM_USER+1, CPM_GETRANGE, CPM_SETPOS, CPM_GETPOS,
			CPM_SETCOLOR, CPM_SETTEXT, CPM_SETTEXTCOLOR, CPM_SETBKCOLOR
		};

		RetroProgressBar(){
			m_className=L"RetroProgressBar_Class";
			m_rangeMin=0;
			m_rangeMax=100;
			m_value=0;
			m_color=RGB(0, 0, 0);
			m_textColor=RGB(120, 120, 120);
			m_bkColor=RGB(255, 255, 255);
			m_text=new wchar_t[1];
			*m_text=L'';
		}

		virtual ~RetroProgressBar(){
			delete[] m_text;
		}

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

		int m_rangeMin;
		int m_rangeMax;
		int m_value;
		COLORREF m_color;
		COLORREF m_textColor;
		COLORREF m_bkColor;
		wchar_t* m_text;
	};

}

}

#endif