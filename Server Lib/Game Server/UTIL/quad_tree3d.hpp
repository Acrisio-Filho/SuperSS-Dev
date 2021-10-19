// Arquivo quad_tree3d.hpp
// Criado em 03/10/2020 as 16:25 por Acrisio
// Definição da classe QuadTree3D

#pragma once
#ifndef _STDA_QUAD_TREE3D_HPP
#define _STDA_QUAD_TREE3D_HPP

#include "vector3d.hpp"
#include "matrix3d.hpp"
#include "ball3d.hpp"
#include "club3d.hpp"
#include "../TYPE/game_type.hpp"

namespace stdA {

	class QuadTree3D {

		public:
			QuadTree3D();
			virtual ~QuadTree3D();

			float getGravity();

			void setGravityFactor(float _factor);

			void init_shot(Ball3D* _ball, Club3D* _club, Vector3D* _wind, options3D _options);

			void init_shot(Ball3D* _ball, Club3D* _club, Vector3D* _wind, ShotEndLocationData& _shot_cube, float _distance);

			float getSlope(float _mira, float _line_ball);

			Matrix3D getMatrixByDegree(float _degree_rad, int _option);

			void ballProcess(float _step_time, float _final = -1.f);

			void boundProcess(float _step_time, float _final = -1.f);

			Vector3D applyForce();

		private:
			float m_gravity_factor;

			Vector3D m_second_influ; // Não sei bem qual é essa segunda influ, pode ser wind hill ou natural
			Vector3D m_slope_influ; // Aqui é sempre: 0.f, 0.f, 1.f

			Ball3D *m_ball;
			Club3D *m_club;
			Vector3D *m_wind;

			Vector3D m_ball_position_init;
			
			float m_power_range_shot;

			uSpecialShot m_shot;

			float m_power_factor_shot;
			float m_percent_shot_sqrt;
			float m_power_factor;

			char m_spike_init;
			char m_spike_med;
			char m_cobra_init;
	};
}

#endif // !_STDA_QUAD_TREE3D_HPP
