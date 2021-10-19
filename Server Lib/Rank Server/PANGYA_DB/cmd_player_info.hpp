// Arquivo cmd_player_info.hpp
// Criado em 15/06/2020 as 16:35 por Acrisio
// Defini��o da classe CmdPlayerInfo

#pragma once
#ifndef _STDA_CMD_PLAYER_INFO_HPP
#define _STDA_CMD_PLAYER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_rank_st.hpp"

namespace stdA {
	class CmdPlayerInfo : public pangya_db {
		public:
			explicit CmdPlayerInfo(bool _waiter = false);
			CmdPlayerInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdPlayerInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			player_info& getInfo();

		protected:
			void lineResult(result_set::ctx_res *_result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// Class Name
			std::string _getName() override { return "CmdPlayerInfo"; };
			std::wstring _wgetName() override { return L"CmdPlayerInfo"; };

		private:
			uint32_t m_uid;
			player_info m_pi;

			const char* m_szConsulta = "pangya.ProcGetPlayerInfoRank";
	};
}

#endif // !_STDA_CMD_PLAYER_INFO_HPP
