// Arquivo cmd_friend_info.hpp
// Criado em 30/07/2018 as 00:19 por Acrisio
// Defini��o da classe CmdFriendInfo

#pragma once
#ifndef _STDA_CMD_FRIEND_INFO_HPP
#define _STDA_CMD_FRIEND_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_message_st.hpp"
#include <map>

namespace stdA {
	class CmdFriendInfo : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,
				ONE,
			};

		public:
			explicit CmdFriendInfo(bool _waiter = false);
			CmdFriendInfo(uint32_t _uid, TYPE _type, uint32_t _friend_uid = 0u, bool _waiter = false);
			virtual ~CmdFriendInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getFriendUID();
			void setFriendUID(uint32_t _friend_uid);

			TYPE getType();
			void setType(TYPE _type);

			std::map< uint32_t, FriendInfoEx >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdFriendInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdFriendInfo"; };

		private:
			uint32_t m_uid;
			uint32_t m_friend_uid;
			TYPE m_type;
			std::map< uint32_t, FriendInfoEx > m_fi;

			const char* m_szConsulta[2] = { "pangya.ProcGetFriendAndGuildMemberInfo", "pangya.ProcGetFriendAndGuildMemberInfo_One" };
	};
}

#endif // !_STDA_CMD_FRIEND_INFO_HPP
