// Arquivo club_info3d.cpp
// Criado em 03/10/2020 as 15:10 por Acrisio
// Implementa��o da classe ClubInfo3D

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "club_info3d.hpp"

using namespace stdA;

ClubInfo3D::ClubInfo3D(eCLUB_TYPE _type, float _rotation_spin, float _rotation_curve, float _power_factor, float _degree, float _power_base)
	: m_type(_type), m_rotation_spin(_rotation_spin), m_rotation_curve(_rotation_curve), m_power_factor(_power_factor), m_degree(_degree), m_power_base(_power_base) {
}

ClubInfo3D::~ClubInfo3D() {
}

AllClubInfo3D::AllClubInfo3D() : m_clubs() {

	// Wood
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::WOOD, 0.55f, 1.61f, 236.0f, 10.0f, 230.0f)); // 1W
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::WOOD, 0.50f, 1.41f, 204.0f, 13.0f, 210.0f)); // 2W
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::WOOD, 0.45f, 1.26f, 176.0f, 16.0f, 190.0f)); // 3W
	
	// Iron
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.45f, 1.07f, 161.0f, 20.0f, 180.0f)); // I2
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.45f, 0.95f, 149.0f, 24.0f, 170.0f)); // I3
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.45f, 0.83f, 139.0f, 28.0f, 160.0f)); // I4
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.45f, 0.73f, 131.0f, 32.0f, 150.0f)); // I5
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.41f, 0.67f, 124.0f, 36.0f, 140.0f)); // I6
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.36f, 0.61f, 118.0f, 40.0f, 130.0f)); // I7
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.30f, 0.57f, 114.0f, 44.0f, 120.0f)); // I8
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::IRON, 0.25f, 0.53f, 110.0f, 48.0f, 110.0f)); // I9
	
	// PW e SW
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::PW, 0.18f, 0.49f, 107.0f, 52.0f, 100.0f)); // PW
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::PW, 0.17f, 0.42f, 93.0f, 56.0f, 80.0f));   // SW
	
	// Putt
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::PT, 0.00f, 0.00f, 30.0f, 0.00f, 20.0f)); // PT1
	m_clubs.push_back(ClubInfo3D(eCLUB_TYPE::PT, 0.00f, 0.00f, 21.0f, 0.00f, 10.0f)); // PT2
}

AllClubInfo3D::~AllClubInfo3D() {

	if (!m_clubs.empty()) {

		m_clubs.clear();
		m_clubs.shrink_to_fit();
	}
}
