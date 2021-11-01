// Arquivo grand_zodiac_base.hpp
// Criado em 23/06/2020 as 15:28 por Acrisio
// Defini��o da classe GrandZodiacBase

#pragma once
#ifndef _STDA_GRAND_ZODIAC_BASE_HPP
#define _STDA_GRAND_ZODIAC_BASE_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#include "../../Projeto IOCP/UTIL/event.hpp"
#endif

#include "tourney_base.hpp"
#include "../TYPE/grand_zodiac_type.hpp"
#include "../../Projeto IOCP/THREAD POOL/thread.h"

namespace stdA {
	class GrandZodiacBase : public TourneyBase {
		public:
			enum eSTATE_GRAND_ZODIAC_SYNC : unsigned char {
				LOAD_HOLE,
				LOAD_CHAR_INTRO,
				FIRST_HOLE,
				START_GOLDEN_BEAM,
				END_GOLDEN_BEAM,
				END_SHOT,
				WAIT_END_GAME,
			};

			struct stStateGrandZodiacSync {
				stStateGrandZodiacSync() : m_state(eSTATE_GRAND_ZODIAC_SYNC::FIRST_HOLE) {

#if defined(_WIN32)
					InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
					INIT_PTHREAD_MUTEXATTR_RECURSIVE;
					INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
					DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
				};

				~stStateGrandZodiacSync() {

					m_state = eSTATE_GRAND_ZODIAC_SYNC::FIRST_HOLE;

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

				eSTATE_GRAND_ZODIAC_SYNC& getState() {
					return m_state;
				};

				void setState(eSTATE_GRAND_ZODIAC_SYNC _state) {

					m_state = _state;
				};

				void setStateWithLock(eSTATE_GRAND_ZODIAC_SYNC _state) {

					lock();

					m_state = _state;

					unlock();
				}

			protected:

				eSTATE_GRAND_ZODIAC_SYNC m_state;

#if defined(_WIN32)
				CRITICAL_SECTION m_cs;
#elif defined(__linux__)
				pthread_mutex_t m_cs;
#endif
			};

		public:
			GrandZodiacBase(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~GrandZodiacBase();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			virtual void sendInitialData(player& _session) override;

			// Met�dos do Game->Course->Hole
			virtual void requestInitHole(player& _session, packet *_packet) override;
			virtual bool requestFinishLoadHole(player& _session, packet *_packet) override;
			virtual void requestFinishCharIntro(player& _session, packet *_packet) override;

			virtual void changeHole(player& _session) = 0;
			virtual void finishHole(player& _session) = 0;

			virtual void requestInitShot(player& _session, packet *_packet) override;

			virtual void requestActiveBooster(player& _session, packet *_packet) override;
			virtual void requestActiveCutin(player& _session, packet *_packet) override;

			// Exclusivo do Grand Zodiac Modo
			virtual void requestStartFirstHoleGrandZodiac(player& _session, packet *_packet) override;
			virtual void requestReplyInitialValueGrandZodiac(player& _session, packet *_packet) override;

			// Game
			virtual bool requestFinishGame(player& _session, packet *_packet) override;

			// Tempo
			virtual void timeIsOver() = 0;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override = 0;

			virtual void requestFinishExpGame() = 0;

			virtual void sendRemainTime(player& _session) override;

			virtual void requestFinishHole(player& _session, int option);

			virtual void requestUpdateItemUsedGame(player& _session) override;

			virtual void requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) override;

			virtual void requestSaveInfo(player& _session, int option) override;

			virtual void requestCalculeRankPlace() override;

			virtual void requestReplySyncShotData(player& _session) override;

			// Envia o Placar do Jogo, Os Scores Finais, pang e experi�ncia e etc
			virtual void sendPlacar(player& _session) override;

			// Envia para o jogador, que o tempo do Tourney Acabou
			virtual void sendTimeIsOver(player& _session) override;

			// Verifica se � a ultima tacada do hole, para passar para o proximo hole
			virtual int checkEndShotOfHole(player& _session) override;

			// Sortea Item Drop do Hole, pode dropar ou n�o
			virtual void drawDropItem(player& _session) override = 0;

			virtual void requestFinishData(player& _session, int option) = 0;

			virtual void finish(int option) = 0;

			virtual void requestMakeTrofel() = 0;

			virtual void init_values_seed();

			virtual void nextHole(player& _session);

			// Set, Check and clear if true
			virtual void setInitFirstHole(PlayerGrandZodiacInfo* _pgi);

			virtual bool checkAllInitFirstHole();

			virtual void clearInitFirstHole();

			// Set, Check and clear if true
			virtual bool setInitFirstHoleAndCheckAllInitFirstHoleAndClear(PlayerGrandZodiacInfo* _pgi);

			// Set, Check and clear if true
			virtual void setEndGame(PlayerGrandZodiacInfo* _pgi);

			virtual bool checkAllEndGame();

			virtual void clearEndGame();

			// Set, Check and clear if true
			virtual bool setEndGameAndCheckAllEndGameAndClear(PlayerGrandZodiacInfo* _pgi);

			virtual void clear_all_init_first_hole();
			virtual void clear_all_end_game();

			virtual void sendReplyInitShotAndSyncShot(player& _session);

		protected:
			static bool sort_grand_zodiac_rank_place(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);

		public:
			virtual bool finish_game(player& _session, int option = 0) override = 0;

		protected:
			// Make Object Player Info Polimofirsmo
			virtual PlayerGameInfo* makePlayerInfoObject(player& _session) override;

			virtual void startGoldenBeam() = 0;
			virtual void endGoldenBeam() = 0;
			virtual void setPlayerGoldenBeam(player& _session);

		protected:
#if defined(_WIN32)
			static DWORD WINAPI _syncFirstHole(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _syncFirstHole(LPVOID lpParameter);
#endif

#if defined(_WIN32)
			DWORD syncFirstHole();
#elif defined(__linux__)
			void* syncFirstHole();
#endif
			
			// Finish Thread Sync first hole
			void finish_thread_sync_first_hole();

#if defined(_WIN32)
			HANDLE m_hEvent_sync_hole;
			HANDLE m_hEvent_sync_hole_pulse;
#elif defined(__linux__)
			Event *m_hEvent_sync_hole;
			Event *m_hEvent_sync_hole_pulse;
#endif

			thread *m_thread_sync_first_hole;

			stStateGrandZodiacSync m_state_gz;

		protected:
			uint32_t volatile m_golden_beam_state;					// Status do golden beam

			std::map< player*, bool > m_mp_golden_beam_player;			// Map de golden beam player, os player que fizeram hio no tempo do golden beam

			std::vector< double > m_initial_values_seed;				// Valores que passa com o pacote1EC

#if defined(_WIN32)
			CRITICAL_SECTION m_cs_sync_shot;
#elif defined(__linux__)
			pthread_mutex_t m_cs_sync_shot;
#endif
	};
}

#endif // !_STDA_GRAND_ZODIAC_BASE_HPP
