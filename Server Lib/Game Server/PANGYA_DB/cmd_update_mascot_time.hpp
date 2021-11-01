// Arquivo cmd_update_mascot_time.hpp
// Criado em 29/05/2018 as 21:23 por Acrisio
// Definie��o da classe CmdUpdateMascotTime

#pragma once
#ifndef _STDA_CMD_UPDATE_MASCOT_TIME_HPP
#define _STDA_CMD_UPDATE_MASCOT_TIME_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdUpdateMascotTime : public pangya_db {
		public:
			explicit CmdUpdateMascotTime(bool _waiter = false);
			CmdUpdateMascotTime(uint32_t _uid, int32_t _id, std::string& _time, bool _waiter = false);
			virtual ~CmdUpdateMascotTime();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

			std::string& getTime();
			void setTime(std::string& _time);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateMascotTime"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateMascotTime"; };

		private:
			uint32_t m_uid;
			int32_t m_id;
			std::string m_time;

			const char* m_szConsulta = "pangya.ProcUpdateMascotTime";
	};
}

#endif // !_STDA_CMD_UPDATE_MASCOT_TIME_HPP
