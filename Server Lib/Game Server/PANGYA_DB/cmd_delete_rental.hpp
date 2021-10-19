// Arquivo cmd_delete_rental.hpp
// Criado em 10/06/2018 as 17:42 por Acrisio
// Defini��o da classe CmdDeleteRental

#pragma once
#ifndef _STDA_CMD_DELETE_RENTAL_HPP
#define _STDA_CMD_DELETE_RENTAL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteRental : public pangya_db {
		public:
			explicit CmdDeleteRental(bool _waiter = false);
			CmdDeleteRental(uint32_t _uid, int32_t _item_id, bool _waiter = false);
			virtual ~CmdDeleteRental();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getItemID();
			void setItemID(int32_t _item_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteRental"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteRental"; };

		private:
			uint32_t m_uid;
			int32_t m_item_id;

			const char* m_szConsulta[2] = { "UPDATE pangya.pangya_item_warehouse SET valid = 0 WHERE UID = ", " AND item_id = " };
	};
}

#endif // !_STDA_CMD_DELETE_RENTAL_HPP
