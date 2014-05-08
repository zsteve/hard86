#include <Windows.h>

#include "global.h"
#include "window.h"
#include "frame.h"
#include "progressbar.h"
#include "button.h"
#include "static.h"
#include "dialog.h"
#include "scrollbar.h"

#include "../../vs2010/win32_generic_unit/win32_generic_unit/resource.h"
#include "../../vs2010/win32_generic_unit/win32_generic_unit/resource1.h"

using namespace nsObjWin32::nsGUI;

class MainFrame : public Frame
{
public:
	MainFrame(){
		this->m_className=L"MainFrame";
	}

	virtual ~MainFrame(){}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_CREATE:
		{

		}
			break;
		case WM_COMMAND:
		{

		}
			break;
		case WM_DESTROY:
			SendMessage(WM_CLOSE, NULL, NULL);
			break;
		case WM_CLOSE:
			PostQuitMessage(WM_QUIT);
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}
};

int CALLBACK WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	Window::SetHInstance(hInstance);

	MainFrame frm;
	frm.Register();
	frm.Create(320, 200);
	frm.Show(SW_SHOWDEFAULT);

	ScrollBar sb;
	sb.Create(0, 0, 100, WindowHeight(frm), frm.GetHWND(), 1002);
	sb.Show(SW_SHOWDEFAULT);

	RetroProgressBar pb;
	pb.Register();
	pb.Create(frm.GetHWND(), ClientWidth(frm.GetHWND()), 24);
	pb.Show(SW_SHOWDEFAULT);

	pb.SendMessage(pb.CPM_SETPOS, 20, NULL);
	pb.SendMessage(pb.CPM_SETTEXT, (WPARAM)L"RetroProgressBar", NULL);

	HBITMAP hBmp;
	hBmp=LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));

	PushButton bn;
	bn.Create(L"Text", 0, 48, 80, 24, frm.GetHWND(), 1001);
	bn.Show(SW_SHOWDEFAULT);
	bn.SetEnabled(false);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}