// Arquivo cmd_delete_card.hpp
// Criado em 31/05/2018 as 21:29 por Acrisio
// Defini��o da classe CmdDeleteCard

#pragma once
#ifndef _STDA_CMD_DELETE_CARD_HPP
#define _STDA_CMD_DELETE_CARD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteCard : public pangya_db {
		public:
			explicit CmdDeleteCard(bool _waiter = false);
			CmdDeleteCard(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdDeleteCard();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteCard"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteCard"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsulta[3] = { "UPDATE pangya.pangya_card SET QNTD = 0, USE_YN = ", " WHERE UID = ", " AND card_itemid = " };
	};
}

#endif // !_STDA_CMD_DELETE_CARD_HPP
