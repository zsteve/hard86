// io_port_watcher.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <WindowsX.h>

HANDLE mainThread=NULL;

unsigned short portNum=0;
static unsigned short portVal=0;

bool readWord=false;	// 16 bit?

HWND hDlg=NULL;
HANDLE hMainThread=NULL;

#define UPDATE_PORT_VAL WM_USER+1

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	switch(uMsg){
	case WM_INITDIALOG:
		::hDlg=hWnd;
		SetTimer(hWnd, 0, 30, NULL);
		break;
	case WM_TIMER:
		SetDlgItemInt(hWnd, IDC_PORTNUM, portVal, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_GO:
			{
				portNum=GetDlgItemInt(hWnd, IDC_PORTNUM, NULL, FALSE);
				SetDlgItemInt(hWnd, IDC_PORTNUM, (unsigned long)portNum, FALSE);
			}
			break;
		case IDC_16:
			readWord=Button_GetCheck(GetDlgItem(hWnd, IDC_16));
			break;
		}
		break;
	case UPDATE_PORT_VAL:
		SetDlgItemInt(hWnd, IDC_PORTVAL, (unsigned long)portVal, FALSE);
		break;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		EndDialog(hWnd, WM_QUIT);
		break;
	default:
		break;
	}
	return 0;
}

DWORD CALLBACK MainThread(LPVOID lpParam){
	DialogBox(::hModule, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	return 0;
}

int VirtualDevice_AcceptEmulationMutex(MUTEX a, sys_state_ptr b)
{
	if(::hDlg){
		if(readWord){
			portVal=b->io_bus[portNum];
			portVal<<=8;
			if(portNum<65535){
				portVal|=b->io_bus[portNum+1];
			}
		}else{
			portVal=b->io_bus[portNum];
		}
	}
	return 0;
}

int VirtualDevice_Terminate()
{
	MessageBoxA(NULL, "Terminating", NULL, NULL);
	DestroyWindow(::hDlg);
	WaitForSingleObject(hMainThread, INFINITE);
	return 0;
}

int VirtualDevice_Initialize(void* a, void* b)
{
	mainThread=CreateThread(NULL, NULL, MainThread, NULL, NULL, NULL);
	return 0;
}