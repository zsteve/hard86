/**
* @file FASM symbols loader
* Stephen Zhang, 2014
*/

#include <iostream>

#include "sym_loader.h"
#include "../../../global/defines.h"

namespace nsSymLoader{
	// helper functions

	// pascal version of string ops
	int pascal_strlen(char* str){
		return (*str);
	}

	void pascal_strcpy(char* dest, char* src){
		int size=src[0];
		dest[0]=size;
		for(int i=0; i<size; i++){
			dest[1+i]=src[1+i];
		}
	}

	void pascal_tocstr(char* dest, char* src){
		int size=src[0];
		for(int i=0; i<size; i++){
			dest[i]=src[1+i];
		}
		dest[size]=0;
	}

	inline uint16 symf_read8(uint8* data, int offset){
		return *(data+offset);
	}

	inline uint16 symf_read16(uint8* data, int offset){
		return *(uint16*)(data+offset);
	}

	inline uint32 symf_read32(uint8* data, int offset){
		return *(uint32*)(data+offset);
	}

	inline uint64 symf_read64(uint8* data, int offset){
		return *(uint64*)(data+offset);
	}

	// HeaderData
	HeaderData::HeaderData(uint8* data){
		l_header=symf_read16(data, 6);
		o_inputFile=symf_read32(data, 8);
		o_outputFile=symf_read32(data, 12);
		o_strTable=symf_read32(data, 16);
		l_strTable=symf_read32(data, 20);
		o_symTable=symf_read32(data, 24);
		l_symTable=symf_read32(data, 28);
		o_prepSrc=symf_read32(data, 32);
		l_prepSrc=symf_read32(data, 36);
		o_asmDmp=symf_read32(data, 40);
		l_asmDmp=symf_read32(data, 44);
		o_sectNameTab=symf_read32(data, 48);
		l_sectNameTab=symf_read32(data, 52);
		o_symRefDmp=symf_read32(data, 56);
		l_symRefDmp=symf_read32(data, 60);
	}

	// Symbol
	Symbol::Symbol(){
		m_value=0;
		m_size=0;
		m_name="";
	}

	Symbol::Symbol(uint8* data, uint32 offset, HeaderData& hdr){
		m_value=symf_read64(data, hdr.o_symTable+offset+0);
		m_size=symf_read8(data, hdr.o_symTable+offset+10);
		uint32 o_name=symf_read32(data, hdr.o_symTable+offset+24);
		if(o_name & (1<<31)){
			// top bit set : name in ASCIIZ strings table
			m_name=(char*)(data+hdr.o_strTable+(o_name & 0x7FFFFFFF));
		}
		else if(!(o_name & (1<<31))){
			// top bit clear : name in pascal preprocessed source
			char* name=new char[80];
			pascal_tocstr(name, (char*)(data+hdr.o_prepSrc+(o_name & 0x7FFFFFFF)));
			m_name=name;
			delete[] name;
		}
	}

	Symbol::Symbol(const Symbol& src){
		m_value=src.m_value;
		m_size=src.m_size;
		m_name=src.m_name;
	}

	// SymbolData
	SymbolData::SymbolData() : m_symVect(0){
		
	}

	SymbolData::SymbolData(uint8* symFileData, int dataSize) : m_symVect(0),\
												m_headerData(symFileData)
	{
		m_dataSize=dataSize;
		m_data=new uint8[dataSize];
		memcpy(m_data, symFileData, dataSize);
		m_symVect.resize(m_headerData.l_symTable/32);
		for(int i=0; i<(m_headerData.l_symTable)/32; i++){
			m_symVect[i]=(Symbol(symFileData, i*32, m_headerData));
		}
	}

	SymbolData::SymbolData(const SymbolData& src){
		m_dataSize=src.m_dataSize;
		m_data=new uint8[m_dataSize];
		memcpy(m_data, src.m_data, m_dataSize);
		m_headerData=src.m_headerData;
		m_symVect=src.m_symVect;
	}

	SymbolData::~SymbolData(){
		delete[] m_data;
	}

}
