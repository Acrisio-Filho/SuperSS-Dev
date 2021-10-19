// Arquivo cmd_put_item_mail_box.hpp
// Criado em 26/05/2018 as 15:31 por Acrisio
// Defini��o da classe CmdPutItemMailBox

#pragma once
#ifndef _STDA_CMD_PUT_ITEM_MAIL_BOX_HPP
#define _STDA_CMD_PUT_ITEM_MAIL_BOX_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdPutItemMailBox : public pangya_db {
		public:
			explicit CmdPutItemMailBox(bool _waiter = false);
			explicit CmdPutItemMailBox(uint32_t _uid_from, uint32_t _uid_to, int32_t _mail_id, stItem& _item, bool _waiter = false);
			CmdPutItemMailBox(uint32_t _uid_from, uint32_t _uid_to, int32_t _mail_id, EmailInfo::item& _item, bool _waiter = false);
			virtual ~CmdPutItemMailBox();

			uint32_t getUIDFrom();
			void setUIDFrom(uint32_t _uid_from);

			uint32_t getUIDTo();
			void setUIDTo(uint32_t _uid_to);

			int32_t getMailID();
			void setLong(int32_t _mail_id);

			stItem& getItem();
			void setItem(stItem& _item);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPutItemMailBox"; };
			virtual std::wstring _wgetName() override { return L"CmdPutItemMailBox"; };

		private:
			uint32_t m_uid_from;
			uint32_t m_uid_to;
			int32_t m_mail_id;
			stItem m_item;

			const char* m_szConsulta = "pangya.ProcInsertItemNoEmail";
	};
}

#endif // !_STDA_CMD_PUT_ITEM_MAIL_BOX_HPP
