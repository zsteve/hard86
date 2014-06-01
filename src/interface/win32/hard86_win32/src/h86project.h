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

#ifndef HARD86_H86PROJECT_H
#define HARD86_H86PROJECT_H

#include <stdexcept>
#include <utility>
#include <string>
#include <vector>

#include "global.h"

#include "resource/resource.h"

#include "../../objwin32/src/gui/dialog.h"

#include "../../objwin32/src/file/file.h"
#include "../../../../lib/rapidxml/rapidxml.hpp"
#include "../../../../lib/rapidxml/rapidxml_print.hpp"

namespace nsHard86Win32{

	using namespace rapidxml;
	using namespace std;
	using namespace nsObjWin32::nsFiles;

	/**
	 * Hard86 project file
	 * Singleton class
	 * (XML format)
	 */
	class H86Project{
	private:
		static H86Project* m_instance;

		H86Project(const wstring& path);

		~H86Project();

		File m_file;

	public:

		static H86Project* GetInstance(){
			if(!m_instance){
				throw std::runtime_error("H86Project::GetInstance() - NULL instance");
			}
			return m_instance;
		}

		static H86Project* GetInstance(const wstring& path){
			if(m_instance){
				delete m_instance;
			}
			m_instance=new H86Project(path);
			return m_instance;
		}

		static void DisposeInstance(){
			if(m_instance){
				delete m_instance;
				m_instance=NULL;
			}
		}

		static bool HasInstance(){
			return m_instance ? true : false;
		}

		void UpdateFile();

		/// @return a pair containing [seg, addr]
		pair<uint16, uint16> Get_LoadSegAddr();

		/// @return std::string path to binary file
		string Get_BinaryPath();

		/// @return std::string path to FAS file
		string Get_FASPath();

		/// @return std::string path to user vdev directory
		string Get_UserVDevPath();

		/// @return vector<pair<string, string> > list of names and paths of included vdevs
		vector<pair<string, string> > Get_VDevList();

		// Removes the VDevs node
		void Remove_VDevList();

		// Appends a vector<pair<string, string> > of VDev <name, path> 
		void Add_VDevList(vector<pair<string, string> >& vdevList);

		// @return vector<pair<uint16, uint16> > of breakpoint <seg, addr>
		vector<pair<uint16, uint16> > Get_BPList();

		// Removes the BPList node
		void Remove_BPList();

		// Appends a vector<pair<uint16, uint16>> to the BPList node
		void Add_BPList(vector<pair<uint16, uint16> >& bpList);

	protected:
		xml_document<>* m_doc;
		char* m_data;
	private:

	};

	using namespace nsObjWin32::nsGUI;

	class NewH86ProjectDlg : public Dialog {

	public:
		NewH86ProjectDlg() : m_projectPath(L""), m_binPath(L"") {
			m_resId=IDD_NEWPROJECT;
		}

		virtual ~NewH86ProjectDlg(){
			DestroyWindow(m_hWnd);
		}

		virtual INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		wstring m_projectPath;
		wstring m_binPath;
		wstring m_fasPath;
	};

}

#endif
