// Arquivo cmd_update_dolfini_locker_pass.hpp
// Criado em 02/06/2018 as 15:17 por Acrisio
// Defini��o da classe CmdUpdateDolfiniLockerPass

#pragma once
#ifndef _STDA_CMD_UPDATE_DOLFINI_LOCKER_PASS_HPP
#define _STDA_CMD_UPDATE_DOLFINI_LOCKER_PASS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdUpdateDolfiniLockerPass : public pangya_db {
		public:
			explicit CmdUpdateDolfiniLockerPass(bool _waiter = false);
			CmdUpdateDolfiniLockerPass(uint32_t _uid, std::string& _pass, bool _waiter = false);
			virtual ~CmdUpdateDolfiniLockerPass();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::string& getPass();
			void setPass(std::string& _pass);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateDolfiniLockerPass"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateDolfiniLockerPass"; };

		private:
			uint32_t m_uid;
			std::string m_pass;

			const char* m_szConsulta = "pangya.ProcChangeDolfiniLockerPass";
	};
}

#endif // !_STDA_CMD_UPDATE_DOLFINI_LOCKER_PASS_HPP
