// Arquivo cmd_delete_dolfini_locker_item.hpp
// Criado em 03/06/2018 as 00:44 por Acrisio
// Defini��o da classe CmdDeleteDolfiniLockerItem

#pragma once
#ifndef _STDA_CMD_DELETE_DOLFINI_LOCKER_ITEM_HPP
#define _STDA_CMD_DELETE_DOLFINI_LOCKER_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteDolfiniLockerItem : public pangya_db {
		public:
			explicit CmdDeleteDolfiniLockerItem(bool _waiter = false);
			CmdDeleteDolfiniLockerItem(uint32_t _uid, uint64_t _index, bool _waiter = false);
			virtual ~CmdDeleteDolfiniLockerItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getIndex();
			void setIndex(uint64_t _index);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteDolfiniLockerItem"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteDolfiniLockerItem"; };

		private:
			uint32_t m_uid;
			uint64_t m_index;

			const char* m_szConsulta = "pangya.ProcMoveItemDolfiniLocker";
	};
}

#endif // !_STDA_CMD_DELETE_DOLFINI_LOCKER_ITEM_HPP
