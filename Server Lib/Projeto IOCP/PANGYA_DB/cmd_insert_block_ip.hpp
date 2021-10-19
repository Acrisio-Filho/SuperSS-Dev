// Arquivo cmd_insert_block_ip.hpp
// Criado em 14/08/2019 as 11:28 por Acrisio
// Definil��o da classe CmdInsertBlockIP

#pragma once
#ifndef _STDA_CMD_INSERT_BLOCK_IP_HPP
#define _STDA_CMD_INSERT_BLOCK_IP_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdInsertBlockIP : public pangya_db {
		public:
			explicit CmdInsertBlockIP(bool _waiter = false);
			CmdInsertBlockIP(std::string _ip, std::string _mask, bool _waiter = false);
			virtual ~CmdInsertBlockIP();

			std::string& getIP();
			void setIP(std::string _ip);

			std::string& getMask();
			void setMask(std::string _mask);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdInsertBlockIP"; };
			virtual std::wstring _wgetName() override { return L"CmdInsertBlockIP"; };

		private:
			std::string m_ip;
			std::string m_mask;

			const char* m_szConsulta = "pangya.ProcInsertBlockIP";
	};
}

#endif // !_STDA_CMD_INSERT_BLOCK_IP_HPP
