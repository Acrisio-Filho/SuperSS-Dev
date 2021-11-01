// Arquivo cmd_update_dolfini_locker_mode.hpp
// Criado em 02/06/2018 as 18:03 por Acrisio
// Defini��o da classe CmdUpdateDolfiniLockerMode

#pragma once
#ifndef _STDA_CMD_UPDATE_DOLFINI_LOCKER_MODE_HPP
#define _STDA_CMD_UPDATE_DOLFINI_LOCKER_MODE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateDolfiniLockerMode : public pangya_db {
		public:
			explicit CmdUpdateDolfiniLockerMode(bool _waiter = false);
			CmdUpdateDolfiniLockerMode(uint32_t _uid, unsigned char _locker, bool _waiter = false);
			virtual ~CmdUpdateDolfiniLockerMode();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			unsigned char getLocker();
			void setLocker(unsigned char _locker);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateDolfiniLockerMode"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateDolfiniLockerMode"; };

		private:
			uint32_t m_uid;
			unsigned char m_locker;

			const char* m_szConsulta[2] = { "UPDATE pangya.pangya_dolfini_locker SET locker = ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_UPDATE_DOLFINI_LOCKER_MODE_HPP
