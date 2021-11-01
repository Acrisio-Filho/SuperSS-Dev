// Arquivo cmd_trofel_info.hpp
// Criado em 18/03/2018 as 16:13 por Acrisio
// Definição da classe CmdTrofelInfo

#pragma once
#ifndef _STDA_CMD_TROFEL_INFO_HPP
#define _STDA_CMD_TROFEL_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdTrofelInfo : public pangya_db {
		public:
			enum TYPE_SEASON : unsigned char {
				ALL,		// Todas SEASON
				ONE,		// 1
				TWO,		// 2
				THREE,		// 3
				FOUR,		// 4
				CURRENT		// Atual
			};

        public:
            explicit CmdTrofelInfo(bool _waiter = false);
            CmdTrofelInfo(uint32_t _uid, TYPE_SEASON _season, bool _waiter = false);
            virtual ~CmdTrofelInfo();

            TrofelInfo& getInfo();
            void setInfo(TrofelInfo& _ti);

            uint32_t getUID();
            void setUID(uint32_t _uid);

			TYPE_SEASON getSeason();
			void setSeason(TYPE_SEASON _season);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdTrofelInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdTrofelInfo"; };

        private:
			TYPE_SEASON m_season;
            uint32_t m_uid;
            TrofelInfo m_ti;

			const char* m_szConsulta = "pangya.ProcGetTrofel";
    };
}

#endif