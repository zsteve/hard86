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

#include "settings.h"
#include "global.h"

namespace nsHard86Win32{

	const char Settings::m_appIniPath[]={ "hard86.ini" };
	Settings* Settings::m_instance=NULL;

	Settings::Settings() : m_appIni(m_appIniPath)
	{
		// Check for INI parsing error
		if(m_appIni.ParseError()){
			OUT_DEBUG("INI parser reported parse error");
		}
		ProcessINI();
	}

	Settings::~Settings(){

	}

	void Settings::ProcessINI(){

// Extra stuff to make life easier
#define GET_INT(a, b, c) m_appIni.GetInteger(a, b, c)
#define SET_COLOR(a, b) m_colors.SetColor(a, RGB2COLORREF(b))
		// Load colors

		SET_COLOR(Colors::SEL_COLOR, GET_INT("colors", "sel_color", 0));
		SET_COLOR(Colors::INACTIVE_SEL_COLOR, GET_INT("colors", "inactive_sel_color", 0));
		SET_COLOR(Colors::BK_COLOR, GET_INT("colors", "bk_color", 0));
		SET_COLOR(Colors::CODELIST_ITEM_COLOR, GET_INT("colors", "codelist_item_color", 0));
		SET_COLOR(Colors::CODELIST_CURRENTLINE_COLOR, GET_INT("colors", "codelist_currentline_color", 0));

		// Load animate speed
		m_nums.SetNum(Nums::ANIMATE_SPEED, GET_INT("numbers", "animate_speed", 10));
		m_nums.SetNum(Nums::STEP_INTO_EXTERN_INT, GET_INT("numbers", "step_into_extern_int", 0));
		m_nums.SetNum(Nums::STEP_INTO_INT, GET_INT("numbers", "step_into_int", 0));

#undef SET_COLOR

#define SET_FONT(a, b) m_fonts.SetFont(a, b);
		// Load fonts
		// TODO : Load fonts from INI file
		SET_FONT(Fonts::GUI_FONT, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
		SET_FONT(Fonts::MONOSPACE_FONT, (HFONT)GetStockObject(ANSI_FIXED_FONT));

#undef SET_FONT

#undef GET_INT

#define GET_STR(a, b, c) m_appIni.Get(a, b, c)

#define SET_PATH(a, b) m_paths.SetPath(a, b)

		SET_PATH(Paths::BIOS_PATH, GET_STR("paths", "bios_path", ""));

#undef SET_PATH

#undef GET_STR
	}

	SettingsDlg* SettingsDlg::m_instance=NULL;

	INT_PTR SettingsDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg){
		case WM_CLOSE:
			EndDialog(hWnd, WM_CLOSE);
			break;

		default:
			return 0;
		}
		return 0;
	}

}