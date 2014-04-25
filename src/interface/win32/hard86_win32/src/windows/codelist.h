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
	CodeList(bool hasScrollBar=true);

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

	// Interface
	/**
	 * Inserts item
	 * @param elem item to be inserted
	 * @param position to be inserted at
	 * @return true/false for success or failure
	 */
	bool Insert(const std::wstring& elem, int pos=-1){
		if(pos==-1){
			m_listData.push_back(elem);
			return true;
		}
		if(pos>m_listData.size()) return false;
		m_listData.insert(m_listData.begin()+pos, elem);
		return true;
	}

	void Clear(){ m_listData.clear(); }

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

	std::vector<std::wstring> m_listData;
	std::vector<std::pair<int, COLORREF>> m_sels;

	int m_curSel;	// index of item currently selected by cursor
	int m_basePos;	// index of item at y-position 0

	int m_itemHeight;	// height of each item

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