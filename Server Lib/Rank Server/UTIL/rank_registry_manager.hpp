// Arquivo rank_registry_manager.hpp
// Criado em 16/06/2020 as 15:17 por Acrisio
// Defini��o da classe RankRegistryManager

#pragma once
#ifndef _STDA_RANK_REGISTRY_MANAGER_HPP
#define _STDA_RANK_REGISTRY_MANAGER_HPP

#if defined(__linux__)
#include <pthread.h>
#include <unistd.h>
#endif

#include "rank_registry.hpp"
#include "rank_character.hpp"

#include "../../Projeto IOCP/PACKET/packet.h"
#include "../SESSION/player.hpp"

#include <fstream>

namespace stdA {

	// Limite de registros por p�gina
	constexpr uint32_t LIMIT_REGISTRY_FOR_PAGE = 12u;

	// Found Player typedef
	typedef std::pair< uint32_t /*Position do player*/, int32_t /*Page or -1 error*/ > FoundPlayer;

	class RankRegistryManager {
		public:
			RankRegistryManager();
			virtual ~RankRegistryManager();

			void load();

			bool isLoad();

			// Coloca a p�gina que o player pediu no packet se ele tiver
			void pageToPacket(packet& _packet, search_dados& _sd);

			// Colocar a posi��o e o valor do player no packet se ele tiver
			void playerPositionToPacket(packet& _packet, player& _session, search_dados& _sd);

			// Envia o info completo do player com character info e os overall completo
			void sendPlayerFullInfo(player& _session, uint32_t _uid/*Player Info*/);

			// Envia a p�gina em que o player procurado foi encontrado
			void sendPageFoundPlayer(player& _session, FoundPlayer& _fp, search_dados& _sd);

			// Procura um player pelo nickname e enviar a p�gina onde ele est� se ele estiver no rank
			void searchPlayerByNicknameAndSendPage(player& _session, std::string _nickname, search_dados& _sd);

			// Procura um player pela position e enviar a p�gina onde ele est� se ele estiver no rank
			void searchPlayerByRankAndSendPage(player& _session, uint32_t _position, search_dados& _sd);

			// Procura um player por nickname no Rank Menu->Item
			FoundPlayer searchPlayerByNickname(std::string _nickname, search_dados& _sd);

			// Procura um player pelo rank dele no Rank Menu->Item
			FoundPlayer searchPlayerByRank(uint32_t _position, search_dados& _sd);

			// Cria log de todos os registro em uma arquivo com data
			void makeLog();

		protected:
			void initialize();

			void clear();

			std::map< eRANK_OVERALL, RankRegistry > getAllOverallInfoFromPlayer(uint32_t _uid);

			std::pair< RankEntryValueRange, bool > getPage(RankEntry::iterator _registrys, unsigned int & _page);

		protected:
			std::ofstream log;

			std::string prex;
			std::string dir;

			void init_log();
			void close_log();

			inline void putLog(std::string _str_log);

		protected:
			RankEntry m_entry;
			RankCharacterEntry m_character_entry;

			bool m_state;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};
}

#endif // !_STDA_RANK_REGISTRY_MANAGER_HPP
