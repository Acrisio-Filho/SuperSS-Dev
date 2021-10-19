// Arquivo memorial_system.hpp
// Criado em 21/07/2018 as 18:48 por Acrisio
// Defini��o da classe MemorialSystem

#pragma once
#ifndef _STDA_MEMORIAL_SYSTEM_HPP
#define _STDA_MEMORIAL_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/memorial_type.hpp"
#include "../SESSION/player.hpp"

#include "../../Projeto IOCP/TYPE/singleton.h"

#include <map>
#include <vector>

#define MEMORIAL_LEVEL_MAX 24

namespace stdA {
	class MemorialSystem {
		public:
			MemorialSystem();
			virtual ~MemorialSystem();

			/*static*/ bool isLoad();

			/*static*/ void load();

			/*static*/ ctx_coin* findCoin(uint32_t _typeid);

			/*static*/ std::vector< ctx_coin_item_ex > drawCoin(player& _session, ctx_coin& _ctx_c);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

			/*static*/ uint32_t calculeMemorialLevel(uint32_t _achievement_pontos);

		private:
			/*static*/ std::map< uint32_t, ctx_coin > m_coin;						// Todas as Coin do Jogo
			/*static*/ std::map< uint32_t, ctx_memorial_level > m_level;			// Levels Tabela
			/*static*/ std::map< uint32_t, ctx_coin_set_item > m_consolo_premio;	// Pr�mio de consolo quando n�o ganha o raro

			/*static*/ bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< MemorialSystem > sMemorialSystem;
}

#endif // !_STDA_MEMORIAL_SYSTEM_HPP
