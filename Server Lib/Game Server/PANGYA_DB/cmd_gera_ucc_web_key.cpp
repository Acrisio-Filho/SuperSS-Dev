// Arquivo cmd_gera_ucc_web_key.cpp
// Criado em 14/07/2018 as 18:46 por Acrisio
// Implementa��o da classe CmdGeraUCCWebkey

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_gera_ucc_web_key.hpp"

#include <cstring>

using namespace stdA;

CmdGeraUCCWebKey::CmdGeraUCCWebKey(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ucc_id(-1), m_key("") {
}

CmdGeraUCCWebKey::CmdGeraUCCWebKey(uint32_t _uid, int32_t _ucc_id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ucc_id(_ucc_id), m_key("") {
}

CmdGeraUCCWebKey::~CmdGeraUCCWebKey() {
}

void CmdGeraUCCWebKey::lineResult(result_set::ctx_res *_result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	if (is_valid_c_string(_result->data[0]))
		m_key.assign(_result->data[0], strlen(_result->data[0]));

	if (m_key.empty())
		throw exception("[CmdGeraUCCWebKey::lineResult][Error] m_key is empty, nao conseguiu pegar uma ucc key do banco de dados.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdGeraUCCWebKey::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdGeraUCCWebKey::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ucc_id <= 0)
		throw exception("[CmdGeraUCCWebKey::prepareConsulta][Error] m_ucc_id[value=" + std::to_string(m_ucc_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ucc_id));

	checkResponse(r, "nao conseguiu gerar um UCC[ID=" + std::to_string(m_ucc_id) + "] Web Key para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdGeraUCCWebKey::getUID() {
	return m_uid;
}

void CmdGeraUCCWebKey::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdGeraUCCWebKey::getUCCID() {
	return m_ucc_id;
}

void CmdGeraUCCWebKey::setUCCID(int32_t _ucc_id) {
	m_ucc_id = _ucc_id;
}

std::string& CmdGeraUCCWebKey::getKey() {
	return m_key;
}
