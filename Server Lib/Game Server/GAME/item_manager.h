// Arquivo item_manager.h
// Criado em 24/02/2018 as 19:05 por Acrisio
// Definição da classe item_manager

#pragma once
#ifndef _STDA_ITEM_MANAGER_H
#define _STDA_ITEM_MANAGER_H

#include "../TYPE/pangya_game_st.h"
#include <vector>

#include "../SESSION/player.hpp"

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

    class item_manager {
		public:
			struct RetAddItem {
				enum TYPE : int32_t {
					T_INIT_VALUE = -5,
					T_ERROR,
					TR_SUCCESS_WITH_ERROR,
					TR_SUCCES_PANG_AND_EXP_AND_CP_WITH_ERROR,
					TR_SUCCESS_PANG_AND_EXP_AND_CP_POUCH_WITH_ERROR,
					T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH,
					T_SUCCESS,
				};

			public:
				RetAddItem(uint32_t _ul = 0u) {
					clear();
				};
				~RetAddItem(){};
				void clear() {

					if (!fails.empty()) {
						fails.clear();
						fails.shrink_to_fit();
					}

					type = T_INIT_VALUE;
				};
				std::vector< stItem > fails;
				TYPE type;
			};

        public:
            item_manager();
            ~item_manager();

			// Gets
			static std::vector< stItem > getItemOfSetItem(player& _session, uint32_t _typeid, bool _shop, int _chk_level);

			// Init Item From Buy --Gift opt-- Item
			static void initItemFromBuyItem(PlayerInfo& _pi, stItem& _item, BuyItem& _bi, bool _shop, int _option, int _gift_opt = 0, int _chk_lvl = 0);
			static void initItemFromEmailItem(PlayerInfo& _pi, stItem& _item, EmailInfo::item& _ei_item);

			// Check is have setitem in email
			static void checkSetItemOnEmail(player& _session, EmailInfo& _ei);

			// Add Itens
			static RetAddItem::TYPE addItem(stItem& _item, uint32_t _uid, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);
			static RetAddItem addItem(std::vector< stItem >& _v_item, uint32_t _uid, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);
			static RetAddItem addItem(std::vector< stItemEx >& _v_item, uint32_t _uid, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);
			static RetAddItem::TYPE addItem(stItem& _item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false); /*_dub pode duplicar*/
			static RetAddItem addItem(std::vector< stItem >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);
			static RetAddItem addItem(std::vector< stItemEx >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);
			static RetAddItem addItem(std::map< uint32_t, stItem >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);
			static RetAddItem addItem(std::map< uint32_t, stItemEx >& _v_item, player& _session, unsigned char _gift_flag, unsigned char _purchase, bool _dup = false);

			// Give Itens
			static int32_t giveItem(stItem& _item, player& _session, unsigned char _gift_flag);
			static int32_t giveItem(std::vector< stItem >& _v_item, player& _session, unsigned char _gift_flag);
			static int32_t giveItem(std::vector< stItemEx >& _v_item, player& _session, unsigned char _gift_flag);

			// Remove Item
			static int32_t removeItem(stItem& _item, player& _session);
			static int32_t removeItem(std::vector< stItem >& _v_item, player& _session);
			static int32_t removeItem(std::vector< stItemEx >& _v_item, player& _session);

			// Transfer Item [Personal Shop]
			//static WarehouseItemEx* transferItem(player& _s_snd, player& _s_rcv, PersonalShopItem& _psi, PersonalShopItem& _psi_r);
			static void* transferItem(player& _s_snd, player& _s_rcv, PersonalShopItem& _psi, PersonalShopItem& _psi_r);

			// CadieMagicBox Exchange Check
			static int32_t exchangeCadieMagicBox(player& _session, uint32_t _typeid, int32_t _id, uint32_t _qntd);

			// Tiki Shop Excgange Item Check
			static std::vector< stItem > exchangeTikiShop(player& _session, uint32_t _typeid, int32_t _id, uint32_t _qntd);

			// Open Ticket Report Scroll
			static void openTicketReportScroll(player& _session, int32_t _ticket_scroll_item_id, int32_t _ticket_scroll_id, bool _upt_on_game = false);

			// Verifies
			static bool isSetItem(uint32_t _typeid);
			static bool isTimeItem(stItem::stDate& _date);
			static bool isTimeItem(stItem::stDate::stDateSys& _date);

			// Owner All Item(ns) "ownerItem"
			static bool ownerItem(uint32_t _uid, uint32_t _typeid);
			static bool ownerSetItem(uint32_t _uid, uint32_t _typeid);
			static bool ownerCaddieItem(uint32_t _uid, uint32_t _typeid);
			static bool ownerHairStyle(uint32_t _uid, uint32_t _typeid);
			static bool ownerMailBoxItem(uint32_t _uid, uint32_t _typeid);

			// Suporte Owner Find
			static CaddieInfoEx _ownerCaddieItem(uint32_t _uid, uint32_t _typeid);
			static CharacterInfo _ownerHairStyle(uint32_t _uid, uint32_t _typeid);
			static MascotInfoEx _ownerMascot(uint32_t _uid, uint32_t _typeid);
			static WarehouseItemEx _ownerBall(uint32_t _uid, uint32_t _typeid);
			static CardInfo _ownerCard(uint32_t _uid, uint32_t _typeid);
			static WarehouseItemEx _ownerAuxPart(uint32_t _uid, uint32_t _typeid);
			static WarehouseItemEx _ownerItem(uint32_t _uid, uint32_t _typeid);
			static TrofelEspecialInfo _ownerTrofelEspecial(uint32_t _uid, uint32_t _typeid);

			static bool betweenTimeSystem(stItem::stDate& _date);
			static bool betweenTimeSystem(IFF::DateDados& _date);
			static bool betweenTimeSystem(stItem::stDate::stDateSys& _date);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);
    };
}

#endif