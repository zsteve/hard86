#include <WindowsX.h>

#include <utility>

#include "codelist.h"
#include "../../../objwin32/src/gui/global.h"
#include "../global.h"

using namespace nsObjWin32::nsGUI;
using namespace nsObjWin32::nsGlobal;

namespace nsHard86Win32{

bool CodeList::m_registered=false;

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

#define MSGHANDLER(name) LRESULT CALLBACK CodeList::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	if(m_hasScrollBar){
		m_scrollBar.Create(ClientWidth(hWnd)-DEF_SCROLLBAR_W, 0, DEF_SCROLLBAR_W, ClientHeight(hWnd), hWnd, SCROLLBAR);
		int itemsAtOnce=ClientHeight(hWnd)/m_itemHeight;
		SetScrollRange(m_scrollBar.GetHWND(), SB_CTL, 0, (m_listData.size() >= itemsAtOnce ? m_listData.size() : 1), true);
		SetScrollPos(m_scrollBar.GetHWND(), SB_CTL, 0, true);
		m_scrollBar.Show();
	}
	return 0;
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
	return 0;
}

MSGHANDLER(LButtonDown){
	if(!m_enabledState) return 0;
	int x=LOWORD(lParam);
	int y=HIWORD(lParam);
	int selIndex=(y/m_itemHeight);
	m_curSel=selIndex+m_basePos;
	
	RECT rect;
	GetClientRect(hWnd, &rect);
	InvalidateRect(hWnd, &rect, true);
	return 0;
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

	COLORREF itemColor=m_defItemColor;

	for(int i=m_basePos; i<m_listData.size(); i++, ypos+=m_itemHeight, itemColor=m_defItemColor){
		if(m_curSel==i){
			if(m_enabledState)
				itemColor=m_curSelColor;
			else
				itemColor=RGB(120, 120, 120);
		}else{
			for(int j=0; j<m_sels.size(); j++){
				if(m_sels[j].first==i){
					if(m_enabledState)
						itemColor=m_sels[j].second;
					else
						itemColor=RGB(120, 120, 120);
					break;
				}
			}
		}
		HBRUSH hBrush=CreateSolidBrush(itemColor);
		HGDIOBJ hOldBrush=SelectObject(hDC, (HGDIOBJ)hBrush);
		Rectangle(hDC, 0, ypos, w, ypos+m_itemHeight);
		SetBkColor(hDC, itemColor);
		TextOut(hDC, Center(0, (int)(w-(m_listData[i].size()*tm.tmAveCharWidth))), ypos+1, m_listData[i].c_str(), m_listData[i].size());
		SetBkColor(hDC, RGB(255, 255, 255));
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
	}
	HBRUSH hBrush=CreateSolidBrush(RGB(255, 255, 255));
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
	return 0;
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
	return 0;
}

MSGHANDLER(Enable){
	m_enabledState=(bool)wParam;
	if(m_hasScrollBar){
		EnableWindow(m_scrollBar.GetHWND(), (BOOL)wParam);
	}
	RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE);
	return 0;
}

#undef MSGHANDLER
}