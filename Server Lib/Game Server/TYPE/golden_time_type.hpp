// Arquivo golden_time_type.hpp
// Criado em 20/10/2020 as 19:03 por Acrisio
// Defini��o dos tipos usados no Golden Time System

#pragma once
#ifndef _STDA_GOLDEN_TIME_TYPE_HPP
#define _STDA_GOLDEN_TIME_TYPE_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include <vector>
#include <algorithm>
#include "../../Projeto IOCP/UTIL/util_time.h"

#include <memory.h>

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	// A cada 100 players adiciona 1 ganhador para ser sorteado
	constexpr uint32_t NUMBER_OF_PLAYER_TO_WINNER = 100u;

	struct stItemReward {
		public:
			void clear() { memset(this, 0, sizeof(stItemReward)); };

		public:
			uint32_t _typeid;
			uint32_t qntd;
			uint32_t qntd_time;
			uint32_t rate;
	};

	struct stRound {
		public:
			stRound(uint32_t _ul = 0u) : executed(false), time{ 0u }, item{ 0u } {};
			stRound(SYSTEMTIME _time, bool _executed, stItemReward _item) 
				: time(_time), executed(_executed), item(_item) {};

			void clear() {

				executed = false;
				time = { 0u };
				item.clear();
			};

		public:
			SYSTEMTIME time; // hora que o round vai ser executado
			bool executed;
			
			stItemReward item;
	};

	struct stGoldenTime {
		public:
			enum eTYPE : unsigned char {
				ONE_DAY,	// Come�a em 1 dia e termina no mesmo dia
				INTERVAL,	// Intervalo de dias
				FOREVER,	// Nunca acaba
			};

		public:
			stGoldenTime(uint32_t _ul = 0u) : id(0u), type(eTYPE::ONE_DAY), date{ 0u }, 
					rate_of_players(1u), current_round(nullptr), rounds(), item_rewards(), is_end(false) {};
			stGoldenTime(uint32_t _id, eTYPE _type, SYSTEMTIME _start_date, SYSTEMTIME _end_date, uint32_t _rate_of_players = 1u)
				: id(_id), type(_type), date{ _start_date, _end_date }, rate_of_players(1u), current_round(nullptr), rounds(), item_rewards(), is_end(false) {};

			void clear() {

				id = 0u;
				
				type = eTYPE::ONE_DAY;

				date[0] = {0u};
				date[1] = {0u};

				is_end = false;

				rate_of_players = 1u;

				current_round = nullptr;
				
				if (!rounds.empty()) {
					rounds.clear();
					rounds.shrink_to_fit();
				}

				if (!item_rewards.empty()) {
					item_rewards.clear();
					item_rewards.shrink_to_fit();
				}

			};

			stRound* updateRound() {

				stRound* ret = nullptr;

				if (rounds.empty())
					return ret;

				auto it = std::find_if(rounds.begin(), rounds.end(), [](stRound& _el) {
					
					bool check = !_el.executed && !isEmpty(_el.time) && getLocalTimeDiffDESC(_el.time) >= 0ll; // aqui o certo � s� ">"

					// Passou o tempo j�, j� foi executado por que passou do tempo
					//_el.executed = !check;

					return check;
				});

				if (it != rounds.end())
					current_round = ret = &(*it);
				else
					current_round = nullptr;

				return ret;
			};

		public:
			uint32_t id;
			eTYPE type;
			SYSTEMTIME date[2]; // Come�o e Fim
			std::vector< stRound > rounds;

			std::vector< stItemReward > item_rewards;

			bool is_end;

			uint32_t rate_of_players; // Rate de quantos players a cada NUMBER_OF_PLAYER players, padr�o 1

			stRound* current_round;
	};

	struct stPlayerReward {
		public:
			uint32_t uid;
			bool is_premium;
			bool is_playing; // se n�o estiver jogando, ele est� na sala lounge
	};

	struct stGoldenTimeReward {
		public:
			void clear() {

				round.clear();

				if (!players.empty()) {
					players.clear();
					players.shrink_to_fit();
				}
			};

		public:
			stRound round;
			std::vector< stPlayerReward > players;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_GOLDEN_TIME_TYPE_HPP
