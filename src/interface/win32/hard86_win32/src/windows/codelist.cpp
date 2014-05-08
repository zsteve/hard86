/*  Hard86 - An 8086 Emulator with support for virtual hardware
	
    Copyright (C) 2014 Stephen Zhang

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.	
*/

#include <WindowsX.h>

#include <utility>

#include "codelist.h"
#include "../../../objwin32/src/gui/global.h"
#include "../global.h"

using namespace nsObjWin32::nsGUI;

namespace nsHard86Win32{

bool CodeList::m_registered=false;

CodeList::CodeList(int textAlignStyle, bool hasScrollBar) : m_listData(0)
{
	m_textAlignStyle=textAlignStyle;
	m_style|=WS_CHILD;
	m_className=L"Hard86_CodeList";
	if(!m_registered){
		Register();
		m_registered=true;
	}

	m_itemHeight=16;
	m_curSel=0;
	m_extraSel=0;
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
		m_scrollBar.Create(ClientWidth()-ScrollBar::DEF_W, 0, ScrollBar::DEF_W, ClientHeight(), hWnd, SCROLLBAR);
		int itemsAtOnce=ClientHeight()/m_itemHeight;
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
	int w=WindowWidth();
	int h=WindowHeight();

	COLORREF defItemColor=Settings::GetColor(Settings::Colors::CODELIST_ITEM_COLOR);
	COLORREF itemColor=defItemColor;

	for(int i=m_basePos; i<m_listData.size(); i++, ypos+=m_itemHeight, itemColor=defItemColor){
		if(m_curSel==i){
			if(m_enabledState)
				itemColor=Settings::GetColor(Settings::Colors::SEL_COLOR);
			else
				itemColor=Settings::GetColor(Settings::Colors::INACTIVE_SEL_COLOR);
		}
		if(m_extraSel==i){
			if(m_enabledState)
				itemColor=Settings::GetColor(Settings::Colors::CODELIST_CURRENTLINE_COLOR);
			else
				itemColor=Settings::GetColor(Settings::Colors::INACTIVE_SEL_COLOR);
		}
		HBRUSH hBrush=CreateSolidBrush(itemColor);
		HGDIOBJ hOldBrush=SelectObject(hDC, (HGDIOBJ)hBrush);
		Rectangle(hDC, 0, ypos, w, ypos+m_itemHeight);
		SetBkColor(hDC, itemColor);
		if(m_textAlignStyle==CENTER)
			TextOut(hDC, Center(0, (int)(w-(m_listData[i].size()*tm.tmAveCharWidth))), ypos+1, m_listData[i].c_str(), m_listData[i].size());
		else if(m_textAlignStyle==LEFT)
			TextOut(hDC, 4, ypos+1, m_listData[i].c_str(), m_listData[i].size());
		SetBkColor(hDC, Settings::GetColor(Settings::Colors::BK_COLOR));
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
	}
	HBRUSH hBrush=CreateSolidBrush(Settings::GetColor(Settings::Colors::BK_COLOR));
	HGDIOBJ hOldBrush=SelectObject(hDC, (HGDIOBJ)hBrush);
	while(ypos<h){
		Rectangle(hDC, 0, ypos, w, ypos+m_itemHeight);
		ypos+=m_itemHeight;
	}

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

	int itemsAtOnce=ClientHeight()/m_itemHeight;
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