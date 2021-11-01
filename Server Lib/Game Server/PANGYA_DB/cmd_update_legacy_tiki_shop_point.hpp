// Arquivo cmd_update_legacy_tiki_shop_point.hpp
// Criado em 26/10/2020 as 15:27 por Acrisio
// Defini��o da classe CmdUpdateLegacyTikiShopPoint

#pragma once
#ifndef _STDA_CMD_UPDATE_LEGACY_TIKI_SHOP_POINT_HPP
#define _STDA_CMD_UPDATE_LEGACY_TIKI_SHOP_POINT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

	class CmdUpdateLegacyTikiShopPoint : public pangya_db {

		public:
			CmdUpdateLegacyTikiShopPoint(uint32_t _uid, uint64_t _tiki_pts, bool _waiter = false);
			CmdUpdateLegacyTikiShopPoint(bool _waiter = false);
			virtual ~CmdUpdateLegacyTikiShopPoint();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getTikiShopPoint();
			void setTikiShopPoint(uint64_t _tiki_pts);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateLegacyTikiShopPoint"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateLegacyTikiShopPoint"; };

		private:
			uint32_t m_uid;
			uint64_t m_tiki_shop_point;

			const char* m_szConsulta[2] = {
				"UPDATE pangya.pangya_tiki_points SET Tiki_Points = ",
				", MOD_DATE = CURRENT_TIMESTAMP WHERE UID = "
			};
	};
}

#endif // !_STDA_CMD_UPDATE_LEGACY_TIKI_SHOP_POINT_HPP
