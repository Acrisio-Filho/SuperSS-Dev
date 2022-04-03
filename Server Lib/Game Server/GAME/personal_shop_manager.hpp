// Arquivo personal_shop_manager.hpp
// Criado em 01/04/2022 as 19:28 por Acrisio
// Definição da classe PersonalShopManager

#pragma once
#ifndef _STDA_PERSONAL_SHOP_MANAGER_HPP
#define _STDA_PERSONAL_SHOP_MANAGER_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "personal_shop.hpp"
#include "../SESSION/player.hpp"
#include "../TYPE/pangya_game_st.h"

#include <map>
#include <string>

namespace stdA {
    class PersonalShopManager {
		public:
			enum eTYPE_LOCK : uint8_t {
				TL_NONE,
				TL_SELECT,
				TL_DELETE,
			};

			struct Locker {
				public:
					Locker(PersonalShopManager& _manager, uint32_t _owner_uid, eTYPE_LOCK _type);
					~Locker();

				protected:
					PersonalShopManager& m_manager;
					uint32_t m_owner_uid;
					eTYPE_LOCK m_type;
			};

			struct PersonalShopCtx {
				public:
					PersonalShopCtx(PersonalShop* _shop, eTYPE_LOCK _type, int32_t _count)
						: m_shop(_shop), m_type(_type), m_count(_count) {};

				public:
					PersonalShop* m_shop;
					eTYPE_LOCK m_type;
					int32_t m_count;
			};

			typedef std::map< player*, PersonalShopCtx > mapShop;

        public:
            PersonalShopManager(RoomInfoEx& _ri);
            ~PersonalShopManager();

			void destroy();

			bool hasNameInSomeShop(std::string _name, uint32_t _owner_uid);

			PersonalShop* findShop(player* _session);
			PersonalShop* findShop(uint32_t _owner_uid);
			mapShop::iterator findShopIt(player* _session);
			mapShop::iterator findShopIt(uint32_t _owner_uid);

			void delete_shop(player* _session);
			void delete_shop(mapShop::iterator _it_shop);

			bool isItemForSale(player& _session, int32_t _item_id);

			PlayerRoomInfo::PersonShop getPersonShop(player& _session);

			void destroyShop(player& _session);

			bool openShopToEdit(player& _session, packet& _out_packet);
			bool cancelEditShop(player& _session, packet& _out_packet);
			bool closeShop(player& _session, packet& _out_packet);
			bool changeShopName(player& _session, std::string _name, packet& _out_packet);

			void openShop(player& _session, packet *_packet);
			void buyInShop(player& _session, packet *_packet);
			void visitCountShop(player& _session);
			void pangShop(player& _session);
			void viewShop(player& _session, uint32_t _owner_uid);
			void closeViewShop(player& _session, uint32_t _owner_uid);

		protected:
			void lock();
			void unlock();

			void clear_shops();

			// unsafe thread
			bool _hasNameInSomeShop(std::string _name, uint32_t _owner_uid);

			// unsafe thread
			PersonalShop* _findShop(player* _session);
			PersonalShop* _findShop(uint32_t _owner_uid);
			mapShop::iterator _findShopIt(player* _session);
			mapShop::iterator _findShopIt(uint32_t _owner_uid);

			// unsafe thread
			void _delete_shop(player* _session);
			void _delete_shop(mapShop::iterator _it_shop);

			bool waitSpinDown();
			void wakeAllLocked();

			void spinUp(uint32_t _owner_uid, eTYPE_LOCK _type);
			void spinDown(uint32_t _owner_uid, eTYPE_LOCK _type);

        protected:
			mapShop m_shops;

			// Owner room info
			RoomInfoEx& m_ri;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
			CONDITION_VARIABLE m_cv;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
			pthread_cond_t m_cv;
#endif
    };
}

#endif // !_STDA_PERSONAL_SHOP_MANAGER_HPP