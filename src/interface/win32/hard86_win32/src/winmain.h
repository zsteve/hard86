#ifndef WINMAIN_H
#define WINMAIN_H

#include <Windows.h>
#include "windows/mainframe.h"
#include "windows/winmanager.h"

namespace nsHard86Win32{

class Application{
private:
	Application(HINSTANCE hInstance)
	{
		m_hInstance=hInstance;
		Window::SetHInstance(hInstance);
	}

	~Application()
	{

	}

	static Application *m_instance;
public:

	static Application* GetInstance(){
		return m_instance;
	}

	static Application* GetInstance(HINSTANCE hInstance){
		if(m_instance) return GetInstance();
		m_instance=new Application(hInstance);
		return m_instance;
	}

	HINSTANCE m_hInstance;

	// Main Window
	MainFrame m_mainFrame;

	// Window manager
	WinManager m_winManager;
private:
protected:
};

}

#endif