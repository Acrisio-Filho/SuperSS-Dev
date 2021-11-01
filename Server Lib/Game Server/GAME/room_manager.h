// Arquivo room_manager.h
// Criado em 14/02/2018 as 21:48 por Acrisio
// Definição da classe RoomManager

#pragma once
#ifndef _STDA_ROOM_MANAGER_H
#define _STDA_ROOM_MANAGER_H

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <vector>
#include <climits>
#include "room.h"
#include "room_grand_prix.hpp"
#include "room_grand_zodiac_event.hpp"
#include "room_bot_gm_event.hpp"
#include "../TYPE/bot_gm_event_type.hpp"

namespace stdA {
    class RoomManager {
        public:
            RoomManager(unsigned char _channel_id);
            ~RoomManager();

			void destroy();

			room* makeRoom(unsigned char _channel_owner, RoomInfoEx _ri, player* _session, int _option = 0);
			void destroyRoom(room* _room);

			// Make room Grand Prix
			RoomGrandPrix* makeRoomGrandPrix(unsigned char _channel_owner, RoomInfoEx _ri, player* _session, IFF::GrandPrixData& _gp, int _option = 0);

			// Make room Grand Zodiac Event
			RoomGrandZodiacEvent*  makeRoomGrandZodiacEvent(unsigned char _channel_owner, RoomInfoEx _ri);

			// Make room Bot GM Event
			RoomBotGMEvent* makeRoomBotGMEvent(unsigned char _channel_owner, RoomInfoEx _ri, std::vector< stReward > _rewards);

			room* findRoom(short _numero);

			RoomGrandPrix* findRoomGrandPrix(uint32_t _typeid);

			// Opt sem sala practice, se não todas as salas
			std::vector< RoomInfo > getRoomsInfo(bool _without_practice_room = true);

			std::vector< RoomGrandZodiacEvent* > getAllRoomsGrandZodiacEvent();

			std::vector< RoomBotGMEvent* > getAllRoomsBotGMEvent();

			// Unlock Room
			void unlockRoom(room* _r);

		protected:
			size_t findIndexRoom(room* _room);

		private:
			unsigned short getNewIndex();
			void clearIndex(unsigned short _index);

			// Member
			unsigned char m_map_index[USHRT_MAX];

			// Dodo do Objeto RoomManager Channel ID
			unsigned char m_channel_id;
        
        protected:
            std::vector< room* > v_rooms;

		protected:
#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
			CONDITION_VARIABLE m_cv;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
			pthread_cond_t m_cv;
#endif
    };
}

#endif