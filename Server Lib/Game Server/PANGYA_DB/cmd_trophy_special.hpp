// Arquivo cmd_trophy_special.hpp
// Criado em 21/03/2018 as 23:00 por Acrisio
// Defini��o da classe CmdTrophySpecial

#pragma once
#ifndef _STDA_CMD_TROPHY_SPECIAL_HPP
#define _STDA_CMD_TROPHY_SPECIAL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdTrophySpecial : public pangya_db {
		public:
			enum TYPE_SEASON : unsigned char {
				ALL,
				ONE,
				TWO,
				THREE,
				FOUR,
				CURRENT,
			};

			enum TYPE : unsigned char {
				NORMAL,
				GRAND_PRIX,
			};

		public:
			explicit CmdTrophySpecial(bool _waiter = false);
			CmdTrophySpecial(uint32_t _uid, TYPE_SEASON _season, TYPE _type, bool _waiter = false);
			virtual ~CmdTrophySpecial();

			std::vector< TrofelEspecialInfo >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE_SEASON getSeason();
			void setSeason(TYPE_SEASON _season);

			TYPE getType();
			void setType(TYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdTrophySpecial"; };
			virtual std::wstring _wgetName() override { return L"CmdTrophySpecial"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			TYPE_SEASON m_season;
			std::vector< TrofelEspecialInfo > v_tei;

			const char* m_szConsulta[2] = { "pangya.ProcGetTrofelSpecial", "pangya.ProcGetTrofelGrandPrix" };
	};
}

#endif // !_STDA_CMD_TROPHY_SPECIAL_HPP
