#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <Windows.h>
#include "window.h"

namespace nsObjWin32{

namespace nsGUI{
	
	class DLLEXPORT UserWindow : public Window{

	public:

		virtual ATOM Register()=0;

		wchar_t* GetClassName(){return m_className;}

	protected:
	};

	class DLLEXPORT ToolWindow : public UserWindow{
	public:
		ToolWindow(){
			m_exStyle|=WS_EX_TOOLWINDOW;
		}

		virtual ~ToolWindow(){

		}

		ATOM Register(){
			WNDCLASSEX wcx;
			wcx.cbSize=sizeof(WNDCLASSEX);
			wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
			wcx.lpfnWndProc=&Base_WndProc;
			wcx.cbClsExtra=NULL;
			wcx.cbWndExtra=NULL;
			wcx.hInstance=m_hInstance;
			wcx.hIcon=NULL;
			wcx.hCursor=NULL;
			wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
			wcx.lpszMenuName=NULL;
			wcx.lpszClassName=m_className;
			wcx.hIconSm=NULL;
			return (RegisterClassEx(&wcx));
		}
	};

}

}

#endif