// Arquivo cmd_update_card_qntd.hpp
// Criado em 31/05/2018 as 09:54 por Acrisio
// Defini��o da classe CmdUpdateCardQntd

#pragma once
#ifndef _STDA_CMD_UPDATE_CARD_QNTD_HPP
#define _STDA_CMD_UPDATE_CARD_QNTD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateCardQntd : public pangya_db {
		public:
			explicit CmdUpdateCardQntd(bool _waiter = false);
			CmdUpdateCardQntd(uint32_t _uid, int32_t _id, uint32_t _qntd, bool _waiter = false);
			virtual ~CmdUpdateCardQntd();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

			uint32_t getQntd();
			void setQntd(uint32_t _qntd);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCardQntd"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCardQntd"; };

		private:
			uint32_t m_uid;
			int32_t m_id;
			uint32_t m_qntd;

			const char* m_szConsulta[3] = { "UPDATE pangya.pangya_card SET QNTD = ", " WHERE UID = ", " AND card_itemid = " };
	};
}

#endif // !_STDA_CMD_UPDATE_CARD_QNTD_HPP
