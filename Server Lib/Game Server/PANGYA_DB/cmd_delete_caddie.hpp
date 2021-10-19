// Arquivo cmd_delete_caddie.hpp
// Criado em 16/06/2018 as 12:58 por Acrisio
// Defini��o da classe CmdDeleteCaddie

#pragma once
#ifndef _STDA_CMD_DELETE_CADDIE_HPP
#define _STDA_CMD_DELETE_CADDIE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteCaddie : public pangya_db {
		public:
			explicit CmdDeleteCaddie(bool _waiter = false);
			CmdDeleteCaddie(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdDeleteCaddie();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteCaddie"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteCaddie"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsulta[2] = { "UPDATE pangya.pangya_caddie_information SET valid = 0 WHERE UID = ", " AND item_id = " };
	};
}

#endif // !_STDA_CMD_DELETE_CADDIE_HPP
