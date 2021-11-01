// Arquivo cmd_update_ball_equiped.hpp
// Criado em 25/03/2018 as 10:54 por Acrisio
// Defini��o da classe CmdUpdateBallEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_BALL_EQUIPED_HPP
#define _STDA_CMD_UPDATE_BALL_EQUIPED_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateBallEquiped : public pangya_db {
		public:
			explicit CmdUpdateBallEquiped(bool _waiter = false);
			CmdUpdateBallEquiped(uint32_t _uid, uint32_t _ball_typeid, bool _waiter = false);
			virtual ~CmdUpdateBallEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getBallTypeid();
			void setBallTypeid(uint32_t _ball_typeid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateBallEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateBallEquiped"; };

		private:
			uint32_t m_uid;
			uint32_t m_ball_typeid;

			const char* m_szConsulta = "pangya.USP_FLUSH_COMET";
	};
}

#endif // !_STDA_CMD_UPDATE_BALL_EQUIPED_HPP
