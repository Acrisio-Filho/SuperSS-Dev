// Arquivo papel_shop_system.hpp
// Criado em 09/07/2018 as 18:07 por Acrisio
// Defini��o da classe PapelShopSystem

#pragma once
#ifndef _STDA_PAPEL_SHOP_SYSTEM_HPP
#define _STDA_PAPEL_SHOP_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/papel_shop_type.hpp"
#include <vector>
#include <map>

#include "../SESSION/player.hpp"

#include "../../Projeto IOCP/TYPE/singleton.h"

#define PAPEL_SHOP_MIN_BALL 1
#define PAPEL_SHOP_MAX_BALL 5
#define PAPEL_SHOP_BIG_BALL 10

#define PAPEL_SHOP_ITEM_MIN_QNTD 1
#define PAPEL_SHOP_ITEM_MAX_QNTD 3

namespace stdA {
	class PapelShopSystem {
		public:
			PapelShopSystem();
			virtual ~PapelShopSystem();

			/*static*/ void load();
			/*static*/ bool isLoad();
			
			/*static*/ bool isLimittedPerDay();

			// Initialize Papel Shop Count Info
			/*static*/ void init_player_papel_shop_info(player& _session);

			/*static*/ void updateDia();
			/*static*/ void updateDiaPlayer(player& _session);

			/*static*/ void updatePlayerCount(player& _session);

			/*static*/ void updateConfig(ctx_papel_shop& _ps);

			/*static*/ uint64_t getPriceNormal();
			/*static*/ uint64_t getPriceBig();

			// Check if he has coupon, return id or -1 if not
			/*static*/ WarehouseItemEx* hasCoupon(player& _session);

			/*static*/ std::vector< ctx_papel_shop_ball > dropBalls(player& _session);

			/*static*/ std::vector< ctx_papel_shop_ball > dropBigBall(player& _session);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

			/*static*/ bool checkUpdate();					// Check Time Papel Shop Sytem
			/*static*/ bool checkUpdate(SYSTEMTIME& _st);	// Check Time Parameter

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		private:
			/*static*/ std::vector< ctx_papel_shop_item > m_ctx_psi;
			/*static*/ std::map< uint32_t, ctx_papel_shop_coupon > m_ctx_psc;

			/*static*/ ctx_papel_shop m_ctx_ps;

			/*static*/ bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< PapelShopSystem > sPapelShopSystem;
}

#endif // !_STDA_PAPEL_SHOP_SYSTEM_HPP
