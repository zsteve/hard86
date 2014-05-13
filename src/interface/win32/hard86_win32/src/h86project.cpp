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
		delete m_doc;
		delete m_data;
	}

}
