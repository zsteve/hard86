#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <Windows.h>
#include "window.h"

namespace nsObjWin32{

namespace nsWindows{
	
	class UserWindow : public Window{

	public:

		virtual ATOM Register()=0;

		wchar_t* GetClassName(){return m_className;}

	protected:
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