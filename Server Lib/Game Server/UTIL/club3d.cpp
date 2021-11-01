// Arquivo club3d.cpp
// Criado em 03/10/2020 as 15:28 por Acrisio
// Implementa��o da classe Club3D

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "club3d.hpp"
#include <algorithm>

using namespace stdA;

Club3D::Club3D(ClubInfo3D& _club_info, eTYPE_DISTANCE _type_distance) 
	: m_club_info(_club_info), m_type_distance(_type_distance) {
}

Club3D::~Club3D() {
}

void Club3D::init(ClubInfo3D& _clubInfo, eTYPE_DISTANCE _type_distance) {

	m_club_info = _clubInfo;
	m_type_distance = _type_distance;
}

float Club3D::getDegreeRad() {
	return (float)(m_club_info.m_degree * PI / 180.f);
}

float Club3D::getDegreeRadByDistanceType(float _spin) {
	return getDegreeRad() + (float)(m_type_distance != eTYPE_DISTANCE::BIGGER_OR_EQUAL_58 ? (_spin * POWER_SPIN_PW_FACTORY) : 0.f);
}

float Club3D::getPower(IExtraPower& _extraPower, float _pwrSlot, ePOWER_SHOT_FACTORY _psf, float _spin) {
	
	float pwr = 0.f;

	switch (m_club_info.m_type) {
		case eCLUB_TYPE::WOOD:

			pwr = _extraPower.getTotal(_psf) + getPowerShotFactory(_psf) + ((_pwrSlot - BASE_POWER_CLUB) * 2);

			pwr *= 1.5f;
			pwr /= m_club_info.m_power_base;
			pwr += 1.f;
			pwr *= m_club_info.m_power_factor;

			break;
		case eCLUB_TYPE::IRON:
			pwr = ((getPowerShotFactory(_psf) / m_club_info.m_power_base + 1.f) * m_club_info.m_power_factor) + (_extraPower.getTotal(_psf) * m_club_info.m_power_factor * 1.3f) / m_club_info.m_power_base;
			break;
		case eCLUB_TYPE::PW: {

			switch (m_type_distance) {
				case eTYPE_DISTANCE::LESS_10:
				case eTYPE_DISTANCE::LESS_15:
				case eTYPE_DISTANCE::LESS_28:
					pwr = (getPowerByDegreeAndSpin(getDegreeRad(), _spin) * (52.f + (_psf != ePOWER_SHOT_FACTORY::NO_POWER_SHOT ? 28.f : 0.f))) + (_extraPower.getTotal(_psf) * m_club_info.m_power_factor) / m_club_info.m_power_base;
					break;
				case eTYPE_DISTANCE::LESS_58:
					pwr = (getPowerByDegreeAndSpin(getDegreeRad(), _spin) * (80.f + (_psf != ePOWER_SHOT_FACTORY::NO_POWER_SHOT ? 18.f : 0.f))) + (_extraPower.getTotal(_psf) * m_club_info.m_power_factor) / m_club_info.m_power_base;
					break;
				case eTYPE_DISTANCE::BIGGER_OR_EQUAL_58:
					pwr = ((getPowerShotFactory(_psf) / m_club_info.m_power_base + 1.f) * m_club_info.m_power_factor) + (_extraPower.getTotal(_psf) * m_club_info.m_power_factor) / m_club_info.m_power_base;
					break;
			}
			break;
		}
		case eCLUB_TYPE::PT:
			pwr = m_club_info.m_power_factor;
			break;
	}

	return pwr;
}

float Club3D::getRotationSpin(IExtraPower& _extraPower, float _pwrSlot, ePOWER_SHOT_FACTORY _psf) {

	float rotation = (_extraPower.getTotal(_psf) / 2) + _pwrSlot;

	rotation /= 170.f;

	return rotation + 1.5f;
}

float Club3D::getRange(IExtraPower& _extraPower, float _pwrSlot, ePOWER_SHOT_FACTORY _psf) {

	float pwr = m_club_info.m_power_base + _extraPower.getTotal(_psf) + getPowerShotFactory(_psf);

	if (m_club_info.m_type == eCLUB_TYPE::WOOD)
		pwr += ((_pwrSlot - BASE_POWER_CLUB) * 2);

	if (m_club_info.m_type == eCLUB_TYPE::PW) {

		switch (m_type_distance) {
			case eTYPE_DISTANCE::LESS_10:
			case eTYPE_DISTANCE::LESS_15:
			case eTYPE_DISTANCE::LESS_28:
				pwr = 30.f + (_psf != ePOWER_SHOT_FACTORY::NO_POWER_SHOT ? 30.f : 0.f) + _extraPower.getTotal(_psf);
				break;
			case eTYPE_DISTANCE::LESS_58:
				pwr = 60.f + (_psf != ePOWER_SHOT_FACTORY::NO_POWER_SHOT ? 20.f : 0.f) + _extraPower.getTotal(_psf);
				break;
			case eTYPE_DISTANCE::BIGGER_OR_EQUAL_58:
				pwr = m_club_info.m_power_base + _extraPower.getTotal(_psf) + getPowerShotFactory(_psf);
				break;
		}
	}

	return pwr;
}
