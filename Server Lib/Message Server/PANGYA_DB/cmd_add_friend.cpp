// Arquivo cmd_add_friend.cpp
// Criado em 04/08/2018 as 22:22 por Acrisio
// Implementa��o da classe CmdAddFriend

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_friend.hpp"

using namespace stdA;

CmdAddFriend::CmdAddFriend(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_fi{0} {
}

CmdAddFriend::CmdAddFriend(uint32_t _uid, FriendInfoEx& _fi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_fi(_fi) {
}

CmdAddFriend::~CmdAddFriend() {
}

void CmdAddFriend::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdAddFriend::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdAddFriend::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_fi.uid == 0)
		throw exception("[CmdAddFriend::prepareConsulta][Error] m_fi.uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	// Zero Bit Online And Sex Of State Flag
	m_fi.state.stState.online = 0;
	m_fi.state.stState.sex = 0;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_fi.uid) + ", " + _db.makeText(m_fi.apelido) 
			+ ", " + std::to_string(m_fi.lUnknown) + ", " + std::to_string(m_fi.lUnknown2) + ", " + std::to_string(m_fi.lUnknown3)
			+ ", " + std::to_string(m_fi.lUnknown4) + ", " + std::to_string(m_fi.lUnknown5) + ", " + std::to_string(m_fi.lUnknown6)
			+ ", " + std::to_string((short)m_fi.cUnknown_flag) + ", " + std::to_string((unsigned short)m_fi.state.ucState)
	);

	checkResponse(r, "nao conseguiu adicionar o Friend[UID=" + std::to_string(m_fi.uid) + ", APELIDO=" + std::string(m_fi.apelido) + ", NICK=" 
			+ std::string(m_fi.nickname) + ", STATE=" + std::to_string((unsigned short)m_fi.state.ucState) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddFriend::getUID() {
	return m_uid;
}

void CmdAddFriend::setUID(uint32_t _uid) {
	m_uid = _uid;
}

FriendInfoEx& CmdAddFriend::getInfo() {
	return m_fi;
}

void CmdAddFriend::setInfo(FriendInfoEx& _fi) {
	m_fi = _fi;
}
