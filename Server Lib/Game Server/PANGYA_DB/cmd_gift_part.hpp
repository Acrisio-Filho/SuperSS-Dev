// Arquivo cmd_gift_part.hpp
// Criado em 31/05/2018 as 22:25 por Acrisio
// Defini��o da classe CmdGiftPart

#pragma once
#ifndef _STDA_CMD_GIFT_PART_HPP
#define _STDA_CMD_GIFT_PART_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdGiftPart : public pangya_db {
		public:
			explicit CmdGiftPart(bool _waiter = false);
			CmdGiftPart(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdGiftPart();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdGiftPart"; };
			virtual std::wstring _wgetName() override { return L"CmdGiftPart"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsulta[2] = { "UPDATE pangya.pangya_item_warehouse SET Gift_flag = 1 WHERE UID = ", " AND item_id = " };
	};
}

#endif // !_STDA_CMD_GIFT_PART_HPP
