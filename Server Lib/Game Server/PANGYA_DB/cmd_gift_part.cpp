// Arquivo cmd_gift_part.cpp
// Criado em 31/05/2018 as 22:29 por Acrisio
// Implementa��o da classe CmdGiftPart

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_gift_part.hpp"

using namespace stdA;

CmdGiftPart::CmdGiftPart(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1) {
}

CmdGiftPart::CmdGiftPart(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdGiftPart::~CmdGiftPart() {
}

void CmdGiftPart::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdGiftPart::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdGiftPart::prepareConsulta][Error] Part id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "player[UID=" + std::to_string(m_uid) + "] nao conseguiu presentear Part[ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdGiftPart::getUID() {
	return m_uid;
}

void CmdGiftPart::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdGiftPart::getID() {
	return m_id;
}

void CmdGiftPart::setID(int32_t _id) {
	m_id = _id;
}
