/*	Required because __declspec(dllexport) wants local definitions of
	static class data members
*/

#include "../../objwin32/src/gui/window.h"
#include "../../objwin32/src/gui/userwindow.h"

HINSTANCE nsObjWin32::nsGUI::Window::m_hInstance=NULL;