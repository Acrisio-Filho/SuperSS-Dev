// Arquivo cmd_update_coin_cube_location.cpp
// Criado em 17/10/2020 as 17:44 por Acrisio
// Implementa��o da classe CmdUpdateCoinCubeLocation

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_update_coin_cube_location.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdUpdateCoinCubeLocation::CmdUpdateCoinCubeLocation(bool _waiter) : pangya_db(_waiter), m_ccu() {
}

CmdUpdateCoinCubeLocation::CmdUpdateCoinCubeLocation(CoinCubeUpdate& _ccu, bool _waiter) 
	: pangya_db(_waiter), m_ccu(_ccu) {
}

CmdUpdateCoinCubeLocation::~CmdUpdateCoinCubeLocation() {
}

void CmdUpdateCoinCubeLocation::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index*/) {
	// N�o usa por que � UPDATE e INSERT
	return;
}

response* CmdUpdateCoinCubeLocation::prepareConsulta(database& _db) {
	
	if (m_ccu.hole_number < 1 || m_ccu.hole_number > 18)
		throw exception("[CmdUpdateCoinCubeLocation::prepareConsulta][Error] m_ccu.hole_number(" 
				+ std::to_string((unsigned short)m_ccu.hole_number) + ") invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	// Prote��o contra os jogos random & 0x7F
	if (sIff::getInstance().findCourse(iff::COURSE << 0x1A | (m_ccu.course_id & 0x7Fu)) == nullptr)
		throw exception("[CmdUpdateCoinCubeLocation::prepareConsulta][Error] m_ccu.course_id(" 
				+ std::to_string((unsigned short)m_ccu.course_id) + ") not exists in IFF_STRUCT", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	response* r = nullptr;

	if (m_ccu.type == CoinCubeUpdate::eTYPE::UPDATE) {

		if (m_ccu.cube.id == 0u)
			throw exception("[CmdUpdateCoinCubeLocation::prepareConsulta][Error] invalid coin/cube id(" 
					+ std::to_string(m_ccu.cube.id) + ") to Update in Database", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

		r = procedure(_db, m_szConsulta[1], 
			std::to_string(m_ccu.cube.id) + ", " + std::to_string((unsigned short)m_ccu.course_id) + ", "
			+ std::to_string((unsigned short)m_ccu.hole_number) + ", " + std::to_string(m_ccu.cube.tipo) + ", "
			+ std::to_string(m_ccu.cube.flag_location) + ", " + std::to_string(m_ccu.cube.rate) + ", "
			+ std::to_string(m_ccu.cube.location.x) + ", " + std::to_string(m_ccu.cube.location.y) + ", "
			+ std::to_string(m_ccu.cube.location.z)
		);

		checkResponse(r, "Nao conseguiu atualizar o Coin/Cube[ID=" + std::to_string(m_ccu.cube.id) + ", COURSE_ID=" + std::to_string((unsigned short)m_ccu.course_id)
			+ ", HOLE=" + std::to_string((unsigned short)m_ccu.hole_number) + ", TIPO=" + std::to_string(m_ccu.cube.tipo) + ", TIPO_LOCATION="
			+ std::to_string(m_ccu.cube.flag_location) + ", RATE=" + std::to_string(m_ccu.cube.rate) + ", X=" + std::to_string(m_ccu.cube.location.x)
			+ ", Y=" + std::to_string(m_ccu.cube.location.y) + ", Z=" + std::to_string(m_ccu.cube.location.z) + "]");

	}else {

		// Add new Coin/Cube Location
		r = procedure(_db, m_szConsulta[0],
			std::to_string((unsigned short)m_ccu.course_id) + ", " + std::to_string((unsigned short)m_ccu.hole_number) + ", " 
			+ std::to_string(m_ccu.cube.tipo) + ", " + std::to_string(m_ccu.cube.flag_location) + ", " 
			+ std::to_string(m_ccu.cube.rate) + ", " + std::to_string(m_ccu.cube.location.x) + ", " 
			+ std::to_string(m_ccu.cube.location.y) + ", " + std::to_string(m_ccu.cube.location.z)
		);

		checkResponse(r, "Nao conseguiu adicionar o Coin/Cube[COURSE_ID=" + std::to_string((unsigned short)m_ccu.course_id)
			+ ", HOLE=" + std::to_string((unsigned short)m_ccu.hole_number) + ", TIPO=" + std::to_string(m_ccu.cube.tipo) + ", TIPO_LOCATION="
			+ std::to_string(m_ccu.cube.flag_location) + ", RATE=" + std::to_string(m_ccu.cube.rate) + ", X=" + std::to_string(m_ccu.cube.location.x)
			+ ", Y=" + std::to_string(m_ccu.cube.location.y) + ", Z=" + std::to_string(m_ccu.cube.location.z) + "]");
	}

	return r;
}

CoinCubeUpdate& CmdUpdateCoinCubeLocation::getInfo() {
	return m_ccu;
}

void CmdUpdateCoinCubeLocation::setInfo(CoinCubeUpdate& _ccu) {
	m_ccu = _ccu;
}
