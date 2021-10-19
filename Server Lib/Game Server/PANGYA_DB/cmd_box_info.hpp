// Arquivo cmd_box_info.hpp
// Criado em 15/07/2018 as 21:57 por Acrisio
// Defini��o da classe CmdBoxInfo

#pragma once
#ifndef _STDA_BOX_INFO_HPP
#define _STDA_BOX_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/box_type.hpp"
#include <map>

namespace stdA {
	class CmdBoxInfo : public pangya_db {
		public:
			explicit CmdBoxInfo(bool _waiter = false);
			virtual ~CmdBoxInfo();

			std::map< uint32_t, ctx_box >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdBoxInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdBoxInfo"; };

		private:
			std::map< uint32_t, ctx_box > m_box;

			const char* m_szConsulta = "pangya.ProcGetBoxInfo";
	};
}

#endif // !_STDA_BOX_INFO_HPP
