#include "global.h"
#include <Windows.h>

namespace nsWin32Component{

namespace nsGlobal{
	int RectWidth(RECT& rc){
		return (rc.right-rc.left);
	}

	int RectHeight(RECT& rc){
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