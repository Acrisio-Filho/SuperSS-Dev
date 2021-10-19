// Arquivo cmd_grand_prix_clear.cpp
// Criado em 14/06/2019 as 09:13 por Acrisio
// Implementa��o da classe CmdGrandPrixClear

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_grand_prix_clear.hpp"

using namespace stdA;

CmdGrandPrixClear::CmdGrandPrixClear(bool _waiter) : pangya_db(_waiter), m_uid(0u) {
}

CmdGrandPrixClear::CmdGrandPrixClear(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid) {
}

CmdGrandPrixClear::~CmdGrandPrixClear() {
}

void CmdGrandPrixClear::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	GrandPrixClear gpc{ 0 };

	gpc._typeid = (uint32_t)IFNULL(atoi, _result->data[0]);
	gpc.position = (uint32_t)IFNULL(atoi, _result->data[1]);

	m_gpc.push_back(gpc);

}

response* CmdGrandPrixClear::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdGrandPrixClear::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_gpc.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o  Grand Prix Clear do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdGrandPrixClear::getUID() {
	return m_uid;
}

void CmdGrandPrixClear::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::vector< GrandPrixClear >& CmdGrandPrixClear::getInfo() {
	return m_gpc;
}
