// Arquivo cmd_update_item_slot.hpp
// Criado em 25/03/2018 as 09:56 por Acrisio
// Defini��o da classe CmdUpdateItemSlot

#pragma once
#ifndef _STDA_CMD_UPDATE_ITEM_SLOT_HPP
#define _STDA_CMD_UPDATE_ITEN_SLOT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateItemSlot : public pangya_db {
		public:
			explicit CmdUpdateItemSlot(bool _waiter = false);
			CmdUpdateItemSlot(uint32_t _uid, uint32_t *_slot, bool _waiter = false);
			virtual ~CmdUpdateItemSlot();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t* getSlot();
			void setSlot(uint32_t *_slot);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateItemSlot"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateItemSlot"; };

		private:
			uint32_t m_uid;
			uint32_t m_slot[10];

			const char* m_szConsulta[11] = { "UPDATE pangya.pangya_user_equip SET item_slot_1 = ", ", item_slot_2 = ", ", item_slot_3 = ",
										   ", item_slot_4 = ", ", item_slot_5 = ", ", item_slot_6 = ", ", item_slot_7 = ", ", item_slot_8 = ",
										   ", item_slot_9 = ", ", item_slot_10 = ", " WHERE uid = " };
	};
}

#endif // !_STDA_CMD_UPDATA_ITEM_SLOT_HPP
