// Arquivo cmd_tuto_event_clear.hpp
// Criado em 28/06/2018 as 21:56 por Acrisio
// Defini��o da classe CmdTutoEventClear

#pragma once
#ifndef _STDA_CMD_TUTO_EVENT_CLEAR_HPP
#define _STDA_CMD_TUTO_EVENT_CLEAR_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdTutoEventClear : public pangya_db {
		public:
			enum TYPE : unsigned {
				T_ROOKIE,
				T_BEGINNER,
				T_ADVANCER,
			};

		public:
			explicit CmdTutoEventClear(bool _waiter = false);
			CmdTutoEventClear(uint32_t _uid, TYPE _type, bool _waiter = false);
			virtual ~CmdTutoEventClear();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdTutoEventClear"; };
			virtual std::wstring _wgetName() override { return L"CmdTutoEventClear"; };

		private:
			uint32_t m_uid;
			TYPE m_type;

			const char* m_szConsulta[5] = { "UPDATE pangya.account SET ", "Event1 = 1", "Event2 = 1", "Event3 = 1", " WHERE UID =" };
	};
}

#endif // !_STDA_CMD_TUTO_EVENT_CLEAR_HPP
