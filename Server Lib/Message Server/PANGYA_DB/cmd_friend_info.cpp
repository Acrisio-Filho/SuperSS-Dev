// Arquivo cmd_friend_info_cpp
// Criado em 30/07/2018 as 00:24 por Acrisio
// Implementa��o da classe CmdFriendInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_friend_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdFriendInfo::CmdFriendInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_type(ALL), m_friend_uid(0u), m_fi() {
}

CmdFriendInfo::CmdFriendInfo(uint32_t _uid, TYPE _type, uint32_t _friend_uid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_friend_uid(_friend_uid), m_fi() {
}

CmdFriendInfo::~CmdFriendInfo() {
}

void CmdFriendInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(13, (uint32_t)_result->cols);

	FriendInfoEx fi{ 0 };

	if (_result->data[0] != nullptr)
#if defined(_WIN32)
		memcpy_s(fi.nickname, sizeof(fi.nickname), _result->data[0], sizeof(fi.nickname));
#elif defined(__linux__)
		memcpy(fi.nickname, _result->data[0], sizeof(fi.nickname));
#endif
	fi.uid = (uint32_t)IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
#if defined(_WIN32)
		memcpy_s(fi.apelido, sizeof(fi.apelido), _result->data[2], sizeof(fi.apelido));
#elif defined(__linux__)
		memcpy(fi.apelido, _result->data[2], sizeof(fi.apelido));
#endif
	fi.lUnknown = IFNULL(atoi, _result->data[3]);
	fi.lUnknown2 = IFNULL(atoi, _result->data[4]);
	fi.lUnknown3 = IFNULL(atoi, _result->data[5]);
	fi.lUnknown4 = IFNULL(atoi, _result->data[6]);
	fi.lUnknown5 = IFNULL(atoi, _result->data[7]);
	fi.lUnknown6 = IFNULL(atoi, _result->data[8]);
	fi.cUnknown_flag = (char)IFNULL(atoi, _result->data[9]);
	fi.state.ucState = (unsigned char)IFNULL(atoi, _result->data[10]);
	fi.level = (unsigned char)IFNULL(atoi, _result->data[11]);
	fi.flag.ucFlag = (unsigned char)IFNULL(atoi, _result->data[12]);

	auto it = m_fi.find(fi.uid);

	if (it == m_fi.end())
		m_fi[fi.uid] = fi;
	else
		_smp::message_pool::getInstance().push(new message("[CmdFriendInfo::lineResult][Error][WARNIG] retornou duplicata de Amigos[UID=" + std::to_string(fi.uid) +"] do player[UID=" + std::to_string(m_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdFriendInfo::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdFriendInfo::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, (m_type == ALL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + ((m_type == ALL) ? std::string("") : ", " + std::to_string(m_friend_uid)));

	checkResponse(r, "nao conseguiu pegar a o Friend list do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFriendInfo::getUID() {
	return m_uid;
}

void CmdFriendInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFriendInfo::getFriendUID() {
	return m_friend_uid;
}

void CmdFriendInfo::setFriendUID(uint32_t _friend_uid) {
	m_friend_uid = _friend_uid;
}

CmdFriendInfo::TYPE CmdFriendInfo::getType() {
	return m_type;
}

void CmdFriendInfo::setType(TYPE _type) {
	m_type = _type;
}

std::map< uint32_t, FriendInfoEx >& CmdFriendInfo::getInfo() {
	return m_fi;
}
