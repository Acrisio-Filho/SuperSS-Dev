// Arquivo extra_power.cpp
// Criado em 07/10/2020 as 14:03 por Acrisio
// Implemeta��o da classe ExtraPower

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "extra_power.hpp"
#include "../TYPE/_3d_type.hpp"

using namespace stdA;

ExtraPower::ExtraPower() : m_drive{ 0l }, m_power_shot{ 0l } {
}

ExtraPower::ExtraPower(stPowerQntd _drive, stPowerQntd _power_shot) 
	: m_drive(_drive), m_power_shot(_power_shot) {
}

ExtraPower::~ExtraPower() {
}

ExtraPower::stPowerQntd& ExtraPower::getPowerDrive() {
	return m_drive;
}

ExtraPower::stPowerQntd& ExtraPower::getPowerShot() {
	return m_power_shot;
}

void ExtraPower::setPowerDrive(stPowerQntd _drive) {
	m_drive = _drive;
}

void ExtraPower::setPowerShot(stPowerQntd _power_shot) {
	m_power_shot = _power_shot;
}

float ExtraPower::getTotal(unsigned char _psf) {
	
	float power = (float)m_drive.total();

	if (_psf != ePOWER_SHOT_FACTORY::NO_POWER_SHOT)
		power += (float)m_power_shot.total();

	return power;
}
