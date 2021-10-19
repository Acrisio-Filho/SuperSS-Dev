// Arquivo cmd_card_equip_info
// Criado em 21/03/2018 as 22:27 por Acrisio
// Defini��o da classe CmdCardEquipInfo

#pragma once
#ifndef _STDA_CMD_CARD_EQUIP_INFO_HPP
#define _STDA_CMD_CARD_EQUIP_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdCardEquipInfo : public pangya_db {
		public:
			explicit CmdCardEquipInfo(bool _waiter = false);
			CmdCardEquipInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdCardEquipInfo();

			std::vector< CardEquipInfoEx >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_reuslt) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCardEquipInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCardEquipInfo"; };

		private:
			uint32_t m_uid;
			std::vector< CardEquipInfoEx > v_cei;

			const char* m_szConsulta = "pangya.ProcGetCardEquip";
	};
}

#endif // !_STDA_CMD_CARD_EQUIP_INFO_HPP
