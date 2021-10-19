// Arquivo pang_battle.hpp
// Criado em 09/01/2020 as 15:07 por Acrisio
// Definição da classe PangBattle

#pragma once
#ifndef _STDA_PANG_BATTLE_HPP
#define _STDA_PANG_BATTLE_HPP

#include "versus_base.hpp"
#include "../TYPE/pang_battle_type.hpp"

namespace stdA {
	class PangBattle : public VersusBase {
		public:
			PangBattle(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie);
			virtual ~PangBattle();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			// Metôdos do Game->Course->Hole
			virtual void requestInitHole(player& _session, packet *_packet) override;
			//virtual bool requestFinishLoadHole(player& _session, packet *_packet) override;

			virtual void changeHole() override;
			virtual void finishHole() override;

			virtual void requestInitShot(player& _session, packet *_packet) override;

			void finish_pang_battle(int _option);

			// Tempo
			/*virtual void startTime(void* _quem) override;
			virtual void stopTime();
			virtual void pauseTime();
			virtual void resumeTime();*/
			virtual void timeIsOver(void* _quem) override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			// Request Trata Finish Hole Data
			virtual void requestTranslateFinishHoleData(player& _session, UserInfoEx& _ui);

			virtual bool checkEndGame(player& _session) override;

			virtual void requestSaveInfo(player& _session, int option) override;

			// !@ Não tem experiência no Pang Battle
			//virtual void requestFinishExpGame();

			virtual void finish();

			virtual void requestFinishData(player& _session);

			virtual void requestFinishHole(player& _session, int option) override;

			virtual bool checkNextStepGame(player& _session) override;

			// Check and clear if true
			virtual bool checkAllClearHole() override;

			// Terminou Hole Update on Game
			virtual void updateFinishHole() override;

			// Send Player Turn
			virtual void sendPlayerTurn() override;

			virtual void changeTurn() override;

			virtual void sendPlacar(player& _session) override;

			virtual void requestCalculeRankPlace() override;

			// Calcula a mensagem que o player vai ser se ele fizer o hole, vai dropar muita moeda, pouca
			// Isso decide se o player ganhou, empatou ou perdeu
			// noexception
			virtual eMSG_MAKE_HOLE calcMsgToPlayerMakeHole(PlayerGameInfo* _pgi);

			virtual void init_pang_battle_data();

			virtual void calculePlayerWinPangBattle();

			virtual void savePangBattleDados(player& _session);

		protected:
			static bool sort_player_top_shot(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);
			static bool sort_player_top_shot_approach(PlayerOrderTurnCtx& _potc1, PlayerOrderTurnCtx& _potc2);
			static bool sort_player_rank_place(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2);

			inline void init_player_order_top_shot();
			inline std::vector< PlayerOrderTurnCtx > init_player_order_top_shot_approach();

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			bool m_pang_battle_state;

			PangBattleData m_pbd;

			std::vector< PlayerGameInfo* > m_player_order_pb;			// Lista de player do rank do jogo do Pang Battle
	};
}

#endif // !_STDA_PANG_BATTLE_HPP
