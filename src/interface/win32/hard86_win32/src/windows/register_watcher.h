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

#ifndef HARD86_REGISTER_WATCHER_H
#define HARD86_REGISTER_WATCHER_H



#include <Windows.h>
#include <CommCtrl.h>

#include "../global.h"

#include "toolwindow.h"
#include "winmanager.h"
#include "../../../objwin32/src/gui/dialog.h"
#include "../resource/resource.h"

namespace nsHard86Win32{

	using namespace nsObjWin32::nsGUI;

	class SingleRegWatcher : public Dialog{
	public:
		SingleRegWatcher(int i){
			m_resId=IDD_REGWATCHER;
			m_regName=L"Undefined";
			m_regNum=i;
		}

		virtual ~SingleRegWatcher(){

		}

		INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,\
										UINT_PTR uIdSubclass, DWORD_PTR dwRefData){
			switch(uMsg){
			case WM_CHAR:
				{
					if(isxdigit((int)wParam) || !IsCharAlphaNumeric((int)wParam)){
						if(isxdigit((int)wParam)){
							if(GetWindowTextLength(hWnd) >= m_maxDigits){
								return TRUE;
							}
						}
						return DefSubclassProc(hWnd, uMsg, wParam, lParam);
					}
					return TRUE;
				}
			default:
				return DefSubclassProc(hWnd, uMsg, wParam, lParam);
			}
			return 0;
		}

		void SetName(const wstring& name){
			m_regName=name;
			SetDlgItemText(m_hWnd, IDC_REGNAME, name.c_str());
		}
	private:
		static const int m_editSubclassID=1;
		static const int m_maxDigits=4;

		wstring m_regName;
		int m_regNum;
	};

	// Register watch window
	class RegisterWatcher : public Hard86ToolWindow{
	public:
		RegisterWatcher() : Hard86ToolWindow(m_registered){
			m_enabled=true;
		}

		virtual ~RegisterWatcher(){
			for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
				delete m_regWatchers[i];
			}
			DestroyWindow(m_hWnd);
		}

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL){
			Window::Create(m_exStyle, L"Registers", m_style, x, y, w, h, hwndParent, hMenu);
			Show();
			return m_hWnd;
		}

	protected:
		// message handlers
#define MSGHANDLER(name) void  On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		MSGHANDLER(Create);
#undef MSGHANDLER

		bool m_enabled;

		void CreateChildren(HWND hWnd);

		enum Children{
			REGWATCHER_AX,
			REGWATCHER_CX,
			REGWATCHER_DX,
			REGWATCHER_BX,
			REGWATCHER_SP,
			REGWATCHER_BP,
			REGWATCHER_SI,
			REGWATCHER_DI,
			REGWATCHER_ES,
			REGWATCHER_CS,
			REGWATCHER_SS,
			REGWATCHER_DS,
			REGWATCHER_IP
		};

		static const wchar_t m_regNames[][6];

		SingleRegWatcher* m_regWatchers[REGWATCHER_IP+1];

		WinManager m_children;
	private:
		static bool m_registered;
	};

	class FlagWatcherDlg : public Dialog{
	public:
		FlagWatcherDlg(){
			m_resId=IDD_FLAGSWATCHER;
		}

		virtual ~FlagWatcherDlg(){

		}

		INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
	};

	class FlagWatcher : public Hard86ToolWindow{
	public:
		FlagWatcher() : Hard86ToolWindow(m_registered), m_flagWatcherDlg() {
			m_enabled=true;
		}

		virtual ~FlagWatcher(){
			DestroyWindow(m_hWnd);
		}

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL){
			Window::Create(m_exStyle, L"Flags", m_style, x, y, w, h, hwndParent, hMenu);
			Show();
			return m_hWnd;
		}

	protected:
		// message handlers
#define MSGHANDLER(name) void  On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		MSGHANDLER(Create);
#undef MSGHANDLER

		bool m_enabled;

		void CreateChildren(HWND hWnd);

		FlagWatcherDlg m_flagWatcherDlg;
	private:
		static bool m_registered;
	};
}

#endif