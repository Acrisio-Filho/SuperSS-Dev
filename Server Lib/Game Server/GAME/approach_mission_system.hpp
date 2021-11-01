// Arquivo approach_mission_system.hpp
// Criado em 14/06/2020 as 17:28 por Acrisio
// Defini��o da classe ApproachMissionSystem

#pragma once
#ifndef _STDA_APPROACH_MISSION_SYSTEM_HPP
#define _STDA_APPROACH_MISSION_SYSTEM_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/approach_type.hpp"
#include "../../Projeto IOCP/TYPE/singleton.h"
#include <vector>

namespace stdA {
	class ApproachMissionSystem {
		public:
			ApproachMissionSystem();
			virtual ~ApproachMissionSystem();

			void load();

			bool isLoad();

			mission_approach_ex drawMission(uint32_t _num_players);

		protected:
			void initialize();

			void clear();

			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		private:
			std::vector< mission_approach_dados> m_mad;	// Approach Mission dados

			bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< ApproachMissionSystem > sApproachMissionSystem;
}

#endif // !_STDA_APPROACH_MISSION_SYSTEM_HPP