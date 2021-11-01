// Arquivo cmd_verify_ip.hpp
// Crido em 30/03/2018 as 09:52 por Acrisio
// Defini��o da classe CmdVerifyIP

#pragma once
#ifndef _STDA_CMD_VERIFY_IP_HPP
#define _STDA_CMD_VERIFY_ip_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdVerifyIP : public pangya_db {
		public:
			CmdVerifyIP(bool _waiter = false);
			CmdVerifyIP(uint32_t _uid, std::string _ip, bool _waiter = false);
			~CmdVerifyIP();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::string& getIP();
			void setIP(std::string& _ip);

			bool getLastVerify();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdVerifyIP"; };
			virtual std::wstring _wgetName() override { return L"CmdVerifyIP"; };

		private:
			uint32_t m_uid;
			std::string m_ip;
			bool m_last_verify;

			const char* m_szConsulta = "pangya.ProcVerifyIP";
	};
}

#endif // !_STDA_CMD_VERIFY_IP_HPP
