// Arquivo cmd_notice_info.hpp
// Criado em 03/12/2018 as 00:48 por Acrisio
// Defini��o da classe CmdNoticeInfo

#pragma once
#ifndef _STDA_CMD_NOTICE_INFO_HPP
#define _STDA_CMD_NOTICE_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdNoticeInfo : public pangya_db {
		public:
			explicit CmdNoticeInfo(bool _waiter = false);
			CmdNoticeInfo(uint32_t _id, bool _waiter = false);
			virtual ~CmdNoticeInfo();

			uint32_t getId();
			void setId(uint32_t _id);

			std::string getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdNoticeInfo"; };
			std::wstring _wgetName() override { return L"CmdNoticeInfo"; };

		private:
			uint32_t m_id;
			std::string m_message;

			const char* m_szConsulta = "pangya.ProcGetNotice";
	};
}

#endif // !_STDA_CMD_NOTICE_INFO_HPP
