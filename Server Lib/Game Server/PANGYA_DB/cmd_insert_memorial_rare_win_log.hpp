// Arquivo cmd_insert_memorial_rare_win_log.hpp
// Criado em 22/07/2018 as 14:54 por Acrisio
// Defini��o da classe CmdInsertMemorialRareWinLog

#pragma once
#ifndef _STDA_CMD_INSERT_MEMORIAL_RARE_WIN_LOG_HPP
#define _STDA_CMD_INSERT_MEMORIAL_RARE_WIN_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/memorial_type.hpp"

namespace stdA {
	class CmdInsertMemorialRareWinLog : public pangya_db {
		public:
			explicit CmdInsertMemorialRareWinLog(bool _waiter = false);
			CmdInsertMemorialRareWinLog(uint32_t _uid, uint32_t _coin_typeid, ctx_coin_item_ex& _ci, bool _waiter = false);
			virtual ~CmdInsertMemorialRareWinLog();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getCoinTypeid();
			void setCoinTypeid(uint32_t _coin_typeid);

			ctx_coin_item_ex& getInfo();
			void setInfo(ctx_coin_item_ex& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdInsertMemorialRareWinLog"; };
			virtual std::wstring _wgetName() override { return L"CmdInsertMemorialRareWinLog"; };

		private:
			uint32_t m_uid;
			uint32_t m_coin_typeid;
			ctx_coin_item_ex m_ci;

			const char* m_szConsulta = "pangya.ProcInsertMemorialRareWinLog";
	};
}

#endif // !_STDA_CMD_INSERT_MEMORIAL_RARE_WIN_LOG_HPP
