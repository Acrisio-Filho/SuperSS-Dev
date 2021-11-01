// Arquivo cmd_update_level_and_exp.hpp
// Criado em 06/07/2018 as 20:21 por Acrisio
// Defini��o da classe CmdUpdateLevelAndExp

#pragma once
#ifndef _STDA_CMD_UPDATE_LEVEL_AND_EXP_HPP
#define _STDA_CMD_UPDATE_LEVEL_AND_EXP_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateLevelAndExp : public pangya_db {
		public:
			explicit CmdUpdateLevelAndExp(bool _waiter = false);
			CmdUpdateLevelAndExp(uint32_t _uid, unsigned char _level, uint32_t _exp, bool _waiter = false);
			virtual ~CmdUpdateLevelAndExp();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			unsigned char getLevel();
			void setLevel(unsigned char _level);

			uint32_t getExp();
			void setExp(uint32_t _exp);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateLevelAndExp"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateLevelAndExp"; };

		private:
			uint32_t m_uid;
			unsigned char m_level;
			uint32_t m_exp;

			const char* m_szConsulta[3] = { "UPDATE pangya.user_info SET level = ", ", Xp = ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_UPDATE_LEVEL_AND_EXP_HPP
