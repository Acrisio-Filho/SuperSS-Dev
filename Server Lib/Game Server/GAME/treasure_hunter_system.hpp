// Arquivo treasure_hunter_system.hpp
// Criado em 06/09/2018 as 18:17 por Acrisio
// Defini��o da classe TreasureHunterSystem

#pragma once
#ifndef _STDA_TREASURE_HUNTER_SYSTEM_HPP
#define _STDA_TREASURE_HUNTER_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/pangya_game_st.h"

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../../Projeto IOCP/TYPE/singleton.h"

#include <ctime>

namespace stdA {
	class TreasureHunterSystem {
		public:
			TreasureHunterSystem();
			virtual ~TreasureHunterSystem();

			/*static */void load();

			/*static */bool isLoad();

			/*static */TreasureHunterInfo* getAllCoursePoint();

			/*static */TreasureHunterInfo* findCourse(unsigned char _course);

			/*static */uint32_t calcPointNormal(uint32_t _tacada, unsigned char _par_hole);
			/*static */uint32_t calcPointSSC(uint32_t _tacada, unsigned char _par_hole);

			/*static */std::vector< TreasureHunterItem > drawItem(uint32_t _point, unsigned char _course);
			/*static */std::vector< TreasureHunterItem > drawApproachBox(uint32_t _num_box, unsigned char _course);

			// Check time update Point Course
			/*static */bool checkUpdateTimePointCourse();

			/*static */void updateCoursePoint(TreasureHunterInfo& _thi, int32_t _point);

		protected:
			/*static */void initialize();

			/*static */void clear();

			/*static */float getCourseRate(unsigned char _course);

			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		private:
			/*static */TreasureHunterInfo m_thi[MS_NUM_MAPS];
			/*static */std::vector< TreasureHunterItem > m_thItem;	// Treasure Hunter Item

			/*static */std::time_t m_time;

			/*static */bool m_load;
			
#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< TreasureHunterSystem > sTreasureHunterSystem;
}

#endif // !_STDA_TREASURE_HUNTER_SYSTEM_HPP
