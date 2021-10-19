// Arquivo cmd_update_papel_shop_info.hpp
// Criado em 09/07/2018 as 21:44 por Acrisio
// Defini��o da classe CmdUpdatePapelShopInfo

#pragma once
#ifndef _STDA_CMD_UPDATE_PAPEL_SHOP_INFO_HPP
#define _STDA_CMD_UPDATE_PAPEL_SHOP_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

namespace stdA {
	class CmdUpdatePapelShopInfo : public pangya_db {
		public:
			explicit CmdUpdatePapelShopInfo(bool _waiter = false);
			CmdUpdatePapelShopInfo(uint32_t _uid, PlayerPapelShopInfo& _ppsi, SYSTEMTIME& _last_update, bool _waiter = false);
			virtual ~CmdUpdatePapelShopInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			SYSTEMTIME& getLastUpdate();
			void setLastUpdate(SYSTEMTIME& _last_update);

			PlayerPapelShopInfo& getInfo();
			void setInfo(PlayerPapelShopInfo& _ppsi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdatePapelShopInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdatePapelShopInfo"; };

		private:
			uint32_t m_uid;
			PlayerPapelShopInfo m_ppsi;
			SYSTEMTIME m_last_update;

			const char* m_szConsulta = "pangya.ProcUpdatePapelShopInfo";
	};
}

#endif // !_STDA_CMD_UPDATE_PAPEL_SHOP_INFO_HPP
