// Arquivo cmd_find_character.hpp
// Criado em 20/05/2018 as 23:13 Acrisio
// Defini��o da classe CmdFindCharacter

#pragma once
#ifndef _STDA_CMD_FIND_CHARACTER_HPP
#define _STDA_CMD_FIND_CHARACTER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFindCharacter : public pangya_db {
		public:
			explicit CmdFindCharacter(bool _waiter = false);
			CmdFindCharacter(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
			virtual ~CmdFindCharacter();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			CharacterInfo& getInfo();

			bool hasFound();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdFindCharacter"; };
			virtual std::wstring _wgetName() override { return L"CmdFindCharacter"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			CharacterInfo m_ci;

			const char* m_szConsulta = "pangya.ProcFindCharacter";
	};
}

#endif // !_STDA_CMD_FIND_CHARACTER_HPP
