// Arquivo mail_box_manager.hpp
// Criado em 26/05/2018 as 14:39 por Acrisio
// Defini��o da classe MailBoxManager

#pragma once 
#ifndef _STDA_CMD_MAIL_BOX_MANAGER_HPP
#define _STDA_CMD_MAIL_BOX_MANAGER_HPP

#include "../TYPE/pangya_game_st.h"
#include <string>
#include <vector>

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class MailBoxManager {
		public:
			MailBoxManager();
			~MailBoxManager();

			static int32_t sendMessage(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg);
			static int32_t sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, std::vector< stItem >& _v_item);
			static int32_t sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, stItem& _item);

			static int32_t sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, EmailInfo::item* _pItem, uint32_t _count);
			static int32_t sendMessageWithItem(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg, EmailInfo::item& _item);

		protected:
			static inline int32_t _sendMessage(uint32_t _from_uid, uint32_t _to_uid, std::string& _msg);

			static void putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, std::vector< stItem >& _v_item);
			static inline void putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, stItem& _item);

			static void putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, EmailInfo::item* _pItem, uint32_t _count);
			static inline void putItemInMail(uint32_t _from_uid, uint32_t _to_uid, int32_t _mail_id, EmailInfo::item& _item);

			static inline void putCommandNewMail(uint32_t _to_uid, int32_t _mail_id);

			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);
	};
}

#endif // !_STDA_CMD_MAIL_BOX_MANAGER_HPP
