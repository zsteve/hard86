#ifndef EDITBOX_H
#define EDITBOX_H

#include <Windows.h>
#include "common_control.h"

namespace nsObjWin32{

namespace nsGUI{

	class DLLEXPORT EditBox : public CommonControl{
	public:
		EditBox(){
			m_className=L"EDIT";
			m_style|=ES_LEFT;
			m_exStyle|=WS_EX_CLIENTEDGE;
		}

		virtual ~EditBox(){

		}

		HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID);

		static const int DEF_H=20;
		static const int DEF_W=84;
	private:

	};

}

}

#endif