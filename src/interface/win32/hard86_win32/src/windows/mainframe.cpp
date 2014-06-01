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

#include "../../../../../ext_itoa/ext_itoa.h"

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
	case H86_USER_INPUT:
		OnH86UserInput(hWnd, uMsg, wParam, lParam);
		break;
	case H86_BREAKPOINT_HIT:
		SendMessage(WM_COMMAND, ID_EXECUTION_BREAK, NULL);
		EmulatorThread::GetInstance()->NotifyMsgWindow();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Breakpoint Hit");
		break;
	case H86_TERMINATE:
		EmulatorThread::DisposeInstance();
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

		btns[5].iBitmap		=5;
		btns[5].idCommand	=ID_EXECUTION_TOGGLEBREAKPOINT;
		btns[5].fsState		=TBSTATE_ENABLED;
		btns[5].fsStyle		=TBSTYLE_BUTTON;
		btns[5].dwData		=0;
		btns[5].iString		=0;

		btns[6].iBitmap		=6;
		btns[6].idCommand	=ID_TOOLS_CALCULATOR;
		btns[6].fsState		=TBSTATE_ENABLED;
		btns[6].fsStyle		=TBSTYLE_BUTTON;
		btns[6].dwData		=0;
		btns[6].iString		=0;

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
	RegisterWatcher*& regWatcher=Child<RegisterWatcher>(REGWATCHER);
	regWatcher=new RegisterWatcher();
	regWatcher->Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	MemoryWatcher*& memWatcher=Child<MemoryWatcher>(MEMWATCHER);
	memWatcher=new MemoryWatcher();
	memWatcher->Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);

	StackWatcher*& stackWatcher=Child<StackWatcher>(STACKWATCHER);
	stackWatcher=new StackWatcher();
	stackWatcher->Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	stackWatcher->Show();

	BPList*& bpList=Child<BPList>(BPLIST);
	bpList=new BPList();
	bpList->Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	bpList->Show();

	FlagWatcher*& flagWatcher=Child<FlagWatcher>(FLAGWATCHER);
	flagWatcher=new FlagWatcher();
	flagWatcher->Create(CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, hWnd);
	flagWatcher->Show();

	// Check if the command line specifies a file to load
	if(Application::GetCmdArgsCount() > 1){
		if(File::GetExt(wstring(Application::GetCmdLine()[1]))==wstring(L"h86")){
			LoadProjectToFrontend(wstring(Application::GetCmdLine()[1]));
		}
		else{
			LoadFileToEmulator(wstring(Application::GetCmdLine()[1]), wstring(L""));
		};
	}
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
		MessageBox(hWnd, L"Hard86 - 8086 Emulator with support for virtual devices\n"
							L"Version 0.20 alpha \n"
							L"(c) 2014 Stephen Zhang - zsteve.phatcode.net/hard86\n"
							L"Hard86 is licensed under the GNU GPL v2\n"
							L"For the latest version and source, visit https://github.com/zsteve/hard86", L"About Hard86", MB_ICONINFORMATION);
		break;
	case ID_EXECUTION_RUN:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::SetSingleStep(false);
		EmulatorThread::Resume();
		EmulatorThread::SysMutex().Unlock();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Running");
		// Disable all tool windows
		EnableToolWindows(false);
		break;
	case ID_EXECUTION_BREAK:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::Suspend();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Suspended");
		if(isAnimating){
			SendMessage(WM_COMMAND, ID_EXECUTION_ANIMATE, NULL);
			EmulatorThread::SetSingleStep(false);
		}
		// Enable all tool windows
		EnableToolWindows(true);
		break;
	case ID_EXECUTION_RELOAD:
		if(!Emulator::HasInstance()) break;
		{
			Emulator* emulator=Emulator::GetInstance();
			EmulatorThread::DisposeInstance();
			EmulatorThread::GetInstance()->SetMsgWindow(this);
			emulator->Reset();
			emulator->SetSysMutex(EmulatorThread::SysMutex());
			emulator->SetStepThroughExternInt(Settings::GetNum(Settings::Nums::STEP_INTO_EXTERN_INT));
			EmulatorThread::NotifyMsgWindow();

			VDevList::TerminateAll();
			VDevList::InitializeAll();

			Child<StatusBar>(STATUSBAR)->SetStatus(L"Reloaded");
		}
		// Enable all tool windows
		EnableToolWindows(true);
		break;
	case ID_EXECUTION_STEPINTO:
		if(!Emulator::HasInstance()) break;
		EmulatorThread::SetSingleStep(true);
		EmulatorThread::Resume();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Suspended");
		// Enable all tool windows
		EnableToolWindows(true);
		break;
	case ID_EXECUTION_ANIMATE:
		if(!Emulator::HasInstance()) break;
		{
			if(isAnimating){
				KillTimer(hWnd, ANIMATION_TIMER);
				isAnimating=false;
			}
			else{
				SetTimer(hWnd, ANIMATION_TIMER, Settings::GetNum(Settings::Nums::ANIMATE_SPEED), NULL);
				isAnimating=true;
			}
		}
		break;
	case ID_EXECUTION_TOGGLEBREAKPOINT:
		if(!Emulator::HasInstance()) break;
		{
			uint32 addr=Child<DasmView>(DASMVIEW)->GetCursorSelection();
			if(Debugger::GetInstance()->BreakpointExists(addr))
				Debugger::GetInstance()->RemoveBreakpoint(addr);
			else
				Debugger::GetInstance()->AddBreakpoint(addr);
			Child<BPList>(BPLIST)->Update();
			InvalidateRect(Child<DasmView>(DASMVIEW)->GetHWND(), NULL, false);
		}
		break;
	case ID_FILE_NEWPROJECT:
		{
			SendMessage(WM_COMMAND, ID_FILE_CLOSE, NULL);
			SendMessage(WM_COMMAND, ID_FILE_CLOSEPROJECT, NULL);
			NewH86ProjectDlg newProjectDlg;
			newProjectDlg.CreateNoReturn();
		}
		break;
	case ID_FILE_LOADPROJECT:
		SendMessage(WM_COMMAND, ID_FILE_CLOSE, NULL);
		SendMessage(WM_COMMAND, ID_FILE_CLOSEPROJECT, NULL);
		LoadProject();
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Ready");
		break;
	case ID_FILE_CLOSEPROJECT:
		if(H86Project::HasInstance()){
			// Discard instances of objects
			H86Project::DisposeInstance();
			EmulatorThread::DisposeInstance();
			Emulator::DisposeInstance();
			VDevList::TerminateAll();
			VDevList::DisposeInstance();
			VDevList::GetInstance();

			Child<DasmView>(DASMVIEW)->Reset();
			Child<StatusBar>(STATUSBAR)->SetInfo(L"Closed project");
		}
		break;
	case ID_FILE_LOAD:
		SendMessage(WM_COMMAND, ID_FILE_CLOSE, NULL);
		SendMessage(WM_COMMAND, ID_FILE_CLOSEPROJECT, NULL);
		LoadFile();
		EmulatorThread::NotifyWindow(this);
		break;
	case ID_FILE_CLOSE:
		if(Emulator::HasInstance() && !H86Project::HasInstance()){
			// Discard instances of objects
			EmulatorThread::DisposeInstance();
			Emulator::DisposeInstance();
			VDevList::TerminateAll();
			VDevList::DisposeInstance();

			Child<DasmView>(DASMVIEW)->Reset();
			Child<StatusBar>(STATUSBAR)->SetInfo(L"Closed binary file");
		}
		break;
	case ID_TOOLS_SETTINGS:
		{
			SettingsDlg::GetInstance()->Create();
		}
		break;
	case ID_TOOLS_VIRTUALDEVICES:
		VDevDlg::GetInstance()->Create();
		break;
	case ID_TOOLS_CALCULATOR:
		ShellExecute(hWnd, L"open", L"calc", NULL, NULL, SW_SHOW);
		break;
	case ID_VIEW_FLAGS:
		Child<FlagWatcher>(FLAGWATCHER)->Show();
		break;
	case ID_VIEW_REGISTERS:
		Child<RegisterWatcher>(REGWATCHER)->Show();
		break;
	case ID_VIEW_STACK:
		Child<StackWatcher>(STACKWATCHER)->Show();
		break;
	case ID_VIEW_MEMORY:
		Child<MemoryWatcher>(MEMWATCHER)->Show();
		break;
	case ID_VIEW_BREAKPOINTS:
		Child<BPList>(BPLIST)->Show();
		break;
	}
}

MSGHANDLER(H86UpdateSysData){
	MUTEX sysMutex=(MUTEX)wParam;
	sys_state_ptr sysState=(sys_state_ptr)lParam;

	if(EmulatorThread::IsSingleStep() || EmulatorThread::State()==EmulatorThread::Suspended){

		BPList*& bpList=Child<BPList>(BPLIST);
		StackWatcher*& stackWatcher=Child<StackWatcher>(STACKWATCHER);
		MemoryWatcher*& memWatcher=Child<MemoryWatcher>(MEMWATCHER);
		RegisterWatcher*& regWatcher=Child<RegisterWatcher>(REGWATCHER);
		FlagWatcher*& flagWatcher=Child<FlagWatcher>(FLAGWATCHER);

		DasmView* dasmView=Child<DasmView>(DASMVIEW);
		dasmView->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		regWatcher->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		flagWatcher->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		memWatcher->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		stackWatcher->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);

		EmulatorThread::Suspend();
	}
	else if(EmulatorThread::State()==EmulatorThread::Terminated){
		Child<StatusBar>(STATUSBAR)->SetStatus(L"Terminated");
	}
}

MSGHANDLER(H86UserInput){
	sys_state_ptr sysState=(sys_state_ptr)lParam;

	if(EmulatorThread::IsSingleStep() || EmulatorThread::State()==EmulatorThread::Suspended){

		BPList*& bpList=Child<BPList>(BPLIST);
		StackWatcher*& stackWatcher=Child<StackWatcher>(STACKWATCHER);
		MemoryWatcher*& memWatcher=Child<MemoryWatcher>(MEMWATCHER);
		RegisterWatcher*& regWatcher=Child<RegisterWatcher>(REGWATCHER);
		FlagWatcher*& flagWatcher=Child<FlagWatcher>(FLAGWATCHER);

		DasmView* dasmView=Child<DasmView>(DASMVIEW);
		dasmView->SendMessage(H86_USER_INPUT, wParam, lParam);

		regWatcher->SendMessage(H86_USER_INPUT, wParam, lParam);

		flagWatcher->SendMessage(H86_USER_INPUT, wParam, lParam);

		memWatcher->SendMessage(H86_USER_INPUT, wParam, lParam);

		stackWatcher->SendMessage(H86_USER_INPUT, wParam, lParam);

	}
}

#undef MSGHANDLER

void MainFrame::EnableToolWindows(bool state){
	// Enable all tool windows
	{
		BPList*& bpList=Child<BPList>(BPLIST);
		StackWatcher*& stackWatcher=Child<StackWatcher>(STACKWATCHER);
		MemoryWatcher*& memWatcher=Child<MemoryWatcher>(MEMWATCHER);
		RegisterWatcher*& regWatcher=Child<RegisterWatcher>(REGWATCHER);
		FlagWatcher*& flagWatcher=Child<FlagWatcher>(FLAGWATCHER);

		bpList->SendMessage(WM_ENABLE, (WPARAM)state, NULL);
		stackWatcher->SendMessage(WM_ENABLE, (WPARAM)state, NULL);
		memWatcher->SendMessage(WM_ENABLE, (WPARAM)state, NULL);
		regWatcher->SendMessage(WM_ENABLE, (WPARAM)state, NULL);
		flagWatcher->SendMessage(WM_ENABLE, (WPARAM)state, NULL);
	}
}

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
	EmulatorThread::SetMsgWindow(this);

	// Load BIOS
	File biosFile(Application::GetAppDirectory()+strtowstr(Settings::GetPath(Settings::Paths::BIOS_PATH)));
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
	Debugger::Init(VDevList::GetInstance());

	Emulator* emulator=Emulator::GetInstance(EmulatorThread::SysMutex(),
		Debugger::BreakPointHit,
		Debugger::PreInstructionExecute,
		Debugger::PostInstructionExecute,
		Debugger::UserInput,
		fileData, fileSize, biosData, biosSize
		);

	emulator->SetStepThroughExternInt(Settings::GetNum(Settings::Nums::STEP_INTO_EXTERN_INT));
	emulator->SetStepThroughInt(Settings::GetNum(Settings::Nums::STEP_INTO_INT));

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
}

void MainFrame::LoadProjectToFrontend(const wstring& path){

	SetCurrentDirectory(File::GetDirectory(path).c_str());

	// Load to Application::project
	H86Project* project=H86Project::GetInstance(path);

	// Load project data
	Application* app=Application::GetInstance();

	if(VDevList::HasInstance()) VDevList::DisposeInstance();

	VDevList* vdevList=VDevList::GetInstance();

	vector<pair<string, string> > vdevListVect=project->Get_VDevList();
	for(vector<pair<string, string> >::iterator it=vdevListVect.begin();
		it!=vdevListVect.end();
		++it){
		// Load library
		vdevList->LoadVDevDLL(it->second);
	}
	
	LoadFileToEmulator(strtowstr(H86Project::GetInstance()->Get_BinaryPath()),
		strtowstr(H86Project::GetInstance()->Get_FASPath()));

	vector<pair<uint16, uint16> > bpListVect=project->Get_BPList();
	for(vector<pair<uint16, uint16> >::iterator it=bpListVect.begin();
		it!=bpListVect.end();
		++it){
		// Load breakpoint
		Debugger::GetInstance()->AddBreakpoint(DWORD_B(it->first, it->second));
	}
	Child<BPList>(BPLIST)->Update();
	Child<StatusBar>(STATUSBAR)->SetInfo(L"Loaded project");
}

}