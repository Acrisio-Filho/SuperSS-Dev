// Arquivo club3d.hpp
// Criado em 03/10/2020 as 14:38 por Acrisio
// Definição da classe Club3D

#pragma once
#ifndef _STDA_CLUB3D_HPP
#define _STDA_CLUB3D_HPP

#include "club_info3d.hpp"
#include "../TYPE/extra_power_interface.hpp"

namespace stdA {

	class Club3D {

		public:
			Club3D(ClubInfo3D& _club_info, eTYPE_DISTANCE _type_distance = eTYPE_DISTANCE::BIGGER_OR_EQUAL_58);
			virtual ~Club3D();

			void init(ClubInfo3D& _clubInfo, eTYPE_DISTANCE _type_distance = eTYPE_DISTANCE::BIGGER_OR_EQUAL_58);

			float getDegreeRad();

			float getDegreeRadByDistanceType(float _spin);

			float getPower(IExtraPower& _extraPower, float _pwrSlot, ePOWER_SHOT_FACTORY _psf, float _spin);

			float getRotationSpin(IExtraPower& _extraPower, float _pwrSlot, ePOWER_SHOT_FACTORY _psf);

			float getRange(IExtraPower& _extraPower, float _pwrSlot, ePOWER_SHOT_FACTORY _psf);

		public:
			ClubInfo3D m_club_info;

			eTYPE_DISTANCE m_type_distance;
	};
}

#endif // !_STDA_CLUB3D_HPP
