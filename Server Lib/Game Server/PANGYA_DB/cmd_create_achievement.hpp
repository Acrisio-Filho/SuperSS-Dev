// Arquivo cmd_create_achievement.hpp
// Criado em 31/03/2018 as 21:50 por Acrisio
// Defini��o da classe CmdCreateAchievement

#pragma once
#ifndef _STDA_CMD_CREATE_ACHIEVEMENT_HPP
#define _STDA_CMD_CREATE_ACHIEVEMENT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdCreateAchievement : public pangya_db {
		public:
			explicit CmdCreateAchievement(bool _waiter = false);
			CmdCreateAchievement(uint32_t _uid, bool _waiter = false);
			CmdCreateAchievement(uint32_t _uid, uint32_t _typeid, std::string& _name, uint32_t _status, bool _waiter = false);
			virtual ~CmdCreateAchievement();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			std::string& getName();
			void setName(std::string& _name);

			uint32_t getStatus();
			void setStatus(uint32_t _status);

			void setAchievement(uint32_t _typeid, std::string& _name, uint32_t _status);

			int32_t getID();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCreateAchievement"; };
			virtual std::wstring _wgetName() override { return L"CmdCreateAchievement"; };

		private:
			uint32_t m_uid;
			int32_t m_id;
			uint32_t m_typeid;
			uint32_t m_status;
			std::string m_name;

			const char* m_szConsulta = "pangya.ProcInsertNewAchievement";
	};
}

#endif // !_STDA_CMD_CREATE_ACHIEVEMENT_HPP
