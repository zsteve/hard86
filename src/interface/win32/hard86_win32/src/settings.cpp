#include "settings.h"

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
#define SET_COLOR(a, b) Colors.SetColor(a, b)
#define GET_INT(a, b, c) m_appIni.GetInteger(a, b, c)
		// Load colors

		SET_COLOR(Color::SEL_COLOR, GET_INT("colors", "sel_color", 0));
		SET_COLOR(Color::INACTIVE_SEL_COLOR, GET_INT("colors", "inactive_sel_color", 0));
		SET_COLOR(Color::BK_COLOR, GET_INT("colors", "bk_color", 0));
		SET_COLOR(Color::CODELIST_ITEM_COLOR, GET_INT("colors", "codelist_item_color", 0));

#undef GET_INT
#undef SET_COLOR
	}

}