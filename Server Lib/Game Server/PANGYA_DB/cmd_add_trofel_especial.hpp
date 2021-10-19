// Arquivo cmd_add_trofel_especial.hpp
// Criado em 21/06/2019 as 18:17 por Acrisio
// Defini��o da classe CmdAddTrofelEspecial

#pragma once
#ifndef _STDA_CMD_ADD_TROFEL_ESPECIAL_HPP
#define _STDA_CMD_ADD_TROFEL_ESPECIAL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddTrofelEspecial : public pangya_db {
		public:
			enum eTYPE : unsigned char {
				ESPECIAL,
				GRAND_PRIX,
			};

		public:
			explicit CmdAddTrofelEspecial(bool _waiter = false);
			CmdAddTrofelEspecial(uint32_t _uid, TrofelEspecialInfo& _tsi, eTYPE _type, bool _waiter = false);
			virtual ~CmdAddTrofelEspecial();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			eTYPE getType();
			void setType(eTYPE _type);

			TrofelEspecialInfo& getInfo();
			void setInfo(TrofelEspecialInfo& _tsi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdAddTrofelEspecial"; };
			std::wstring _wgetName() override { return L"CmdAddTrofelEspecial"; };

		private:
			uint32_t m_uid;
			TrofelEspecialInfo m_tsi;
			eTYPE m_type;

			const char* m_szConsulta[2] = { "pangya.ProcAddTrofelSpecial",
											"pangya.ProcAddTrofelGrandPrix"
			};
	};
}

#endif // !_STDA_CMD_ADD_TROFEL_ESPECIAL_HPP
