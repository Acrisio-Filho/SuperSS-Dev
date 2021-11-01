// Arquivo cmd_add_dolfini_locker_item.hpp
// Criado em 02/06/2018 as 23:20 por Acrisio
// Defini��o da classe CmdAddDolfiniLockerItem

#pragma once
#ifndef _STDA_CMD_ADD_DOLFINI_LOCKER_ITEM_HPP
#define _STDA_CMD_ADD_DOLFINI_LOCKER_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddDolfiniLockerItem : public pangya_db {
		public:
			explicit CmdAddDolfiniLockerItem(bool _waiter = false);
			CmdAddDolfiniLockerItem(uint32_t _uid, DolfiniLockerItem& _dli, bool _waiter = false);
			virtual ~CmdAddDolfiniLockerItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			DolfiniLockerItem& getInfo();
			void setInfo(DolfiniLockerItem& _dli);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddDolfiniLockerItem"; };
			virtual std::wstring _wgetName() override { return L"CmdAddDolfiniLockerItem"; };

		private:
			uint32_t m_uid;
			DolfiniLockerItem m_dli;

			const char* m_szConsulta = "pangya.ProcAddItemDolfiniLocker";
	};
}

#endif // !_STDA_CMD_ADD_DOLFINI_LOCKER_ITEM_HPP
