// Arquivo cmd_update_caddie_equiped.hpp
// Criado em 25/03/2018 as 10:35 por Acrisio
// Defini��o da classe CmdUpdateCaddieEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_CADDIE_EQUIPED_HPP
#define _STDA_CMD_UPDATE_CADDIE_EQUIPED_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateCaddieEquiped : public pangya_db {
		public:
			explicit CmdUpdateCaddieEquiped(bool _waiter = false);
			CmdUpdateCaddieEquiped(uint32_t _uid, int32_t _caddie_id, bool _waiter = false);
			virtual ~CmdUpdateCaddieEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getCaddieID();
			void setCaddieID(int32_t _caddie_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCaddieEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCaddieEquiped"; };

		private:
			uint32_t m_uid;
			int32_t m_caddie_id;

			const char* m_szConsulta = "pangya.USP_FLUSH_CADDIE";
	};
}

#endif // !_STDA_CMD_UPDATE_CADDIE_EQUIPED_HPP
