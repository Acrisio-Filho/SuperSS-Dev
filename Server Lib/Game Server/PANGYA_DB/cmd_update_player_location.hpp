// Arquivo cmd_update_player_location.hpp
// Criado em 11/05/2019 as 17:32 por Acrisio
// Definição da classe CmdUpdatePlayerLocation

#pragma once
#ifndef _STDA_CMD_UPDATE_PLAYER_LOCATION_HPP
#define _STDA_CMD_UPDATE_PLAYER_LOCATION_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdUpdatePlayerLocation : public pangya_db {
        public:
            CmdUpdatePlayerLocation(stPlayerLocationDB& _pl, bool _waiter = false);
            explicit CmdUpdatePlayerLocation(uint32_t _uid, stPlayerLocationDB& _pl, bool _waiter = false);
            virtual ~CmdUpdatePlayerLocation();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			stPlayerLocationDB& getInfo();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            virtual std::string _getName() override { return "CmdUpdatePlayerLocation"; };
            virtual std::wstring _wgetName() override { return L"CmdUpdatePlayerLocation"; };
        
        private:
			stPlayerLocationDB& m_pl;
			uint32_t m_uid;

            const char *m_szConsulta = "pangya.ProcUpdatePlayerLocation";
    };
}

#endif // !_STDA_CMD_UPDATE_PLAYER_LOCATION_HPP