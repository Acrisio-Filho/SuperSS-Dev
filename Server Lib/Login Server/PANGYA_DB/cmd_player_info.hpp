// Arquivo cmd_player_info.hpp
// Criado em 17/03/2018 as 18:18 por Acrisio
// Definição da classe CmdPlayerInfo

#pragma once
#ifndef _STDA_CMD_PLAYER_INFO_HPP
#define _STDA_CMD_PLAYER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_login_st.h"

namespace stdA {
    class CmdPlayerInfo : public pangya_db {
        public:
            CmdPlayerInfo(bool _waiter = false);
			CmdPlayerInfo(uint32_t _uid, bool _waiter = false);
            ~CmdPlayerInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

            player_info& getInfo();
            void updateInfo(player_info& _pi);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdPlayerInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdPlayerInfo"; };

        protected:
            player_info m_pi;
			uint32_t m_uid;

		private:
			const char* m_szConsulta = "pangya.ProcGetPlayerInfoLogin";
    };
}

#endif