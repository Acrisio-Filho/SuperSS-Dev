// Arquivo login_reward_system.hpp
// Criado em 27/10/2020 as 12:21 por Acrisio
// Defini��o da classe LoginRewardSystem

#pragma once
#ifndef _STDA_LOGIN_REWARD_SYSTEM_HPP
#define _STDA_LOGIN_REWARD_SYSTEM_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../../Projeto IOCP/TYPE/singleton.h"

#include "../TYPE/login_reward_type.hpp"
#include "../SESSION/player.hpp"

namespace stdA {

	class LoginRewardSystem {

		public:
			LoginRewardSystem();
			virtual ~LoginRewardSystem();

			void load();

			bool isLoad();

			void checkRewardLoginAndSend(player& _session);

			void updateLoginReward();

		protected:
			void initialize();

			void clear();

			void sendGiftToPlayer(player& _session, stLoginReward& _lr);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		private:
			bool m_load;

			std::vector< stLoginReward > m_events;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< LoginRewardSystem > sLoginRewardSystem;
}

#endif // !_STDA_LOGIN_REWARD_SYSTEM_HPP
