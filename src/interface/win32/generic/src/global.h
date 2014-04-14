#ifndef GLOBAL_H
#define GLOBAL_H

#include <Windows.h>
#include "window.h"

#define ALERT_ERR(msg)\
	MessageBox(NULL, L"Error", L"Error : "##L##msg, MB_ICONERROR);

namespace nsObjWin32{

namespace nsGlobal{

	using namespace nsObjWin32::nsWindows;

	extern const int DEF_BUTTON_HT;
	extern const int DEF_BUTTON_WID;

	int RectWidth(const RECT& rc);
	int RectHeight(const RECT& rc);
	int WindowWidth(HWND hWnd);
	int WindowHeight(HWND hWnd);
	int WindowWidth(const Window& w);
	int WindowHeight(const Window& w);
	int ClientWidth(HWND hWnd);
	int ClientHeight(HWND hWnd);

	template<typename T>
	T Center(T a, T b){
		return (a+b)/2;
	}

	template<typename T>
	T Center(T a){
		return a/2;
	}

}

}

#endif