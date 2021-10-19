// Arquivo cmd_dolfini_locker_info.hpp
// Criado em 22/03/2018 as 21:14 por Acrisio
// Defini��o da classe CmdDolfiniLockerInfo

#pragma once
#ifndef _STDA_CMD_DOLFINI_LOCKER_INFO_HPP
#define _STDA_CMD_DOLFINI_LOCKER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdDolfiniLockerInfo : public pangya_db {
		public:
			explicit CmdDolfiniLockerInfo(bool _waiter = false);
			CmdDolfiniLockerInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdDolfiniLockerInfo();

			DolfiniLocker& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDolfiniLockerInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdDolfiniLockerInfo"; };

		private:
			uint32_t m_uid;
			DolfiniLocker m_df;

			const char* m_szConsulta[2] = { "pangya.ProcGetDolfiniLockerInfo", "pangya.ProcGetDolfiniLockerItem" };
	};
}

#endif // !_STDA_CMD_DOLFINI_LOCKER_INFO_HPP
