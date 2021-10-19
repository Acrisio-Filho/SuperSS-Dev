// Arquivo approach.hpp
// Criado em 12/06/2020 as 11:53 por Acrisio
// Defini��o da classe Approach

#pragma once
#ifndef _STDA_APPROACH_HPP
#define _STDA_APPROACH_HPP

#include "tourney_base.hpp"
#include "../../Projeto IOCP/THREAD POOL/thread.h"

#include "../TYPE/approach_type.hpp"

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/event.hpp"
#endif

#include <map>

namespace stdA {
	class Approach : public TourneyBase {
		protected:
			enum eSTATE_APPROACH_SYNC : unsigned char {
				LOAD_HOLE,
				LOAD_CHAR_INTRO,
				END_SHOT,
				WAIT_END_GAME,
			};

			struct stStateApproachSync {
				stStateApproachSync() : m_state(eSTATE_APPROACH_SYNC::LOAD_HOLE) {

#if defined(_WIN32)
					InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
					INIT_PTHREAD_MUTEXATTR_RECURSIVE;
					INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
					DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
				};

				~stStateApproachSync() {

					m_state = eSTATE_APPROACH_SYNC::LOAD_HOLE;

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

				eSTATE_APPROACH_SYNC& getState() {
					return m_state;
				};

				void setState(eSTATE_APPROACH_SYNC _state) {

					m_state = _state;
				};

				void setStateWithLock(eSTATE_APPROACH_SYNC _state) {

					lock();

					m_state = _state;

					unlock();
				}

			protected:

				eSTATE_APPROACH_SYNC m_state;

#if defined(_WIN32)
				CRITICAL_SECTION m_cs;
#elif defined(__linux__)
				pthread_mutex_t m_cs;
#endif
			};

		public:
			Approach(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~Approach();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			// Met�dos do Game->Course->Hole
			virtual bool requestFinishLoadHole(player& _session, packet *_packet) override;
			virtual void requestFinishCharIntro(player& _session, packet *_packet) override;

			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			virtual void requestInitShot(player& _session, packet *_packet) override;

			void finish_approach(player& _session, int _option);

			// Game
			virtual bool requestFinishGame(player& _session, packet *_packet) override;

			// Tempo
			/*virtual void startTime() override;
			virtual void stopTime();
			virtual void pauseTime();
			virtual void resumeTime();*/
			virtual void timeIsOver() override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual uint32_t getCountPlayersGame() override;

			virtual void requestUpdateItemUsedGame(player& _session) override;

			virtual void finish();

			virtual void requestFinishData(player& _session);

			virtual void requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) override;

			virtual void requestReplySyncShotData(player& _session) override;

			virtual void sendRemainTime(player& _session) override {}; // no Approach n�o envia nada

			virtual int checkEndShotOfHole(player& _session) override;

			virtual void updateFinishHole(player& _session, int option) override;

			virtual void sendRatesOfApproach();

			virtual void finish_thread_sync_hole();

			virtual void requestFinishHole(player& _session, int option) override;

			virtual void requestSaveInfo(player& _session, int option) override;

			virtual void requestDrawTreasureHunterItem(player& _session) override;

			virtual void sendPlacar(player& _session) override;

			virtual void sendSyncShot(player& _session) override;

			// Make Object Player Info Polimofirsmo
			virtual PlayerGameInfo* makePlayerInfoObject(player& _session) override;

			// Set, Check and clear if true
			virtual void setFinishShot(PlayerGameInfo* _pgi);

			virtual bool checkAllFinishShot();

			virtual void clearFinishShot();

			// Set, Check and clear if true
			virtual bool setFinishShotAndCheckAllFinishShotAndClear(PlayerGameInfo* _pgi);

			// Set, Check and clear if true
			virtual void setLoadHole(PlayerGameInfo* _pgi);

			virtual bool checkAllLoadHole();

			virtual void clearLoadHole();

			// Set, Check and clear if true
			virtual bool setLoadHoleAndCheckAllLoadHoleAndClear(PlayerGameInfo* _pgi);

			// Set, Check and clear if true
			virtual void setFinishCharIntro(PlayerGameInfo* _pgi);

			virtual bool checkAllFinishCharIntro();

			virtual void clearFinishCharIntro();

			// Set, Check and clear if true
			virtual bool setFinishCharIntroAndCheckAllFinishCharIntroAndClear(PlayerGameInfo* _pgi);

			// Set, Check and clear if true
			virtual void setSyncShot(PlayerGameInfo* _pgi);

			virtual bool checkAllSyncShot();

			virtual void clearSyncShot();

			// Set, Check and clear if true
			virtual bool setSyncShotAndCheckAllSyncShotAndClear(PlayerGameInfo* _pgi);

			virtual void clear_all_load_hole();
			virtual void clear_all_finish_shot();
			virtual void clear_all_finish_char_intro();
			virtual void clear_all_sync_shot();

			virtual void requestCalculeRankPlace() override;
			virtual void requestCalculeRankPlaceHole();

			virtual void top_rank_win();
			virtual void finishAllDadosApproach();

			virtual void sendScoreBoard();

			virtual void init_mission();
			virtual void mission_win();

			// Set value to all player that quited from Approach
			virtual void delete_all_quiter();

		protected:
#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _syncHoleTime(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _syncHoleTime(LPVOID lpParameter);
#endif

			static bool sort_approach_rank_place(approach_dados_ex& _ad1, approach_dados_ex& _ad2);

#if defined(_WIN32)
			DWORD syncHoleTime();
#elif defined(__linux__)
			void* syncHoleTime();
#endif

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			thread *m_thread_sync_hole;			// Thread que sincroniza os player no hole

#if defined(_WIN32)
			HANDLE m_hEvent_sync_hole;			// Evento para terminar a thread sync hole
			HANDLE m_hEvent_sync_hole_pulse;	// Evento para pulsar a thread sync_hole para ir mais r�pido quando um player terminar a sua tacada
#elif defined(__linux__)
			Event *m_hEvent_sync_hole;			// Evento para terminar a thread sync hole
			Event *m_hEvent_sync_hole_pulse;	// Evento para pulsar a thread sync_hole para ir mais r�pido quando um player terminar a sua tacada
#endif

			// Approach State Sync
			stStateApproachSync m_state_app;

			// Current mission
			mission_approach_ex m_mission;

			bool m_approach_state;

			uint32_t volatile m_timeout;	// Tempo do hole acabou

#if defined(_WIN32)
			CRITICAL_SECTION m_cs_sync_shot;
#elif defined(__linux__)
			pthread_mutex_t m_cs_sync_shot;
#endif
	};
}

#endif // !_STDA_APPROACH_HPP
