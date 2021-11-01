// Arquivo cmd_gift_clubset.cpp
// Criado em 31/05/2018 as 22:15 por Acrisio
// Implementa��o da classe CmdGiftClubSet

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_gift_clubset.hpp"

using namespace stdA;

CmdGiftClubSet::CmdGiftClubSet(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1) {
}

CmdGiftClubSet::CmdGiftClubSet(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdGiftClubSet::~CmdGiftClubSet() {
}

void CmdGiftClubSet::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdGiftClubSet::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdGiftClubSet::prepareConsulta][Error] ClubSet id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "player[UID=" + std::to_string(m_uid) + "] nao conseguiu presentear o ClubSet[ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdGiftClubSet::getUID() {
	return m_uid;
}

void CmdGiftClubSet::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdGiftClubSet::getID() {
	return m_id;
}

void CmdGiftClubSet::setID(int32_t _id) {
	m_id = _id;
}
