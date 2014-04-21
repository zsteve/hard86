#include "mainframe.h"
#include "../../../objwin32/src/gui/global.h"

using namespace nsObjWin32::nsGlobal;

namespace nsHard86Win32{

HWND MainFrame::Create(int w, int h, int x, int y){
	return Window::Create(m_exStyle, L"Hard86-Win32", m_style, x, y, w, h, NULL, NULL);
}

LRESULT CALLBACK MainFrame::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_CREATE:
		return OnCreate(hWnd, uMsg, wParam, lParam);
	case WM_CLOSE:
		SendMessage(WM_DESTROY, NULL, NULL);
		break;
	case WM_SIZING:
		OnSizing(hWnd, uMsg, wParam, lParam);
		break;
	case WM_MOVING:
		OnMoving(hWnd, uMsg, wParam, lParam);
		break;
	case WM_NCLBUTTONDOWN:
		OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		{
			PostQuitMessage(WM_QUIT);
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void MainFrame::CreateChildren(HWND hWnd){
	Child<CodeList>(DASM_CODE_LIST)=new CodeList;
	Child<CodeList>(DASM_CODE_LIST)->Create(L"Dasm", 0, 0, ClientWidth(hWnd)/2, ClientHeight(hWnd), hWnd, DASM_CODE_LIST);
	m_children.SetDefFont(DASM_CODE_LIST);
}

// Message handlers
#define MSGHANDLER(name) LRESULT CALLBACK MainFrame::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	m_dragManager.SetDockerState(true);

	CreateChildren(hWnd);

	// Create tool windows
	m_regWatcher.Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	m_memWatchers.push_back(MemoryWatcher());
	m_memWatchers.back().Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	return 0;
}

MSGHANDLER(Sizing){
	StickyWindow::OnSizing(hWnd, uMsg, wParam, lParam);
	return 0;
}

MSGHANDLER(Moving){
	StickyWindow::OnMoving(hWnd, uMsg, wParam, lParam);
	return 0;
}

MSGHANDLER(NCLButtonDown){
	StickyWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#undef MSGHANDLER

}