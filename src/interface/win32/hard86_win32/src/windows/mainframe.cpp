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

#include "mainframe.h"

#include "../winmain.h"
#include "dasmview.h"
#include "../emulator.h"
#include "statusbar.h"

#include "../../../objwin32/src/gui/global.h"
#include "../resource/resource.h"

namespace nsHard86Win32{

HWND MainFrame::Create(int w, int h, int x, int y){
	return Window::Create(m_exStyle, L"Hard86 8086 Emulator", m_style, x, y, w, h, NULL, NULL);
}

ATOM MainFrame::Register(){
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(WNDCLASSEX);
	wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wcx.lpfnWndProc=&Base_WndProc;
	wcx.cbClsExtra=NULL;
	wcx.cbWndExtra=NULL;
	wcx.hInstance=m_hInstance;
	wcx.hIcon=NULL;
	wcx.hCursor=LoadCursor(Application::hInstance, IDC_ARROW);
	wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName=MAKEINTRESOURCE(IDR_MAINMENU);
	wcx.lpszClassName=m_className;
	wcx.hIconSm=LoadIcon(Application::hInstance, MAKEINTRESOURCE(IDI_ICON1));
	return (RegisterClassEx(&wcx));
}

LRESULT CALLBACK MainFrame::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_CREATE:
		OnCreate(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CLOSE:
		Application::SetExiting(true);
		EmulatorThread::SetMsgWindow(NULL);	// quick hack to make emulator thread exit without blocking
		SendMessage(WM_DESTROY, NULL, NULL);
		break;
	case WM_SIZING:
		OnSizing(hWnd, uMsg, wParam, lParam);
		break;
	case WM_MOVING:
		OnMoving(hWnd, uMsg, wParam, lParam);
		break;
	case WM_NCLBUTTONDOWN:
		OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		{
			PostQuitMessage(WM_QUIT);
		}
		break;
	case WM_COMMAND:
		OnCommand(hWnd, uMsg, wParam, lParam);
		break;
	case WM_TIMER:
		SendMessage(WM_COMMAND, ID_EXECUTION_STEPINTO, NULL);
		break;
	case H86_UPDATE_SYS_DATA:
		OnH86UpdateSysData(hWnd, uMsg, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void MainFrame::CreateChildren(HWND hWnd){
	// Create toolbar
	{
		TBBUTTON btns[NUM_TOOLBAR_BUTTONS];
		btns[0].iBitmap		=0;
		btns[0].idCommand	=ID_EXECUTION_RUN;
		btns[0].fsState		=TBSTATE_ENABLED;
		btns[0].fsStyle		=TBSTYLE_BUTTON;
		btns[0].dwData		=0;
		btns[0].iString		=0;

		btns[1].iBitmap		=1;
		btns[1].idCommand	=ID_EXECUTION_BREAK;
		btns[1].fsState		=TBSTATE_ENABLED;
		btns[1].fsStyle		=TBSTYLE_BUTTON;
		btns[1].dwData		=0;
		btns[1].iString		=0;

		btns[2].iBitmap		=2;
		btns[2].idCommand	=ID_EXECUTION_STEPINTO;
		btns[2].fsState		=TBSTATE_ENABLED;
		btns[2].fsStyle		=TBSTYLE_BUTTON;
		btns[2].dwData		=0;
		btns[2].iString		=0;

		btns[3].iBitmap		=3;
		btns[3].idCommand	=ID_EXECUTION_ANIMATE;
		btns[3].fsState		=TBSTATE_ENABLED;
		btns[3].fsStyle		=TBSTYLE_BUTTON;
		btns[3].dwData		=0;
		btns[3].iString		=0;

		btns[4].iBitmap		=4;
		btns[4].idCommand	=ID_EXECUTION_RELOAD;
		btns[4].fsState		=TBSTATE_ENABLED;
		btns[4].fsStyle		=TBSTYLE_BUTTON;
		btns[4].dwData		=0;
		btns[4].iString		=0;

		m_hwndToolbar=CreateToolbarEx(hWnd, 
										WS_VISIBLE | WS_CHILD | WS_BORDER,
										IDR_TOOLBAR1,
										NUM_TOOLBAR_BUTTONS,
										Application::hInstance,
										IDR_TOOLBAR1,
										btns,
										NUM_TOOLBAR_BUTTONS,
										16, 15,
										16, 15,
										sizeof(TBBUTTON)
										);

		m_toolbarHeight=nsObjWin32::nsGUI::WindowHeight(m_hwndToolbar);
	}

	// Create other windows
	DasmView*& dasmView=Child<DasmView>(DASMVIEW);
	dasmView=new DasmView();
	dasmView->Create(0, m_toolbarHeight, ClientWidth(), ClientHeight()-StatusBar::DEF_H-m_toolbarHeight, hWnd, DASMVIEW);
	dasmView->SetFont(Settings::GetFont(Settings::Fonts::MONOSPACE_FONT));
	dasmView->Show();

	StatusBar*& statusBar=Child<StatusBar>(STATUSBAR);
	statusBar=new StatusBar();
	statusBar->Create(0, ClientHeight()-StatusBar::DEF_H, ClientWidth(), hWnd, STATUSBAR);
	statusBar->Show();

	statusBar->SetStatus(L"Ready");
	statusBar->SetInfo(L"Application initialized");
}

// Message handlers
#define MSGHANDLER(name) void  MainFrame::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	m_dragManager.SetDockerState(true);

	CreateChildren(hWnd);

	// Create tool windows
	m_regWatcher.Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	m_memWatchers.push_back(MemoryWatcher());
	m_memWatchers.back().Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);

	// Register with EmulatorThread
	EmulatorThread::SetMsgWindow(this);
}

MSGHANDLER(Sizing){
	StickyWindow::OnSizing(hWnd, uMsg, wParam, lParam);
}

MSGHANDLER(Moving){
	StickyWindow::OnMoving(hWnd, uMsg, wParam, lParam);
}

MSGHANDLER(NCLButtonDown){
	StickyWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
	DefWindowProc(hWnd, uMsg, wParam, lParam);
}

MSGHANDLER(Command){
	static bool isAnimating=false;
	using namespace nsEmulator;
	switch(LOWORD(wParam)){
	case ID_FILE_EXIT:
		this->Close();
		break;
	case ID_HELP_ABOUTHARD86:
		MessageBox(hWnd, L"Hard86 - 8086 Emulator with support for virtual devices", L"About Hard86", MB_ICONINFORMATION);
		break;
	case ID_EXECUTION_RUN:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::SetSingleStep(false);
		EmulatorThread::Resume();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Running");
		break;
	case ID_EXECUTION_BREAK:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::Suspend();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Suspended");
		if(isAnimating){
			SendMessage(WM_COMMAND, ID_EXECUTION_ANIMATE, NULL);
			EmulatorThread::SetSingleStep(false);
		}
		break;
	case ID_EXECUTION_RELOAD:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::SetMsgWindow(NULL);	// quick hack to make emulator thread exit without blocking
		EmulatorThread::DisposeInstance();
		EmulatorThread::SetMsgWindow(this);	// set msg window again
		EmulatorThread::GetInstance();
		Emulator::GetInstance()->Reset();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Suspended");
		break;
	case ID_EXECUTION_STEPINTO:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::SetSingleStep(true);
		EmulatorThread::Resume();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Suspended");
		break;
	case ID_EXECUTION_ANIMATE:
		{
			if(isAnimating){
				KillTimer(hWnd, 10);
				isAnimating=false;
			}
			else{
				SetTimer(hWnd, 10, 10, NULL);
				isAnimating=true;
			}
		}
		break;
	case ID_FILE_LOADPROJECT:
		LoadProject();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Ready");
		break;
	case ID_FILE_CLOSEPROJECT:
		H86Project::DisposeInstance();
		Child<StatusBar>(STATUSBAR)->SetInfo(L"Closed project");
		break;
	case ID_FILE_LOAD:
		LoadFile();
		EmulatorThread::NotifyWindow(this);
		break;
	case ID_TOOLS_SETTINGS:
		{
			SettingsDlg::GetInstance()->Create();
		}
		break;
	case ID_TOOLS_VIRTUALDEVICES:
		VDevDlg::GetInstance()->Create();
		break;
	}
}

MSGHANDLER(H86UpdateSysData){
	MUTEX sysMutex=(MUTEX)wParam;
	sys_state_ptr sysState=(sys_state_ptr)lParam;

	if(EmulatorThread::IsSingleStep() || EmulatorThread::State()==EmulatorThread::Suspended){

		DasmView* dasmView=Child<DasmView>(DASMVIEW);
		dasmView->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		m_regWatcher.SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		for(int i=0; i<m_memWatchers.size(); i++){
			m_memWatchers[i].SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);
		}

		EmulatorThread::Suspend();
	}

}

#undef MSGHANDLER

void MainFrame::LoadFile(){

	wchar_t path[MAX_PATH]={ 0 };

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=m_hWnd;
	ofn.hInstance=Application::hInstance;
	ofn.lpstrFilter=L"Raw executables\0*.com\0All files\0*.*\0\0";
	ofn.lpstrCustomFilter=NULL;
	ofn.nMaxCustFilter=NULL;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=path;
	ofn.nMaxFile=MAX_PATH;
	ofn.lpstrFileTitle=NULL;
	ofn.nMaxFileTitle=NULL;
	ofn.lpstrInitialDir=NULL;
	ofn.lpstrTitle=NULL;
	ofn.Flags=NULL;

	if(!GetOpenFileName(&ofn)) return;	// if user pressed cancel, abort operation

	LoadFileToEmulator(wstring(path), wstring(L""));
	Child<StatusBar>(STATUSBAR)->SetInfo(L"Loaded file");
}

void MainFrame::LoadFileToEmulator(const wstring& path, const wstring& fasPath){

	// Load file
	File inFile(path);
	if(!inFile.Exists()){
		OUT_DEBUG("Input file does not exist");
		return;
	}
	inFile.Open();
	uint32 fileSize=inFile.Size();
	uint8* fileData=new uint8[fileSize];
	inFile.Read(fileData, fileSize);
	inFile.Close();

	// Load FAS file
	File fasFile(fasPath);
	if(fasFile.Exists()){
		fasFile.Open();
		uint32 fasSize=fasFile.Size();
		uint8* fasData=new uint8[fasSize];
		fasFile.Read(fasData, fasSize);
		fasFile.Close();
		EmulatorThread::GetInstance()->GetSymData()=SymbolData(fasData, fasSize);
		delete[] fasData;
	}
	else{
		EmulatorThread::GetInstance()->GetSymData()=SymbolData();
	}

	// Load BIOS
	File biosFile(Application::GetAppDirectory()+L"\\"+strtowstr(Settings::GetPath(Settings::Paths::BIOS_PATH)));
	if(!biosFile.Exists()){
		OUT_DEBUG_FATAL("BIOS file does not exist");
		return;
	}
	biosFile.Open();
	uint32 biosSize=biosFile.Size();
	uint8* biosData=new uint8[biosSize];
	biosFile.Read(biosData, biosSize);
	biosFile.Close();

	using namespace nsEmulator;
	using namespace nsDebugger;
	if(Emulator::HasInstance()) Emulator::DisposeInstance();
	Debugger::Reset();

	Emulator* emulator=Emulator::GetInstance(EmulatorThread::SysMutex(),
		Debugger::BreakPointHit,
		Debugger::PreInstructionExecute,
		Debugger::PostInstructionExecute,
		fileData, fileSize, biosData, biosSize
		);

	delete[] fileData;
	delete[] biosData;

	Debugger::GetInstance()->InitVDevs();

	OnH86UpdateSysData(m_hWnd, H86_UPDATE_SYS_DATA, (WPARAM)EmulatorThread::SysMutex().GetHandle(), (LPARAM)Emulator::GetInstance()->SystemState());
}

void MainFrame::LoadProject(){
	wchar_t path[MAX_PATH]={ 0 };

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=m_hWnd;
	ofn.hInstance=Application::hInstance;
	ofn.lpstrFilter=L"Hard86 Project files\0*.h86\0\0All files\0*.*\0\0";
	ofn.lpstrCustomFilter=NULL;
	ofn.nMaxCustFilter=NULL;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=path;
	ofn.nMaxFile=MAX_PATH;
	ofn.lpstrFileTitle=NULL;
	ofn.nMaxFileTitle=NULL;
	ofn.lpstrInitialDir=NULL;
	ofn.lpstrTitle=NULL;
	ofn.Flags=NULL;

	if(!GetOpenFileName(&ofn)) return;	// if user pressed cancel, abort operation

	LoadProjectToFrontend(wstring(path));
	Child<StatusBar>(STATUSBAR)->SetInfo(L"Loaded project");
}

void MainFrame::LoadProjectToFrontend(const wstring& path){

	// Load to Application::project
	H86Project* project=H86Project::GetInstance(path);

	// Load project data
	Application* app=Application::GetInstance();
	VDevList* vdevList=VDevList::GetInstance();

	vector<pair<string, string> > vdevListVect=project->Get_VDevList();
	for(vector<pair<string, string> >::iterator it=vdevListVect.begin();
		it!=vdevListVect.end();
		++it){
		// Load library
		HMODULE hMod=LoadLibrary(strtowstr(it->second).c_str());
		if(hMod){
			vdevList->Add(VDev((VDev::InitFunc)GetProcAddress(hMod, "VirtualDevice_Initialize"), \
				(VDev::TermFunc)GetProcAddress(hMod, "VirtualDevice_Terminate"), \
				(VDev::AcceptMutexFunc)GetProcAddress(hMod, "VirtualDevice_AcceptEmulationMutex"),
				strtowstr(it->second)));
		}
		
	}
	
	
	LoadFileToEmulator(strtowstr(H86Project::GetInstance()->Get_BinaryPath()),
		strtowstr(H86Project::GetInstance()->Get_FASPath()));

}

}