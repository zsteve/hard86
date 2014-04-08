#ifndef GLOBAL_H
#define GLOBAL_H

#include <Windows.h>

#define ALERT_ERR(msg)\
	MessageBox(NULL, L"Error", L"Error : "##L##msg, MB_ICONERROR);

namespace nsWin32Component{

namespace nsGlobal{
	int RectWidth(RECT& rc);
	int RectHeight(RECT& rc);
	int WindowWidth(HWND hWnd);
	int WindowHeight(HWND hWnd);
	int ClientWidth(HWND hWnd);
	int ClientHeight(HWND hWnd);
}

}

#endif