// Arquivo cmd_add_counter_item.hpp
// Criado em 14/04/2018 as 15:25 por Acrisio
// Defini��o da classe CmdAddCounterItem

#pragma once
#ifndef _STDA_CMD_ADD_COUNTER_ITEM_HPP
#define _STDA_CMD_ADD_COUNTER_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdAddCounterItem : public pangya_db {
		public:
			explicit CmdAddCounterItem(bool _waiter = false);
			CmdAddCounterItem(uint32_t _uid, uint32_t _typeid, uint32_t _value, bool _waiter = false);
			virtual ~CmdAddCounterItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			uint32_t getValue();
			void setValue(uint32_t _value);

			int32_t getId();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddCounterItem"; };
			virtual std::wstring _wgetName() override { return L"CmdAddCounterItem"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			uint32_t m_value;
			int32_t m_id;

			const char* m_szConsulta = "pangya.ProcAddCounterItem";
	};
}

#endif // !_STDA_CMD_ADD_COUNTER_ITEM_HPP
