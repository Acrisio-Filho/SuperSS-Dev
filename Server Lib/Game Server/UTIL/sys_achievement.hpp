// Arquivo sys_achievement.hpp
// Criado em 15/04/2018 as 17:17 por Acrisio
// Definição da classe SysAchievement

#pragma once
#ifndef _STDA_SYS_ACHIEVEMENT_HPP
#define _STDA_SYS_ACHIEVEMENT_HPP

#include "../TYPE/pangya_game_st.h"
#include "../SESSION/player.hpp"
#include <vector>

namespace stdA {
	class SysAchievement {
		struct Counter {
			void clear() { memset(this, 0, sizeof(Counter)); };
			uint32_t _typeid;
			int32_t value;
		};

		struct CounterItemCtx : CounterItemInfo {
			CounterItemCtx(uint32_t _ul = 0u) : CounterItemInfo(0) {
				clear();
			};
			CounterItemCtx(CounterItemInfo& _cii, int32_t _last_value, int32_t _increase_value) 
				: CounterItemInfo(_cii), last_value(_last_value), increase_value(_increase_value) {
			};
			void clear() { memset(this, 0, sizeof(CounterItemCtx)); };
			int32_t last_value;
			int32_t increase_value;
		};

		// Guarda o typeid do achievement e o typeid da ques concluída do player
		struct QuestClear {
			void clear() { memset(this, 0, sizeof(QuestClear)); };
			uint32_t achievement_typeid;
			uint32_t quest_typeid;
		};
		
		struct Reward {
			void clear() { memset(this, 0, sizeof(Reward)); };
			uint32_t _typeid;
			int32_t qntd;
			int32_t time;
		};

		enum eSTATE : unsigned char {
			NOT_MATCH = 0u,
			INCREMENT_COUNTER = 1,
			CLEAR = 2,
		};

		public:
			SysAchievement();
			~SysAchievement();

			void clear();

			void incrementCounter(uint32_t _typeid);
			void incrementCounter(uint32_t _typeid, int32_t _value);
			void decrementCounter(uint32_t _typeid);

			void finish_and_update(player& _session);

			/**
			* O segundo argumento força a pegar todos countadores, mesmos das quests concluída
			*/
			inline static std::vector< CounterItemInfo > getCounterItemInfo(std::vector< AchievementInfoEx >& _v_ai, bool _all_force);

			// get Typeid do Counter Item do Character TypeId
			static uint32_t getCharacterCounterTypeId(uint32_t _character_typeid);

			// get Typeid do Counter Item do Caddie TypeId
			static uint32_t getCaddieCounterTypeId(uint32_t _caddie_typeid);

			// get Typeid do Counter Item do Mascot TypeId
			static uint32_t getMascotCounterTypeId(uint32_t _mascot_typeid);

			// get Typeid do Counter Item do Course TypeId
			static uint32_t getCourseCounterTypeId(uint32_t _course_typeid);

			// get Typeid do Counter Item do Número de Holes do Jogo
			static uint32_t getQntdHoleCounterTypeId(uint32_t _qntd_hole);

			// get Typeid do Counter Item do Score do Hole
			static uint32_t getScoreCounterTypeId(uint32_t _tacada_num, uint32_t _par_hole);

			// get Typeid do Counter Item do Active Item TypeId
			static uint32_t getActiveItemCounterTypeId(uint32_t _active_item_typeid);

			// get Typeid do Counter Item do Passive Item TypeId
			static uint32_t getPassiveItemCounterTypeId(uint32_t _passive_item_typeid);

			// get score num 0 HIO, 1 Alba, 2 Eagle, 3 Birdie, 4 Par, 5 Bogey, 6 Double Bogey
			static int32_t getScoreNum(uint32_t _tacada_num, uint32_t _par_hole);

		protected:
			std::vector< Counter > getCounterChanged();
			std::vector< AchievementInfoEx > getAchievementChanged(player& _session, std::vector< Counter >& _v_c);

			unsigned char checkQuestClear(player& _session, QuestStuffInfo& _qsi, CounterItemInfo& _cii, std::vector< Counter >& _v_c);

			bool checkAchievement(player& _session, AchievementInfoEx& _ai, Counter& _c, std::vector< Counter >& _v_c);

			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			std::vector< Counter > v_c;

			std::vector< QuestClear > v_quest_clear;
			std::map< int32_t, CounterItemCtx > map_cii_change;

			std::vector< Reward > v_reward;
	};
}

#endif // !_STDA_SYS_ACHIEVEMENT_HPP
