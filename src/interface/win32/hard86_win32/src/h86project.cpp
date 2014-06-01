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

#include "h86project.h"

#include "winmain.h"
#include "../../../../ext_itoa/ext_itoa.h"

namespace nsHard86Win32{

	H86Project* H86Project::m_instance=NULL;

	H86Project::H86Project(const wstring& path){
		m_file=File(path.c_str());
		if(!m_file.Exists()){
			OUT_DEBUG("m_file nonexistent");
			return;
		}
		m_file.Open();
		m_data=new char[m_file.Size()];
		m_file.Read(m_data, m_file.Size());
		m_file.Close();
		m_doc=new xml_document<>();
		try{
			m_doc->parse<0>(m_data);
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
		}
	}

	H86Project::~H86Project(){
		UpdateFile();
		delete m_doc;
		delete m_data;
	}

	void H86Project::UpdateFile(){
		// Write to file
		// Make filesize zero
		m_file.Open();
		m_file.SetPointer(0);
		m_file.SetEOF();
		char* xmlBuffer=new char[1048576];	// hopefully that is large enough!
		*(print(xmlBuffer, *m_doc, 0))=NULL;
		m_file.Write(xmlBuffer);
		m_file.Close();
		delete[] xmlBuffer;
	}

	// Manipulation functions
	pair<uint16, uint16> H86Project::Get_LoadSegAddr(){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return make_pair(0, 0);
			}
			node=node->first_node("Variables");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return make_pair(0, 0);
			}
			node=node->first_node("LoadSegAddr");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return make_pair(0, 0);
			}
			xml_attribute<> *seg, *addr;
			seg=node->first_attribute("seg");
			addr=node->first_attribute("addr");
			return make_pair(strtol(seg->value(), NULL, 16), strtol(addr->value(), NULL, 16));
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return make_pair(0, 0);
		}
	}

	string H86Project::Get_BinaryPath(){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return string("");
			}
			node=node->first_node("Paths");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return string("");
			}
			xml_attribute<> *path;
			path=node->first_attribute("binaryPath");
			return string(path->value());
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return string("");
		}
	}

	string H86Project::Get_FASPath(){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return string("");
			}
			node=node->first_node("Paths");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return string("");
			}
			xml_attribute<> *path;
			path=node->first_attribute("FASPath");
			return string(path->value());
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return string("");
		}
	}

	string H86Project::Get_UserVDevPath(){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return string("");
			}
			node=node->first_node("Paths");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return string("");
			}
			xml_attribute<> *path;
			path=node->first_attribute("userVDevPath");
			return string(path->value());
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return string("");
		}
	}

	vector<pair<string, string> > H86Project::Get_VDevList(){
		vector<pair<string, string> > vdevList(0);

		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return vector<pair<string, string> >(0);
			}
			node=node->first_node("VDevs");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return vector<pair<string, string> >(0);
			}
			node=node->first_node();
			while(node){
				xml_attribute<> *name, *path;
				name=node->first_attribute("name");
				path=node->first_attribute("path");
				vdevList.push_back(make_pair(string(name->value()), string(path->value())));

				node=node->next_sibling();
			}
			return vdevList;
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return vector<pair<string, string> >(0);
		}
	}

	void H86Project::Remove_VDevList(){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return;
			}
			xml_node<>* vdevNode=node->first_node("VDevs");
			if(!vdevNode){
				OUT_DEBUG("RapidXML failed to locate node");
				return;
			}
			node->remove_node(vdevNode);
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return;
		}
	}

	void H86Project::Add_VDevList(vector<pair<string, string> >& vdevList){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return;
			}
			xml_node<>* test=node->first_node("VDevs");
			if(!test){
				node->append_node(m_doc->allocate_node(node_element, "VDevs"));
				node=node->last_node();
			}
			else{
				node=test;
			}
			for(vector<pair<string, string> >::iterator it=vdevList.begin();
				it!=vdevList.end();
				++it){

				node->append_node(m_doc->allocate_node(node_element, "VDev"));
				xml_node<> *appendedNode=node->last_node();
				appendedNode->append_attribute(m_doc->allocate_attribute("name", m_doc->allocate_string(it->first.c_str())));
				appendedNode->append_attribute(m_doc->allocate_attribute("path", m_doc->allocate_string(it->second.c_str())));

			}
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return;
		}
	}

	vector<pair<uint16, uint16> > H86Project::Get_BPList(){
		vector<pair<uint16, uint16> > bpList(0);

		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return vector<pair<uint16, uint16> >(0);
			}
			node=node->first_node("BPList");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return vector<pair<uint16, uint16> >(0);
			}
			node=node->first_node();
			while(node){
				xml_attribute<> *name, *path;
				name=node->first_attribute("seg");
				path=node->first_attribute("addr");
				bpList.push_back(make_pair((uint16)strtol(name->value(), NULL, 16), (uint16)strtol(path->value(), NULL, 16)));

				node=node->next_sibling();
			}
			return bpList;
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return vector<pair<uint16, uint16> >(0);
		}
	}

	void H86Project::Remove_BPList(){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return;
			}
			xml_node<>* bpListNode=node->first_node("BPList");
			if(!bpListNode){
				return;
			}
			node->remove_node(bpListNode);
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return;
		}
	}

	void H86Project::Add_BPList(vector<pair<uint16, uint16> >& bpList){
		try{
			xml_node<>* node=m_doc->first_node("Hard86Project");
			if(!node){
				OUT_DEBUG("RapidXML failed to locate node");
				return;
			}
			xml_node<>* test=node->first_node("BPList");
			if(!test){
				node->append_node(m_doc->allocate_node(node_element, "BPList"));
				node=node->last_node();
			}
			else{
				node=test;
			}
			for(vector<pair<uint16, uint16> >::iterator it=bpList.begin();
				it!=bpList.end();
				++it){

				node->append_node(m_doc->allocate_node(node_element, "BP"));
				xml_node<> *appendedNode=node->last_node();
				appendedNode->append_attribute(m_doc->allocate_attribute("seg", m_doc->allocate_string(ext_itoa(it->first, 16).c_str())));
				appendedNode->append_attribute(m_doc->allocate_attribute("addr", m_doc->allocate_string(ext_itoa(it->second, 16).c_str())));

			}
		}
		catch(rapidxml::parse_error e){
			OUT_DEBUG("RapidXML parse error encountered");
			return;
		}
	}

	// NewH86ProjectDlg

	INT_PTR CALLBACK NewH86ProjectDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_CLOSE:
			EndDialog(hWnd, WM_QUIT);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
			case IDC_BROWSE_PROJECT_PATH:
				{
					wchar_t path[MAX_PATH]={ 0 };

					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize=sizeof(ofn);
					ofn.hwndOwner=m_hWnd;
					ofn.hInstance=Application::hInstance;
					ofn.lpstrFilter=L"Hard86 Project files\0*.h86\0\0All files\0*.*\0\0";
					ofn.nFilterIndex=1;
					ofn.lpstrFile=path;
					ofn.nMaxFile=MAX_PATH;

					if(!GetSaveFileName(&ofn)) break;	// if user pressed cancel, abort operation
					SetDlgItemText(hWnd, IDC_PROJECT_PATH, path);
				}
				break;
			case IDC_BROWSE_BINARY_PATH:
				{
					wchar_t path[MAX_PATH]={ 0 };

					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize=sizeof(ofn);
					ofn.hwndOwner=m_hWnd;
					ofn.hInstance=Application::hInstance;
					ofn.lpstrFilter=L"Binary files\0*.com\0\0All files\0*.*\0\0";
					ofn.nFilterIndex=1;
					ofn.lpstrFile=path;
					ofn.nMaxFile=MAX_PATH;

					if(!GetSaveFileName(&ofn)) break;	// if user pressed cancel, abort operation
					SetDlgItemText(hWnd, IDC_BINARY_PATH, path);
				}
				break;

			case IDC_BROWSE_FAS_PATH:
				{
					wchar_t path[MAX_PATH]={ 0 };

					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize=sizeof(ofn);
					ofn.hwndOwner=m_hWnd;
					ofn.hInstance=Application::hInstance;
					ofn.lpstrFilter=L"FASM symbol files\0*.fas\0\0All files\0*.*\0\0";
					ofn.nFilterIndex=1;
					ofn.lpstrFile=path;
					ofn.nMaxFile=MAX_PATH;

					if(!GetSaveFileName(&ofn)) break;	// if user pressed cancel, abort operation
					SetDlgItemText(hWnd, IDC_FAS_PATH, path);
				}
				break;
			case IDOK:
			{
					wchar_t tmp_path[MAX_PATH]={ 0 };
					GetDlgItemText(hWnd, IDC_PROJECT_PATH, tmp_path, MAX_PATH);
					m_projectPath=tmp_path;
					GetDlgItemText(hWnd, IDC_BINARY_PATH, tmp_path, MAX_PATH);
					m_binPath=File::GetRelativePath(tmp_path, m_projectPath);
					GetDlgItemText(hWnd, IDC_FAS_PATH, tmp_path, MAX_PATH);
					m_fasPath=File::GetRelativePath(tmp_path, m_projectPath);
					try{
						xml_document<>* doc=new xml_document<>();
						doc->append_node(doc->allocate_node(node_element, "Hard86Project"));
						xml_node<>* node=doc->first_node();
						node->append_node(doc->allocate_node(node_element, "Variables"));
						node=node->first_node();
						node->append_node(doc->allocate_node(node_element, "LoadSegAddr"));
						node=node->first_node();
						node->append_attribute(doc->allocate_attribute("seg", doc->allocate_string("0")));
						node->append_attribute(doc->allocate_attribute("addr", doc->allocate_string("0")));
						node=node->parent();	// Variables
						node=node->parent();	// Hard86Project
						node->append_node(doc->allocate_node(node_element, "Paths"));
						node=node->first_node("Paths");
						node->append_attribute(doc->allocate_attribute("binaryPath", doc->allocate_string(wstrtostr(m_binPath).c_str())));
						node->append_attribute(doc->allocate_attribute("FASPath", doc->allocate_string(wstrtostr(m_fasPath).c_str())));
						node->append_attribute(doc->allocate_attribute("userVDevPath", ""));
						node=node->parent();	// Hard86Project
						node->append_node(doc->allocate_node(node_element, "VDevs"));
						node->append_node(doc->allocate_node(node_element, "BPList"));

						File projectFile(m_projectPath);
						if(projectFile.Exists()){
							projectFile.Delete();
						}
						projectFile.Create();
						projectFile.Open();
						
						char* xmlBuffer=new char[1024*1024];

						*(print(xmlBuffer, *doc, 0))=NULL;
						projectFile.Write(xmlBuffer);
						projectFile.Close();

						delete[] xmlBuffer;
					}
					catch(rapidxml::parse_error e){
						OUT_DEBUG("RapidXML error encountered");
					}
					Application::GetInstance()->mainFrame->LoadProjectToFrontend(m_projectPath);
					EndDialog(hWnd, WM_QUIT);
				}
				break;
			}
		default:
			return 0;
		}
		return 0;
	}
}
