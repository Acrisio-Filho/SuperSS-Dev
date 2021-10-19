// Arquivo cmd_memorial_normal_item_info.cpp
// Criado em 22/07/2018 as 11:09 por Acrisio
// Implementa��o da classe CmdMemorialNormalItemInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_memorial_normal_item_info.hpp"

using namespace stdA;

CmdMemorialNormalItemInfo::CmdMemorialNormalItemInfo(bool _waiter) : pangya_db(_waiter), m_item() {
}

CmdMemorialNormalItemInfo::~CmdMemorialNormalItemInfo() {
}

void CmdMemorialNormalItemInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(4, (uint32_t)_result->cols);

	ctx_coin_set_item csi{ 0 };
	ctx_coin_item_ex ci{ 0 };
	
	csi.flag = -100;	// SetItem Flag, SEMPRE TEM QUE SER -100
	csi._typeid = (uint32_t)IFNULL(atoi, _result->data[0]);

	ci.tipo = -1;	// Normal Item
	ci.gacha_number = -1;
	ci.probabilidade = 0;

	ci._typeid = (uint32_t)IFNULL(atoi, _result->data[2]);
	ci.qntd = (uint32_t)IFNULL(atoi, _result->data[3]);

	auto it = m_item.find(csi._typeid);

	if (it != m_item.end())	// add um item novo ao vector do map
		it->second.item.push_back(ci);
	else {	// Add um novo ao map
		csi.tipo = (unsigned char)IFNULL(atoi, _result->data[1]);

		csi.item.push_back(ci);

		m_item[csi._typeid] = csi;
	}
}

response* CmdMemorialNormalItemInfo::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao conseguiu pegar os Memorial Normal Item Info");

	return r;
}

std::map< uint32_t, ctx_coin_set_item >& CmdMemorialNormalItemInfo::getInfo() {
	return m_item;
}
