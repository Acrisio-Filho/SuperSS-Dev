// Arquivo cmd_gera_ucc_web_key.hpp
// Criado em 14/07/2018 as 18:41 por Acrisio
// Defini��o da classe CmdGeraUCCWebKey

#pragma once
#ifndef _STDA_CMD_GERA_UCC_WEB_KEY_HPP
#define _STDA_CMD_GERA_UCC_WEB_KEY_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdGeraUCCWebKey : public pangya_db {
		public:
			explicit CmdGeraUCCWebKey(bool _waiter = false);
			CmdGeraUCCWebKey(uint32_t _uid, int32_t _ucc_id, bool _waiter = false);
			virtual ~CmdGeraUCCWebKey();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getUCCID();
			void setUCCID(int32_t _ucc_id);

			std::string& getKey();

		protected:
			void lineResult(result_set::ctx_res *_result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdGeraUCCWebKey"; };
			virtual std::wstring _wgetName() override { return L"CmdGeraUCCWebKey"; };

		private:
			uint32_t m_uid;
			int32_t m_ucc_id;
			std::string m_key;

			const char* m_szConsulta = "pangya.ProcGeraSecurityKey";
	};
}

#endif // !_STDA_CMD_GERA_UCC_WEB_KEY_HPP
