// Arquivo cmd_last_player_game_info.hpp
// Criado em 24/03/2018 as 19:01 por Acrisio
// Defini��o da classe CmdLastPlayerGameInfo

#pragma once
#ifndef _STDA_CMD_LAST_PLAYER_GAME_INFO_HPP
#define _STDA_CMD_LAST_PLAYER_GAME_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdLastPlayerGameInfo : public pangya_db {
		public:
			explicit CmdLastPlayerGameInfo(bool _waiter = false);
			CmdLastPlayerGameInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdLastPlayerGameInfo();

			Last5PlayersGame& getInfo();
			void setInfo(Last5PlayersGame& _l5pg);

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdLastPlayerGameInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdLastPlayerGameInfo"; };

		private:
			uint32_t m_uid;
			Last5PlayersGame m_l5pg;

			const char* m_szConsulta = "pangya.ProcGetLastPlayerGame";
	};
}

#endif // !_STDA_CMD_LAST_PLAYER_GAME_HPP
