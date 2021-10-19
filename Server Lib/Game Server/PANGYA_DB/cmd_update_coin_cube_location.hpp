// Arquivo cmd_update_coin_cube_location.hpp
// Criado em 17/10/2020 as 17:25 por Acrisio
// Defini��o da classe CmdUpdateCoinCubeLocation

#pragma once
#ifndef _STDA_CMD_UPDATE_COIN_CUBE_LOCATION_HPP
#define _STDA_CMD_UPDATE_COIN_CUBE_LOCATION_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/coin_cube_type.hpp"

namespace stdA {

	class CmdUpdateCoinCubeLocation : public pangya_db {

		public:
			explicit CmdUpdateCoinCubeLocation(bool _waiter = false);
			CmdUpdateCoinCubeLocation(CoinCubeUpdate& _ccu, bool _waiter = false);
			virtual ~CmdUpdateCoinCubeLocation();

			CoinCubeUpdate& getInfo();
			void setInfo(CoinCubeUpdate& _ccu);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateCoinCubeLocation"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCoinCubeLocation"; };

		private:
			CoinCubeUpdate m_ccu;

			const char* m_szConsulta[2] = {
				"pangya.ProcInsertCoinCubeLocation",
				"pangya.ProcUpdateCoinCubeLocation"
			};
	};
}

#endif // !_STDA_CMD_UPDATE_COIN_CUBE_LOCATION_HPP
