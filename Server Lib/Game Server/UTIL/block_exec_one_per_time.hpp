// Arquivo block_exec_one_per_time.hpp
// Criado em 03/04/2020 as 17:41 por Acrisio
// Defini��o da classe SyncBlockExecOnePerTime

#pragma once
#ifndef _STDA_BLOCK_EXEC_ONE_PER_TIME_HPP
#define _STDA_BLOCK_EXEC_ONE_PER_TIME_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <map>

#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/TYPE/singleton.h"

#define STDA_BLOCK_PACKET_ONE_TIME_DISABLE 0x1

#define STDA_BLOCK_PACKET_ONE_TIME_VER 0x2

namespace stdA {

	// Identify Packet Key
	class IdentifyPacketKey {
		public:
			union uKey {
				uKey(uint64_t _key = 0ull) {
					key = _key;
				};
				uint64_t key;
				struct {
					uint32_t uid;
					uint32_t packet_id;
				}_stkey;
			};

		public:
			IdentifyPacketKey(uint32_t _uid, unsigned short _packet_id) : /*m_threadId(0u),*/ m_key(0ull) {

				// Make Key
				//m_threadId = GetCurrentThreadId();

				m_key._stkey.uid = _uid;
				m_key._stkey.packet_id = _packet_id;
			};
			inline bool operator < (const IdentifyPacketKey& _ipk) const {
				return m_key.key < _ipk.m_key.key;
			};

		public:
			uKey m_key;
	};

	// declara aqui primeiro para usar como friend no ThreadSafeCS
	class EventCV;

	// Thread Safe Critical Section
	class ThreadSafeCS {
		public:
			ThreadSafeCS();
			virtual ~ThreadSafeCS();

			void lock();

			bool try_lock();

			void unlock();

		private:
			int32_t m_lock_spin;

			// Thread safe
#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif

			friend EventCV;
	};

	// Event Condition Variable
	constexpr unsigned short INVALID_PACKET_ID = (unsigned short)~0;

	class EventCV {
		public:
			EventCV();
			virtual ~EventCV();

			void waitEvent(ThreadSafeCS& _cs, unsigned short _id, DWORD _wait_time = INFINITE);

			void setEvent(unsigned short _id);

		private:
#if defined(_WIN32)
			CONDITION_VARIABLE m_cv;
#elif defined(__linux__)
			pthread_cond_t m_cv;
#endif

			unsigned short m_id;
	};

	// Sync Block Exec On Per Time
	class SyncBlockExecOnePerTime {
		public:
			struct stState {
				bool active;
				DWORD thread_id;
			};

		public:
#if STDA_BLOCK_PACKET_ONE_TIME_VER == 0x2
			SyncBlockExecOnePerTime(uint32_t _uid);
#else
			SyncBlockExecOnePerTime();
#endif
			virtual ~SyncBlockExecOnePerTime();

#if STDA_BLOCK_PACKET_ONE_TIME_VER == 0x2
			IdentifyPacketKey enter(unsigned short _packet_id);
#else
			IdentifyPacketKey enter(uint32_t _uid, unsigned short _packet_id);
#endif
			void leave(IdentifyPacketKey* _ipk);

#if STDA_BLOCK_PACKET_ONE_TIME_VER == 0x1
			EventCV* getPlayer(uint32_t _uid);
			void removePlayer(uint32_t _uid);
#endif

		private:
			// Types
			typedef std::map< IdentifyPacketKey, stState > MapIdentifyPacketKey;
#if STDA_BLOCK_PACKET_ONE_TIME_VER == 0x2
			typedef EventCV MapPlayerEvent;
#else
			typedef std::map< uint32_t/*uid*/, EventCV > MapPlayerEvent;
#endif

#if STDA_BLOCK_PACKET_ONE_TIME_VER == 0x2
			uint32_t m_uid;
#endif

			// Members
			MapIdentifyPacketKey m_mp_event_block;
			MapPlayerEvent m_mp_player;

			// Thread Safe CS
			ThreadSafeCS m_ts_cs;
	};

	// Singleton SyncBlockExecOnePerTime
	typedef Singleton< SyncBlockExecOnePerTime > sSyncBlockExecOnePerTime;

	// Block smart
#if STDA_BLOCK_PACKET_ONE_TIME_VER == 0x2
	class SmartBlock {
		public:
			SmartBlock(SyncBlockExecOnePerTime* _sbeopt, unsigned short _packet_id);
			virtual ~SmartBlock();

		private:
			SyncBlockExecOnePerTime* m_sbeopt;
			IdentifyPacketKey m_ipk;
	};
#else
	class SmartBlock {
		public:
			SmartBlock(uint32_t _uid, unsigned short _packet_id);
			virtual ~SmartBlock();

		private:
			IdentifyPacketKey m_ipk;
	};
#endif

// Macro
#if STDA_BLOCK_PACKET_ONE_TIME_DISABLE == 0x1
#define M_SMART_BLOCK_PACKET_ONE_TIME
#elif STDA_BLOCK_PACKET_ONE_TIME_VER == 0x2
#define M_SMART_BLOCK_PACKET_ONE_TIME SmartBlock sbSyncBlockExecOnePerTime(_session.m_sbeopt, _packet->getTipo());
#else
#define M_SMART_BLOCK_PACKET_ONE_TIME SmartBlock sbSyncBlockExecOnePerTime((_session.m_pi.uid == 0u) ? _session.m_oid + 1u : _session.m_pi.uid, _packet->getTipo());
#endif
}

#endif // !_STDA_BLOCK_EXEC_ONE_PER_TIME_HPP
