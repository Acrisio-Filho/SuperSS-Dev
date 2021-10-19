// Arquivo cmd_gera_web_key.hpp
// Criado em 25/03/2018 as 17:17 por Acrisio
// Defini��o da classe CmdGeraWebKey

#pragma once
#ifndef _STDA_CMD_GERA_WEB_KEY_HPP
#define _STDA_CMD_GERA_WEB_KEY_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdGeraWebKey : public pangya_db {
		public:
			explicit CmdGeraWebKey(bool _waiter = false);
			CmdGeraWebKey(uint32_t _uid, bool _waiter = false);
			virtual ~CmdGeraWebKey();

			std::string& getKey();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdGeraWebKey"; };
			virtual std::wstring _wgetName() override { return L"CmdGeraWebKey"; };

		private:
			uint32_t m_uid;
			std::string m_web_key;

			const char* m_szConsulta = "pangya.ProcGeraWeblinkKey";
	};
}

#endif // !_STDA_CMD_GERA_WEB_KEY_HPP
