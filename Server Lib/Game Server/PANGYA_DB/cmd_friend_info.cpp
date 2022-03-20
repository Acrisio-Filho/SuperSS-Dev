// Arquivo cmd_friend_info.cpp
// Criado em 28/10/2018 as 15:44 por Acrisio
// Implementa��o da classe CmdFriendInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_friend_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdFriendInfo::CmdFriendInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_fi() {
}

CmdFriendInfo::CmdFriendInfo(uint32_t _uid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_fi() {
}

CmdFriendInfo::~CmdFriendInfo() {
}

void CmdFriendInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	FriendInfo fi{ 0 };

	fi.uid = (uint32_t)IFNULL(atoi, _result->data[0]);
	
	if (is_valid_c_string(_result->data[1]))
		STRCPY_TO_MEMORY_FIXED_SIZE(fi.apelido, sizeof(fi.apelido), _result->data[1]);

	if (is_valid_c_string(_result->data[2]))
		STRCPY_TO_MEMORY_FIXED_SIZE(fi.id, sizeof(fi.id), _result->data[2]);

	if (is_valid_c_string(_result->data[3]))
		STRCPY_TO_MEMORY_FIXED_SIZE(fi.nickname, sizeof(fi.nickname), _result->data[3]);

	fi.sex = (unsigned char)IFNULL(atoi, _result->data[4]);

	auto it = m_fi.find(fi.uid);

	if (it == m_fi.end())	// N�o tem add um novo Amigo
		m_fi.insert(std::make_pair(fi.uid, fi));
	else
		_smp::message_pool::getInstance().push(new message("[CmdFriendInfo::lineResult][Error][WARNING] player[UID=" + std::to_string(m_uid) + "] tento o Amigo[UID=" 
				+ std::to_string(fi.uid) + ", ID=" + std::string(fi.id) + "] duplicado no banco de dados.", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdFriendInfo::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdFriendInfo::prepareConsulta][Error] m_uid is invalid(0)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar a lista de amigos do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFriendInfo::getUID() {
	return m_uid;
}

void CmdFriendInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::map< uint32_t/*UID*/, FriendInfo >& CmdFriendInfo::getInfo() {
	return m_fi;
}
