#ifndef LISTBOX_H
#define LISTBOX_H

#include "common_control.h"
#include <Windows.h>
#include <WindowsX.h>
#include <vector>

namespace nsObjWin32{

namespace nsGUI{

	using namespace std;

	class DLLEXPORT ListBox : public CommonControl{
	public:
		ListBox(DWORD style=NULL, DWORD exStyle=NULL) : m_listData(0){
			m_className=L"LISTBOX";
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~ListBox(){}

		HWND Create(LPCTSTR lpName, int x, int y, int w, int h, HWND hwndParent, int ID);

		void push_back(wstring& elem){
			m_listData.push_back(elem);
		}

		void pop_back(){
			m_listData.pop_back();
		}

		vector<wstring>::iterator begin(){ return m_listData.begin(); }
		vector<wstring>::iterator end(){ return m_listData.end(); }

		void clear(){ m_listData.clear(); }
		bool empty(){ return m_listData.empty(); }

		void Update(){
			SendMessage(LB_RESETCONTENT, NULL, NULL);
			for(vector<wstring>::iterator it=m_listData.begin();
				it!=m_listData.end();
				++it){
				SendMessage(LB_ADDSTRING, NULL, (LPARAM)it->c_str());
			}
		}

	protected:

		vector<wstring> m_listData;

	};

}

}

#endif