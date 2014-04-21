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
		SingleRegWatcher(){
			m_resId=IDD_REGWATCHER;
			m_regName=L"Undefined";
		}

		virtual ~SingleRegWatcher(){

		}

		INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
			switch(uMsg){
			case WM_INITDIALOG:
				{
					// Create subclass of edit box function
					SetWindowSubclass(GetDlgItem(hWnd, IDC_REGVALUE), &EditSubclassProc, m_editSubclassID, NULL);
				}
				break;
			case WM_CLOSE:
				{
					RemoveWindowSubclass(GetDlgItem(hWnd, IDC_REGVALUE), &EditSubclassProc, m_editSubclassID);
				}
			default:
				return 0;
			}
			return 0;
		}

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
		wstring m_regName;
		static const int m_maxDigits=4;
	};

	// Register watch window
	class RegisterWatcher : public Hard86ToolWindow{
	public:
		RegisterWatcher() : Hard86ToolWindow(m_registered){

		}

		virtual ~RegisterWatcher(){
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
#define MSGHANDLER(name) LRESULT CALLBACK On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		MSGHANDLER(Create);
#undef MSGHANDLER

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

		template<typename T>
		T*& Child(int id){
			return (T*&)m_children[id];
		}

		WinManager m_children;
	private:
		static bool m_registered;
	};

}

#endif