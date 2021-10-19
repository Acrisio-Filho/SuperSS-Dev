// Arquivo cmd_delete_item.hpp
// Criado em 31/05/2018 as 18:29 por Acrisio
// Defini��o da classe CmdDeleteItem

#pragma once
#ifndef _STDA_CMD_DELETE_ITEM_HPP
#define _STDA_CMD_DELETE_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteItem : public pangya_db {
		public:
			explicit CmdDeleteItem(bool _waiter = false);
			CmdDeleteItem(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdDeleteItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteItem"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteItem"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsulta[2] = { "UPDATE pangya.pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = ", " AND item_id = " };
	};
}

#endif // !_STDA_CMD_DELETE_ITEM_HPP
