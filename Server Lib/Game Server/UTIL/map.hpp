// Arquivo map.hpp
// Criado em 21/09/2018 as 20:47 por Acrisio
// Definição da classe Map

#pragma once
#ifndef _STDA_MAP_HPP
#define _STDA_MAP_HPP

#include "../../Projeto IOCP/TYPE/singleton.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <string>
#include <map>

#include <memory.h>
#include <cstdint>

namespace stdA {
	class Map {
		public:
			struct stCtx {
				stCtx(uint32_t _ul = 0u) {
					clear();
				};
				void clear() {

					name.clear();
					range_score.clear();

					clear_bonus = 0u;
					star = 0.f;
				};
				struct stParRangeScore {
					void clear() { memset(this, 0, sizeof(stParRangeScore)); };
					char par[18];
					char min[18];
					char max[18];
				};
				std::string name;
				uint32_t clear_bonus;
				float star;
				stParRangeScore range_score;
			};

		public:
			Map();
			~Map();

			/*static*/ bool isLoad();
			/*static*/ void load();

			/*static*/ stCtx* getMap(unsigned char _course);

			/*static*/ uint32_t calculeClearVS(stCtx& _ctx, uint32_t _num_player, uint32_t _qntd_hole);
			/*static*/ uint32_t calculeClearMatch(stCtx& _ctx, uint32_t _qntd_hole);
			/*static*/ uint32_t calculeClear30s(stCtx& _ctx, uint32_t _qntd_hole);
			/*static*/ uint32_t calculeClearSSC(stCtx& _ctx);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::map< unsigned char, stCtx > m_map;		// Mapas (Course)

			/*static*/ bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< Map > sMap;
}

#endif // !_STDA_MAP_HPP
