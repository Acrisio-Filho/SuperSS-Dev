// Arquivo cmd_update_skin_equiped.hpp
// Criado em 25/03/2018 as 12:40 por Acrisio
// Defini��o da classe CmdUpdateSkinEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_SKIN_EQUIPED_HPP
#define _STDA_CMD_UPDATE_SKIN_EQUIPED_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateSkinEquiped : public pangya_db {
		public:
			explicit CmdUpdateSkinEquiped(bool _waiter = false);
			CmdUpdateSkinEquiped(uint32_t _uid, UserEquip& _ue, bool _waiter = false);
			virtual ~CmdUpdateSkinEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			UserEquip& getInfo();
			void setInfo(UserEquip& _ue);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateSkinEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateSkinEquiped"; };

		private:
			uint32_t m_uid;
			UserEquip m_ue;

			const char* m_szConsulta = "pangya.USP_FLUSH_SKIN";
	};
}

#endif // !_STDA_CMD_UPDATE_SKIN_EQUIPED_HPP
