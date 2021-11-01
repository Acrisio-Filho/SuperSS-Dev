// Arquivo pratice.hpp
// Criado em 18/08/2018 as 15:23 por Acrisio
// Defini��o da classe Practice

#pragma once
#ifndef _STDA_PRACTICE_HPP
#define _STDA_PRACTICE_HPP

#include "tourney_base.hpp"

namespace stdA {
	class Practice : public TourneyBase {
		public:
			Practice(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~Practice();

			// Met�dos do Game->Course->Hole
			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			virtual void requestInitHole(player& _session, packet *_packet) override;

			virtual void requestCalculePang(player& _session) override;

			void finish_practice(player& _session, int _option);

			virtual void requestChangeWindNextHoleRepeat(player& _session, packet *_packet) override;

			// Tempo
			/*virtual void startTime();
			virtual void stopTime();
			virtual void pauseTime();
			virtual void resumeTime();*/
			virtual void timeIsOver() override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual void requestReplySyncShotData(player& _session) override;

			virtual void requestSavePang(player& _session);

			virtual void requestFinishExpGame();

			virtual void finish();

			virtual void requestFinishData(player& _session);

			// Verifica se � a ultima tacada do hole, para passar para o proximo hole
			virtual int checkEndShotOfHole(player& _session);

			// request calcule shot spinning cube - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotSpinningCube(player& _session, ShotSyncData& _ssd) override;

			// request calcule shot coin - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotCoin(player& _session, ShotSyncData& _ssd) override;

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			bool m_practice_state;
	};
}

#endif // !_STDA_PRACTICE_HPP