// Arquivo cmd_find_mascot.hpp
// Criado em 22/05/2018 as 21:55 por Acrisio
// Defini��o da classe CmdFindMascot

#pragma once
#ifndef _STDA_CMD_FIND_MASCOT_HPP
#define _STDA_CMD_FIND_MASCOT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFindMascot : public pangya_db {
		public:
			explicit CmdFindMascot(bool _waiter = false);
			CmdFindMascot(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
			virtual ~CmdFindMascot();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			bool hasFound();

			MascotInfoEx& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdFindMascot"; };
			virtual std::wstring _wgetName() override { return L"CmdFindMascot"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			MascotInfoEx m_mi;

			const char* m_szConsulta = "pangya.ProcFindMascot";
	};
}

#endif // !_STDA_CMD_FIND_MASCOT_HPP
