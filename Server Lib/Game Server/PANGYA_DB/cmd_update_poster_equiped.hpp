// Arquivo cmd_update_poster_equiped.hpp
// Criado em 25/03/2018 as 12:56 por Acrisio
// Defini��o da classe CmdUpdatePosterEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_POSTER_EQUIPED_HPP
#define _STDA_CMD_UPDATE_POSTER_EQUIPED_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdatePosterEquiped : public pangya_db {
		public:
			explicit CmdUpdatePosterEquiped(bool _waiter = false);
			CmdUpdatePosterEquiped(uint32_t _uid, UserEquip& _ue, bool _waiter = false);
			virtual ~CmdUpdatePosterEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			UserEquip& getInfo();
			void setInfo(UserEquip& _ue);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdatePosterEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdatePosterEquiped"; };

		private:
			uint32_t m_uid;
			UserEquip m_ue;

			const char* m_szConsulta = "pangya.USP_FLUSH_EQUIP_POSTER";
	};
}

#endif // !_STDA_CMD_UPDATE_POSTER_EQUIPED_HPP
