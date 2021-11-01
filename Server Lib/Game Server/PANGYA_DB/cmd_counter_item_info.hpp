// Arquivo cmd_counter_item_info.hpp
// Criado em 19/03/2018 as 23:13 por Acrisio
// Defini��o da classe CmdCounterItemInfo

#pragma once
#ifndef _STDA_CMD_COUNTER_ITEM_INFO_HPP
#define _STDA_CMD_COUNTER_ITEM_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdCounterItemInfo : public pangya_db {
		public:
			explicit CmdCounterItemInfo(bool _waiter = false);
			CmdCounterItemInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdCounterItemInfo();

			std::vector< CounterItemInfo >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCounterItemInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCounterItemInfo"; };

		private:
			uint32_t m_uid;
			std::vector< CounterItemInfo > v_cii;

			const char* m_szConsulta = "pangya.ProcGetNewCounterItem"; //"pangya.ProcGetCountItem";
	};
}

#endif // !_STDA_CMD_COUNTER_ITEM_INFO_HPP
