// Arquivo cmd_update_friend.hpp
// Criado em 05/08/2018 as 22:15 por Acrisio
// Defini��o da classe CmdUpdateFriend.hpp

#pragma once
#ifndef _STDA_CMD_UPDATE_FRIEND_HPP
#define _STDA_CMD_UPDATE_FRIEND_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_message_st.hpp"

namespace stdA {
	class CmdUpdateFriend : public pangya_db {
		public:
			explicit CmdUpdateFriend(bool _waiter = false);
			CmdUpdateFriend(uint32_t _uid, FriendInfoEx& _fi, bool _waiter = false);
			virtual ~CmdUpdateFriend();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			FriendInfoEx& getInfo();
			void setInfo(FriendInfoEx& _fi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateFriend"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateFriend"; };

		private:
			uint32_t m_uid;
			FriendInfoEx m_fi;

			const char* m_szConsulta = "pangya.ProcUpdateFriendInfo";
	};
}

#endif // !_STDA_CMD_UPDATE_FRIEND_HPP
