#ifndef FRAME_H
#define FRAME_H

#include <Windows.h>
#include "userwindow.h"

namespace nsObjWin32{

namespace nsGUI{
	
	class DLLEXPORT Frame : public UserWindow{
		
	public:

		Frame(){
			m_className=L"Frame_Class";
			m_style|=WS_OVERLAPPEDWINDOW;
		}

		virtual ~Frame(){}

		virtual HWND Create(int w, int h, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT);

		ATOM Register();

	protected:
	};

}

}

#endif