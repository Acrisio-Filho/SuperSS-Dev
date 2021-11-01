// Arquivo guild_battle.hpp
// Criado em 28/12/2019 as 17:30 por Acrisio
// Defini��o da classe GuildBattle

#pragma once
#ifndef _STDA_GUILD_BATTLE_HPP
#define _STDA_GUILD_BATTLE_HPP

#include "tourney_base.hpp"
#include "guild_room_manager.hpp"

namespace stdA {
	class GuildBattle : public TourneyBase {
		public:
			GuildBattle(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie, GuildRoomManager& _guild_manager);
			virtual ~GuildBattle();

			virtual bool deletePlayer(player* _session, int _option) override;

			virtual void deleteAllPlayer();

			virtual void sendInitialData(player& _session) override;

			virtual void requestInitHole(player& _session, packet *_packet) override;

			virtual void changeHole(player& _session) override;
			virtual void finishHole(player& _session) override;

			void finish_guild_battle(player& _session, int _option);

			// Tempo
			virtual void timeIsOver() override;

		protected:
			// Inicializa Jogo e Finaliza Jogo
			virtual bool init_game() override;

			virtual void requestFinishExpGame();

			virtual void finish();

			virtual void init_duplas();

			virtual bool AllTeamQuit();

			virtual void requestFinishData(player& _session);

			virtual void requestSaveGuildData();

			virtual void sendFinishHoleDupla(player& _session);

			virtual void sendPlacar(player& _session) override;

		public:
			virtual bool finish_game(player& _session, int option = 0) override;

		protected:
			bool m_guild_battle_state;

			GuildRoomManager& m_guild_manager;
	};
}

#endif // !_STDA_GUILD_BATTLE_HPP
