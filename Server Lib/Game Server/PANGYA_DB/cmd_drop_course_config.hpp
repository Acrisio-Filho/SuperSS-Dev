// Arquivo cmd_drop_course_config.hpp
// Criado em 07/09/2018 as 15:04 por Acrisio
// Defini��o da classe CmdDropCourseConfig

#pragma once
#ifndef _STDA_CMD_DROP_COURSE_CONFIG_HPP
#define _STDA_CMD_DROP_COURSE_CONFIG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/game_type.hpp"
#include "../GAME/drop_system.hpp"

namespace stdA {
	class CmdDropCourseConfig : public pangya_db {
		public:
			CmdDropCourseConfig(bool _waiter = false);
			virtual ~CmdDropCourseConfig();

			DropSystem::stConfig& getConfig();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdDropCourseConfig"; };
			std::wstring _wgetName() override { return L"CmdDropCourseConfig"; };

		private:
			DropSystem::stConfig m_config;

			const char* m_szConsulta = "pangya.ProcGetDropCourseConfig";
	};
}

#endif // !_STDA_CMD_DROP_COURSE_CONFIG_HPP
