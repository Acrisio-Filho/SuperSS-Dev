// Arquivo cmd_item_buff_info.hpp
// Criado em 21/03/2018 as 22:43 por Acrisio
// Defini��o da classe CmdItemBuffInfo

#pragma once
#ifndef _STDA_CMD_ITEM_BUFF_INFO_HPP
#define STDA_CMD_ITEM_BUFF_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdItemBuffInfo : public pangya_db {
		public:
			explicit CmdItemBuffInfo(bool _waiter = false);
			CmdItemBuffInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdItemBuffInfo();

			std::vector< ItemBuffEx >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdItemBuffInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdItemBuffInfo"; };

		private:
			uint32_t m_uid;
			std::vector< ItemBuffEx > v_ib;

			const char* m_szConsulta = "pangya.ProcGetItemBuff";
	};
}

#endif // !_STDA_CMD_ITEM_BUFF_INFO_HPP
