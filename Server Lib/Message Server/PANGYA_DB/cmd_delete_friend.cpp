// Arquivo cmd_delete_friend.cpp
// Criado em 05/08/2018 as 20:45 por Acrisio
// Implementa��o da classe CmdDeleteFriend

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_friend.hpp"

using namespace stdA;

CmdDeleteFriend::CmdDeleteFriend(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_friend_uid(0u) {
}

CmdDeleteFriend::CmdDeleteFriend(uint32_t _uid, uint32_t _friend_uid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_friend_uid(_friend_uid) {
}

CmdDeleteFriend::~CmdDeleteFriend() {
}

void CmdDeleteFriend::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um DELETE
	return;
}

response* CmdDeleteFriend::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdDeleteFriend::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_friend_uid == 0)
		throw exception("[CmdDeleteFriend::prepareConsulta][Error] m_friend_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _delete(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_friend_uid));

	checkResponse(r, "nao conseguiu deletar Amigo[UID=" + std::to_string(m_friend_uid) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteFriend::getUID() {
	return m_uid;
}

void CmdDeleteFriend::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdDeleteFriend::getFriendUID() {
	return m_friend_uid;
}

void CmdDeleteFriend::setFriendUID(uint32_t _friend_uid) {
	m_friend_uid = _friend_uid;
}
