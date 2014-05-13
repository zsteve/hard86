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

#ifndef HARD86_SETTINGS_H
#define HARD86_SETTINGS_H

#include <Windows.h>

#include <vector>
#include <map>
#include <utility>
#include <string>

#include "global.h"

#include "../../../../lib/inih/cpp/INIReader.h"

#include "../../objwin32/src/gui/dialog.h"
#include "resource/resource.h"

using namespace std;

namespace nsHard86Win32{

	/**
	 * Singleton settings manager class
	 */
	class Settings{
	private:
		static Settings* m_instance;

		Settings();

		~Settings();

	public:
		static Settings* GetInstance(){
			if(m_instance) return m_instance;
			m_instance=new Settings();
			return m_instance;
		}

		static COLORREF GetColor(int id){
			return GetInstance()->m_colors.GetColor(id);
		}

		static HFONT GetFont(int id){
			return GetInstance()->m_fonts.GetFont(id);
		}

		static string GetPath(int id){
			return GetInstance()->m_paths.GetPath(id);
		}

		struct Colors{
			enum{
				// General
				SEL_COLOR,
				INACTIVE_SEL_COLOR,
				BK_COLOR,

				// CodeList
				CODELIST_ITEM_COLOR,
				CODELIST_CURRENTLINE_COLOR
			};
		};

		struct Fonts{
			enum{
				// General
				GUI_FONT,
				MONOSPACE_FONT
			};
		};

		struct Paths{
			enum{
				BIOS_PATH
			};
		};

	private:

		class{
		public:

			COLORREF GetColor(int id){
				return m_colors[id];
			}

		private:
			friend class Settings;
			void SetColor(int id, COLORREF c){
				m_colors[id]=c;
			}
			map<int, COLORREF> m_colors;
		}m_colors;

		class{
		public:
			HFONT GetFont(int id){
				return m_fonts[id];
			}

		private:
			friend class Settings;
			void SetFont(int id, HFONT f){
				m_fonts[id]=f;
			}

			map<int, HFONT> m_fonts;
		}m_fonts;

		class{
		public:
			string& GetPath(int id){
				return m_paths[id];
			}
		private:
			friend class Settings;
			void SetPath(int id, string p){
				if(p==""){
					OUT_DEBUG("SetPath() was given an empty path. Perhaps the ini file is corrupted?");
				}
				m_paths[id]=p;
			}

			map<int, string> m_paths;
		}m_paths;

		static const char m_appIniPath[];
		INIReader m_appIni;

		void ProcessINI();
	};

	using namespace nsObjWin32::nsGUI;

	class SettingsDlg : public Dialog{
	private:
		SettingsDlg(){
			m_resId=IDD_APPSETTINGS;
		}

		virtual ~SettingsDlg(){
			EndDialog(m_hWnd, WM_CLOSE);
		}

		static SettingsDlg* m_instance;
	public:

		static SettingsDlg* GetInstance(){
			if(m_instance){
				delete m_instance;
			}
			m_instance=new SettingsDlg();
			return m_instance;
		}

		virtual INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}

#endif