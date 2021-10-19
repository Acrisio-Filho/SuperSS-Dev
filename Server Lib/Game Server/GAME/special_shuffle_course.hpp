// Arquivo special_shuffle_course.hpp
// Criado em 16/10/2018 as 22:03 por Acrisio
// Defini��o da classe SpecialShuffleCourse

#pragma once
#ifndef _STDA_SPECIAL_SHUFFLE_COURSE_HPP
#define _STDA_SPECIAL_SHUFFLE_COURSE_HPP

#include "tourney_base.hpp"

namespace stdA {
	class SpecialShuffleCourse : public TourneyBase {
		public:
			SpecialShuffleCourse(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~SpecialShuffleCourse();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			// Met�dos do Game->Course->Hole
			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			void finish_SSC(player& _session, int _option);

			// Tempo
			/*virtual void startTime() override;
			virtual void stopTime();
			virtual void pauseTime();
			virtual void resumeTime();*/
			virtual void timeIsOver() override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual void finish();

			virtual DropItemRet requestInitDrop(player& _session) override;

			virtual void requestUpdateItemUsedGame(player& _session) override;

			virtual void requestFinishData(player& _session);

			virtual void requestDrawTreasureHunterItem(player& _session);

			virtual void requestMakeMasterCoin();

			virtual void requestSendMasterCoin(player& _session);

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			bool m_SSC_state;

			uint32_t m_coin_SSC;		// Que o master da sala ganha se ficar at� o final

	};
}

#endif // !_STDA_SPECIAL_SHUFFLE_COURSE_HPP
