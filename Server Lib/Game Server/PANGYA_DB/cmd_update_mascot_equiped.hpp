// Arquivo cmd_update_mascot_equiped.hpp
// Criado em 25/03/2018 as 11:42 por Acrisio
// Defini��o da classe CmdUpdateMascotEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_MASCOT_EQUIPED_HPP
#define _STDA_CMD_UPDATE_MASCOT_EQUIPED_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateMascotEquiped : public pangya_db {
		public:
			explicit CmdUpdateMascotEquiped(bool _waiter = false);
			CmdUpdateMascotEquiped(uint32_t _uid, int32_t _mascot_id, bool _waiter = false);
			virtual ~CmdUpdateMascotEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getMascotID();
			void setMascotID(int32_t _mascot_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateMascotEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateMascotEquiped"; };

		private:
			uint32_t m_uid;
			int32_t m_mascot_id;

			const char* m_szConsulta = "pangya.USP_FLUSH_MASCOT";
	};
}

#endif // !_STDA_CMD_UPDATE_MASCOT_EQUIPED
