// Arquivo cmd_update_user_info.hpp
// Criado em 08/09/2018 as 12:36 por Acrisio
// Defini��o da classe CmdUpdateUserInfo

#pragma once
#ifndef _STDA_CMD_UPDATE_USER_INFO_HPP
#define _STDA_CMD_UPDATE_USER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateUserInfo : public pangya_db {
		public:
			explicit CmdUpdateUserInfo(bool _waiter = false);
			CmdUpdateUserInfo(uint32_t _uid, UserInfoEx& _ui, bool _waiter = false);
			virtual ~CmdUpdateUserInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			UserInfoEx& getInfo();
			void setInfo(UserInfoEx& _ui);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateUserInfo"; };
			std::wstring _wgetName() override { return L"CmdUpdateUserInfo"; };

		private:
			uint32_t m_uid;
			UserInfoEx m_ui;

			const char* m_szConsulta = "pangya.ProcUpdateUserInfo";
	};
}

#endif // !_STDA_CMD_UPDATE_USER_INFO_HPP
