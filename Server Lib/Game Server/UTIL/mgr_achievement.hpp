// Arquivo mgr_achievement.hpp
// Criado em 08/04/2018 as 23:43 por Acrisio
// Definição da classe MgrAchievement

#pragma once
#ifndef _STDA_MGR_ACHIEVEMENT_HPP
#define _STDA_MGR_ACHIEVEMENT_HPP

#include "../../Projeto IOCP/TYPE/data_iff.h"
#include "../TYPE/pangya_game_st.h"
#include <map>
#include <vector>

#include "../../Projeto IOCP/SOCKET/session.h"

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
    class MgrAchievement {
        public:
			MgrAchievement();
            virtual ~MgrAchievement();

			void clear();

			void initAchievement(uint32_t _uid, bool _create = false);
			void initAchievement(uint32_t _uid, std::multimap< uint32_t, AchievementInfoEx >& _mp_achievement);

			// Gets
			std::vector< CounterItemInfo > getCounterItemInfo();
			std::multimap< uint32_t, AchievementInfoEx >& getAchievementInfo();

			uint32_t getPontos();

			// Sets

			// Reset Achievement para os valores iniciais
			virtual void resetAchievement(int32_t _id);
			virtual void resetAchievement(std::multimap< uint32_t, AchievementInfoEx >::iterator _it);

			// Remove Achievement
			virtual void removeAchievement(int32_t _id);
			virtual void removeAchievement(std::multimap< uint32_t, AchievementInfoEx >::iterator _it);

			// Add Achievement
			virtual std::multimap< uint32_t, AchievementInfoEx >::iterator addAchievement(AchievementInfoEx& _ai);

			// Sender
			virtual void sendAchievementGuiToPlayer(session& _session);
			virtual void sendAchievementToPlayer(session& _session);
			virtual void sendCounterItemToPlayer(session& _session);

			// Increasers
			void incrementPoint(uint32_t _increase = 1u);

			// Auxíliar
			CounterItemInfo* findCounterItemById(int32_t _id);
			CounterItemInfo* findCounterItemByTypeid(uint32_t _typeid);

			QuestStuffInfo* findQuestStuffById(int32_t _id);
			QuestStuffInfo* findQuestStuffByTypeId(uint32_t _typeid);

			std::multimap< uint32_t, AchievementInfoEx >::iterator findAchievementById(int32_t _id);

			// Statics methods
            static AchievementInfoEx createAchievement(uint32_t _uid, IFF::Achievement& _achievement, AchievementInfo::ACHIEVEMENT_STATUS _status);
            static AchievementInfoEx createAchievement(uint32_t _uid, IFF::QuestItem& _qi, AchievementInfo::ACHIEVEMENT_STATUS _status);

		protected:
			void initAchievement(bool _create);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			std::multimap< uint32_t, AchievementInfoEx > map_ai;
			
			uint32_t m_uid;				// Owner(dono)

			uint32_t m_pontos;				// Todos os pontos do achievement

			bool m_state;
    };
}

#endif // !_STDA_MGR_ACHIEVEMENT_HPP
