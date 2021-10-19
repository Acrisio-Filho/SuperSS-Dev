// Arquivo cmd_treasure_hunter_info.cpp
// Criado em 19/03/2018 as 23:02 por Acrisio
// Implementa��o da classe CmdTreasureHunterInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_treasure_hunter_info.hpp"

using namespace stdA;

CmdTreasureHunterInfo::CmdTreasureHunterInfo(bool _waiter) : pangya_db(_waiter), v_thi() {
}

CmdTreasureHunterInfo::~CmdTreasureHunterInfo() {
}

void CmdTreasureHunterInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);
	
	TreasureHunterInfo thi{ 0 };	// treasure hunter info

	thi.course = (unsigned char)IFNULL(atoi, _result->data[0]);
	thi.point = IFNULL(atoi, _result->data[1]);

	v_thi.push_back(thi);
}

response* CmdTreasureHunterInfo::prepareConsulta(database& _db) {

	v_thi.clear();
	v_thi.shrink_to_fit();

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao conseguiu pegar Treasure Hunter do server");

	return r;
}

std::vector< TreasureHunterInfo >& CmdTreasureHunterInfo::getInfo() {
	return v_thi;
}
