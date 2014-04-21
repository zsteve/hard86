#include "register_watcher.h"
#include "../resource/resource.h"
#include "../../../objwin32/src/gui/dialog.h"

namespace nsHard86Win32{

	bool RegisterWatcher::m_registered=false;

	const wchar_t RegisterWatcher::m_regNames[][6]=\
	{
		L"AX", L"CX", L"DX", L"BX", L"SP",
		L"BP", L"SI", L"DI", L"ES", L"CS",
		L"SS", L"DS", L"IP"
	};

	LRESULT CALLBACK RegisterWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_CREATE:
			{
				m_dragManager.AddWindow(m_hWnd);
				OnCreate(hWnd, uMsg, wParam, lParam);
				break;
			}
		case WM_MOVING:
			{
				Hard86ToolWindow::OnMoving(hWnd, uMsg, wParam, lParam);
				break;
			}
		case WM_SIZING:
			{
				Hard86ToolWindow::OnSizing(hWnd, uMsg, wParam, lParam);
				break;
			}
		case WM_NCLBUTTONDOWN:
			{
				Hard86ToolWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
				break;
			}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	void RegisterWatcher::CreateChildren(HWND hWnd){
		int ypos=0;
		int height=0, width=0;
		for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
			SingleRegWatcher*& w=Child<SingleRegWatcher>(i);
			w=new SingleRegWatcher();
			w->Create(hWnd);
			w->SetName(m_regNames[i]);
			SetWindowXY(w->GetHWND(), 0, ypos);
			height=WindowHeight(w->GetHWND());
			width=WindowWidth(w->GetHWND());
			ypos+=height;
		}
		SetWindowSize(hWnd, width, ypos+height);
	}

#define MSGHANDLER(name) LRESULT CALLBACK RegisterWatcher::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create){
		CreateChildren(hWnd);
		return 0;
	}

#undef MSGHANDLER
}
