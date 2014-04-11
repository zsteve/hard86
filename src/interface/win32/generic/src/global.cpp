#include "global.h"
#include <Windows.h>

namespace nsObjWin32{

namespace nsGlobal{

	const int DEF_BUTTON_HT = 24;
	const int DEF_BUTTON_WID = 80;

	int RectWidth(const RECT& rc){
		return (rc.right-rc.left);
	}

	int RectHeight(const RECT& rc){
		return (rc.bottom-rc.top);
	}

	int WindowWidth(HWND hWnd){
		static RECT rc;
		GetWindowRect(hWnd, &rc);
		return RectWidth(rc);
	}

	int WindowHeight(HWND hWnd){
		static RECT rc;
		GetWindowRect(hWnd, &rc);
		return RectHeight(rc);
	}

	int ClientWidth(HWND hWnd){
		static RECT rc;
		GetClientRect(hWnd, &rc);
		return RectWidth(rc);
	}

	int ClientHeight(HWND hWnd){
		static RECT rc;
		GetClientRect(hWnd, &rc);
		return RectHeight(rc);
	}

}

}