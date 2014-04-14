#include "scrollbar.h"

namespace nsObjWin32{

namespace nsWindows{

	HWND ScrollBar::Create(int x, int y, int w, int h, HWND hwndParent, int ID, int range, int pos){
		return Window::Create(m_exStyle, L"", m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

}

}