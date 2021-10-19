// Arquivo cmd_drop_course_info.hpp
// Criado em 07/09/2018 as 14:16 por Acrisio
// Defini��o da classe CmdDropCourseInfo

#pragma once
#ifndef _STDA_CMD_DROP_COURSE_INFO_HPP
#define _STDA_CMD_DROP_COURSE_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/game_type.hpp"
#include "../GAME/drop_system.hpp"

#include <map>

namespace stdA {
	class CmdDropCourseInfo : public pangya_db {
		public:
			CmdDropCourseInfo(bool _waiter = false);
			virtual ~CmdDropCourseInfo();

			std::map< unsigned char, DropSystem::stDropCourse >& getInfo();

		protected:
			void lineResult(result_set::ctx_res *_result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// Gets Name
			std::string _getName() override { return "CmdDropCourseInfo"; };
			std::wstring _wgetName() override { return L"CmdDropCourseInfo"; };

		private:
			std::map< unsigned char, DropSystem::stDropCourse > m_course;

			const char* m_szConsulta = "SELECT course, tipo, typeid, quantidade, probabilidade_3H, probabilidade_6H, probabilidade_9H, probabilidade_18H, active FROM pangya.pangya_new_course_drop_item WHERE active = 1";
	};
}

#endif // !_STDA_CMD_DROP_COURSE_INFO_HPP
