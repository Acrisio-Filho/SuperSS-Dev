// Arquivo room_grand_zodiac_event.hpp
// Criado em 26/06/2020 as 16:33 por Acrisio
// Defini��o da classe RoomGrandZodiacEvent

#pragma once
#ifndef _STDA_ROOM_GRAND_ZODIAC_EVENT_HPP
#define _STDA_ROOM_GRAND_ZODIAC_EVENT_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>

#include "../../Projeto IOCP/UTIL/event.hpp"
#endif

#include "room.h"

#include "../../Projeto IOCP/THREAD POOL/thread.h"
#include "../../Projeto IOCP/TIMER/timer.h"

namespace stdA {
	class RoomGrandZodiacEvent : public room {
		public:
			enum eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC : unsigned char {
				WAIT_TIME_START,
				WAIT_10_SECONDS_START,
				WAIT_END_GAME,
			};

			struct stStateRoomGrandZodiacEventSync {
				stStateRoomGrandZodiacEventSync() : m_state(eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC::WAIT_TIME_START) {

#if defined(_WIN32)
					InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
					INIT_PTHREAD_MUTEXATTR_RECURSIVE;
					INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
					DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
				};

				~stStateRoomGrandZodiacEventSync() {

					m_state = eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC::WAIT_TIME_START;

#if defined(_WIN32)
					DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_destroy(&m_cs);
#endif
				};

				void lock() {
#if defined(_WIN32)
					EnterCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_lock(&m_cs);
#endif
				};

				void unlock() {
#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif
				};

				eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC& getState() {
					return m_state;
				};

				void setState(eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC _state) {

					m_state = _state;
				};

				void setStateWithLock(eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC _state) {

					lock();

					m_state = _state;

					unlock();
				}

			protected:

				eSTATE_ROOM_GRAND_ZODIAC_EVENT_SYNC m_state;

#if defined(_WIN32)
				CRITICAL_SECTION m_cs;
#elif defined(__linux__)
				pthread_mutex_t m_cs;
#endif
			};

			// Static Instance vector strunct
			struct RoomGrandZodiacEventInstanciaCtx {
			public:
				enum eSTATE : unsigned char {
					GOOD,
					DESTROYING,
					DESTROYED,
				};

			public:
				RoomGrandZodiacEventInstanciaCtx(RoomGrandZodiacEvent* _rgze, eSTATE _state)
					: m_rgze(_rgze), m_state(_state) {
				};

			public:
				RoomGrandZodiacEvent* m_rgze;
				eSTATE m_state;
			};

			struct CriticalSectionInstancia {
				CriticalSectionInstancia() : m_state(false), m_lock(false) {

					init();

				};
				~CriticalSectionInstancia() {

					if (m_state)
#if defined(_WIN32)
						DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
						pthread_mutex_destroy(&m_cs);
#endif
				};
				void init() {

					if (!m_state) {
#if defined(_WIN32)
						InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
						INIT_PTHREAD_MUTEXATTR_RECURSIVE;
						INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
						DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
					}

					m_state = true;
				};
				void lock() {

					if (!m_state)
						init();

#if defined(_WIN32)
					EnterCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_lock(&m_cs);
#endif

					// Est� bloqueado
					m_lock = true;

				};
				void unlock() {

					if (!m_lock)
						return;	// N�o est� bloqueado

					// Desbloquea
					m_lock = false;

#if defined(_WIN32)
					LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs);
#endif
				};
#if defined(_WIN32)
				CRITICAL_SECTION m_cs;
#elif defined(__linux__)
				pthread_mutex_t m_cs;
#endif
				bool m_state;
				bool m_lock;
			};

		public:
			RoomGrandZodiacEvent(unsigned char _channel_owner, RoomInfoEx _ri);
			virtual ~RoomGrandZodiacEvent();

			virtual bool isAllReady() override;

			// O Grand Prix de tempo tem o seu pr�prio startGame j� que quem come�a � o server
			bool startGame();

			// Init Instance vector and lock, para n�o d� erro no destrutor por que vai destruir ele primeiro do que a instance da classe
			static void initFirstInstance();

		protected:
#if defined(_WIN32)
			static DWORD WINAPI _waitTimeStart(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _waitTimeStart(LPVOID lpParameter);
#endif
			static int _count_down_time(void* _arg1, void* _arg2);

#if defined(_WIN32)
			DWORD waitTimeStart();
#elif defined(__linux__)
			void* waitTimeStart();
#endif
			void count_down(int64_t _sec_to_start);

			// Finish Thread Sync wait time start
			void finish_thread_sync_wait_time_start();

			void clear_timer_count_down();

		protected:
			stStateRoomGrandZodiacEventSync m_state_rgze;

			SYSTEMTIME m_create_room;			// Data que a sala foi criada

			timer *m_timer_count_down;

			thread *m_wait_time_start;

#if defined(_WIN32)
			HANDLE m_hEvent_wait_start;			// Evento para terminar a thread sync wait time start
			HANDLE m_hEvent_wait_start_pulse;	// Evento para pulsar a thread sync wait time start para ir mais r�pido quando um player entra na sala
#elif defined(__linux__)
			Event *m_hEvent_wait_start;			// Evento para terminar a thread sync wait time start
			Event *m_hEvent_wait_start_pulse;	// Evento para pulsar a thread sync wait time start para ir mais r�pido quando um player entra na sala
#endif

		private:
			// Static fun��es e variaveis para garantir mexer com o ponteiro das sala Grand Zodiac Event que usar o time com um fun��o callback
			static void push_instancia(RoomGrandZodiacEvent* _rgze);
			static void pop_instancia(RoomGrandZodiacEvent* _rgze);

			static void set_instancia_state(RoomGrandZodiacEvent* _rgze, RoomGrandZodiacEventInstanciaCtx::eSTATE _state);

			static int get_instancia_index(RoomGrandZodiacEvent* _rgze);

			static bool instancia_valid(RoomGrandZodiacEvent* _rgze);

			typedef Singleton< CriticalSectionInstancia > m_cs_instancia;
			typedef Singleton< std::vector< RoomGrandZodiacEventInstanciaCtx > > m_instancias;
	};
}

#endif // !_STDA_ROOM_GRAND_ZODIAC_EVENT_HPP
