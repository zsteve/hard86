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

		/// @return a pair containing [seg, addr]
		pair<uint16, uint16> Get_LoadSegAddr(){
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

		/// @return std::string path to binary file
		string Get_BinaryPath(){
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

		/// @return std::string path to FAS file
		string Get_FASPath(){
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

		/// @return std::string path to user vdev directory
		string Get_UserVDevPath(){
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

		/// @return vector<pair<string, string> > list of names and paths of included vdevs
		vector<pair<string, string> > Get_VDevList(){
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

		// Removes all children of the VDevs node
		void Remove_VDevList(){
			try{
				xml_node<>* node=m_doc->first_node("Hard86Project");
				if(!node){
					OUT_DEBUG("RapidXML failed to locate node");
					return;
				}
				xml_node<>* vdevNode=node->first_node("VDevs");
				if(!node){
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

		// Appends a vector<pair<string, string> > of VDev <name, path> 
		void Add_VDevList(vector<pair<string, string> >& vdevList){
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

	protected:
		xml_document<>* m_doc;
		char* m_data;
	private:

	};

}

#endif
