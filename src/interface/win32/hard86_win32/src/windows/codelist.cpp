#include <WindowsX.h>

#include <utility>

#include "codelist.h"
#include "../../../objwin32/src/gui/global.h"
#include "../global.h"

using namespace nsObjWin32::nsGUI;
using namespace nsObjWin32::nsGlobal;

namespace nsHard86Win32{

bool CodeList::m_registered=false;

CodeList::CodeList(bool hasScrollBar) : m_listData(0), m_sels(0)
{
	m_style|=WS_CHILD;
	m_className=L"Hard86_CodeList";
	if(!m_registered){
		Register();
		m_registered=true;
	}

	for(int i=0; i<120; i++){
		wchar_t str[16];
		_itow(i, str, 10);
		m_listData.push_back(std::wstring(L"Item ")+str);
	}

	m_itemHeight=16;
	m_curSel=0;
	m_basePos=0;

	m_hasScrollBar=hasScrollBar;

	m_enabledState=true;
}

ATOM CodeList::Register(){
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(WNDCLASSEX);
	wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wcx.lpfnWndProc=&Base_WndProc;
	wcx.cbClsExtra=NULL;
	wcx.cbWndExtra=NULL;
	wcx.hInstance=m_hInstance;
	wcx.hIcon=NULL;
	wcx.hCursor=NULL;
	wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName=NULL;
	wcx.lpszClassName=m_className;
	wcx.hIconSm=NULL;
	return (RegisterClassEx(&wcx));
}

LRESULT CALLBACK CodeList::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_CREATE:
		OnCreate(hWnd, uMsg, wParam, lParam);
		break;
	case WM_PAINT:
		OnPaint(hWnd, uMsg, wParam, lParam);
		break;
	case WM_SETFONT:
		OnSetFont(hWnd, uMsg, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, uMsg, wParam, lParam);
		break;
	case WM_VSCROLL:
		OnVScroll(hWnd, uMsg, wParam, lParam);
		break;
	case WM_ENABLE:
		OnEnable(hWnd, uMsg, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

#define MSGHANDLER(name) void  CodeList::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	if(m_hasScrollBar){
		m_scrollBar.Create(ClientWidth(hWnd)-DEF_SCROLLBAR_W, 0, DEF_SCROLLBAR_W, ClientHeight(hWnd), hWnd, SCROLLBAR);
		int itemsAtOnce=ClientHeight(hWnd)/m_itemHeight;
		SetScrollRange(m_scrollBar.GetHWND(), SB_CTL, 0, (m_listData.size() >= itemsAtOnce ? m_listData.size() : 1), true);
		SetScrollPos(m_scrollBar.GetHWND(), SB_CTL, 0, true);
		m_scrollBar.Show();
	}
}

MSGHANDLER(SetFont){
	m_defFont=(HFONT)wParam;
	if(LOWORD(lParam)){
		RECT rect;
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, true);
	}
	HDC hDC=GetDC(hWnd);

	SelectFont(hDC, (HFONT)wParam);
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	m_itemHeight=tm.tmHeight+2;

	ReleaseDC(hWnd, hDC);
}

MSGHANDLER(LButtonDown){
	if(!m_enabledState) return;
	int x=LOWORD(lParam);
	int y=HIWORD(lParam);
	int selIndex=(y/m_itemHeight);
	m_curSel=selIndex+m_basePos;
	
	RECT rect;
	GetClientRect(hWnd, &rect);
	InvalidateRect(hWnd, &rect, true);
}

MSGHANDLER(Paint){
	PAINTSTRUCT ps;
	HDC hDC=BeginPaint(hWnd, &ps);

	HGDIOBJ hOldPen=SelectObject(hDC, (HGDIOBJ)GetStockObject(NULL_PEN));
	HGDIOBJ hOldFont=SelectObject(hDC, (HGDIOBJ)m_defFont);

	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);

	int ypos=0;
	int w=WindowWidth(*this);
	int h=WindowHeight(*this);

	COLORREF defItemColor=Settings::GetColor(Settings::Color::CODELIST_ITEM_COLOR);
	COLORREF itemColor=defItemColor;

	for(int i=m_basePos; i<m_listData.size(); i++, ypos+=m_itemHeight, itemColor=defItemColor){
		if(m_curSel==i){
			if(m_enabledState)
				itemColor=Settings::GetColor(Settings::Color::SEL_COLOR);
			else
				itemColor=Settings::GetColor(Settings::Color::INACTIVE_SEL_COLOR);
		}else{
			for(int j=0; j<m_sels.size(); j++){
				if(m_sels[j].first==i){
					if(m_enabledState)
						itemColor=m_sels[j].second;
					else
						itemColor=Settings::GetColor(Settings::Color::INACTIVE_SEL_COLOR);
					break;
				}
			}
		}
		HBRUSH hBrush=CreateSolidBrush(itemColor);
		HGDIOBJ hOldBrush=SelectObject(hDC, (HGDIOBJ)hBrush);
		Rectangle(hDC, 0, ypos, w, ypos+m_itemHeight);
		SetBkColor(hDC, itemColor);
		TextOut(hDC, Center(0, (int)(w-(m_listData[i].size()*tm.tmAveCharWidth))), ypos+1, m_listData[i].c_str(), m_listData[i].size());
		SetBkColor(hDC, Settings::GetColor(Settings::Color::BK_COLOR));
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
	}
	HBRUSH hBrush=CreateSolidBrush(Settings::GetColor(Settings::Color::BK_COLOR));
	HGDIOBJ hOldBrush=SelectObject(hDC, (HGDIOBJ)hBrush);
	while(ypos<h){
		Rectangle(hDC, 0, ypos, w, ypos+m_itemHeight);
		ypos+=m_itemHeight;
	}

	SelectObject(hDC, (HGDIOBJ)GetStockObject(BLACK_PEN));

	MoveToEx(hDC, 0, 0, NULL);
	LineTo(hDC, w-DEF_SCROLLBAR_W-1, 0);
	LineTo(hDC, w-DEF_SCROLLBAR_W-1, h-1);
	LineTo(hDC, 0, h-1);
	LineTo(hDC, 0, 0);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);

	SelectObject(hDC, (HGDIOBJ)hOldPen);
	SelectObject(hDC, (HGDIOBJ)hOldFont);

	EndPaint(hWnd, &ps);
}

MSGHANDLER(VScroll){
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_ALL;
	GetScrollInfo(m_scrollBar.GetHWND(), SB_CTL, &si);
	switch(LOWORD(wParam)){
	case SB_LINEDOWN:
		if(m_curSel < m_listData.size()-1){
			m_curSel++;
			si.nPos++;
		}
		break;
	case SB_LINEUP:
		if(m_curSel > 0){
			m_curSel--;
			si.nPos--;
		}
		break;
	case SB_THUMBTRACK:
		m_curSel=HIWORD(wParam);
		si.nPos=si.nTrackPos;
		break;
	case SB_THUMBPOSITION:
		m_curSel=HIWORD(wParam);
		si.nPos=si.nTrackPos;
		break;
	}

	int itemsAtOnce=ClientHeight(hWnd)/m_itemHeight;
	if(m_curSel > (m_basePos+itemsAtOnce)){
		m_basePos=m_curSel-itemsAtOnce;
	}
	else if(m_curSel < m_basePos){
		m_basePos=m_curSel;
	}

	si.fMask=SIF_POS;
	SetScrollInfo(m_scrollBar.GetHWND(), SB_CTL, &si, TRUE);
	RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE);
}

MSGHANDLER(Enable){
	m_enabledState=(bool)wParam;
	if(m_hasScrollBar){
		EnableWindow(m_scrollBar.GetHWND(), (BOOL)wParam);
	}
	RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE);
}

#undef MSGHANDLER
}