#ifndef HARD86_SETTINGS_H
#define HARD86_SETTINGS_H

#include <Windows.h>

#include <vector>
#include <map>
#include <utility>
#include <string>

#include "global.h"

#include "../../../../lib/inih/cpp/INIReader.h"

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
			return GetInstance()->Colors.GetColor(id);
		}

		struct Color{
			enum{
				// General
				SEL_COLOR,
				INACTIVE_SEL_COLOR,
				BK_COLOR,

				// CodeList
				CODELIST_ITEM_COLOR,
			};
		};

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
		}Colors;

	private:
		static const char m_appIniPath[];
		INIReader m_appIni;

		void ProcessINI();
	};

}

#endif