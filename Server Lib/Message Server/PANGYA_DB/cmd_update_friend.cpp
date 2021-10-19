// Arquivo cmd_update_friend.cpp
// Criado em 05/08/2018 as 22:20 por Acrisio
// Implementa��o da classe CmdUpdateFriend

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_friend.hpp"

using namespace stdA;

CmdUpdateFriend::CmdUpdateFriend(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_fi{0} {
}

CmdUpdateFriend::CmdUpdateFriend(uint32_t _uid, FriendInfoEx& _fi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_fi(_fi) {
}

CmdUpdateFriend::~CmdUpdateFriend() {
}

void CmdUpdateFriend::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateFriend::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateFriend::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_fi.uid == 0)
		throw exception("[CmdUpdateFriend::prepareConsulta][Error] m_fi.uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	// Zera o bit Online e o Sexo do State Flag
	m_fi.state.stState.online = 0;
	m_fi.state.stState.sex = 0;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_fi.uid) + ", " + _db.makeText(m_fi.apelido)
			+ ", " + std::to_string(m_fi.lUnknown) + ", " + std::to_string(m_fi.lUnknown2) + ", " + std::to_string(m_fi.lUnknown3)
			+ ", " + std::to_string(m_fi.lUnknown4) + ", " + std::to_string(m_fi.lUnknown5) + ", " + std::to_string(m_fi.lUnknown6)
			+ ", " + std::to_string((short)m_fi.cUnknown_flag) + ", " + std::to_string((unsigned short)m_fi.state.ucState)
	);

	checkResponse(r, "nao consegiu atualizar Friend Info[UID=" + std::to_string(m_fi.uid) + ", APELIDO=" + std::string(m_fi.apelido) + ", UNK1=" 
			+ std::to_string(m_fi.lUnknown) + ", UNK2=" + std::to_string(m_fi.lUnknown2) + ", UNK3=" + std::to_string(m_fi.lUnknown3) + ", UNK4=" 
			+ std::to_string(m_fi.lUnknown4) + ", UNK5=" + std::to_string(m_fi.lUnknown5) + ", UNK6=" + std::to_string(m_fi.lUnknown6) + ", UNK_FLAG=" 
			+ std::to_string((short)m_fi.cUnknown_flag) + ", STATE=" + std::to_string((unsigned char)m_fi.state.ucState) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateFriend::getUID() {
	return m_uid;
}

void CmdUpdateFriend::setUID(uint32_t _uid) {
	m_uid = _uid;
}

FriendInfoEx& CmdUpdateFriend::getInfo() {
	return m_fi;
}

void CmdUpdateFriend::setInfo(FriendInfoEx& _fi) {
	m_fi = _fi;
}
