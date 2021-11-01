// Arquivo _3d_type.cpp
// Criado em 03/10/2020 as 14:44 por Acrisio
// Implemta��o dos tipos usados no calculos 3D

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "_3d_type.hpp"
#include <algorithm>

using namespace stdA;

// Struct options3D
options3D::options3D(uSpecialShot _shot, Vector3D _position, IExtraPower& _extra_power, ePOWER_SHOT_FACTORY _power_shot, 
					 float _distance, float _power_slot, float _percent_shot, float _spin, float _curve, float _mira)
	: m_shot(_shot), m_position(_position), m_extra_power(_extra_power), m_power_shot(_power_shot), m_distance(_distance), 
	  m_power_slot(_power_slot), m_percent_shot(_percent_shot), m_spin(_spin), m_curve(_curve), m_mira(_mira) {
}

options3D::~options3D() {
}

eTYPE_DISTANCE stdA::calculeTypeDistance(float _distance) {

	eTYPE_DISTANCE type = eTYPE_DISTANCE::BIGGER_OR_EQUAL_58;

	if (_distance >= 58.f)
		return eTYPE_DISTANCE::BIGGER_OR_EQUAL_58;
	else if (_distance < 10.f)
		return eTYPE_DISTANCE::LESS_10;
	else if (_distance < 15.f)
		return eTYPE_DISTANCE::LESS_15;
	else if (_distance < 28.f)
		return eTYPE_DISTANCE::LESS_28;
	else if (_distance < 58.f)
		return eTYPE_DISTANCE::LESS_58;

	return type;
}

eTYPE_DISTANCE stdA::calculeTypeDistanceByPosition(Vector3D& _vec1, Vector3D& _vec2) {
	return calculeTypeDistance(_vec1.distanceXZTo(_vec2) * DIVIDE_SCALE_PANGYA);
}

float stdA::getPowerShotFactory(unsigned char _ps) {
	
	float power_shot_factory = 0.f;

	switch (_ps) {
		case ePOWER_SHOT_FACTORY::ONE_POWER_SHOT:
			power_shot_factory = 10.f;
			break;
		case ePOWER_SHOT_FACTORY::TWO_POWER_SHOT:
			power_shot_factory = 20.f;
			break;
		case ePOWER_SHOT_FACTORY::ITEM_15_POWER_SHOT:
			power_shot_factory = 15.f;
			break;
	}

	return power_shot_factory;
}

float stdA::getPowerByDegreeAndSpin(float _degree, float _spin) {
	return	0.5f + (float)((0.5f * (_degree + (_spin * POWER_SPIN_PW_FACTORY))) / (56.f / 180.f * PI));
}
