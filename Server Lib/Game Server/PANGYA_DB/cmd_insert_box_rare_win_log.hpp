// Arquivo cmd_insert_box_rare_win_log.hpp
// Criado em 18/07/2018 as 22:06 por Acrisio
// Defini��o da classe CmdInsertBoxRareWinLog

#pragma once
#ifndef _STDA_CMD_INSERT_BOX_RARE_WIN_LOG_HPP
#define _STDA_CMD_INSERT_BOX_RARE_WIN_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/box_type.hpp"

namespace stdA {
	class CmdInsertBoxRareWinLog : public pangya_db {
		public:
			explicit CmdInsertBoxRareWinLog(bool _waiter = false);
			CmdInsertBoxRareWinLog(uint32_t _uid, uint32_t _box_typeid, ctx_box_item& _ctx_bi, bool _waiter = false);
			virtual ~CmdInsertBoxRareWinLog();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getBoxTypeid();
			void setBoxTypeid(uint32_t _box_typeid);

			ctx_box_item& getInfo();
			void setInfo(ctx_box_item& _ctx_bi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdInsertBoxRareWinLog"; };
			virtual std::wstring _wgetName() override { return L"CmdInsertBoxRareWinLog"; };

		private:
			uint32_t m_uid;
			uint32_t m_box_typeid;
			ctx_box_item m_ctx_bi;

			const char* m_szConsulta = "pangya.ProcInsertBoxRareWinLog";
	};
}

#endif // !_STDA_CMD_INSERT_BOX_RARE_WIN_LOG_HPP
