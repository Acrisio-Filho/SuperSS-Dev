// Arquivo quad_tree.cpp
// Criado em 03/10/2020 as 17:10 por Acrisio
// Implementa��o da classe QuadTree3D

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "quad_tree3d.hpp"
#include "matrix3d.hpp"
#include <algorithm>

using namespace stdA;

QuadTree3D::QuadTree3D() : m_gravity_factor(1.f), m_second_influ(0.f, 0.f, 0.f), m_slope_influ(0.f, 0.f, 1.f), 
	m_ball(nullptr), m_club(nullptr), m_wind(nullptr), m_ball_position_init(0.f, 0.f, 0.f), m_power_range_shot(0.f), 
	m_shot{0u}, m_power_factor_shot(0.f), m_percent_shot_sqrt(0.f), m_power_factor(0.f), m_spike_init(-1),
	m_spike_med(-1), m_cobra_init(-1) {
}

QuadTree3D::~QuadTree3D() {
}

float QuadTree3D::getGravity() {
	return (float)GRAVITY_SCALE_PANGYA * m_gravity_factor;
}

void QuadTree3D::setGravityFactor(float _factor) {
	m_gravity_factor = _factor;
}

void QuadTree3D::init_shot(Ball3D* _ball, Club3D* _club, Vector3D* _wind, options3D _options) {

	m_ball = _ball;
	m_club = _club;
	m_wind = _wind;

	m_shot = _options.m_shot;
	m_spike_init = -1;
	m_spike_med = -1;
	m_cobra_init = -1;

	m_ball->m_position = _options.m_position.clone();
	m_ball_position_init = _options.m_position.clone();

	m_club->m_type_distance = calculeTypeDistance(_options.m_distance);

	m_ball->m_max_height = m_ball->m_position.m_y;

	m_ball->m_count = 0u;

	m_ball->m_num_max_height = -1l;

	float pwr = m_club->getPower(_options.m_extra_power, _options.m_power_slot, _options.m_power_shot, _options.m_spin);

	m_power_range_shot = m_club->getRange(_options.m_extra_power, _options.m_power_slot, _options.m_power_shot);

	m_power_factor = pwr;

	pwr *= std::sqrt(_options.m_percent_shot);

	pwr *= (m_shot.stSpecialShot.tomahawk || m_shot.stSpecialShot.spike) ? 1.3f : 1.f;

	// !@ Error de pangya e ch�o(ground)
	pwr *= std::sqrt(1.f); // 100% por enquanto

	m_power_factor_shot = pwr;
	m_percent_shot_sqrt = std::sqrt(_options.m_percent_shot);

	m_ball->m_curve = _options.m_curve;
	m_ball->m_spin = _options.m_spin;

	auto mtx1 = getMatrixByDegree(_options.m_mira + (0 - (m_ball->m_curve * (float)POWER_CURVE_PW_FACRORY)), 1);
	auto mtx2 = getMatrixByDegree(m_club->getDegreeRadByDistanceType(m_ball->m_spin), 0);

	m_ball->m_curve -= getSlope(_options.m_mira, (float)std::rand());

	pwr *= (std::abs(m_ball->m_curve) * 0.1f) + 1.f;

	/*
	* Ball Velocity init
	*/
	
	m_ball->m_velocity = mtx2.m_v3;
	m_ball->m_velocity.multiplyScalar(pwr);
	m_ball->m_velocity.applyMatrix4(mtx1);

	/*
	* end
	*/

	m_ball->m_rotation_curve = m_ball->m_curve * _options.m_percent_shot;
	m_ball->m_rotation_spin = m_club->m_type_distance == eTYPE_DISTANCE::BIGGER_OR_EQUAL_58
		? (m_club->getRotationSpin(_options.m_extra_power, _options.m_power_slot, _options.m_power_shot) * _options.m_percent_shot) * _options.m_percent_shot
		: 0.f;
}

void QuadTree3D::init_shot(Ball3D* _ball, Club3D* _club, Vector3D* _wind, ShotEndLocationData& _shot_cube, float _distance) {

	m_ball = _ball;
	m_club = _club;
	m_wind = _wind;

	m_shot = _shot_cube.special_shot; //_options.m_shot;
	m_spike_init = -1;
	m_spike_med = -1;
	m_cobra_init = -1;

	m_ball->m_position = Vector3D(_shot_cube.location.x, _shot_cube.location.y, _shot_cube.location.z);
	m_ball_position_init = m_ball->m_position.clone();

	m_club->m_type_distance = calculeTypeDistance(_distance/*_options.m_distance*/);

	m_ball->m_max_height = m_ball->m_position.m_y;

	m_ball->m_count = 0u;

	m_ball->m_num_max_height = -1l;

	float pwr = _shot_cube.power_factor; //m_club->getPower(_options.m_extra_power, _options.m_power_slot, _options.m_power_shot, _options.m_spin);

	m_power_range_shot = _shot_cube.power_club; //m_club->getRange(_options.m_extra_power, _options.m_power_slot, _options.m_power_shot);

	m_power_factor = pwr;

	pwr *= std::sqrt(_shot_cube.porcentagem/*_options.m_percent_shot*/);

	pwr *= (m_shot.stSpecialShot.tomahawk || m_shot.stSpecialShot.spike) ? 1.3f : 1.f;

	// !@ Error de pangya e ch�o(ground)
	pwr *= std::sqrt(1.f); // 100% por enquanto

	m_power_factor_shot = pwr;
	m_percent_shot_sqrt = std::sqrt(_shot_cube.porcentagem/*_options.m_percent_shot*/);

	m_ball->m_curve = _shot_cube.ball_point.x; //_options.m_curve;
	m_ball->m_spin = _shot_cube.ball_point.y; //_options.m_spin;

	/*auto mtx1 = getMatrixByDegree(_options.m_mira + (0 - (m_ball->m_curve * (float)POWER_CURVE_PW_FACRORY)), 1);
	auto mtx2 = getMatrixByDegree(m_club->getDegreeRadByDistanceType(m_ball->m_spin), 0);

	m_ball->m_curve -= getSlope(_options.m_mira, (float)std::rand());*/

	pwr *= (std::abs(m_ball->m_curve) * 0.1f) + 1.f;

	/*
	* Ball Velocity init
	*/

	/*m_ball->m_velocity = mtx2.m_v3;
	m_ball->m_velocity.multiplyScalar(pwr);
	m_ball->m_velocity.applyMatrix4(mtx1);*/
	m_ball->m_velocity = Vector3D(_shot_cube.ball_velocity.x, _shot_cube.ball_velocity.y, _shot_cube.ball_velocity.z);

	/*
	* end
	*/

	m_ball->m_rotation_curve = _shot_cube.ball_rotation_curve; //m_ball->m_curve * _options.m_percent_shot;
	m_ball->m_rotation_spin = _shot_cube.ball_rotation_spin; /*m_club->m_type_distance == eTYPE_DISTANCE::BIGGER_OR_EQUAL_58
		? (m_club->getRotationSpin(_options.m_extra_power, _options.m_power_slot, _options.m_power_shot) * _options.m_percent_shot) * _options.m_percent_shot
		: 0.f;*/
}

float QuadTree3D::getSlope(float _mira, float _line_ball) {
	
	Vector3D slope_cross = m_ball->m_slope.clone().cross(m_slope_influ);

	Matrix3D mtx_slope(
		slope_cross.clone().normalize(),
		m_ball->m_slope,
		slope_cross.cross(m_ball->m_slope).normalize(),
		Vector3D(0.f, 0.f, 0.f)
	);

	auto mtx1 = getMatrixByDegree(_mira * -1.f, 1);
	auto mtx2 = getMatrixByDegree(_line_ball * 2.f, 1);

	mtx_slope = Matrix3D::crossMatrix(mtx2, mtx_slope);
	mtx_slope = Matrix3D::crossMatrix(mtx_slope, mtx1);

	return mtx_slope.m_v2.m_x * 0.5f;
}

Matrix3D QuadTree3D::getMatrixByDegree(float _degree_rad, int _option) {
	
	Matrix3D mtx(
		Vector3D(0.f, 0.f, 0.f), 
		Vector3D(0.f, 0.f, 0.f), 
		Vector3D(0.f, 0.f, 0.f),
		Vector3D(0.f, 0.f, 0.f)
	);

	if (_option == 0) {

		mtx.m_v1.m_x = 1.f;
		mtx.m_v1.m_y = 0.f;
		mtx.m_v1.m_z = 0.f;
		
		mtx.m_v2.m_x = 0.f;
		mtx.m_v2.m_y = std::cos(_degree_rad);
		mtx.m_v2.m_z = -std::sin(_degree_rad);

		mtx.m_v3.m_x = 0.f;
		mtx.m_v3.m_y = std::sin(_degree_rad);
		mtx.m_v3.m_z = std::cos(_degree_rad);

		mtx.m_v4.m_x = 0.f;
		mtx.m_v4.m_y = 0.f;
		mtx.m_v4.m_z = 0.f;

	}else if (_option == 1) {

		mtx.m_v1.m_x = std::cos(_degree_rad);
		mtx.m_v1.m_y = 0.f;
		mtx.m_v1.m_z = std::sin(_degree_rad);

		mtx.m_v2.m_x = 0.f;
		mtx.m_v2.m_y = 1.f;
		mtx.m_v2.m_z = 0.f;

		mtx.m_v3.m_x = -std::sin(_degree_rad);
		mtx.m_v3.m_y = 0.f;
		mtx.m_v3.m_z = std::cos(_degree_rad);

		mtx.m_v4.m_x = 0.f;
		mtx.m_v4.m_y = 0.f;
		mtx.m_v4.m_z = 0.f;
	}

	return mtx;
}

void QuadTree3D::ballProcess(float _step_time, float _final) {

	boundProcess(_step_time, _final);

	// Cobra
	if (m_shot.stSpecialShot.cobra && m_cobra_init < 0) {

		if (m_percent_shot_sqrt < std::sqrt(0.8))
			m_percent_shot_sqrt = (float)std::sqrt(0.8);

		if (m_ball->m_count == 0u) {

			m_ball->m_velocity.m_y = 0.f;

			m_ball->m_velocity.normalize().multiplyScalar(m_power_factor_shot);
		}

		float diff = m_ball->m_position.clone().sub(m_ball_position_init).length();
		float cobra_init_up = ((m_power_range_shot * m_percent_shot_sqrt) - 100.f) * SCALE_PANGYA;

		if (diff >= cobra_init_up) {

			float power_multiply = 0.f;

			if (m_club->m_club_info.m_type == eCLUB_TYPE::WOOD) {

				switch ((int)m_club->m_club_info.m_power_base) {
				case 230:
					power_multiply = 74.f;
					break;
				case 210:
					power_multiply = 76.f;
					break;
				case 190:
					power_multiply = 80.f;
					break;
				}
			}

			m_cobra_init = (char)m_ball->m_count;

			m_ball->m_velocity.normalize().multiplyScalar(power_multiply).multiplyScalar(m_percent_shot_sqrt);

			m_ball->m_rotation_spin = BALL_ROTATION_SPIN_COBRA;
		}
	
	}else {

		if (m_spike_init < 0 && m_cobra_init < 0 && m_club->m_type_distance == eTYPE_DISTANCE::BIGGER_OR_EQUAL_58)
			m_ball->m_rotation_spin -= ((0.5f - (m_ball->m_spin * SPIN_DECAI_FACTOR)) * STEP_TIME);
		else if ((m_shot.stSpecialShot.spike && m_spike_init >= 0) || (m_shot.stSpecialShot.cobra && m_cobra_init >= 0))
			m_ball->m_rotation_spin -= STEP_TIME;

		if (m_shot.stSpecialShot.spike && m_ball->m_count == 0u) {

			m_ball->m_velocity.m_y = 0.f;
			m_ball->m_velocity.normalize().multiplyScalar(m_power_factor_shot);

			m_ball->m_velocity.normalize().multiplyScalar(72.5f).multiplyScalar(m_percent_shot_sqrt * 2.f);

			m_ball->m_rotation_spin = BALL_ROTATION_SPIN_SPIKE;

			m_spike_init = (char)m_ball->m_count;
		}

		if (m_shot.stSpecialShot.spike && m_ball->m_num_max_height >= 0 && (uint32_t)(m_ball->m_num_max_height + 0x3C) < m_ball->m_count && m_spike_med < 0) {

			m_spike_med = (char)m_ball->m_count;

			if (m_club->m_club_info.m_type == eCLUB_TYPE::WOOD) {

				float new_power = 0.f;

				switch ((int)m_club->m_club_info.m_power_base) {
					case 230:

						new_power = 344.f;

						if ((m_club->m_club_info.m_power_factor * m_percent_shot_sqrt) < 344.f)
							new_power -= (m_club->m_club_info.m_power_factor * m_percent_shot_sqrt);
						else
							new_power = 0.f;

						new_power = new_power / 112.f * 21.5f;

						new_power = -8.f - new_power;

						m_ball->m_velocity.m_y = new_power;

						break;
					case 210:

						new_power = 306.f;

						if ((m_club->m_club_info.m_power_factor * m_percent_shot_sqrt) < 306.f)
							new_power -= (m_club->m_club_info.m_power_factor * m_percent_shot_sqrt);
						else
							new_power = 0.f;

						new_power = new_power / 105.f * 20.5f;

						new_power = -10.3f - new_power;

						m_ball->m_velocity.m_y = new_power;

						break;
					case 190:

						new_power = 273.f;

						if ((m_club->m_club_info.m_power_factor * m_percent_shot_sqrt) < 273.f)
							new_power -= (m_club->m_club_info.m_power_factor * m_percent_shot_sqrt);
						else
							new_power = 0.f;

						new_power = new_power / 100.f * 20.2f;

						new_power = -10.8f - new_power;

						m_ball->m_velocity.m_y = new_power;

						break;
				}
			}

			m_ball->m_velocity.multiplyScalar(m_percent_shot_sqrt * 7.f);

			m_ball->m_rotation_spin = m_ball->m_spin;
		}
	}

	if (m_ball->m_velocity.m_y < 0 && m_ball->m_num_max_height < 0) {

		m_ball->m_max_height = m_ball->m_position.m_y;
		m_ball->m_num_max_height = (int)m_ball->m_count;
	}

	m_ball->m_count++;
}

void QuadTree3D::boundProcess(float _step_time, float _final) {

	if (m_shot.stSpecialShot.spike && m_ball->m_num_max_height >= 0 && (uint32_t)(m_ball->m_num_max_height + 0x3C) > m_ball->m_count)
		return;

	auto accell = applyForce();

	m_ball->m_velocity.add(accell.divideScalar(m_ball->m_mass).multiplyScalar(_step_time));

	if (m_ball->m_num_max_height == -1)
		m_ball->m_velocity.add(m_second_influ.divideScalar(m_ball->m_mass).multiplyScalar(_step_time));

	m_ball->m_acumulation_curve += (m_ball->m_rotation_curve * ACUMULATION_CURVE_FACTOR * _step_time);
	m_ball->m_acumulation_spin += (m_ball->m_rotation_spin * ACUMULATION_SPIN_FACTOR * _step_time);

	m_ball->m_position.add(m_ball->m_velocity.clone().multiplyScalar((_final != -1.f ? _final : _step_time)));
}

Vector3D QuadTree3D::applyForce() {
	
	Vector3D force(0.f, 0.f, 0.f);

	// != 0
	if (std::abs(m_ball->m_rotation_curve) > ROUND_ZERO) {

		Vector3D curveInflu(m_ball->m_velocity.m_z * -1.f, 0.f, m_ball->m_velocity.m_x);

		curveInflu.normalize();

		if (m_cobra_init < 0 || m_spike_init < 0)
			curveInflu.multiplyScalar(ROTATION_CURVE_FACTOR * m_ball->m_rotation_curve * m_club->m_club_info.m_rotation_curve);

		force.add(curveInflu);
	}

	if (m_shot.stSpecialShot.spike && m_spike_init < 0)
		return Vector3D(0.f, 0.f, 0.f);
	else if (m_shot.stSpecialShot.cobra && m_cobra_init < 0)
		return force;

	force.add(m_wind->clone().multiplyScalar((m_shot.stSpecialShot.spike ? WIND_SPIKE_FACTOR : STEP_TIME)));

	force.m_y = force.m_y - (getGravity() * m_ball->m_mass);

	// != 0
	if (std::abs(m_ball->m_rotation_spin) > ROUND_ZERO)
		force.m_y = force.m_y + (ROTATION_SPIN_FACTOR * m_ball->m_rotation_spin * m_club->m_club_info.m_rotation_spin);

	force.sub(m_ball->m_velocity.clone().multiplyScalar(m_ball->m_velocity.length() * EFECT_MAGNUS));

	return force;
}
