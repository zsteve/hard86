/**
* @file FASM symbols loader
* Stephen Zhang, 2014
*/

#ifndef SYM_LOADER
#define SYM_LOADER

#include <string>
#include <vector>
#include "../../../global/typedefs.h"

namespace nsSymLoader{

	using namespace std;

	int pascal_strlen(char* stf);
	void pascal_strcpy(char* dest, char* src);
	void pascal_tocstr(char* dest, char* src);

	struct HeaderData {
		// o_ prefix means offset
		// l_ prefix means length
		uint16 l_header;
		uint32 o_inputFile;
		uint32 o_outputFile;
		uint32 o_strTable;
		uint32 l_strTable;
		uint32 o_symTable;
		uint32 l_symTable;
		uint32 o_prepSrc;
		uint32 l_prepSrc;
		uint32 o_asmDmp;
		uint32 l_asmDmp;
		uint32 o_sectNameTab;
		uint32 l_sectNameTab;
		uint32 o_symRefDmp;
		uint32 l_symRefDmp;

		HeaderData(uint8* data);

		HeaderData(){

		}
	};

	class Symbol{
	public:
		Symbol();
		Symbol(const Symbol& src);
		Symbol(uint8* data, uint32 offset, HeaderData& hdr);

		~Symbol(){}

		string& Name(){ return m_name; }
		uint64& Value(){ return m_value; }
		uint16& Size(){ return m_size; }

	protected:
		uint64 m_value;
		uint16 m_size;
		string m_name;

	private:
	};

	class SymbolData{
	public:
		SymbolData();
		SymbolData(uint8* symFileData, int dataSize);
		SymbolData(const SymbolData& src);
		~SymbolData();

		class iterator : \
			public std::iterator<random_access_iterator_tag, Symbol, int>
		{
		protected:
			vector<Symbol>::iterator m_it;
		public:
			iterator(vector<Symbol>::iterator it){ m_it=it; }
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

			Symbol& operator*(){ return *m_it; }
			Symbol& operator[](int i){ return *(m_it+i); }
			Symbol& operator->(){ return *m_it; }
		};

		iterator begin(){ return m_symVect.begin(); }
		iterator end(){ return m_symVect.end(); }

		Symbol& operator[](int i){ return m_symVect[i]; }

		bool empty(){ return m_symVect.empty(); }
		void clear(){ return m_symVect.clear(); }

		int size(){ return m_symVect.size(); }
	protected:
		uint8* m_data;
		int m_dataSize;
		HeaderData m_headerData;

		vector<Symbol> m_symVect;
	private:
	};
}

#endif