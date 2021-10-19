// Arquivo hole.hpp
// Criado em 12/08/2018 as 10:44 por Acrisio
// Defini��o da classe Hole

#pragma once
#ifndef _STDA_HOLE_HPP
#define _STDA_HOLE_HPP

#include "../TYPE/pangya_game_st.h"
#include "../TYPE/hole_type.hpp"
#include <vector>

namespace stdA {
	class Hole {
		public:
			enum eMODO : unsigned char {
				M_FRONT,
				M_BACK,
				M_RANDOM,
				M_SHUFFLE,
				M_REPEAT,
				M_SHUFFLE_COURSE,
			};

		public:
			Hole(unsigned char _course, unsigned short _numero, unsigned char _pin, eMODO _modo, unsigned char _hole_repeat, unsigned char _weather, unsigned char _wind, unsigned short _degree, uCubeCoinFlag _cube_coin);
			virtual ~Hole();

			void init(stXZLocation& _tee, stXZLocation& _pin);
			void init(Location& _tee, Location& _pin);

			bool isGood();

			// Get
			uint32_t getId();
			unsigned short getNumero();
			unsigned char getTipo();
			stHoleWind& getWind();
			stHolePar& getPar();
			unsigned char getPin();
			unsigned char getWeather();
			unsigned getCourse();
			uCubeCoinFlag& getCubeCoin();
			eMODO getModo();
			unsigned char getHoleRepeat();

			Location& getPinLocation();
			Location& getTeeLocation();

			std::vector< CubeEx >& getCubes();

			// Set
			void setWeather(unsigned char _weather);
			void setWind(unsigned char _wind, unsigned short _degree);
			void setWind(stHoleWind& _wind);

			// Finders
			CubeEx* findCubeCoin(uint32_t _id);

		protected:
			void init_cube_coin();
			void init_from_IFF_STRUCT();

		protected:
			Location m_pin_location;
			Location m_tee_location;

			std::vector< CubeEx > m_cube;

			uint32_t m_id;
			unsigned short m_numero;
			unsigned char m_tipo;
			stHoleWind m_wind;
			stHolePar m_par;
			unsigned char m_pin;
			unsigned char m_weather;
			unsigned char m_course;

			uCubeCoinFlag m_cube_coin;

			eMODO m_modo;
			unsigned char m_hole_repeat;

			bool m_good;

	};
}

#endif // !_STDA_HOLE_HPP
