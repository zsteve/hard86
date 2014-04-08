#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <Windows.h>
#include "window.h"

namespace nsWin32Component{

namespace nsWindows{
	
	class UserWindow : public Window{

	public:

		UserWindow(HINSTANCE hInstance){
			m_hInstance=hInstance;
		}

		UserWindow(){

		}

		virtual ~UserWindow(){
			
		}

		virtual ATOM Register()=0;

		wchar_t* GetClassName(){return m_className;}

	protected:
		static wchar_t* m_className;
		ATOM m_classAtom;
	};

}

}

#endif