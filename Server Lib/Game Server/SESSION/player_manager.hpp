// Arquivo player_manager.hpp
// Criado em 07/03/2018 as 22:15 por Acrisio
// Definição da classe player_manager

#pragma once
#ifndef _STDA_PLAYER_MANAGER_HPP
#define _STDA_PLAYER_MANAGER_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "player.hpp"
#include <vector>
#include "../../Projeto IOCP/SOCKET/session_manager.hpp"

#include <string>

namespace stdA {
	class player_manager : public session_manager {
		public:
			union uIndexOID {
				void clear() { memset(this, 0, sizeof(uIndexOID)); };
				unsigned char ucFlag;
				struct stFlag {
					unsigned char busy : 1;
					unsigned char block : 1, : 0;
				}flag;
			};

		public:
			player_manager(threadpool& _threapool, uint32_t _max_session);
			virtual ~player_manager();

			virtual void clear();

			virtual player *findPlayer(uint32_t _uid, bool _oid = false);
			virtual player *findPlayerBySocketId(uint32_t _socket_id);
			virtual std::vector< player* > findAllGM();

			// Override methods
			virtual bool deleteSession(session *_session) override;

			virtual void checkPlayersItens();
			virtual void checkPlayersGameGuard();

			virtual void blockOID(uint32_t _oid);
			virtual void unblockOID(uint32_t _oid);

			static void checkItemBuff(player& _session);
			static void checkCardSpecial(player& _session);
			static void checkCaddie(player& _session);
			static void checkMascot(player& _session);
			static void checkWarehouse(player& _session);

			static bool checkGameGuard(player& _session);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			std::map< uint32_t, uIndexOID > m_indexes;		// Index de OID

			// Sem proteção de sincronização, chamar ela em uma função thread safe(thread com seguranção de sincronização)
			virtual int32_t findSessionFree() override;
			
			// Sem proteção de sincronização, chamar ela em uma função thread safe(thread com seguranção de sincronização)
			virtual uint32_t getNewOID();
			virtual void freeOID(uint32_t _oid);
	};
}

#endif