#include "global.h"

namespace nsObjWin32{

namespace nsGUI{

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

	int WindowWidth(const Window& w){
		return WindowWidth(w.GetHWND());
	}

	int WindowHeight(const Window& w){
		return WindowHeight(w.GetHWND());
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

	int WindowX(HWND hWnd){
		static RECT rc;
		GetWindowRect(hWnd, &rc);
		return (rc.left);
	}

	int WindowY(HWND hWnd){
		static RECT rc;
		GetWindowRect(hWnd, &rc);
		return (rc.top);
	}

	int ClientX(HWND hWnd){
		static RECT rc;
		GetClientRect(hWnd, &rc);
		return (rc.left);
	}

	int ClientY(HWND hWnd){
		static RECT rc;
		GetClientRect(hWnd, &rc);
		return (rc.top);
	}

	int SetWindowSize(HWND hWnd, int w, int h){
		return MoveWindow(hWnd, WindowX(hWnd), WindowY(hWnd), w, h, true);
	}

	int SetChildWindowSize(HWND hWnd, int w, int h){
		return MoveWindow(hWnd, ClientX(hWnd), ClientY(hWnd), w, h, true);
	}

	int SetWindowXY(HWND hWnd, int x, int y){
		return MoveWindow(hWnd, x, y, WindowWidth(hWnd), WindowHeight(hWnd), true);
	}

	// Font functions
	int FontHeight(HDC hDC){
		static TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		return tm.tmHeight;
	}

	int FontWidth(HDC hDC){
		static TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		return tm.tmAveCharWidth;
	}

}

}