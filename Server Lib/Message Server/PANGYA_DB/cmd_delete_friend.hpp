// Arquivo cmd_delete_friend.hpp
// Criado em 05/08/2018 as 20:39 por Acrisio
// Defini��o da classe CmdDeleteFriend

#pragma once
#ifndef _STDA_CMD_DELETE_FRIEND_HPP
#define _STDA_CMD_DELETE_FRIEND_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteFriend : public pangya_db {
		public:
			explicit CmdDeleteFriend(bool _waiter = false);
			CmdDeleteFriend(uint32_t _uid, uint32_t _friend_uid, bool _waiter = false);
			virtual ~CmdDeleteFriend();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getFriendUID();
			void setFriendUID(uint32_t _friend_uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdDeleteFriend"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteFriend"; };

		private:
			uint32_t m_uid;
			uint32_t m_friend_uid;

			const char* m_szConsulta[2] = { "DELETE FROM pangya.pangya_friend_list WHERE UID = ", " AND uid_friend = " };
	};
}

#endif // !_STDA_CMD_DELETE_FRIEND_HPP
