// Arquivo cmd_find_caddie.hpp
// Criado em 22/05/2018 as 21:01 por Acrisio
// Defini��o da classe CmdFindCaddie


#pragma once
#ifndef _STDA_CMD_FIND_CADDIE_HPP
#define _STDA_CMD_FIND_CADDIE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFindCaddie : public pangya_db {
		public:
			explicit CmdFindCaddie(bool _waiter = false);
			CmdFindCaddie(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
			virtual ~CmdFindCaddie();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			bool hasFound();

			CaddieInfoEx& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdFindCaddie"; };
			virtual std::wstring _wgetName() override { return L"CmdFindCaddie"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			CaddieInfoEx m_ci;

			const char* m_szConsulta = "pangya.ProcFindCaddie";
	};
}

#endif // !_STDA_CMD_FIND_CADDIE_HPP
