// Arquivo grand_prix_type.hpp
// Criado em 27/06/2019 as 14:54 por Acrisio
// Definição dos tipos usados na classe GrandPrix

#ifndef _STDA_GRAND_PRIX_TYPE_HPP
#define _STDA_GRAND_PRIX_TYPE_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <vector>
#include "../../Projeto IOCP/TIMER/timer.h"
#include "game_type.hpp"

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	enum eRULE : uint32_t {
		SPECIAL_SHOT = 0x1A000267u,
		TIME_10_SEC = 0x1A000268u,
		TIME_15_SEC = 0x1A00029Eu,
	};

	struct Bot {
		public:
			enum eTYPE_SCORE : uint8_t {
				MIN_SCORE,
				MED_SCORE,
				MAX_SCORE
			};

		public:
			struct Hole {
				Hole(uint32_t _ul = 0u) {
					clear();
				};
				Hole(uint32_t _course, uint32_t _hole, int32_t _score, uint64_t _pang, uint64_t _bonus_pang)
					: m_course(_course), m_hole(_hole), m_score(_score), m_pang(_pang), m_bonus_pang(_bonus_pang),
					m_ulUnknown(0u), m_ullUnknown(0ull) {
				}
				void clear() {
					memset(this, 0, sizeof(Hole));
				};
				uint32_t	m_course;
				uint32_t	m_hole;
				int32_t		m_score;
				uint32_t	m_ulUnknown;
				uint64_t	m_pang;
				uint64_t	m_bonus_pang;
				uint64_t	m_ullUnknown;
			};

		public:
			Bot(uint32_t _ul = 0u) {
				clear();
			};
			~Bot() {
				clear();
			};
			void clear() {

				id = 0u;
				qntd_hole = 0u;
				pang_total = 0u;
				bonus_pang_total = 0u;
				record = 0l;
				max_record = 0l;
				med_shot_per_hole = 0l;
				type_score = eTYPE_SCORE::MIN_SCORE;

				pi.clear();

				if (!hole.empty()) {
					hole.clear();
					hole.shrink_to_fit();
				}
			};

		public:
			uint32_t		id;
			unsigned char	qntd_hole;
			uint64_t		pang_total;
			uint64_t		bonus_pang_total;
			int32_t			record;

			int32_t			max_record;
			int32_t			med_shot_per_hole;
			eTYPE_SCORE		type_score;

			// Player Game Info do Bot para usar na hora de classificação do rank
			PlayerGameInfo pi;

			std::vector< Hole > hole;
	};

	// Rank Player Display Character
	struct RankPlayerDisplayChracter {
		RankPlayerDisplayChracter(uint32_t _ul = 0u) {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(RankPlayerDisplayChracter));
		};
		uint32_t uid;
		uint32_t rank;
		unsigned char default_hair;
		unsigned char default_shirts;
		uint32_t parts_typeid[24];
		uint32_t auxparts[5];
		uint32_t parts_id[24];
	};

#if defined(__linux__)
#pragma pack()
#endif

	// Estrutura que controle o tempo dos player no Grand Prix
	struct TimerManager {
		public:
			struct timer_ctx {
				timer_ctx(uint32_t _ul = 0u) : m_player(nullptr), m_timer(nullptr) {};
				timer_ctx(player* _player, timer* _timer) : m_player(_player), m_timer(_timer) {};
				void clear() {

					if (m_player != nullptr)
						m_player = nullptr;

					if (m_timer != nullptr)
						m_timer = nullptr;
				};
				player* m_player;
				timer *m_timer;
			};

		public:
			TimerManager() : m_timers(), m_lock(false) {
#if defined(_WIN32)
				InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
				INIT_PTHREAD_MUTEXATTR_RECURSIVE;
				INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
				DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
			};
			~TimerManager() {

				clear();

				// Verifica se está bloqueado e libera por que a classe vai ser destruída
				if (m_lock)
					unlock();

#if defined(_WIN32)
				DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_destroy(&m_cs);
#endif
			};

			void clear() {

				lock();

				if (!m_timers.empty()) {
					m_timers.clear();
					m_timers.shrink_to_fit();
				}

				unlock();
			};

			timer_ctx* insertTimer(player* _player, timer* _timer) {
				return insertTimer(timer_ctx(_player, _timer));
			};

			timer_ctx* insertTimer(timer_ctx _tc) {

				lock();

				auto it = m_timers.insert(m_timers.end(), _tc);

				unlock();

				return (it != m_timers.end() ? &(*it) : nullptr);
			};

			timer_ctx* findTimer(player* _player) {

				if (_player == nullptr)
					return nullptr;

				timer_ctx* tc = nullptr;

				lock();

				auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](auto& _el) {
					return _el.m_player == _player;
				});

				if (it != m_timers.end())
					tc = &(*it);

				unlock();

				return tc;
			};

			// lock
			void lock() {

#if defined(_WIN32)
				EnterCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_lock(&m_cs);
#endif

				m_lock = true;
			};

			// unlock
			void unlock() {

				// Não está bloqueado para poder desbloquear
				if (!m_lock)
					return;

				m_lock = false;

#if defined(_WIN32)
				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs);
#endif
			};

			std::vector< timer_ctx >& getTimers() {
				return m_timers;
			};

		protected:
			std::vector< timer_ctx > m_timers;

			bool m_lock;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	// Player Lock Manager
	struct LockManager {
		public:
			struct lock_ctx {
				public:
					lock_ctx() : m_player(nullptr), m_lock(false) {
#if defined(_WIN32)
						InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
						INIT_PTHREAD_MUTEXATTR_RECURSIVE;
						INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
						DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
					};
					lock_ctx(player* _player) : m_player(_player), m_lock(false) {
#if defined(_WIN32)
						InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
						INIT_PTHREAD_MUTEXATTR_RECURSIVE;
						INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
						DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
					};
					~lock_ctx() {

						// Verifica se está bloqueado e libera por que a classe vai ser destruída
						if (m_lock)
							unlock();

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

						m_lock = true;
					};

					void unlock() {

						// Não está bloqueado para desbloquear
						if (!m_lock)
							return;

						m_lock = false;

#if defined(_WIN32)
						LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
						pthread_mutex_unlock(&m_cs);
#endif
					};

				public:
					player* m_player;

				protected:
					bool m_lock;
#if defined(_WIN32)
					CRITICAL_SECTION m_cs;
#elif defined(__linux__)
					pthread_mutex_t m_cs;
#endif
			};

		public:
			LockManager() : m_lockers(), m_lock(false) {
#if defined(_WIN32)
				InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
				INIT_PTHREAD_MUTEXATTR_RECURSIVE;
				INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
				DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
			};
			~LockManager() {

				clear();

				// Verifica se está bloqueado e libera por que a classe vai ser destruída
				if (m_lock)
					unlock();

#if defined(_WIN32)
				DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_destroy(&m_cs);
#endif
			};

			void clear() {

				lock();

				if (!m_lockers.empty()) {
					m_lockers.clear();
					m_lockers.shrink_to_fit();
				}

				unlock();

			};

			void lock(player* _player) {

				lock();

				auto it = findLocker(_player);

				if (it != m_lockers.end())
					it->lock();
				else {

					// O player ainda não tem um locker, cria uma para ele e bloquea
					auto lc = insertLock(_player);

					if (lc != nullptr)
						lc->lock();
				}

				unlock();
			};

			void unlock(player* _player) {

				lock();

				auto it = findLocker(_player);

				if (it != m_lockers.end())
					it->unlock();
				// eslse não precisa criar um locker para desbloquear um locker que o player não tem

				unlock();
			};

		protected:
			std::vector< lock_ctx >::iterator findLocker(player* _player) {

				if (_player == nullptr)
					return m_lockers.end();

				return std::find_if(m_lockers.begin(), m_lockers.end(), [&](auto& _el) {
					return _el.m_player == _player;
				});
			};

			lock_ctx* insertLock(player* _player) {

				auto it = m_lockers.insert(m_lockers.end(), lock_ctx(_player));

				return (it != m_lockers.end() ? &(*it) : nullptr);
			};

			void lock() {

#if defined(_WIN32)
				EnterCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_lock(&m_cs);
#endif

				m_lock = true;
			};

			void unlock() {

				// Não está bloqueado para poder desbloquear
				if (!m_lock)
					return;

				m_lock = false;

#if defined(_WIN32)
				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs);
#endif
			};

		protected:
			std::vector< lock_ctx > m_lockers;

			bool m_lock;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	enum STATE_TURN : unsigned char {
		WAIT_HIT_SHOT,
		SHOTING,
		END_SHOT,
		LOAD_HOLE,
		WAIT_END_GAME,
	};

	struct stStateTurn {
		public:
			stStateTurn() : m_state(STATE_TURN::WAIT_HIT_SHOT), m_lock(false) {

#if defined(_WIN32)
				InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
				INIT_PTHREAD_MUTEXATTR_RECURSIVE;
				INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
				DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
			};

			~stStateTurn() {

				m_state = STATE_TURN::WAIT_HIT_SHOT;

				if (m_lock)
					unlock();

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

				m_lock = true;
			};

			void unlock() {

				// Não está bloqueado para poder liberar
				if (!m_lock)
					return;

				m_lock = false;

#if defined(_WIN32)
				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs);
#endif
			};

			STATE_TURN& getState() {
				return m_state;
			};

			void setState(STATE_TURN _state) {
				m_state = _state;
			};

			void setStateWithLock(STATE_TURN _state) {

				lock();

				m_state = _state;

				unlock();
			}

		protected:
			STATE_TURN m_state;

			bool m_lock;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};
}

#endif // !_STDA_GRAND_PRIX_TYPE_HPP
