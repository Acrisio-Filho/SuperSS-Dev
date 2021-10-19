// Arquivo cmd_add_caddie.cpp
// Criado em 25/03/2018 as 18:58 por Acrisio
// Implementa��o da classe CmdAddCaddie

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_caddie.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdAddCaddie::CmdAddCaddie(bool _waiter) : CmdAddItemBase(_waiter), m_ci{0} {
}

CmdAddCaddie::CmdAddCaddie(uint32_t _uid, CaddieInfoEx& _ci, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
		: CmdAddItemBase(_uid, _purchase, _gift_flag, _waiter), m_ci(_ci) {
}

CmdAddCaddie::~CmdAddCaddie() {
}

void CmdAddCaddie::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	m_ci.id = IFNULL(atoi, _result->data[0]);

	//m_ci.end_date_unix = (unsigned short)IFNULL(atoi, _result->data[1]);
	if (_result->data[1] != nullptr)
		_translateDate(_result->data[1], &m_ci.end_date);
}

response* CmdAddCaddie::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdAddCaddie::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id) + ", " + std::to_string(m_ci._typeid)
				+ ", " + std::to_string((unsigned short)m_gift_flag) + ", " + std::to_string((unsigned short)m_purchase)
				+ ", " + std::to_string((unsigned short)m_ci.rent_flag) + ", " + std::to_string(m_ci.end_date_unix)
	);

	checkResponse(r, "nao conseguiu adicionar o caddie[TYPEID=" + std::to_string(m_ci._typeid) + "] para o player: " + std::to_string(m_uid));

	return r;
}

CaddieInfoEx& CmdAddCaddie::getInfo() {
	return m_ci;
}

void CmdAddCaddie::setInfo(CaddieInfoEx& _ci) {
	m_ci = _ci;
}
