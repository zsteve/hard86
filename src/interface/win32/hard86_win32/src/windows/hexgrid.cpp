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

#include "hexgrid.h"
#include "../../../objwin32/src/gui/global.h"
#include "../global.h"
#include <WindowsX.h>

#include "../emulator.h"
#include "../../../../../system/multithreading/mutex/cpp/mutex.h"

#include "../../../../../ext_itoa/ext_itoa.h"

namespace nsHard86Win32{

bool HexGrid::m_registered=false;

HexGrid::HexGrid(bool hasScrollBar) : m_gridData(0), m_sel(0, 0){
	m_enabled=true;
	m_style|=WS_CHILD;
	m_className=L"Hard86_HexGrid";
	if(!m_registered){
		Register();
		m_registered=true;
	}

	m_hasScrollBar=hasScrollBar;

	m_defFont=NULL;
	m_itemWidth=m_itemHeight=16;	// these should never be used, as WM_SETFONT should be sent first.
	m_rows=0;
	m_hexColumns=m_ascColumns=0;
	m_basePos=0;
	m_isSelecting=false;
	m_lastClickedPane=false;
}

ATOM HexGrid::Register(){
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(WNDCLASSEX);
	wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wcx.lpfnWndProc=&Base_WndProc;
	wcx.cbClsExtra=NULL;
	wcx.cbWndExtra=NULL;
	wcx.hInstance=m_hInstance;
	wcx.hIcon=NULL;
	wcx.hCursor=LoadCursor(m_hInstance, IDC_ARROW);
	wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName=NULL;
	wcx.lpszClassName=m_className;
	wcx.hIconSm=NULL;
	return (RegisterClassEx(&wcx));
}

LRESULT CALLBACK HexGrid::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	case WM_SIZE:
		OnSize(hWnd, uMsg, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, uMsg, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(hWnd, uMsg, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, uMsg, wParam, lParam);
		break;
	case WM_VSCROLL:
		OnVScroll(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CHAR:
		OnChar(hWnd, uMsg, wParam, lParam);
		break;
	case WM_ENABLE:
		m_enabled=(bool)wParam;
		InvalidateRect(hWnd, NULL, false);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

#define MSGHANDLER(name) void HexGrid::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	SendMessage(WM_SETFONT, (WPARAM)GetStockObject(ANSI_FIXED_FONT), (LPARAM)TRUE);
	if(m_hasScrollBar){
		m_scrollBar.Create(ClientWidth()-ScrollBar::DEF_W, 0, ScrollBar::DEF_W, ClientHeight(), hWnd, SCROLLBAR);
		int itemsAtOnce=ClientHeight()/m_itemHeight;
		SetScrollRange(m_scrollBar.GetHWND(), SB_CTL, 0, (m_gridData.size()/m_hexColumns)-m_rows, true);
		SetScrollPos(m_scrollBar.GetHWND(), SB_CTL, 0, true);
		m_scrollBar.Show();
	}
}

MSGHANDLER(Paint){
	PAINTSTRUCT ps;
	HDC hDC=BeginPaint(hWnd, &ps);

	int w, h;
	w=ClientWidth();
	h=ClientHeight();

	HDC hMemDC=CreateCompatibleDC(hDC);
	HBITMAP hMemBitmap=CreateCompatibleBitmap(hDC, w, h);
	HGDIOBJ hOldMemBitmap=SelectObject(hMemDC, hMemBitmap);
	RECT rct;
	GetClientRect(hWnd, &rct);
	FillRect(hMemDC, &rct, GetStockBrush(WHITE_BRUSH));

	int numItems=m_gridData.size();

	HGDIOBJ hOldFont=SelectFont(hMemDC, m_defFont);

	// The contents of m_sel must be temporarily be sorted from
	// largest to smallest. They must be swapped back at the end
	// of paint in order for the selection code to work properly
	bool wasSwapped=false;
	if(m_sel.first > m_sel.second){
		swap(m_sel.first, m_sel.second);
		wasSwapped=true;
	}

	// Draw hex
	for(int i=0; i<m_rows; i++){
		for(int j=0; j<m_hexColumns; j++){
			int index=(i*m_hexColumns + j);
			if(index+m_basePos >= numItems){
				break;
			}

			wchar_t hex[3]=L"";
			ext_itow(m_gridData[index+m_basePos], hex, 16, 2);

			if(index >= m_sel.first && index <= m_sel.second){
				if(m_enabled)
					SetBkColor(hMemDC, Settings::GetColor(Settings::Colors::SEL_COLOR));
				else
					SetBkColor(hMemDC, Settings::GetColor(Settings::Colors::INACTIVE_SEL_COLOR));

				TextOut(hMemDC, j*m_itemWidth, i*m_itemHeight, hex, 2);
				SetBkColor(hMemDC, Settings::GetColor(Settings::Colors::BK_COLOR));
			}
			else{
				TextOut(hMemDC, j*m_itemWidth, i*m_itemHeight, hex, 2);
			}
		}
	}

	// separator line
	MoveToEx(hMemDC, m_hexColumns*m_itemWidth+(m_itemWidth/2), 0, NULL);
	LineTo(hMemDC, m_hexColumns*m_itemWidth+(m_itemWidth/2), h);

	// Draw ASCII
	for(int i=0; i<m_rows; i++){
		for(int j=0; j<m_ascColumns; j++){

			int index=(i*m_ascColumns + j);

			if(index+m_basePos >= numItems)
			{
				break;
			}

			wchar_t asc[2]={ (wchar_t)(char)m_gridData[index+m_basePos], L'\0' };
			if(index >= m_sel.first && index <= m_sel.second){
				if(m_enabled)
					SetBkColor(hMemDC, Settings::GetColor(Settings::Colors::SEL_COLOR));
				else
					SetBkColor(hMemDC, Settings::GetColor(Settings::Colors::INACTIVE_SEL_COLOR));

				TextOut(hMemDC,
					(j*(m_itemWidth/2))+(m_hexColumns*m_itemWidth)+m_itemWidth,
					i*m_itemHeight,
					asc,
					1);
				SetBkColor(hMemDC, Settings::GetColor(Settings::Colors::BK_COLOR));
			}
			else{
				TextOut(hMemDC,
						(j*(m_itemWidth/2))+(m_hexColumns*m_itemWidth)+m_itemWidth,
						i*m_itemHeight,
						asc,
						1);
			}
		}
	}

	// Swap back
	if(wasSwapped){
		swap(m_sel.first, m_sel.second);
	}

	BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldMemBitmap);
	DeleteBitmap(hMemBitmap);
	DeleteDC(hMemDC);
	SelectFont(hDC, hOldFont);

	EndPaint(hWnd, &ps);
}

MSGHANDLER(SetFont){
	m_defFont=(HFONT)wParam;
	if(LOWORD(lParam)){
		RECT rect;
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, false);
	}
	HDC hDC=GetDC(hWnd);

	SelectFont(hDC, (HFONT)wParam);
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	m_itemWidth=tm.tmAveCharWidth*2+8;
	m_itemHeight=tm.tmHeight+2;

	// Calculate number of columns and number of rows
	SetGridDimensions();

	ReleaseDC(hWnd, hDC);
}

MSGHANDLER(Size){
	SetGridDimensions();
	MoveWindow(hWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), true);
	if(m_hasScrollBar){
		SetWindowSize(m_scrollBar.GetHWND(), ScrollBar::DEF_W, HIWORD(lParam));
		SetWindowXY(m_scrollBar.GetHWND(), LOWORD(lParam)-ScrollBar::DEF_W, 0);
		SetScrollRange(m_scrollBar.GetHWND(), SB_CTL, 0, (m_gridData.size()/m_hexColumns)-m_rows, true);
		SetScrollPos(m_scrollBar.GetHWND(), SB_CTL, 0, true);
	}
	DefWindowProc(hWnd, uMsg, wParam, lParam);
}

MSGHANDLER(LButtonDown){
	if(m_enabled){
		int x, y;
		x=GET_X_LPARAM(lParam);
		y=GET_Y_LPARAM(lParam);
		SetSelection(x, y);
		InvalidateRect(hWnd, NULL, false);

		// Set keyboard focus
		SetFocus(hWnd);
		if(x > m_hexColumns*m_itemWidth+(m_itemWidth/2)){
			m_lastClickedPane=true;		// ascii pane
		}
		else{
			m_lastClickedPane=false;	// hex pane
		}
	}
}

MSGHANDLER(LButtonUp){
	if(m_enabled)
		m_isSelecting=false;
}

MSGHANDLER(MouseMove){
	if(m_isSelecting){
		int x, y;
		x=GET_X_LPARAM(lParam);
		y=GET_Y_LPARAM(lParam);
		SetSelection(x, y);
		InvalidateRect(hWnd, NULL, false);
	}
}

MSGHANDLER(VScroll){
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_ALL;
	GetScrollInfo(m_scrollBar.GetHWND(), SB_CTL, &si);
	switch(LOWORD(wParam)){
	case SB_LINEUP:
		if(m_basePos > m_hexColumns){
			m_basePos-=m_hexColumns;
		}
		else{
			m_basePos=0;
		}
		si.nPos--;
		break;
	case SB_LINEDOWN:
		if(m_basePos < (m_gridData.size()-m_hexColumns)){
			m_basePos+=m_hexColumns;
		}
		si.nPos++;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if(m_basePos < (m_gridData.size()-(m_hexColumns*m_rows))){
			m_basePos=HIWORD(wParam)*m_hexColumns;
			si.nPos=si.nTrackPos;
		}
		break;
	}
	si.fMask=SIF_POS;
	SetScrollInfo(m_scrollBar.GetHWND(), SB_CTL, &si, TRUE);
	RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE);
}

MSGHANDLER(Char){
	if(!m_enabled) return;
	static bool editingCell=false;
	if(!m_gridData.empty()){
		char c[2]={ (char)wParam, NULL };
		if(m_lastClickedPane==false){	// hex pane
			if(isxdigit(c[0]) && m_sel.first == m_sel.second){
				uint8 val=strtol(c, NULL, 16);
				if(!editingCell){
					m_gridData[m_sel.first]&=0x0f;
					m_gridData[m_sel.first]|=(val << 4);
					editingCell=true;
				}
				else{
					m_gridData[m_sel.first]&=0xf0;
					m_gridData[m_sel.first]|=(val & 0xf);
					editingCell=false;
					// add edit entry
					m_editList[m_sel.first]=m_gridData[m_sel.first];
					m_sel.first=m_sel.second+=1;
				}
			}
		}
		else{	// ascii pane
			m_gridData[m_sel.first]=(uint8)c[0];
			m_editList[m_sel.first]=m_gridData[m_sel.first];
			m_sel.first=m_sel.second+=1;
		}
		InvalidateRect(hWnd, NULL, false);
	}
}

#undef MSGHANDLER

void HexGrid::SetGridDimensions(){
	int w, h;
	w=ClientWidth();
	h=ClientHeight();
	float w_units=((w-m_itemWidth)/m_itemWidth);
	w_units/=3;
	m_hexColumns=w_units*2;
	m_ascColumns=w_units*2;
	m_rows=h/m_itemHeight;
}

void HexGrid::SetSelection(int curx, int cury){
	int selIndex;
	if(curx < m_hexColumns*m_itemWidth){
		int xIndex=(curx/m_itemWidth);
		int yIndex=(cury/m_itemHeight);
		selIndex=(yIndex*m_hexColumns)+xIndex;
	}
	else{
		curx-=(m_hexColumns*m_itemWidth)+m_itemWidth;
		int xIndex=(curx/(m_itemWidth/2));
		int yIndex=(cury/m_itemHeight);
		selIndex=(yIndex * m_ascColumns) + xIndex;
	}
	if(m_isSelecting){
		m_sel.second=selIndex;
	}
	else{
		m_sel.first=m_sel.second=selIndex;
		m_isSelecting=true;
	}
}

}