// Arquivo cmd_add_friend.hpp
// Criado em 04/08/2018 as 22:17 por Acrisio
// Defini��o da classe CmdAddFriend

#pragma once
#ifndef _STDA_CMD_ADD_FRIEND_HPP
#define _STDA_CMD_ADD_FRIEND_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_message_st.hpp"

namespace stdA {
	class CmdAddFriend : public pangya_db {
		public:
			explicit CmdAddFriend(bool _waiter = false);
			CmdAddFriend(uint32_t _uid, FriendInfoEx& _fi, bool _waiter = false);
			virtual ~CmdAddFriend();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			FriendInfoEx& getInfo();
			void setInfo(FriendInfoEx& _fi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdAddFriend"; };
			virtual std::wstring _wgetName() override { return L"CmdAddFriend"; };

		private:
			uint32_t m_uid;
			FriendInfoEx m_fi;

			const char* m_szConsulta = "pangya.ProcAddFriend";
	};
}

#endif // !_STDA_CMD_ADD_FRIEND_HPP
