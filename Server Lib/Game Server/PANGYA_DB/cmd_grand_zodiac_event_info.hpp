// Arquivo cmd_grand_zodiac_event_info.hpp
// Criado em 26/06/2020 as 13:31 por Acrisio
// Defini��o da classe CmdGrandZodiacEventInfo

#pragma once
#ifndef _STDA_CMD_GRAND_ZODIAC_EVENT_INFO_HPP
#define _STDA_CMD_GRAND_ZODIAC_EVENT_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/grand_zodiac_type.hpp"

#include <vector>

namespace stdA {
	class CmdGrandZodiacEventInfo : public pangya_db {
		public:
			CmdGrandZodiacEventInfo(bool _waiter = false);
			virtual ~CmdGrandZodiacEventInfo();

			std::vector< range_time >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_reuslt) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdGrandZodiacEventInfo"; };
			std::wstring _wgetName() override { return L"CmdGrandZodiacEventInfo"; };

		private:
			std::vector< range_time > m_rt;

			const char* m_szConsulta = "SELECT inicio_time, fim_time, type FROM pangya.pangya_grand_zodiac_times WHERE valid = 1";
	};
}

#endif // !_STDA_CMD_GRAND_ZODIAC_EVENT_INFO_HPP
