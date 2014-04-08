#include <Windows.h>

#include "frame.h"
#include "progressbar.h"
#include "global.h"

using namespace nsWin32Component::nsWindows;
using namespace nsWin32Component::nsGlobal;

int CALLBACK WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	Frame frm(hInstance);
	frm.Register();
	frm.Create(320, 200);
	frm.Show(SW_SHOWDEFAULT);

	ProgressBar pb(hInstance);
	pb.Register();
	pb.Create(frm.GetHWND(), ClientWidth(frm.GetHWND()), 10);
	pb.Show(SW_SHOWDEFAULT);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}