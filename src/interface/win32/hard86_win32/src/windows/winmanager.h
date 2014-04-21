#ifndef HARD86_WINMANAGER_H
#define HARD86_WINMANAGER_H

#include <map>
#include <windows.h>
#include <WindowsX.h>
#include "../../../objwin32/src/gui/window.h"
#include "../../../../../lib/stickywindows/StickyDragManager.h"

namespace nsHard86Win32{

using namespace nsObjWin32::nsGUI;

// Window Manager
class WinManager{
public:
	WinManager(const wchar_t* defFontName=L"Consolas", int defFontHeight=16);
	virtual ~WinManager();

	bool SetDefFont(int id);

	bool AddWindow(int id, Window* win);
	bool RemoveWindow(int id);

	bool Exists(int id){ return (bool)m_wndMap.count(id); }
	
	Window*& operator[](int id){ return (Window*)m_wndMap[id]; }
protected:
	std::map<int, Window*> m_wndMap;
	HFONT m_defFont;
private:
};

class StickyWindow{
public:
	StickyWindow(){}

#define MSGHANDLER(name) LRESULT CALLBACK On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Sizing){
		m_dragManager.SnapSize((LPRECT)lParam);
		return 0;
	}

	MSGHANDLER(Moving){
		POINT pt;
		GetCursorPos(&pt);
		vector<RECT> rects;
		RECT* pRect=(LPRECT)lParam;
		RECT saveR=*pRect;
		vector<HWND> *windows=m_dragManager.SnapMove(pRect, rects);
		//Correct for original mouse position if no snap behavior occurs...
		if(EqualRect(pRect, &saveR))
		{
			int dx=pt.x - pRect->left;
			int dy=pt.y - pRect->top;
			if((abs(dx-m_relativeOffset.x) > 5) || (abs(dy-m_relativeOffset.y) > 5))
				OffsetRect(pRect, pt.x-saveR.left-m_relativeOffset.x, pt.y-saveR.top-m_relativeOffset.y);
		}

		// Note - this will only look right in show-contents-while-dragging mode.
		// if you need better performance or OS aggreement - you'll have to write your own dragger.
		HDWP hDWP=BeginDeferWindowPos(windows->size());
		for(unsigned int i=0; i < windows->size(); i++)
			::DeferWindowPos(hDWP, (*windows)[i], 0, rects[i].left, rects[i].top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
		//			::SetWindowPos((*windows)[i], 0, rects[i].left, rects[i].top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
		EndDeferWindowPos(hDWP);
		return 0;
	}

	MSGHANDLER(NCLButtonDown){
		m_dragManager.Init(hWnd);
		// Register application windows.
		EnumThreadWindows(GetCurrentThreadId(), EnumWindows, (LPARAM)&m_dragManager);
		// Throw in the desktop window for good measure.
		m_dragManager.StartTrack();
		RECT r;
		GetWindowRect(hWnd, &r);
		m_relativeOffset.x=GET_X_LPARAM(lParam) - r.left;
		m_relativeOffset.y=GET_Y_LPARAM(lParam) - r.top;
		return 0;
	}

#undef MSGHANDLER

	static BOOL CALLBACK EnumWindows(
		HWND hwnd,      // handle to window
		LPARAM lParam   // application-defined value
		)
	{
		CStickyDragManager* smgr=(CStickyDragManager*)lParam;
		if(hwnd != smgr->GetActiveWindow())
			smgr->AddWindow(hwnd);
		return TRUE;
	}

protected:
	CStickyDragManager m_dragManager;
	POINT m_relativeOffset;
};
}

#endif