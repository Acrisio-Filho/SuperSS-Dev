// Arquivo comet_refill_system.hpp
// Criado em 08/07/2018 as 21:57 por Acrisio
// Defini��o da classe CometRefillSystem

#pragma once
#ifndef _STDA_COMET_REFILL_SYSTEM_HPP
#define _STDA_COMET_REFILL_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/comet_refill_type.hpp"

#include "../../Projeto IOCP/TYPE/singleton.h"

#include <map>

namespace stdA {
	class CometRefillSystem {
		public:
			CometRefillSystem();
			virtual ~CometRefillSystem();

			/*static*/ void load();

			/*static*/ bool isLoad();

			/*static*/ ctx_comet_refill* findCometRefill(uint32_t _typeid);

			/*static*/ uint32_t drawsCometRefill(ctx_comet_refill& _ctx_cr);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::map< uint32_t, ctx_comet_refill > m_comet_refill;	// Todos os Comet Refill Item Conhecido do Server no DB
			
			/*static*/ bool m_load;													// Load Server

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< CometRefillSystem > sCometRefillSystem;
}

#endif // !_STDA_COMET_REFILL_SYSTEM_HPP
