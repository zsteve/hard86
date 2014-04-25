#define DLLIMPORT __declspec(dllimport)
#define DLLEXPORT __declspec(dllexport)

#ifndef OBJWIN32_GUI_GLOBAL_H
#define OBJWIN32_GUI_GLOBAL_H

#include <Windows.h>
#include "window.h"

#define ALERT_ERR(msg)\
	MessageBox(NULL, L"Error", L"Error : "##L##msg, MB_ICONERROR);


namespace nsObjWin32{

namespace nsGlobal{

	using namespace nsObjWin32::nsGUI;

	DLLEXPORT int RectWidth(const RECT& rc);
	DLLEXPORT int RectHeight(const RECT& rc);
	DLLEXPORT int WindowWidth(HWND hWnd);
	DLLEXPORT int WindowHeight(HWND hWnd);
	DLLEXPORT int WindowWidth(const Window& w);
	DLLEXPORT int WindowHeight(const Window& w);
	DLLEXPORT int ClientWidth(HWND hWnd);
	DLLEXPORT int ClientHeight(HWND hWnd);
	DLLEXPORT int SetWindowSize(HWND hWnd, int w, int h);
	DLLEXPORT int SetWindowXY(HWND hWnd, int x, int y);

	DLLEXPORT int WindowX(HWND hWnd);
	DLLEXPORT int WindowY(HWND hWnd);

	template<typename T>
	DLLEXPORT T Center(T a, T b){
		return (a+b)/2;
	}

	template<typename T>
	DLLEXPORT T Center(T a){
		return a/2;
	}

}

}

#endif