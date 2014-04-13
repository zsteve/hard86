#ifndef COMMON_CONTROL_H
#define COMMON_CONTROL_H

#include <windows.h>
#include "window.h"

namespace nsObjWin32{

namespace nsWindows{

	class CommonControl : virtual public Window{

	public:

		CommonControl(){
			m_style|=WS_CHILD;
		}

		virtual ~CommonControl(){

		}

		virtual HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID)=0;

	protected:

		static wchar_t* m_className;
		ATOM m_classAtom;

	private:
		// Some things that are irrelevant to common control
		// classes we will set as private to ensure that they
		// are not used...

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override {return -1;};
	};

}

}

#endif