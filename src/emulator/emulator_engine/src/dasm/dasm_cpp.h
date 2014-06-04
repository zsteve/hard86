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

#ifndef DASM_PROCESS_H
#define DASM_PROCESS_H

#include <cstdio>
#include <cstdlib>
#include <string>

#include <vector>
#include <list>
#include <iterator>

#include "dasm.h"
#include "../../../symbols_loader/src/sym_loader.h"

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

#include "../../../../system/datastruct/clist/clist.h"

#define DLLEXPORT __declspec(dllexport)

namespace nsDasm{

	using namespace std;

	using namespace nsSymLoader;

	class DasmList;

	class DLLEXPORT Disassembler{
	public:
		Disassembler(){}
		~Disassembler(){}

		DasmList Disassemble(int nInstructions, uint16 initIP, uint16 initCS, SymbolData& symData);
	};

	/**
	 * one line of disassembled code
	 */
	class DLLEXPORT DasmLine{
	protected:
		string m_line;	/// disassembled line
		uint16 m_seg;	///
		uint16 m_addr;	/// seg:addr of line
		DasmLine* m_jumpTo;	/// possible pointer to DasmLine to jump to in case of Jxx/CALL instruction

	public:
		DasmLine(string& line, uint16 seg, uint16 addr, DasmLine* jumpTo=NULL){ 
			m_line=line;
			m_seg=seg;
			m_addr=addr;
			m_jumpTo=jumpTo;
		}
		DasmLine(const char* line, uint16 seg, uint16 addr, DasmLine* jumpTo=NULL){
			*this=DasmLine(string(line), seg, addr, jumpTo);
		}

		const DasmLine& operator=(const DasmLine& rhs){
			m_line=rhs.m_line;
			m_seg=rhs.m_seg;
			m_addr=rhs.m_addr;
			m_jumpTo=rhs.m_jumpTo;
			return *this;
		}

		string& GetLine(){ return m_line; }
		void SetLine(string& line){ m_line=line; }
		void SetLine(char* line){ m_line=string(line); }

		const char* GetCStr(){ return m_line.c_str(); }

		uint32 GetExtAddr(){ return (((uint32)m_seg)<<4) + m_addr; }

		uint16 GetAddr(){ return m_addr; }
		uint16 GetSeg(){ return m_seg; }

		void SetJumpTo(DasmLine* jumpTo){
			m_jumpTo=jumpTo;
		}
	};

	/**
	 * disassembly list class
	 * Implements copy counting
	 * Actual pointers are only freed when
	 * The last copy of a certain object
	 * is destructed.
	 */
	class DLLEXPORT DasmList{
	public:
		class iterator : \
			public std::iterator<random_access_iterator_tag, DasmLine*, int>
		{
		protected:
			vector<DasmLine*>::iterator m_it;
		public:
			iterator(vector<DasmLine*>::iterator it){ m_it=it; }
			iterator(const iterator& it){ m_it=it.m_it; }
			iterator(){}

			iterator& operator++(){ ++m_it; return *this; }
			iterator& operator--(){ --m_it; return *this; }

			const iterator& operator=(const iterator& rhs){
				m_it=rhs.m_it;
				return *this;
			}

			iterator operator-(int n){ return m_it-n; }

			int operator-(const iterator& n){
				return (m_it-n.m_it);
			}

			iterator operator+(int n){ return m_it+n; }


			bool operator==(const iterator& rhs){ return m_it==rhs.m_it; }
			bool operator!=(const iterator& rhs){ return !(*this==rhs); }
			bool operator<(const iterator& rhs){ return m_it<rhs.m_it; }

			DasmLine*& operator*(){ return *m_it; }
			DasmLine*& operator[](int i){ return *(m_it+i); }
			DasmLine*& operator->(){ return *m_it; }
		};

	protected:
		vector<DasmLine*> m_dasmLines;
		
		// helper functions

		static uint32 ProcessJump(DasmLine* e);
		static uint32 ProcessSymbol(DasmLine* e, SymbolData& symData);
		static iterator FindEntry(uint32 searchAddr, iterator searchFrom, iterator searchBegin, iterator searchEnd);

		int* m_copyCount;

	public:
		DasmList(int n);
		DasmList(const DasmList& src);
		const DasmList& operator=(const DasmList& rhs);

		virtual ~DasmList();

		int push_back(DasmLine* dasmLine);
		void pop_back();

		void reserve(int n);
		void resize(int n);

		iterator begin(){ return iterator(m_dasmLines.begin()); }
		iterator end(){ return iterator(m_dasmLines.end()); }
		bool empty(){ return m_dasmLines.empty(); }
		void clear(){
			for(vector<DasmLine*>::iterator it=m_dasmLines.begin();
				it!=m_dasmLines.end();
				++it){
				delete *it;
			}
		}

		// static 

		static DasmList ProcessDisassembly(clist dasmList, SymbolData& symData);

	private:
	};
}

#undef DLLEXPORT

#endif
