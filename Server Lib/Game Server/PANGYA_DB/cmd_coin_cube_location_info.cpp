// Arquivo cmd_coin_cube_location_info.cpp
// Criado em 16/10/2020 as 03:23 por Acrisio
// Implementa��o da classe CmdCoinCubeLocationInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_coin_cube_location_info.hpp"

#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdCoinCubeLocationInfo::CmdCoinCubeLocationInfo(bool _waiter) : pangya_db(_waiter), m_coin_cube(), m_course(0u) {
}

CmdCoinCubeLocationInfo::CmdCoinCubeLocationInfo(unsigned char _course, bool _waiter) :
	pangya_db(_waiter), m_coin_cube(), m_course(_course) {
}

CmdCoinCubeLocationInfo::~CmdCoinCubeLocationInfo() {

	if (!m_coin_cube.empty())
		m_coin_cube.clear();
}

void CmdCoinCubeLocationInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index*/) {

	checkColumnNumber(9, (uint32_t)_result->cols);

	uint32_t course = (uint32_t)IFNULL(atoi, _result->data[1]);
	unsigned char hole = (unsigned char)IFNULL(atoi, _result->data[2]);
	
	if (course != m_course)
		throw exception("[CmdCoinCubeLocationInfo::lineResult][Error] course retornado eh diferento do requisitado[REQ=" + std::to_string((unsigned char)m_course) 
				+ ", RET=" + std::to_string(course) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

	CubeEx cube(
		(uint32_t)IFNULL(atoi, _result->data[0]), /*Index*/
		Cube::eTYPE((uint32_t)IFNULL(atoi, _result->data[3])),
		0u,
		Cube::eFLAG_LOCATION((uint32_t)IFNULL(atoi, _result->data[4])),
		(float)IFNULL(atof, _result->data[6]), // x
		(float)IFNULL(atof, _result->data[7]), // y
		(float)IFNULL(atof, _result->data[8]), // z
		(uint32_t)IFNULL(atoi, _result->data[5]) // rate
	);

	auto it = m_coin_cube.find(hole);

	if (it != m_coin_cube.end())
		it->second.push_back(cube);
	else {

		auto ret = m_coin_cube.insert(std::make_pair(hole, std::vector< CubeEx >{ cube }));

		if (!ret.second && ret.first == m_coin_cube.end())
			_smp::message_pool::getInstance().push(new message("[CmdCoinCubeLocationInfo::lineResult][WARNING] nao conseguiu inserir hole[NUMBER=" 
					+ std::to_string((unsigned short)hole) + "] e cube no map<>", CL_FILE_LOG_AND_CONSOLE));
	}
}

response* CmdCoinCubeLocationInfo::prepareConsulta(database& _db) {
	
	if (!m_coin_cube.empty())
		m_coin_cube.clear();

	auto r = consulta(_db, m_szConsulta + std::to_string((unsigned short)m_course));

	checkResponse(r, "nao conseguiu pegar os coin, cube do course[ID=" + std::to_string((unsigned short)m_course) + "]");

	return r;
}

unsigned char CmdCoinCubeLocationInfo::getCourse() {
	return m_course;
}

MAP_HOLE_COIN_CUBE& CmdCoinCubeLocationInfo::getInfo() {
	return m_coin_cube;
}

void CmdCoinCubeLocationInfo::setCourse(unsigned char _course) {
	m_course = _course;
}

std::vector< CubeEx > CmdCoinCubeLocationInfo::getAllCoinCubeHole(unsigned char _hole_number) {
	
	std::vector< CubeEx > v_coin_cube;

	auto it = m_coin_cube.find(_hole_number);

	if (it != m_coin_cube.end())
		v_coin_cube = it->second;

	return v_coin_cube;
}
