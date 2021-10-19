// Arquivo cmd_memorial_level_info.hpp
// Criado em 22/07/2018 as 12:09 por Acrisio
// Defini��o da classe CmdMemorialLevelInfo

#pragma once
#ifndef _STDA_CMD_MEMORIAL_LEVEL_INFO_HPP
#define _STDA_CMD_MEMORIAL_LEVEL_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/memorial_type.hpp"
#include <map>

namespace stdA {
	class CmdMemorialLevelInfo : public pangya_db {
		public:
			explicit CmdMemorialLevelInfo(bool _waiter = false);
			virtual ~CmdMemorialLevelInfo();

			std::map< uint32_t, ctx_memorial_level >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMemorialLevelInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdMemorialLevelInfo"; };

		private:
			std::map< uint32_t, ctx_memorial_level > m_level;

			const char* m_szConsulta = "SELECT level, gacha_end FROM pangya.pangya_new_memorial_level";
	};
}

#endif // !_STDA_CMD_MEMORIAL_LEVEL_INFO_HPP
