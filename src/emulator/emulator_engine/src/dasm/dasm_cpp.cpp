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

#include "dasm_cpp.h"

#include <iostream>

namespace nsDasm{

	using namespace std;
	using namespace nsSymLoader;

	// Disassembler
	DasmList Disassembler::Disassemble(int nInstructions, uint16 initIP, uint16 initCS, SymbolData& symData){
		clist cDasmList=dasm_disassemble(nInstructions, initIP, initCS);
		DasmList dasmList=DasmList::ProcessDisassembly(cDasmList, symData);
		return dasmList;
	}

	// DasmList

	DasmList::DasmList(int n) : m_dasmLines(n){
		m_copyCount=new int;
		*m_copyCount=1;	/* only one copy */
	}

	DasmList::DasmList(const DasmList& src) : m_dasmLines(src.m_dasmLines){
		m_copyCount=src.m_copyCount;
		(*m_copyCount)++;	/* one more copy */
	}

	DasmList::~DasmList(){
		(*m_copyCount)--;
		/* if *m_copyCount==0, then this is the last copy of this object */
		if(!(*m_copyCount)){
			// first, free up m_copyCount
			delete m_copyCount;
			if(!m_dasmLines.empty()){
				for(vector<DasmLine*>::iterator it=m_dasmLines.begin();
					it!=m_dasmLines.end();
					++it){
					delete (*it);
				}
			}
		}
	}

	const DasmList& DasmList::operator=(const DasmList& rhs){
		// prevent self assignment!
		if(this==&rhs) return *this;
		m_dasmLines=rhs.m_dasmLines;
		m_copyCount=rhs.m_copyCount;
		(*m_copyCount)++;	/* one more copy */
		return *this;
	}

	/**
	 * appends the DasmLine object to internal vector
	 * assumes that DasmList object will now be responsible for
	 * freeing object (caller must NOT use delete on dasmLine
	 */
	int DasmList::push_back(DasmLine* dasmLine){
		m_dasmLines.push_back(dasmLine);
		return 0;
	}

	/**
	 * removes the last DasmLine object from internal vector
	 */
	void DasmList::pop_back(){
		m_dasmLines.pop_back();
	}

	void DasmList::reserve(int n){
		m_dasmLines.reserve(n);
	}

	void DasmList::resize(int n){
		m_dasmLines.resize(n);
	}

	// DasmList static functions

	/**
	 * Checks if a disassembly instruction is a jumping instruction
	 * Jxx/CALL
	 * @param e pointer to DasmLine object in question
	 * @return address if present, or -1 if not jumping instruction.
	 */
	uint32 DasmList::ProcessJump(DasmLine* const e){
		static string strs[14]={
			"JMP",
			"JE",
			"JNE",
			"JA",
			"JAE",
			"JB",
			"JBE",
			"JL",
			"JLE",
			"JG",
			"JGE",
			"JZ",
			"JNZ",
			"CALL"
		};
		string name=e->GetLine().substr(0, 3);
		for(int i=0; i<14; i++){
			if(name==strs[i]){
				// found : now we need to find the address
				string rest=e->GetLine().substr(4, string::npos);
				for(int i=0; i<rest.size(); i++){
					if(!isxdigit(rest[i])){
						return 0;
					}
				}
				return strtoul(rest.c_str(), NULL, 16);
			}
		}
		return -1;
	}

	uint32 DasmList::ProcessSymbol(DasmLine* e, SymbolData& symData){
		for(SymbolData::iterator it=symData.begin();
			it!=symData.end();
			++it){

			char value[80];
			itoa((*it).Value(), value, 16);
			string valueStr=string(value);

			vector<int> indexes;
			int last=0;
			while(last!=string::npos){
				last=e->GetLine().find(valueStr, last+1);
				if(last!=string::npos){
					indexes.push_back(last);
				}
			}
			for(vector<int>::iterator vit=indexes.begin();
				vit!=indexes.end();
				++vit){
				e->GetLine().replace(*vit, valueStr.size(), (*it).Name());

				int sizeDiff=(*it).Name().size()-valueStr.size();
				for(vector<int>::iterator _vit=vit;
					_vit!=indexes.end();
					++_vit){
					(*vit)+=sizeDiff;
				}
			}
		}
		return 0;
	}


	/**
	 * Attempts to resolve address of entry with address searchAddr
	 * using binary search.
	 * Searches in the range [searchBegin, searchEnd)
	 * @param searchAddr address in question
	 * @param searchFrom where the address is referenced
	 * @param searchBegin start of range
	 * @param searchEnd end of range 
	 * @return iterator pointing to the entry if successfully found, otherwise searchEnd
	 */
	DasmList::iterator DasmList::FindEntry(uint32 searchAddr, DasmList::iterator searchFrom,\
											DasmList::iterator searchBegin, DasmList::iterator searchEnd){
		DasmList::iterator lBound, uBound;
		// best case
		if(searchAddr == searchFrom->GetExtAddr()){
			return searchFrom;
		}
		if(searchAddr > searchFrom->GetExtAddr()){
			// targeted address must exist in range
			// (searchFrom, searchEnd)
			if(searchAddr > (searchEnd-1)->GetExtAddr()){
				// sought after address does not exist in range.
				return searchEnd;
			}
			lBound=searchFrom+1;
			uBound=searchEnd;
		}
		else if(searchAddr < searchFrom->GetExtAddr()){
			// targeted address must exist in range
			// [searchBegin, searchFrom)
			if(searchAddr < searchBegin->GetExtAddr()){
				// sought after address does not exist in range.
				return searchEnd;
			}
			lBound=searchBegin;
			uBound=searchFrom;
		}
		// search within half open range [lBound, uBound)
		while(lBound<uBound){
			DasmList::iterator mid=lBound+(uBound-lBound)/2;
			if(searchAddr==mid->GetExtAddr()){
				cout << "Found DasmList entry : " << endl;
				cout << mid->GetCStr() << endl;
				return mid;
			}
			if(searchAddr>mid->GetExtAddr()){
				lBound=mid+1;
			}
			else if(searchAddr<mid->GetExtAddr()){
				uBound=mid-1;
			}
		}
		return searchEnd;
	}

	/**
	 * Processes disassembly output from C disassembler module
	 * into a DasmList object. Also resolves jumping addresses
	 * to pointers to DasmLine objects.
	 * @param dasmList output from C disassembler (dasm.c) module
	 * @return DasmList object containing processed disassembly data
	 */
	DasmList DasmList::ProcessDisassembly(clist dasmList, SymbolData& symData){

		// find size of disassembly list
		int listSize=0;
		clist_begin(&dasmList);
		do{
			listSize++;
		} while(clist_next(&dasmList)!=-1);

		DasmList dasmOut(listSize);

		// copy all items into Dasmlist object
		{
			dasm_list_entry* clistEntry=NULL;
			clist_begin(&dasmList);
			DasmList::iterator it=dasmOut.begin();
			do{
				clistEntry=(dasm_list_entry*)clist_getcurr(&dasmList);

				*it=new DasmLine(clistEntry->line, clistEntry->seg, clistEntry->addr);
				it++;
			} while(clist_next(&dasmList)!=-1);
		}

		// process all DasmList items
		{
			for(DasmList::iterator it=dasmOut.begin();
				it!=dasmOut.end();
				++it){
				uint32 addr;
				if((addr=ProcessJump(*it))!=-1){
					// we have found a jumping instruction
					// use binary search and find the target
					DasmList::iterator target=FindEntry(addr, it, dasmOut.begin(), dasmOut.end());
					if(target!=dasmOut.end()){
						// looks like we found one!
						it->SetJumpTo(*target);
					}
				}

				// process symbols
				ProcessSymbol(*it, symData);
			}
		}

		return dasmOut;
	}
}
