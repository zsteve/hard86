#include <Windows.h>

#include "window.h"
#include "frame.h"
#include "progressbar.h"
#include "button.h"
#include "global.h"
#include "static.h"
#include "dialog.h"

#include "../vs2010/win32_generic_unit/win32_generic_unit/resource.h"
#include "../vs2010/win32_generic_unit/win32_generic_unit/resource1.h"

using namespace nsObjWin32::nsWindows;
using namespace nsObjWin32::nsGlobal;

class dlg : public Dialog{
public:
	dlg(DWORD id){
		m_resId=id;
	}

	 INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	 {
		 switch(uMsg){
		 default:
			 return 0;
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

	Frame* pfrm=new Frame();
	Frame& frm=*pfrm;
	frm.Register();
	frm.Create(320, 200);
	frm.Show(SW_SHOWDEFAULT);

	RetroProgressBar pb;
	pb.Register();
	pb.Create(frm.GetHWND(), ClientWidth(frm.GetHWND()), 24);
	pb.Show(SW_SHOWDEFAULT);

	pb.SendMessage(pb.CPM_SETPOS, 20, NULL);
	pb.SendMessage(pb.CPM_SETTEXT, (WPARAM)L"RetroProgressBar", NULL);

	HBITMAP hBmp;
	hBmp=LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));

	TextStatic bn;
	bn.Create(L"Text", 0, 48, DEF_BUTTON_WID, DEF_BUTTON_HT, frm.GetHWND(), 1001);
	bn.Show(SW_SHOWDEFAULT);

	dlg d(IDD_DIALOG1);
	d.Create(NULL);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}