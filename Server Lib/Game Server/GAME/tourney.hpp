// Arquivo tourney.hpp
// Criado em 22/09/2018 as 12:06 por Acrisio
// Defini��o da classe Tourney

#pragma once
#ifndef _STDA_TOURNEY_HPP
#define _STDA_TOURNEY_HPP

#include "tourney_base.hpp"

namespace stdA {
	class Tourney : public TourneyBase {
		public:
			Tourney(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~Tourney();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			// Met�dos do Game->Course->Hole
			virtual bool requestFinishLoadHole(player& _session, packet *_packet) override;

			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			void finish_tourney(player& _session, int _option);

			virtual bool requestUseTicketReport(player& _session, packet *_packet) override;

			virtual void requestStartAfterEnter(job& _job) override;
			virtual void requestEndAfterEnter() override;

			// Tempo
			/*virtual void startTime() override;
			virtual void stopTime();
			virtual void pauseTime();
			virtual void resumeTime();*/
			virtual void timeIsOver() override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual void clear_time_after_enter();

			virtual void requestFinishExpGame();

			virtual void finish();

			virtual void requestMakeMedal();

			virtual void requestMakeTrofel();

			virtual void requestSaveTicketReport();

			virtual void requestSendTicketReport();

			virtual void requestGiveMedalAndItens();

			virtual void requestFinishData(player& _session);

			// request calcule shot spinning cube - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotSpinningCube(player& _session, ShotSyncData& _ssd) override;

			// request calcule shot coin - Ele � implementado nas classes que v�o usar ele
			virtual void requestCalculeShotCoin(player& _session, ShotSyncData& _ssd) override;

		protected:
			static bool speediest_sort(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);
			static bool best_drive_sort(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);
			static bool best_chipin_sort(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);
			static bool best_long_puttin_sort(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);
			static bool best_recovery(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			timer *m_pTimer_after_enter;		// Timer de entrar depois no Tourney

			bool m_tourney_state;
	};
}

#endif // !_STDA_TOURNEY_HPP
