// Arquivo cmd_find_trofel_especial.hpp
// Criado em 21/06/2019 as 19:31 por Acrisio
// Defini��o da classe CmdFindTrofelEspecial

#pragma once
#ifndef _STDA_CMD_FIND_TROFEL_ESPECIAL_HPP
#define _STDA_CMD_FIND_TROFEL_ESPECIAL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFindTrofelEspecial : public pangya_db {
		public:
			enum eTYPE : unsigned char {
				ESPECIAL,
				GRAND_PRIX,
			};

		public:
			explicit CmdFindTrofelEspecial(bool _waiter = false);
			CmdFindTrofelEspecial(uint32_t _uid, uint32_t _typeid, eTYPE _type, bool _waiter = false);
			virtual ~CmdFindTrofelEspecial();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			eTYPE getType();
			void setType(eTYPE _type);

			bool hasFound();

			TrofelEspecialInfo& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdFindTrofelEspecial"; };
			std::wstring _wgetName() override { return L"CmdFindTrofelEspecial"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			TrofelEspecialInfo m_tsi;
			eTYPE m_type;

			const char* m_szConsulta[2] = { 
				"pangya.ProcFindTrofelSpecial",
				"pangya.ProcFindTrofelGrandPrix"
			};
	};
}

#endif // !_STDA_CMD_FIND_TROFEL_ESPECIAL_HPP
