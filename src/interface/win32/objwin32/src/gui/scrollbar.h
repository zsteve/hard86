#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "common_control.h"
#include <windows.h>


namespace nsObjWin32{

namespace nsGUI{

	class DLLEXPORT ScrollBar : public CommonControl
	{
	public:
		ScrollBar(DWORD style=SBS_VERT){
			m_className=L"SCROLLBAR";
			m_style|=style;
		}
		virtual ~ScrollBar(){}

		HWND Create(int x, int y, int w, int h, HWND hwndParent, int ID, int range=100, int pos=0);
	
	private:
		HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID){return NULL;};

	};

}

}

#endif