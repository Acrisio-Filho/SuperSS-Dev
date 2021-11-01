// Arquivo dupla_manager.hpp
// Criado em 29/12/2019 as 11:45 por Acrisio
// Definição da classe DuplaManager

#pragma once
#ifndef _STDA_DUPLA_MANAGER_HPP
#define _STDA_DUPLA_MANAGER_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "dupla.hpp"
#include "guild.hpp"
#include "../TYPE/game_type.hpp"

namespace stdA {
	struct DuplaManager {
	public:
		DuplaManager();
		~DuplaManager();

		void init_duplas(Guild& _g1, Guild& _g2);

		void addDupla(player* _p1, player* _p2);
		void deleteDupla(Dupla* _dupla);
		void deleteDupla(unsigned char _numero);

		Dupla* findDuplaByPlayer(player& _session);
		Dupla* findDuplaByPlayerUID(uint32_t _uid);
		Dupla* findDuplaByNumero(unsigned char _numero);

		uint32_t getNumDuplas();

		uint32_t getNumPlayersQuit();
		uint32_t getNumPlayersQuitGuild(Guild *_g);
		uint32_t getNumPlayersQuitGuild(uint32_t _uid);

		void updateGuildDados(Guild *_g1, Guild *_g2);

		void updatePangWinDuplas(Guild *_g, uint32_t _pang_win);
		void updatePangWinDuplas(uint32_t _uid, uint32_t _pang_win);

		bool oneGuildRest();	// Verifica se só sobrou os players de uma guild

		void saveGuildMembersData();

		void initPacketDuplas(packet& _p);

		bool finishHoleDupla(PlayerGameInfo& _pgi, unsigned short _seq_hole);

	protected:
		static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

	private:
		std::vector< Dupla > v_duplas;

#if defined(_WIN32)
		CRITICAL_SECTION m_cs;
#elif defined(__linux__)
		pthread_mutex_t m_cs;
#endif
	};
}

#endif // !_STDA_DUPLA_MANAGER_HPP
