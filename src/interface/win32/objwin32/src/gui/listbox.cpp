#include "listbox.h"

namespace nsObjWin32{

namespace nsGUI{

	HWND ListBox::Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID){
		Window::Create(m_exStyle, lpName, m_style, x, y, w, h, hwndParent, (HMENU)ID);
		Show();
		return m_hWnd;
	}

}

}