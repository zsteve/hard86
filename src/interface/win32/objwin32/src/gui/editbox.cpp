#include "editbox.h"

namespace nsObjWin32{

namespace nsGUI{

	HWND EditBox::Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Window::Create(m_exStyle, lpName, m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

}

}