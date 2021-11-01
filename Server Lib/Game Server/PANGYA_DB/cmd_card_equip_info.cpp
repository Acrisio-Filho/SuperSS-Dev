// Arquivo cmd_card_equip_info.cpp
// Criado em 21/03/2018 as 22:33 por Acrisio
// Implementa��o da classe CmdCardEquipInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_card_equip_info.hpp"

using namespace stdA;

CmdCardEquipInfo::CmdCardEquipInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), v_cei() {
}

CmdCardEquipInfo::CmdCardEquipInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), v_cei() {
}

CmdCardEquipInfo::~CmdCardEquipInfo() {
}

void CmdCardEquipInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_reuslt*/) {

	checkColumnNumber(13/*tempo*/, (uint32_t)_result->cols);

	CardEquipInfoEx cei{ 0 };

	cei.index = IFNULL(atoll, _result->data[0]);
	cei._typeid = IFNULL(atoi, _result->data[1]);
	cei.parts_typeid = IFNULL(atoi, _result->data[3]);
	cei.parts_id = IFNULL(atoi, _result->data[4]);
	cei.efeito = IFNULL(atoi, _result->data[5]);
	cei.efeito_qntd = IFNULL(atoi, _result->data[6]);
	cei.slot = IFNULL(atoi, _result->data[7]);
	if (_result->data[8] != nullptr)
		_translateDate(_result->data[8], &cei.use_date);
	if (_result->data[9] != nullptr)
		_translateDate(_result->data[9], &cei.end_date);
	// tempo[10]
	cei.tipo = IFNULL(atoi, _result->data[11]);
	cei.use_yn = (unsigned char)IFNULL(atoi, _result->data[12]);

	v_cei.push_back(cei);
}

response* CmdCardEquipInfo::prepareConsulta(database& _db) {

	v_cei.clear();
	v_cei.shrink_to_fit();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", 0");

	checkResponse(r, "nao conseguiu pegar o card equip info do player: " + std::to_string(m_uid));

	return r;
}

std::vector< CardEquipInfoEx >& CmdCardEquipInfo::getInfo() {
	return  v_cei;
}

uint32_t CmdCardEquipInfo::getUID() {
	return m_uid;
}

void CmdCardEquipInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
