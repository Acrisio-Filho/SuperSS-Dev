// Arquivo versus.hpp
// Criado em 20/10/2018 as 22:46 por Acrisio
// Defini��o da classe Versus

#pragma once
#ifndef _STDA_VERSUS_HPP
#define _STDA_VERSUS_HPP

#include "versus_base.hpp"

namespace stdA {
	class Versus : public VersusBase {
		public:
			Versus(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~Versus();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			// Met�dos do Game->Course->Hole
			//virtual bool requestFinishLoadHole(player& _session, packet *_packet) override;

			virtual void changeHole() override;
			virtual void finishHole() override;

			void finish_versus(int _option);

			// Tempo
			/*virtual void startTime(void* _quem) override;
			virtual void stopTime();
			virtual void pauseTime();
			virtual void resumeTime();*/
			virtual void timeIsOver(void* _quem) override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual void requestFinishExpGame();

			virtual void finish();

			virtual void requestFinishData(player& _session);

			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			bool m_versus_state;
	};
}

#endif // !_STDA_VERSUS_HPP
