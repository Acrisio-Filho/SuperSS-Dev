// Arquivo golden_time_system.hpp
// Criado em 20/10/220 as 18:54 por Acrisio
// Defini��o da classe GoldenTimeSystem

#pragma once
#ifndef  _STDA_GOLDEN_TIME_SYSTEM_HPP
#define _STDA_GOLDEN_TIME_SYSTEM_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../../Projeto IOCP/TYPE/singleton.h"
#include "../TYPE/golden_time_type.hpp"
#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

	class GoldenTimeSystem {

		public:
			GoldenTimeSystem();
			virtual ~GoldenTimeSystem();

			bool isLoad();

			void load();

			bool checkRound();

			stGoldenTimeReward calculeRoundReward(std::vector< stPlayerReward > _player_reward);

			bool checkFirstMessage();

			stRound* checkNextRound();

			stRound* getCurrentRound();

			void sendRewardToMailOfPlayers(stGoldenTimeReward& _reward);

		protected:
			void initialize();

			void clear();

			void initCurrentGoldenTime();

			stGoldenTime* findNewGoldenTime();

			void updateGoldenTimeEnd();

			static bool findForever(stGoldenTime& _el);

			static bool findInterval(stGoldenTime& _el);

			static bool findOneDay(stGoldenTime& _el);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		private:
			bool m_load;

			std::vector< stGoldenTime > m_events;

			stGoldenTime* m_current_golden_time;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< GoldenTimeSystem > sGoldenTimeSystem;
}

#endif // ! _STDA_GOLDEN_TIME_SYSTEM_HPP
