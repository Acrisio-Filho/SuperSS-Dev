// Arquivo cmd_counter_item_info.cpp
// Criado em 19/03/2018 as 23:19 por Acrisio
// Implementa��o da classe CmdCounterItemInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_counter_item_info.hpp"

using namespace stdA;

CmdCounterItemInfo::CmdCounterItemInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), v_cii() {
}

CmdCounterItemInfo::CmdCounterItemInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), v_cii() {
}

CmdCounterItemInfo::~CmdCounterItemInfo() {
}

void CmdCounterItemInfo::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {
	
	checkColumnNumber(4, (uint32_t)_result->cols);

	CounterItemInfo cii{ 0 };

	cii.active = (unsigned char)IFNULL(atoi, _result->data[0]);
	cii._typeid = IFNULL(atoi, _result->data[1]);
	cii.id = IFNULL(atoi, _result->data[2]);
	cii.value = IFNULL(atoi, _result->data[3]);

	v_cii.push_back(cii);
}

response* CmdCounterItemInfo::prepareConsulta(database& _db) {

	v_cii.clear();
	v_cii.shrink_to_fit();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o CounterItemInfo do player: " + std::to_string(m_uid));

	return r;
}

std::vector< CounterItemInfo >& CmdCounterItemInfo::getInfo() {
	return v_cii;
}

uint32_t CmdCounterItemInfo::getUID() {
	return m_uid;
}

void CmdCounterItemInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
};
