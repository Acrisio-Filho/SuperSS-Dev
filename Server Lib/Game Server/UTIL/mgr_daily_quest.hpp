// Arquivo mgr_daily_quest.hpp
// Criado em 13/04/2018 as 15:09 por Acrisio
// Defini��o da classe MgrDailyQuest

#pragma once
#ifndef _STDA_MGR_DAILY_QUEST_HPP
#define _STDA_MGR_DAILY_QUEST_HPP

#include "mgr_daily_quest.hpp"
#include "../SESSION/player.hpp"
#include "../../Projeto IOCP/TYPE/pangya_st.h"
#include <vector>

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

#include <cstdint>

namespace stdA {
	class MgrDailyQuest {
		public :
			static void requestCheckAndSendDailyQuest(player& _session, packet *_packet);
			static void requestLeaveQuest(player& _session, packet *_packet);
			static void requestAcceptQuest(player& _session, packet *_packet);
			static void requestTakeRewardQuest(player& _session, packet *_packet);

			// Auxiliares
			static bool checkCurrentQuestUser(DailyQuestInfo& _dqi, player& _session);
			static bool checkCurrentQuest(DailyQuestInfo& _dqi);

			static std::vector< RemoveDailyQuestUser > getOldQuestUser(player& _session);
			static std::vector< AchievementInfoEx > addQuestUser(DailyQuestInfo& _dqi, player& _session);

			static void removeQuestUser(player& _session, std::vector< RemoveDailyQuestUser >& _v_el);
			
			static std::vector< AchievementInfoEx > leaveQuestUser(player& _session, int32_t* _quest_id, uint32_t _count);
			static std::vector< AchievementInfoEx > acceptQuestUser(player& _session, int32_t* _quest_id, uint32_t _count);

			static int32_t addCounterItemUser(player& _session, CounterItemInfo& _cii);

			static void updateDailyQuest(DailyQuestInfo& _dqi);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);
	};
}

#endif // !_STDA_MGR_DAILY_QUEST_HPP
