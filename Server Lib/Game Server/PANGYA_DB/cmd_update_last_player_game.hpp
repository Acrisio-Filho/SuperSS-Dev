// Arquivo cmd_update_last_player_game.hpp
// Criado em 28/10/2018 as 14:18 por Acrisio
// Defini��o da classe CmdUpdateLastPlayerGame

#pragma once
#ifndef _STDA_CMD_UPDATE_LAST_PLAYER_GAME_HPP
#define _STDA_CMD_UPDATE_LAST_PLAYER_GAME_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateLastPlayerGame : public pangya_db {
		public:
			explicit CmdUpdateLastPlayerGame(bool _waiter = false);
			CmdUpdateLastPlayerGame(uint32_t _uid, Last5PlayersGame& _l5pg, bool _waiter = false);
			virtual ~CmdUpdateLastPlayerGame();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			Last5PlayersGame& getInfo();
			void setInfo(Last5PlayersGame& _l5pg);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateLastPlayerGame"; };
			std::wstring _wgetName() override { return L"CmdUpdateLastPlayerGame"; };

		private:
			uint32_t m_uid;
			Last5PlayersGame m_l5pg;

			const char* m_szConsulta = "pangya.ProcUpdateLast5PlayerGame";
	};
}

#endif // !_STDA_CMD_UPDATE_LAST_PLAYER_GAME_HPP
