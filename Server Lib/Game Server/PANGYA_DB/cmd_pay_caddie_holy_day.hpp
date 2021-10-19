// Arquivo cmd_pay_caddie_holy_day.hpp
// Criado em 15/07/2018 as 18:26 por Acrisio
// Defini��o da classe CmdPayCaddieHolyDay

#pragma once
#ifndef _STDA_CMD_PAY_CADDIE_HOLY_DAY_HPP
#define _STDA_CMD_PAY_CADDIE_HOLY_DAY_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdPayCaddieHolyDay : public pangya_db {
		public:
			explicit CmdPayCaddieHolyDay(bool _waiter = false);
			CmdPayCaddieHolyDay(uint32_t _uid, int32_t _id, std::string& _end_dt, bool _waiter = false);
			virtual ~CmdPayCaddieHolyDay();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getId();
			void setId(int32_t _id);

			std::string& getEndDate();
			void setEndDate(std::string& _end_dt);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPayCaddieHolyDay"; };
			virtual std::wstring _wgetName() override { return L"CmdPayCaddieHolyDay"; };

		private:
			uint32_t m_uid;
			int32_t m_id;
			std::string m_end_dt;

			const char* m_szConsulta = "pangya.ProcUpdateCaddieHolyDay";
	};
}

#endif // !_STDA_CMD_PAY_CADDIE_HOLY_DAY_HPP
