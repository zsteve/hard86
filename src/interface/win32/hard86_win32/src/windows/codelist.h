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

#ifndef HARD86_CODELIST_H
#define HARD86_CODELIST_H

#include <Windows.h>
#include <vector>
#include <string>
#include <utility>
#include <cstdlib>
#include "../../../objwin32/src/gui/userwindow.h"
#include "../../../objwin32/src/gui/scrollbar.h"
#include "../settings.h"

using namespace nsObjWin32::nsGUI;

namespace nsHard86Win32{

class CodeList : public UserWindow{
public:
	CodeList(int textAlignStyle=CENTER, bool hasScrollBar=true);

	virtual ~CodeList(){
		DestroyWindow(m_hWnd);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND Create(int x, int y, int w, int h, HWND hwndParent, int id){
		Window::Create(m_exStyle, L"", m_style, x, y, w, h, hwndParent, (HMENU)id);
		Show();
		return m_hWnd;
	}

	ATOM Register();

	int GetCapacity(){ return ClientHeight()/m_itemHeight; }

	// Data interface
	/**
	 * Inserts item
	 * @param elem item to be inserted
	 * @param position to be inserted at
	 * @return true/false for success or failure
	 */
	bool Insert(const pair<uint32, std::wstring>& elem, int pos=-1){
		if(pos==-1){
			m_listData.push_back(elem);
			if(m_hasScrollBar){
				SetScrollRange(m_scrollBar.GetHWND(), SB_CTL, 0, m_listData.size(), false);
			}
			return true;
		}
		if(pos>m_listData.size()) return false;
		m_listData.insert(m_listData.begin()+pos, elem);

		if(m_hasScrollBar){
			SetScrollRange(m_scrollBar.GetHWND(), SB_CTL, 0, m_listData.size(), false);
		}
		return true;
	}

	void Clear(){ m_listData.clear(); }

	std::vector<pair<uint32, std::wstring> >::iterator begin(){ return m_listData.begin(); }
	std::vector<pair<uint32, std::wstring> >::iterator end(){ return m_listData.end(); }

	std::vector<pair<uint32, std::wstring> >::iterator GetCurSel(){ return m_listData.begin()+m_curSel; }

	void SetSelection(int i){
		m_curSel=i;
		InvalidateRect(m_hWnd, NULL, true);
	}

	void SetExtraSelection(int i){
		m_extraSel=i;
		InvalidateRect(m_hWnd, NULL, true);
	}

	vector<pair<int, COLORREF> >& GetSelectionList(){ return m_selList; }

	enum Styles{
		CENTER,
		LEFT
	};

protected:

#define MSGHANDLER(name) void On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	// Message handlers

	MSGHANDLER(Create);
	MSGHANDLER(Paint);
	MSGHANDLER(SetFont);
	MSGHANDLER(LButtonDown);
	MSGHANDLER(VScroll);
	MSGHANDLER(Enable);

#undef MSGHANDLER

	int m_textAlignStyle;

	std::vector<pair<uint32, std::wstring> > m_listData;

	int m_curSel;	// index of item currently selected by cursor
	int m_extraSel;	// extra selection
	int m_basePos;	// index of item at y-position 0

	vector<pair<int, COLORREF> > m_selList;	// extra selections list

	int m_itemHeight;	// height of each item

	int m_addrFieldWidth;	// width of address field

	static const int m_sidePadding=16;	// side padding, only relevant if left-aligned

	bool m_hasScrollBar;
	ScrollBar m_scrollBar;		// only if scrollbar is enabled

	enum Children{
		SCROLLBAR
	};

	bool m_enabledState;		// enabled/disabled state


private:
	static bool m_registered;
};

}


#endif