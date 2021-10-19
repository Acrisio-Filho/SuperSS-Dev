// Arquivo cmd_treasure_hunter_item.cpp
// Criado em 06/09/2018 as 20:30 por Acrisio
// Implementa��o da classe CmdTreasureHunterItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_treasure_hunter_item.hpp"

using namespace stdA;

CmdTreasureHunterItem::CmdTreasureHunterItem(bool _waiter) : pangya_db(_waiter), v_thi() {
}

CmdTreasureHunterItem::~CmdTreasureHunterItem() {
}

void CmdTreasureHunterItem::lineResult(result_set::ctx_res *_result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	TreasureHunterItem thi{ 0 };

	thi._typeid = (uint32_t)IFNULL(atoi, _result->data[0]);
	thi.qntd = (uint32_t)IFNULL(atoi, _result->data[1]);
	thi.probabilidade = (uint32_t)IFNULL(atoi, _result->data[2]);
	thi.active = (unsigned char)IFNULL(atoi, _result->data[3]);
	thi.flag = (unsigned char)IFNULL(atoi, _result->data[4]);

	v_thi.push_back(thi);
}

response* CmdTreasureHunterItem::prepareConsulta(database& _db) {

	if (!v_thi.empty()) {
		v_thi.clear();
		v_thi.shrink_to_fit();
	}

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar os Treasure Hunter Item");

	return r;
}

std::vector< TreasureHunterItem >& CmdTreasureHunterItem::getInfo() {
	return v_thi;
}
