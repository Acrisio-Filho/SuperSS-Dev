// Arquivo cube_coin_system.hpp
// Criado em 01/09/2018 as 17:57 por Acrisio
// Definição da classe CubeCoinSystem

#pragma once
#ifndef _STDA_CUBE_COIN_SYSTEM_HPP
#define _STDA_CUBE_COIN_SYSTEM_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/hole_type.hpp"

#include "../../Projeto IOCP/TYPE/singleton.h"

#include "../TYPE/game_type.hpp"

#include <map>

namespace stdA {
	class CubeCoinSystem {
		protected:
			class CourseCtx {
				public:
					struct Hole {
						public:
							Hole(unsigned char _numero, uint32_t _number_of_cube, uint32_t _mcc) 
									: numero(_numero), number_of_cube(_number_of_cube), max_coin_and_cube(_mcc), v_cube() {
							};
							~Hole() {};
							std::vector< CubeEx > getAllCoinCubeWizCity(bool _cube);
							std::vector< CubeEx > getAllCoinCube(bool _cube);

						public:
							void clear() { memset(this, 0, sizeof(Hole)); };
							unsigned char numero;
							uint32_t number_of_cube;		// Número de cubos no hole
							uint32_t max_coin_and_cube;	// Número máximo de coin e cube que pode ter no hole
							std::vector< CubeEx > v_cube;
					};

				public:
					CourseCtx(uint32_t _typeid, bool _active);
					virtual ~CourseCtx();

					bool isActived();

					Hole* findHole(unsigned char _hole);

				protected:
					void initialize();

					void clear();

				private:
					uint32_t m_typeid;
					std::map< unsigned char, Hole > m_hole;

					bool m_active;

#if defined(_WIN32)
					CRITICAL_SECTION m_cs;
#elif defined(__linux__)
					pthread_mutex_t m_cs;
#endif
			};

			struct CoinCubeInHole {
				uint32_t m_all_cube;
				uint32_t m_all_coin_and_cube;
			};

		public:
			CubeCoinSystem();
			virtual ~CubeCoinSystem();

			/*static*/ void load();

			/*static*/ bool isLoad();

			/*static*/ CourseCtx* findCourse(uint32_t _course_typeid);

			static CoinCubeInHole getAllCoinCubeInHoleWizCity(unsigned char _number_hole);
			static CoinCubeInHole getAllCoinCubeInHole(unsigned char _course_id, unsigned char _number_hole);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::map< uint32_t, CourseCtx > m_course;

			/*static*/ bool m_load;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< CubeCoinSystem > sCubeCoinSystem;
}

#endif // !_STDA_CUBE_COIN_SYSTEM_HPP
