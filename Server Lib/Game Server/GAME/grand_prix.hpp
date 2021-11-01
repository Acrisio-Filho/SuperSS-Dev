// Arquivo grand_prix.hpp
// Criado em 18/06/2019 as 08:22 por Acrisio
// Defini��o da classe GrandPrix

#pragma once
#ifndef _STDA_GRAND_PRIX_HPP
#define _STDA_GRAND_PRIX_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "tourney_base.hpp"
#include "../../Projeto IOCP/THREAD POOL/thread.h"
#include "../TYPE/grand_prix_type.hpp"

namespace stdA {
	class GrandPrix : public TourneyBase {
		public:
			GrandPrix(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie, IFF::GrandPrixData& _gp);
			virtual ~GrandPrix();

			virtual void sendInitialData(player& _session) override;

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			// Met�dos do Game->Course->Hole
			virtual void requestFinishCharIntro(player& _session, packet *_packet) override;
			virtual void requestActiveBooster(player& _session, packet *_packet) override;

			virtual void requestStartTurnTime(player& _session, packet *_packet) override;

			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			virtual void requestInitShot(player& _session, packet *_packet) override;
			//virtual void requestSyncShot(player& _session, packet *_packet) override;
			//virtual RetFinishShot requestFinishShot(player& _session, packet *_packet) override;

			void finish_grand_prix(player& _session, int _option);

			// Tempo
			virtual void startTime(void* _quem);
			virtual bool stopTime(void* _quem);
			virtual void timeIsOver(void* _quem);

			// timeIsOver abstract da classe TourneyBase
			virtual void timeIsOver() override { return; };

		protected:

			// Calcula Rank Place Grand Prix
			virtual void requestCalculeRankPlace();

			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual int checkEndShotOfHole(player& _session) override;

			virtual void requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) override;

			// Inicializa os Bots do Grand Prix se tiver espa�o para eles
			virtual void init_bots();

			// Consome o ticket dos player que v�o jogar o Grand Prix
			virtual void consomeTicket();

			virtual void requestFinishExpGame();

			virtual void requestMakeRankPlayerDisplayCharacter();

			virtual void finish();

			virtual void requestFinishData(player& _session);

			virtual void requestSaveGrandPrixClear(player& _session);

			// Sends
			virtual void sendTrofel(player& _session);

			virtual void sendRankPlayerDisplayCharacter(player& _session);

			virtual void sendRewardRankAndGrandPrix(player& _session);

			virtual void sendAllToNextHole();

			virtual int changeTurn(player& _session);

			// Sync
			virtual bool checkAllShotPacket(player& _session);
			virtual void clearAllShotPacket(player& _session);

			// Check and clear if true
			virtual bool checkAllClearHole();

			virtual void setClearHole(PlayerGameInfo* _pgi);

			virtual void clearAllClearHole();

			// Check and clear if true
			virtual bool checkAllClearHoleAndClear();

			virtual void clear_all_clear_hole();

			// Limpa os tempos criado no Grand Prix
			virtual void clear_timers();

			// request calcule shot spinning cube - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotSpinningCube(player& _session, ShotSyncData& _ssd) override;

			// request calcule shot coin - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotCoin(player& _session, ShotSyncData& _ssd) override;

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			static int end_time(void* _arg1, void* _arg2);

		protected:
			static int end_time_rule(void* _arg1, void* _arg2);

			// Tempo Rule
			virtual void startTimeRule(void* _quem);
			virtual bool stopTimeRule(void* _quem);
			virtual void timeRuleIsOver(void* _quem);

		protected:
			IFF::GrandPrixData m_gp;
			std::vector< IFF::GrandPrixRankReward > m_gp_reward;
			std::vector< Bot > m_bot;

			std::vector< RankPlayerDisplayChracter > m_rank_player_display_char;

			TimerManager m_timer_manager;
			TimerManager m_timer_manager_rule;

			LockManager m_lock_manager;

		protected:
			bool m_grand_prix_state;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs_sync_shot;
#elif defined(__linux__)
			pthread_mutex_t m_cs_sync_shot;
#endif
	};
}

#endif // !_STDA_GRAND_PRIX_HPP
