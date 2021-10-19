// Arquivo cmd_msg_off_info.hpp
// Criado em 24/03/2018 as 17:55 por Acrisio
// Defini��o da classe CmdMsgOffInfo

#pragma once
#ifndef _STDA_CMD_MSG_OFF_INFO_HPP
#define _STDA_CMD_MSG_OFF_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdMsgOffInfo : public pangya_db {
		public:
			explicit CmdMsgOffInfo(bool _waiter = false);
			CmdMsgOffInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdMsgOffInfo();

			std::vector< MsgOffInfo >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMsgOffInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdMsgOffInfo"; };

		private:
			uint32_t m_uid;
			std::vector< MsgOffInfo > v_moi;

			const char* m_szConsulta = "pangya.ProcGetMsgOff";
	};
}

#endif // !_STDA_CMD_MSG_OFF_INFO_HPP
