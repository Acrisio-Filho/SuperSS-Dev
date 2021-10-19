// Arquivo cmd_tutorial_info.hpp
// Criado em 18/03/2018 as 22:18 por Acrisio
// Defini��o da classe CmdTutorialInfo

#pragma once
#ifndef _STDA_CMD_TUTORIAL_INFO_HPP
#define _STDA_CMD_TUTORIAL_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdTutorialInfo : public pangya_db {
		public:
			explicit CmdTutorialInfo(bool _waiter = false);
			CmdTutorialInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdTutorialInfo();

			TutorialInfo& getInfo();
			void setInfo(TutorialInfo& _ti);

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdTutorialInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdTutorialInfo"; };

		private:
			uint32_t m_uid;
			TutorialInfo m_ti;

			const char* m_szConsulta = "pangya.GetTutorial";
	};
}

#endif // !_STDA_CMD_TUTORIAL_INFO_HPP
