// Arquivo drop_system.hpp
// Criado em 07/09/2018 as 13:37 por Acrisio
// Definição da classe DropSystem

#pragma once
#ifndef _STDA_DROP_SYSTEM_HPP
#define _STDA_DROP_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <map>
#include <vector>
#include "../TYPE/game_type.hpp"
#include "../SESSION/player.hpp"
#include "../../Projeto IOCP/TYPE/singleton.h"

namespace stdA {
	class DropSystem {
		public:
			struct stDropCourse {
				stDropCourse(uint32_t _ul = 0u) {
					clear();
				};
				~stDropCourse() {};
				struct stDropItem {
					enum eTIPO : unsigned char {
						ALL_PROBABILITY,			// Todos holes pode dropar, tem chance
						SEQUENCE_DROP,				// Dropa em uma quantidade fixe de 2 em 2 holes 1 em 1 hole, pode ser de 1 a 18
						LAST_HOLE_PROBABILITY,		// Ultimo hole tem a chance de dropar
					};
					enum ePROB_TIPO : unsigned char {
						_3HOLES_ALL,				// 3 Holes ou todos os holes, para outros tipos de drop
						_6HOLES_SEQUENCE,			// 6 Holes ou a sequência que o item pode dropar
						_9HOLES,					// 9 Holes
						_18HOLES,					// 18 Holes
					};
					void clear() { memset(this, 0, sizeof(stDropItem)); };
					uint32_t _typeid;
					unsigned char tipo;
					uint32_t qntd;
					uint32_t probabilidade[4];	// 3H_ALL, 6H, 9H, 18H, probabilidade
					unsigned char active : 1;
				};
				void clear() {
					
					course = 0u;

					if (!v_item.empty()) {
						v_item.clear();
						v_item.shrink_to_fit();
					}
				};
				unsigned char course;
				std::vector< stDropItem > v_item;
			};

			struct stCourseInfo {
				void clear() { memset(this, 0, sizeof(stCourseInfo)); };
				unsigned char course;
				unsigned char hole;				// Número do hole em relação do course
				unsigned char seq_hole;			// Sequência do hole de 1 a 18
				unsigned char qntd_hole;		// Quantidade de holes do jogo
				uint32_t artefact;
				unsigned char char_motion : 1;
				unsigned char angel_wings : 2;	// 1 2 3 YES, 0 NO
				uint32_t rate_drop;
			};

			struct stConfig {
				void clear() { memset(this, 0, sizeof(stConfig)); };
				uint32_t rate_mana_artefact;
				uint32_t rate_grand_prix_ticket;
				uint32_t rate_SSC_ticket;
			};

		public:
			DropSystem();
			virtual ~DropSystem();

			/*static*/ void load();

			/*static*/ bool isLoad();

			/*static*/ DropItem drawArtefactPang(stCourseInfo& _ci, uint32_t _num_players);
			/*static*/ std::vector< DropItem > drawCourse(stDropCourse& _dc, stCourseInfo& _ci);
			/*static*/ DropItem drawManaArtefact(stCourseInfo& _ci);
			/*static*/ DropItem drawGrandPrixTicket(stCourseInfo& _ci, player& _session);
			/*static*/ std::vector< DropItem > drawSSCTicket(stCourseInfo& _ci);

			/*static*/ stDropCourse* findCourse(unsigned char _course);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::map< unsigned char, stDropCourse > m_course;

			/*static*/ stConfig m_config;

			/*static*/ bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< DropSystem > sDropSystem;
}

#endif // !_STDA_DROP_SYSTEM_HPP
