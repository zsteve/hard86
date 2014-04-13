#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <Windows.h>
#include "window.h"

namespace nsObjWin32{

namespace nsWindows{
	
	class UserWindow : virtual public Window{

	public:

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

	class ToolWindow : public UserWindow{
	public:
		ToolWindow(){
			m_exStyle|=WS_EX_TOOLWINDOW;
		}

		virtual ~ToolWindow(){

		}
	};

}

}

#endif