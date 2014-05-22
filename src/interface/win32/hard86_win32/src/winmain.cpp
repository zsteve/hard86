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

#include "winmain.h"
#include "windows/mainframe.h"
#include "windows/winmanager.h"

using namespace nsObjWin32::nsGUI;
using namespace nsHard86Win32;

Application* Application::m_instance=NULL;
HINSTANCE Application::hInstance=NULL;
HACCEL Application::hMainAccel=NULL;

namespace nsHard86Win32{
	wstring& GetAppDir(){ return Application::GetAppDirectory(); }
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	OUT_DEBUG("WinMain() Startup");

	Application* app=Application::GetInstance(hInstance);
	app->mainFrame=new MainFrame();
	app->mainFrame->Register();
	app->mainFrame->Create(380, 480, CW_USEDEFAULT, CW_USEDEFAULT);
	app->winManager.AddWindow(WND_MAINFRAME, app->mainFrame);
	app->mainFrame->Show();

	MSG msg;
	while(GetMessage(&msg, NULL, NULL, NULL)){
		if(!TranslateAccelerator(app->mainFrame->GetHWND(), Application::hMainAccel, &msg))
			TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete app;
	return 0;
}