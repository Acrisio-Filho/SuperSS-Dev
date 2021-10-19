// Arquivo grand_zodiac.hpp
// Criado em 25/06/2020 as 18:27 por Acrisio
// Definição da classe GrandZodiac

#pragma once
#ifndef _STDA_GRAND_ZODIAC_HPP
#define _STDA_GRAND_ZODIAC_HPP

#include "grand_zodiac_base.hpp"

namespace stdA {
	class GrandZodiac : public GrandZodiacBase {
		public:
			GrandZodiac(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~GrandZodiac();

			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			void finish_grand_zodiac(player& _session, int _option);

			// Tempo
			virtual void timeIsOver() override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual void requestFinishExpGame() override;

			virtual void finish(int option) override;

			// Sortea Item Drop do Hole, pode dropar ou não
			virtual void drawDropItem(player& _session) override;

			virtual void requestFinishData(player& _session, int option) override;

			// Terminou Hole Update on Game
			// Opt 0 Não terminou o hole, 1 Terminou o Hole
			virtual void updateFinishHole(player& _session, int _option) override;

			virtual void requestMakeTrofel() override;

			virtual void startGoldenBeam() override;
			virtual void endGoldenBeam() override;

			virtual void requestCalculePontos();

			virtual void sendTrofel(player& _session);

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			bool m_grand_zodiac_state;
	};
}

#endif // !_STDA_GRAND_ZODIAC_HPP
