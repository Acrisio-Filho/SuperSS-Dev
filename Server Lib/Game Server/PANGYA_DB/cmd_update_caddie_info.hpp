// Arquivo cmd_update_caddie_info.hpp
// Criado em 23/09/2018 as 19:55 por Acrisio
// Defini��o da classe CmdUpdateCaddieInfo

#pragma once
#ifndef _STDA_CMD_UPDATE_CADDIE_INFO_HPP
#define _STDA_CMD_UPDATE_CAdDIE_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateCaddieInfo : public pangya_db {
		public:
			explicit CmdUpdateCaddieInfo(bool _waiter = false);
			CmdUpdateCaddieInfo(uint32_t _uid, CaddieInfoEx& _ci, bool _waiter = false);
			virtual ~CmdUpdateCaddieInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			CaddieInfoEx& getInfo();
			void setInfo(CaddieInfoEx& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateCaddieInfo"; };
			std::wstring _wgetName() override { return L"CmdUpdateCaddieInfo"; };

		private:
			uint32_t m_uid;
			CaddieInfoEx m_ci;

			const char* m_szConsulta = "pangya.ProcUpdateCaddieInfo";
	};
}

#endif // !_STDA_CMD_UPDATE_CADDIE_INFO_HPP
