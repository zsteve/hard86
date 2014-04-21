#include "winmanager.h"
namespace nsHard86Win32{

WinManager::WinManager(const wchar_t* defFontName, int defFontHeight){
	m_defFont=CreateFont(defFontHeight,
		NULL,
		0, 0, FW_DONTCARE, 0, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		FF_DONTCARE, defFontName);
	if(!m_defFont){
		MessageBox(NULL, L"Error : WinManager::WinManager() - m_defFont==NULL", L"Error", MB_ICONERROR);
	}
}

WinManager::~WinManager(){
	DeleteFont(m_defFont);
}

bool WinManager::SetDefFont(int id){
	if(!Exists(id)) return false;
	m_wndMap[id]->SendMessage(WM_SETFONT, (WPARAM)m_defFont, true);
}

bool WinManager::AddWindow(int id, Window* win){
	if(m_wndMap.count(id)>0) return false;	// cannot replace existing
	m_wndMap[id]=win;
}

bool WinManager::RemoveWindow(int id){
	if(m_wndMap.count(id)) return false;	// non existent
	m_wndMap.erase(id);
	return true;
}

}

