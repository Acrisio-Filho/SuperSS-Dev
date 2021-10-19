// Arquivo cmd_pang.hpp
// Criado em 03/10/2019 as 18:07 por Acrisio
// Defini��o da classe CmdPang

#pragma once
#ifndef _STDA_CMD_PANG_HPP
#define _STDA_CMD_PANG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdPang : public pangya_db {
		public:
			explicit CmdPang(bool _waiter = false);
			CmdPang(uint32_t _uid, bool _waiter = false);
			virtual ~CmdPang();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getPang();
			void setPang(uint64_t _pang);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdPang";  };
			std::wstring _wgetName() override { return L"CmdPang"; };
		
		private:
			uint32_t m_uid;
			uint64_t m_pang;

			const char* m_szConsulta = "SELECT uid, pang FROM pangya.user_info WHERE UID = ";
	};
}

#endif // !_STDA_CMD_PANG_HPP
