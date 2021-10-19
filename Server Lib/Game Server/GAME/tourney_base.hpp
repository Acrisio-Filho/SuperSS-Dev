// Arquivo tourney_base.hpp
// Criado em 18/08/2018 as 15:00 por Acrisio
// Defini��o da classe TourneyBase

#pragma once
#ifndef _STDA_TOURNEY_BASE_HPP
#define _STDA_TOURNEY_BASE_HPP

#include "game.hpp"
#include "../TYPE/tourney_base_type.hpp"

namespace stdA {
	class TourneyBase : public Game {
		public:
			TourneyBase(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~TourneyBase();

			virtual void sendInitialData(player& _session) override;
			virtual void sendInitialDataAfter(player& _session) override;

			// Met�dos do Game->Course->Hole
			virtual void requestInitHole(player& _session, packet *_packet) override;
			virtual bool requestFinishLoadHole(player& _session, packet *_packet) override;
			virtual void requestFinishCharIntro(player& _session, packet *_packet) override;
			virtual void requestFinishHoleData(player& _session, packet *_packet) override;

			virtual void changeHole(player& _session) = 0;
			virtual void finishHole(player& _session) = 0;

			virtual void requestInitShot(player& _session, packet *_packet) override;
			virtual void requestSyncShot(player& _session, packet *_packet) override;
			virtual void requestInitShotArrowSeq(player& _session, packet *_packet) override;
			virtual void requestShotEndData(player& _session, packet *_packet) override;
			virtual RetFinishShot requestFinishShot(player& _session, packet *_packet) override;

			virtual void requestChangeMira(player& _session, packet *_packet) override;
			virtual void requestChangeStateBarSpace(player& _session, packet *_packet) override;
			virtual void requestActivePowerShot(player& _session, packet *_packet) override;
			virtual void requestChangeClub(player& _session, packet  *_packet) override;
			virtual void requestUseActiveItem(player& _session, packet *_packet) override;
			virtual void requestChangeStateTypeing(player& _session, packet *_packet) override;	// Escrevendo
			virtual void requestMoveBall(player& _session, packet *_packet) override;
			virtual void requestChangeStateChatBlock(player& _session, packet *_packet) override;
			virtual void requestActiveBooster(player& _session, packet *_packet) override;
			virtual void requestActiveReplay(player& _session, packet *_packet) override;
			virtual void requestActiveCutin(player& _session, packet *_packet) override;

			// Hability Item
			virtual void requestActiveRing(player& _session, packet *_packet) override;
			virtual void requestActiveRingGround(player& _session, packet *_packet) override;
			virtual void requestActiveRingPawsRainbowJP(player& _session, packet *_packet) override;
			virtual void requestActiveRingPawsRingSetJP(player& _session, packet *_packet) override;
			virtual void requestActiveRingPowerGagueJP(player& _session, packet *_packet) override;
			virtual void requestActiveRingMiracleSignJP(player& _session, packet *_packet) override;
			virtual void requestActiveWing(player& _session, packet *_packet) override;
			virtual void requestActivePaws(player& _session, packet *_packet) override;
			virtual void requestActiveGlove(player& _session, packet *_packet) override;
			virtual void requestActiveEarcuff(player& _session, packet *_packet) override;

			virtual void requestUpdateTrofel() override;

			// Exclusivo do Modo Tourney
			virtual void requestSendTimeGame(player& _session) override;
			virtual void requestUpdateEnterAfterStartedInfo(player& _session, EnterAfterStartInfo& _easi) override;

			// Game
			virtual bool requestFinishGame(player& _session, packet *_packet) override;

			// Tempo
			virtual void startTime();
			/*virtual bool stopTime();
			virtual bool pauseTime();
			virtual bool resumeTime();*/
			virtual void timeIsOver() = 0;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override = 0;

			// Request Trate Shot Sync Data
			virtual void requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) override;
			virtual void requestReplySyncShotData(player& _session) override;

			virtual void sendRemainTime(player& _session);

			// Terminou Hole Update on Game
			// Opt 0 N�o terminou o hole, 1 Terminou o Hole
			virtual void updateFinishHole(player& _session, int _option);

			// Atuliza o Treasure Hunter Point no Jogo
			virtual void updateTreasureHunterPoint(player& _session);

			// Sorteia o Treasure Hunter Item do Player
			virtual void requestDrawTreasureHunterItem(player& _session);

			// Sync Shot Player
			virtual void sendSyncShot(player& _session);

			// Envia o pacote que finaliza a tacada,
			// e se tiver item capturado como coin e spinning cube, manda nesse pacote
			virtual void sendEndShot(player& _session, DropItemRet& _cube);

			// Envia resposta que o player terminou o jogou o saiu
			// Atualizar o Score Board do player
			// Opt 2 Terminou, 3 Saiu
			virtual void sendUpdateState(player& _session, int _option);

			// Envia Drop Item que o player ganhou no Tourney
			virtual void sendDropItem(player& _session);

			// Envia o Placar do Jogo, Os Scores Finais, pang e experi�ncia e etc
			virtual void sendPlacar(player& _session);

			// Envia o Treasure Hunter Item sorteado s� a interface(Visual)
			virtual void sendTreasureHunterItemDrawGUI(player& _session);

			// Envia para o jogador, que o tempo do Tourney Acabou
			virtual void sendTimeIsOver(player& _session);

			// Verifica se � a ultima tacada do hole, para passar para o proximo hole
			virtual int checkEndShotOfHole(player& _session);

			// Sortea Item Drop do Hole, pode dropar ou n�o
			virtual void drawDropItem(player& _session);

			// Achievement Top 3 player achievement
			virtual void achievement_top_3_1st(player& _session);

			// Calcula Shot to Spinning Cube
			virtual void calcule_shot_to_spinning_cube(player& _session, ShotSyncData& _ssd);

			// Calcula Shot to Coin
			virtual void calcule_shot_to_coin(player& _session, ShotSyncData& _ssd);

			// request calcule shot spinning cube - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotSpinningCube(player& _session, ShotSyncData& _ssd);

			// request calcule shot coin - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotCoin(player& _session, ShotSyncData& _ssd);

		public:
			virtual bool finish_game(player& _session, int option = 0) override = 0;

		protected:
			static int end_time(void* _arg1, void* _arg2);

		protected:
			uint32_t m_max_player;
			int32_t m_entra_depois_flag;

			TicketReportInfo m_tri;

			Medal m_medal[12];
	};
}

#endif // !_STDA_TOURNEY_BASE_HPP
