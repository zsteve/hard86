#ifndef HARD86_HEXGRID_H
#define HARD86_HEXGRID_H

#include <Windows.h>
#include <vector>
#include <string>
#include <utility>
#include <cstdlib>
#include "../../../objwin32/src/gui/userwindow.h"
#include "../../../objwin32/src/gui/scrollbar.h"

#include "../settings.h"

using namespace nsObjWin32::nsGUI;
using namespace std;

namespace nsHard86Win32{

class HexGrid : public UserWindow{

	typedef unsigned char uint8;

public:
	HexGrid(bool hasScrollBar=true);

	virtual ~HexGrid(){
		DestroyWindow(m_hWnd);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND Create(int x, int y, int w, int h, HWND hwndParent, int id){
		Window::Create(m_exStyle, L"", m_style, x, y, w, h, hwndParent, (HMENU)id);
		Show();
		return m_hWnd;
	}

	ATOM Register();

protected:

	// Window information
	HFONT m_defFont;

	enum Children{
		SCROLLBAR
	};

	bool m_hasScrollBar;
	ScrollBar m_scrollBar;	// only if scrollbar is enabled

	vector<uint8> m_gridData;

	int m_basePos;

	/*
		The format of the hex table is :
		[ hex data ] | [ ascii representation ].
		For every 2 chars in hex data, there is 1 character in ascii representation.
		So the ratio of characters of hex data to characters of ascii representation
		is 2:1.

		m_itemWidth is the width of a HEX entry.
		the width of an ascii entry is m_itemWidth/2
	*/

	// Dimensions
	int m_itemWidth, m_itemHeight;
	int m_rows;
	int m_hexColumns, m_ascColumns;

	// Selections
	pair<int, int> m_sel;
	bool m_isSelecting;

	// Message handlers
#define MSGHANDLER(name) void On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create);
	MSGHANDLER(Paint);
	MSGHANDLER(SetFont);
	MSGHANDLER(Size);
	MSGHANDLER(LButtonDown);
	MSGHANDLER(VScroll);
	MSGHANDLER(LButtonUp);
	MSGHANDLER(MouseMove);

#undef MSGHANDLER

	void SetGridDimensions();
	void SetSelection(int curx, int cury);

private:
	static bool m_registered;

};

}

#endif