/**
* @file debugger module
* Stephen Zhang, 2014
*/

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "vdev.h"
#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"

#include  <map>

namespace nsDebugger{

	using namespace std;
	using namespace nsVDev;

	/**
	* List of breakpoint entries
	*/
	class BreakpointList{
	public:
		class Breakpoint{
		public:
			Breakpoint(){
				m_addr=0;
				m_orig_byte=m_new_byte=0;
				m_active=false;
			}

			Breakpoint(uint32 addr, uint8 new_byte=0xcc){
				m_addr=addr;
				m_new_byte=new_byte;
				m_orig_byte=read_mem_8(addr);
				m_active=false;
			}

			Breakpoint(const Breakpoint& src){
				m_addr=src.m_addr;
				m_orig_byte=src.m_orig_byte;
				m_new_byte=src.m_new_byte;
				m_active=src.m_active;
			}

			~Breakpoint(){}

			/**
			* Activates breakpoint
			* Assumes that system mutex is owned by
			* debugger
			*/
			void Activate(){
				m_orig_byte=read_mem_8(m_addr);
				write_mem_8(m_new_byte, m_addr);
				m_active=true;
			}

			/**
			* Deactivates breakpoint
			* Assumes that system mutex is owned by
			* debugger
			*/
			void Deactivate(){
				write_mem_8(m_orig_byte, m_addr);
				m_active=false;
			}

		protected:
			uint32 m_addr;
			uint8 m_orig_byte;
			uint8 m_new_byte;

			bool m_active;
		private:
		};

	protected:
		map<uint32, Breakpoint> m_bpMap;

	public:

		typedef map<uint32, Breakpoint>::iterator iterator;

		BreakpointList();
		BreakpointList(const BreakpointList& src);

		virtual ~BreakpointList();

		iterator begin(){ return m_bpMap.begin(); }
		iterator end() { return m_bpMap.end(); }

		bool empty() { return m_bpMap.empty(); }
		void clear(){ m_bpMap.clear(); }

		void erase(uint32 key){ m_bpMap.erase(key); }

		Breakpoint& operator[](int i){
			return m_bpMap[i];
		}

		bool Exists(uint32 key){ return m_bpMap.count(key); }

	private:
	};

	/**
	 * singleton Debugger class
	 */
	class Debugger{
	private:
		Debugger()
		{
		}
	public:
		/**
		 * must be called prior to any GetInstance() call
		 */
		static void Init(VDevList* vdevList);

		/**
		 * destroys instance, invalidating all references
		 */
		static void Reset();

		static Debugger* GetInstance();
		virtual ~Debugger(){}

		static void BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState);
		static void PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);
		static void PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);

		bool AddBreakpoint(uint32 addr);
		bool RemoveBreakpoint(uint32 addr);

		bool ActivateBreakpoint(uint32 addr);
		bool DeactivateBreakpoint(uint32 addr);

		bool BreakpointExists(uint32 addr);

		void ClearBreakpoints();

		bool HasBreakpoints();

		BreakpointList::iterator BreakpointBegin(){ return m_bpList.begin(); }
		BreakpointList::iterator BreakpointEnd(){ return m_bpList.end(); }

	protected:
		static VDevList* m_vdevList;
		static Debugger* m_instance;
		BreakpointList m_bpList;
	};
}

#endif