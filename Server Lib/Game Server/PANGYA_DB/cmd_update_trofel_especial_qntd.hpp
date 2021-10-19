// Arquivo cmd_update_trofel_especial_qntd.hpp
// Criado em 21/06/2019 as 18:49 por Acrisio
// Defini��o classe CmdUpdateTrofelEspecialQntd

#pragma once
#ifndef _STDA_CMD_UPDATE_TROFEL_ESPECIAL_QNTD_HPP
#define _STDA_CMD_UPDATE_TROFEL_ESPECIAL_QNTD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateTrofelEspecialQntd : public pangya_db {
		public:
			enum eTYPE : unsigned char {
				ESPECIAL,
				GRAND_PRIX,
			};

		public:
			explicit CmdUpdateTrofelEspecialQntd(bool _waiter = false);
			CmdUpdateTrofelEspecialQntd(uint32_t _uid, int32_t _id, uint32_t _qntd, eTYPE _type, bool _waiter = false);
			virtual ~CmdUpdateTrofelEspecialQntd();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getId();
			void setId(int32_t _id);

			uint32_t getQntd();
			void setQntd(uint32_t _qntd);

			eTYPE getType();
			void setType(eTYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateTrofelEspecialQntd"; };
			std::wstring _wgetName() override { return L"CmdUpdateTrofelEspecialQntd"; };

		private:
			uint32_t m_uid;
			uint32_t m_qntd;
			int32_t m_id;
			eTYPE m_type;

			const char* m_szConsulta[2][3] = { { "UPDATE pangya.pangya_trofel_especial SET qntd = ", " WHERE UID = ", " AND item_id = " },
											   { "UPDATE pangya.pangya_trofel_grandprix SET qntd = ", " WHERE UID = ", " AND item_id = " } };
	};
}

#endif // !_STDA_CMD_UPDATE_TROFEL_ESPECIAL_QNTD_HPP
