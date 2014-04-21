#include "winmain.h"
#include "windows/mainframe.h"
#include "windows/winmanager.h"

using namespace nsObjWin32::nsGUI;
using namespace nsHard86Win32;

Application* Application::m_instance=NULL;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WriteDebug(L"Application startup", __FUNCTIONW__, __FILEW__, __LINE__);

	Application* app=Application::GetInstance(hInstance);
	app->m_mainFrame.Register();
	app->m_mainFrame.Create(640, 480, CW_USEDEFAULT, CW_USEDEFAULT);
	app->m_winManager.AddWindow(WND_MAINFRAME, &app->m_mainFrame);
	app->m_mainFrame.Show();

	MSG msg;
	while(GetMessage(&msg, NULL, NULL, NULL)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}