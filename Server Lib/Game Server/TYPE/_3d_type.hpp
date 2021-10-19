// Arquivo _3d_type.hpp
// Criado em 03/10/2020 as 14:39 por Acrisio
// Defini��o dos tipos usados nos calculos 3D

#pragma once
#ifndef _STDA__3D_TYPE_HPP
#define _STDA__3D_TYPE_HPP

#include "../UTIL/vector3d.hpp"
#include "game_type.hpp"
#include "extra_power_interface.hpp"

#include <cstdint>

namespace stdA {

	constexpr double PI = 3.1415926535897932384626433832795;

	constexpr float SCALE_PANGYA = 3.2f;
	constexpr float DIVIDE_SCALE_PANGYA = 1.f / SCALE_PANGYA;

	constexpr float DIVIDE_SLOPE_CURVE_SCALE = 0.00875f;
	constexpr float SCALE_SLOPE_CURVE = 1.f / DIVIDE_SLOPE_CURVE_SCALE;

	constexpr double GRAVITY_SCALE_PANGYA = 34.295295715332;

	constexpr float DESVIO_SCALE_PANGYA_TO_YARD = DIVIDE_SCALE_PANGYA / 1.5f;

	constexpr float STEP_TIME = 0.02f;

	constexpr float EFECT_MAGNUS = 0.00008f;

	constexpr float ROTATION_SPIN_FACTOR = 3.f;
	constexpr float ROTATION_CURVE_FACTOR = 0.75f;

	constexpr float SPIN_DECAI_FACTOR = 0.1f;

	constexpr float WIND_SPIKE_FACTOR = 0.01f;

	constexpr uint32_t BASE_POWER_CLUB = 15; // 15 * 2 = 30, 200 + 230, power base

	constexpr double POWER_SPIN_PW_FACTORY = 0.0698131695389748;
	constexpr double POWER_CURVE_PW_FACRORY = 0.349065847694874;

	constexpr float ACUMULATION_SPIN_FACTOR = 25.132742f;
	constexpr float ACUMULATION_CURVE_FACTOR = 12.566371f;

	constexpr float BALL_ROTATION_SPIN_COBRA = 2.5f;
	constexpr float BALL_ROTATION_SPIN_SPIKE = 3.1f;

	constexpr double ROUND_ZERO = 0.00001;

	enum eTYPE_DISTANCE : unsigned char {
		LESS_10,
		LESS_15,
		LESS_28,
		LESS_58,
		BIGGER_OR_EQUAL_58,
	};

	enum ePOWER_SHOT_FACTORY : unsigned char {
		NO_POWER_SHOT,
		ONE_POWER_SHOT,
		TWO_POWER_SHOT,
		ITEM_15_POWER_SHOT,
	};

	enum eCLUB_TYPE : unsigned char {
		WOOD,
		IRON,
		PW,
		PT
	};

	struct options3D {
		public:
			options3D(uSpecialShot _shot, Vector3D _position, IExtraPower& _extra_power, ePOWER_SHOT_FACTORY _power_shot, 
					  float _distance, float _power_slot, float _percent_shot, float _spin, float _curve, float _mira);
			virtual ~options3D();

		public:
			uSpecialShot m_shot;
			Vector3D m_position;
			IExtraPower& m_extra_power;
			ePOWER_SHOT_FACTORY m_power_shot;

			float m_distance;
			float m_power_slot;
			float m_percent_shot;

			float m_spin;
			float m_curve;

			float m_mira;
	};

	eTYPE_DISTANCE calculeTypeDistance(float _distance);
	eTYPE_DISTANCE calculeTypeDistanceByPosition(Vector3D& _vec1, Vector3D& _vec2);

	float getPowerShotFactory(unsigned char _ps);

	float getPowerByDegreeAndSpin(float _degree, float _spin);
}

#endif // !_STDA__3D_TYPE_HPP