// Arquivo cmd_update_tutorial.hpp
// Criado em 28/06/2018 as 22:27 por Acrisio
// Defini��o da classe CmdUpdateTutorial

#pragma once
#ifndef _STDA_CMD_UPDATE_TUTORIAL_HPP
#define _STDA_CMD_UPDATE_TUTORIAL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateTutorial : public pangya_db {
		public:
			explicit CmdUpdateTutorial(bool _waiter = false);
			CmdUpdateTutorial(uint32_t _uid, TutorialInfo& _ti, bool _waiter = false);
			virtual ~CmdUpdateTutorial();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TutorialInfo& getInfo();
			void setInfo(TutorialInfo& _ti);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateTutorial"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateTutorial"; };

		private:
			uint32_t m_uid;
			TutorialInfo m_ti;

			const char* m_szConsulta[4] = { "UPDATE pangya.tutorial SET Rookie = ", ", Beginner = ", ", Advancer = ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_UPDATE_TUTORIAL_HPP
