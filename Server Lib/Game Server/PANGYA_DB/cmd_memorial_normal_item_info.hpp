// Arquivo cmd_memorial_normal_item_info.hpp
// Criado em 22/07/2018 as 11:05 por Acrisio
// Defini��o da classe CmdMemorialNormalItemInfo

#pragma once
#ifndef _STDA_CMD_MEMORIAL_NORMAL_ITEM_INFO_HPP
#define _STDA_CMD_MEMORIAL_NORMAL_ITEM_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/memorial_type.hpp"
#include <map>

namespace stdA {
	class CmdMemorialNormalItemInfo : public pangya_db {
		public:
			CmdMemorialNormalItemInfo(bool _waiter = false);
			virtual ~CmdMemorialNormalItemInfo();

			std::map< uint32_t, ctx_coin_set_item >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMemorialNormalItemInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdMemorialNormalItemInfo"; };

		private:
			std::map< uint32_t, ctx_coin_set_item > m_item;

			const char* m_szConsulta = "pangya.ProcGetMemorialNormalItemInfo";
	};
}

#endif // !_STDA_CMD_MEMORIAL_NORMAL_ITEM_INFO_HPP
