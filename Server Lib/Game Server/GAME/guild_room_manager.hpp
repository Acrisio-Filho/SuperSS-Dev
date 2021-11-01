// Arquivo guild_room_manager.hpp
// Criado em 26/12/2019 as 20:51 por Acrisio
// Defini��o da classe GuildRoomManager

#pragma once
#ifndef STDA_GUILD_ROOM_MANAGER_HPP
#define _STDA_GUILD_ROOM_MANGER_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "dupla_manager.hpp"
#include "guild.hpp"

#include <vector>

namespace stdA {

	// Guild Room Manager
	class GuildRoomManager {
		public:
			enum eGUILD_WIN : unsigned char {
				RED,
				BLUE,
				DRAW,
			};

		public:
			GuildRoomManager();
			~GuildRoomManager();

			Guild* addGuild(Guild::eTEAM _team, uint32_t _uid);
			Guild* addGuild(Guild& _guild);
			
			void deleteGuild(Guild* _guild);

			uint32_t getNumGuild();

			eGUILD_WIN getGuildWin();

			Guild* findGuildByTeam(Guild::eTEAM _team);
			Guild* findGuildByUID(uint32_t _uid);
			Guild* findGuildByPlayer(player& _session);

			Dupla* findDupla(player& _session);

			void init_duplas();

			// Verifica se tem a quantidade de jogadores para come�ar o Guild Battle
			int isGoodToStart();

			// Verifica se sobrou s� players de uma guild s�
			bool oneGuildRest();

			// update dados guilds
			void update();

			void calcGuildWin();

			void saveGuildsData();

			void initPacketDuplas(packet& _p);

			bool finishHoleDupla(PlayerGameInfo& _pgi, unsigned short _seq_hole);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			std::vector< Guild > v_guilds;		// Guilds

			DuplaManager m_dupla_manager;		// Dupla Manager

			eGUILD_WIN m_guild_win;				// Guild Win

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};
}
#endif // !STDA_GUILD_ROOM_MANAGER_HPP
