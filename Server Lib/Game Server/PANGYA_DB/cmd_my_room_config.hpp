// Arquivo cmd_my_room_config.hpp
// Criado em 22/03/2018 as 20:19 por Acrisio
// Defini��o da classe CmdMyRoomConfig

#pragma once
#ifndef _STDA_CMD_MY_ROOM_CONFIG_HPP
#define _STDA_CMD_MY_ROOM_CONFIG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdMyRoomConfig : public pangya_db {
		public:
			explicit CmdMyRoomConfig(bool _waiter = false);
			CmdMyRoomConfig(uint32_t _uid, bool _waiter = false);
			virtual ~CmdMyRoomConfig();

			MyRoomConfig& getMyRoomConfig();
			void setMyRoomConfig(MyRoomConfig& _mrc);

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMyRoomConfig"; };
			virtual std::wstring _wgetName() override { return L"CmdMyRoomConfig"; };

		private:
			uint32_t m_uid;
			MyRoomConfig m_mrc;

			const char* m_szConsulta = "SELECT senha, public_lock, state FROM pangya.pangya_myroom WHERE uid = ";
	};
}

#endif // !_STDA_CMD_MY_ROOM_CONFIG_HPP
