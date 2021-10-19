// Arquivo cmd_use_item_buff.hpp
// Criado em 07/07/2018 as 21:25 por Acrisio
// Defini��o da classe CmdUseItemBuff

#pragma once
#ifndef _STDA_CMD_USE_ITEM_BUFF_HPP
#define _STDA_CMD_USE_ITEM_BUFF_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUseItemBuff : public pangya_db {
		public:
			explicit CmdUseItemBuff(bool _waiter = false);
			CmdUseItemBuff(uint32_t _uid, ItemBuffEx& _ib, uint32_t _time, bool _waiter = false);
			virtual ~CmdUseItemBuff();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTime();
			void setTime(uint32_t _time);

			ItemBuffEx& getInfo();
			void setInfo(ItemBuffEx& _ib);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUseItemBuff"; };
			virtual std::wstring _wgetName() override { return L"CmdUseItemBuff"; };

		private:
			uint32_t m_uid;
			uint32_t m_time;
			ItemBuffEx m_ib;

			const char* m_szConsulta = "pangya.ProcUseItemBuff";
	};
}

#endif // !_STDA_CMD_USE_ITEM_BUFF_HPP
