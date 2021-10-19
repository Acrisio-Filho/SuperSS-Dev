// Arquivo cmd_shutdown_info.hpp
// Criado em 02/12/2018 as 23:14 por Acrisio
// Defini��o da classe CmdShutdownInfo

#pragma once
#ifndef _STDA_CMD_SHUTDOWN_INFO_HPP
#define _STDA_CMD_SHUTDOWN_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdShutdownInfo : public pangya_db {
		public:
			explicit CmdShutdownInfo(bool _waiter = false);
			CmdShutdownInfo(uint32_t _id, bool _waiter = false);
			virtual ~CmdShutdownInfo();

			uint32_t getId();
			void setId(uint32_t _id);

			int32_t getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _result_index) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdShutdownInfo"; };
			std::wstring _wgetName() override { return L"CmdShutdownInfo"; };

		private:
			uint32_t m_id;
			int32_t m_time_sec;

			const char* m_szConsulta = "pangya.ProcGetShutdownServer";
	};
}

#endif // !_STDA_CMD_SHUTDOWN_INFO_HPP
