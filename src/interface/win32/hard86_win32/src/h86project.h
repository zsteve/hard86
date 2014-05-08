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
#include "../../../../lib/rapidxml/rapidxml.hpp"

namespace nsHard86Win32{

	using namespace rapidxml;
	using namespace std;

	/**
	 * Hard86 project file
	 * Singleton class
	 * (XML format)
	 */
	class H86Project{
	private:
		static H86Project* m_instance;

		H86Project(char* data);

		~H86Project();

	public:

		static H86Project* GetInstance(){
			if(!m_instance){
				throw std::runtime_error("H86Project::GetInstance() - NULL instance");
			}
			return m_instance;
		}

		static H86Project* GetInstance(char* data){
			if(m_instance){
				delete m_instance;
			}
			m_instance=new H86Project(data);
			return m_instance;
		}

		static void DisposeInstance(){
			if(m_instance){
				delete m_instance;
				m_instance=NULL;
			}
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

	protected:
		xml_document<>* m_doc;
		char* m_data;
	private:

	};

}

#endif
