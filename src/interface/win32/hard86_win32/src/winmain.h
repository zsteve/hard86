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

#ifndef WINMAIN_H
#define WINMAIN_H

#include <Windows.h>
#include <CommCtrl.h>
#include "settings.h"
#include "h86project.h"
#include "windows/mainframe.h"
#include "windows/winmanager.h"

#include "emulator.h"

#include "../../../../emulator/debugger_engine/src/debugger.h"
#include "../../../../emulator/debugger_engine/src/vdev.h"

namespace nsHard86Win32{

class Application{
private:
	Application(HINSTANCE hInstance) : appDir(L"")
	{
		this->hInstance=hInstance;
		Window::SetHInstance(hInstance);
		settings=Settings::GetInstance();

		{
			wchar_t path[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, path);
			appDir=path;
			appDir+=L"\\";

			// Also set the DLL directory
			SetDllDirectory(appDir.c_str());
		}

		// initialize common controls
		INITCOMMONCONTROLSEX icx;
		icx.dwSize=sizeof(icx);
		icx.dwICC=ICC_BAR_CLASSES;
		InitCommonControlsEx(&icx);

		// load accelerator table
		hMainAccel=LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINACCEL));
		int e=GetLastError();

		// initialize nsDebugger::Debugger
		nsDebugger::Debugger::Init(nsVDev::VDevList::GetInstance());
	}

	static Application *m_instance;
public:

	~Application()
	{
		DestroyWindow(mainFrame->GetHWND());
		EmulatorThread::DisposeInstance();
		if(H86Project::HasInstance())
			H86Project::GetInstance()->UpdateFile();
	}

	static Application* GetInstance(){
		return m_instance;
	}

	static Application* GetInstance(HINSTANCE hInstance){
		if(m_instance) return GetInstance();
		m_instance=new Application(hInstance);
		return m_instance;
	}

	static wstring& GetAppDirectory(){ return GetInstance()->appDir; }

	static HINSTANCE hInstance;

	static HACCEL hMainAccel;

	// Main Window
	MainFrame* mainFrame;

	// Window manager
	WinManager winManager;

	// Settings manager
	Settings* settings;

	// Application directory
	wstring appDir;

	// Exiting condition
	bool isExiting;

	static bool IsExiting(){ return GetInstance()->isExiting; }
	static void SetExiting(bool b){ GetInstance()->isExiting=b; }

private:
protected:
};

extern wstring& GetAppDir();

}

#endif