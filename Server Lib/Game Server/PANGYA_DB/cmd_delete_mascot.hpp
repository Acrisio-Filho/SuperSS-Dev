// Arquivo cmd_delete_mascot.hpp
// Criado em 16/06/2018 as 13:39 por Acrisio
// Defini��o da classe CmdDeleteMascot

#pragma once
#ifndef _STDA_CMD_DELETE_MASCOT_HPP
#define _STDA_CMD_DELETE_MASCOT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteMascot : public pangya_db {
		public:
			explicit CmdDeleteMascot(bool _waiter = false);
			CmdDeleteMascot(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdDeleteMascot();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteMascot"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteMascot"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsulat[2] = { "UPDATE pangya.pangya_mascot_info SET valid = 0 WHERE UID = ", " AND item_id = " };
	};
}

#endif // !_STDA_CMD_DELETE_MASCOT_HPP
