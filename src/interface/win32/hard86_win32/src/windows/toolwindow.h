#ifndef HARD86_TOOLWINDOW_H
#define HARD86_TOOLWINDOW_H

#include "../../../objwin32/src/gui/userwindow.h"
#include "winmanager.h"

using namespace nsObjWin32::nsGUI;

namespace nsHard86Win32{
	class Hard86ToolWindow : public ToolWindow, public StickyWindow{
	public:
		Hard86ToolWindow(bool& registeredFlag){
			m_style|=WS_SYSMENU;
			m_className=L"Hard86_ToolWindow";
			if(!registeredFlag){
				Register();
				registeredFlag=true;
			}
		}
		virtual ~Hard86ToolWindow(){}

		virtual HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL)=0;

#define MSGHANDLER(name) LRESULT CALLBACK On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

		MSGHANDLER(Sizing);
		MSGHANDLER(Moving);
		MSGHANDLER(NCLButtonDown);

#undef MSGHANDLER
	};
}

#endif