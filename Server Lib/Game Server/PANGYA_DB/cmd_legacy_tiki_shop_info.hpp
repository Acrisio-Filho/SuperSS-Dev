// Arquivo cmd_legacy_tiki_shop_info.hpp
// Criado em 26/10/2020 as 15:14 por Acrisio
// Defini��o da classe CmdLegacyTikiShopInfo

#pragma once
#ifndef _STDA_CMD_LEGACY_TIKI_SHOP_INFO_HPP
#define _STDA_CMD_LEGACY_TIKI_SHOP_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

	class CmdLegacyTikiShopInfo : public pangya_db {

		public:
			CmdLegacyTikiShopInfo(uint32_t _uid, bool _waiter = false);
			explicit CmdLegacyTikiShopInfo(bool _waiter = false);
			virtual ~CmdLegacyTikiShopInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdLegacyTikiShopInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdLegacyTikiShopInfo"; };

		private:
			uint32_t m_uid;
			uint64_t m_tiki_pts;

			const char* m_szConsulta = "pangya.ProcGetLegacyTikiShopInfo";
	};
}

#endif // !_STDA_CMD_LEGACY_TIKI_SHOP_INFO_HPP
