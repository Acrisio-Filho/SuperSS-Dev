// Arquivo cmd_item_buff_info.cpp
// Criado em 21/03/2018 as 22:49 por Acrisio
// Implementa��o da classe CmdItemBuffInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_item_buff_info.hpp"

using namespace stdA;

CmdItemBuffInfo::CmdItemBuffInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), v_ib() {
}

CmdItemBuffInfo::CmdItemBuffInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), v_ib() {
}

CmdItemBuffInfo::~CmdItemBuffInfo() {
}

void CmdItemBuffInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(7, (uint32_t)_result->cols);

	ItemBuffEx ib{ 0 };

	ib.index = IFNULL(atoll, _result->data[0]);
	ib._typeid = IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
		_translateDate(_result->data[2], &ib.use_date);
	if (_result->data[3] != nullptr)
		_translateDate(_result->data[3], &ib.end_date);
	ib.tipo = IFNULL(atoi, _result->data[4]);
	ib.percent = IFNULL(atoi, _result->data[5]);
	ib.use_yn = (unsigned char)IFNULL(atoi, _result->data[6]);

	ib.tempo.setTime((uint32_t)(SystemTimeToUnix(ib.end_date) - GetLocalTimeAsUnix()));	// fim - agora (em segundos)

	v_ib.push_back(ib);
}

response* CmdItemBuffInfo::prepareConsulta(database& _db) {

	v_ib.clear();
	v_ib.shrink_to_fit();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar Yam(ou itens parecido com o efeito do yam) Equip do player: " + std::to_string(m_uid));

	return r;
}

std::vector< ItemBuffEx >& CmdItemBuffInfo::getInfo() {
	return v_ib;
}

uint32_t CmdItemBuffInfo::getUID() {
	return m_uid;
}

void CmdItemBuffInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
