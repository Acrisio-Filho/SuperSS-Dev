// Arquivo cmd_update_mascot_info.hpp
// Criado em 23/03/2018 as 20:42 por Acrisio
// Defini��o da classe CmdUpdateMascotInfo

#pragma once
#ifndef _STDA_CMD_UPDATE_MASCOT_INFO_HPP
#define _STDA_CMD_UPDATE_MASCOT_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateMascotInfo : public pangya_db {
		public:
			explicit CmdUpdateMascotInfo(bool _waiter = false);
			CmdUpdateMascotInfo(uint32_t _uid, MascotInfoEx& _mi, bool _waiter = false);
			virtual ~CmdUpdateMascotInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			MascotInfoEx& getInfo();
			void setInfo(MascotInfoEx& _mi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateMascotInfo"; };
			std::wstring _wgetName() override { return L"CmdUpdateMascotInfo"; };

		private:
			uint32_t m_uid;
			MascotInfoEx m_mi;

			const char* m_szConsulta = "pangya.ProcUpdateMascotInfo";
	};
}

#endif // !_STDA_CMD_UPDATE_MASCOT_INFO_HPP
