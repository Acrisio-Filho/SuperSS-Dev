// Arquivo cmd_delete_achievement.hpp
// Criado em 11/11/2018 as 14:34 por Acrisio
// Implementa��o da classe CmdDeleteAchievement

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_achievement.hpp"

using namespace stdA;

CmdDeleteAchievement::CmdDeleteAchievement(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1) {
}

CmdDeleteAchievement::CmdDeleteAchievement(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteAchievement::~CmdDeleteAchievement() {
}

void CmdDeleteAchievement::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um DELETE
	return;
}

response* CmdDeleteAchievement::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdDeleteAchievement::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdDeleteAchievement::prepareConsulta][Error] m_id[VALUE=" + std::to_string(m_id) + "] is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 1));

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletar o Achievement[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteAchievement::getUID() {
	return m_uid;
}

void CmdDeleteAchievement::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteAchievement::getId() {
	return m_id;
}

void CmdDeleteAchievement::setId(int32_t _id) {
	m_id = _id;
}
