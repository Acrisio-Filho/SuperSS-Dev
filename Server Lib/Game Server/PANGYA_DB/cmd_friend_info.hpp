// Arquivo cmd_friend_info.hpp
// Criado em 28/10/2018 as 15:33 por Acrisio
// Defini��o da classe CmdFriendInfo

#pragma once
#ifndef _STDA_CMD_FRIEND_INFO_HPP
#define _STDA_CMD_FRIEND_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>

namespace stdA {
	class CmdFriendInfo : public pangya_db {
		public:
			explicit CmdFriendInfo(bool _waiter = false);
			CmdFriendInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdFriendInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::map< uint32_t/*UID*/, FriendInfo >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdFriendInfo"; };
			std::wstring _wgetName() override { return L"CmdFriendInfo"; };

		private:
			uint32_t m_uid;
			std::map< uint32_t/*UID*/, FriendInfo > m_fi;

			const char* m_szConsulta = "pangya.ProcGetFriendInfo";
	};
}

#endif // !_STDA_CMD_FRIEND_INFO_HPP
