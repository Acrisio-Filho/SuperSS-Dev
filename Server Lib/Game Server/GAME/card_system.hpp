// Arquivo card_system.hpp
// Criado em 30/06/2018 as 16:47 por Acrisio
// Defini��o da classe CardSystem

#pragma once
#ifndef _STDA_CARD_SYSTEM_HPP
#define _STDA_CARD_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <vector>
#include <map>

#include "../TYPE/card_type.hpp"
#include "../../Projeto IOCP/TYPE/singleton.h"

namespace stdA {
	class CardSystem {
		public:
			CardSystem();
			virtual ~CardSystem();

			// Load
			/*static*/ void load();

			/*static*/ bool isLoad();

			// finders
			/*static*/ CardPack* findCardPack(uint32_t _typeid);
			/*static*/ CardPack* findBoxCardPack(uint32_t _typeid);
			/*static*/ Card* findCard(uint32_t _typeid);

			/*static*/ std::vector< Card > draws(CardPack& _cp);
			/*static*/ Card drawsLoloCardCompose(LoloCardComposeEx& _lcc);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::vector< Card > m_card;							// Todos os Card
			/*static*/ std::map< uint32_t, CardPack > m_card_pack;		// Todos os Card Pack
			/*static*/ std::map< uint32_t, CardPack > m_box_card_pack;	// Todos os Box Card Pack

			/*static*/ bool m_load;											// Load CardSystem

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< CardSystem > sCardSystem;
}

#endif // !_STDA_CARD_SYSTEM_HPP
