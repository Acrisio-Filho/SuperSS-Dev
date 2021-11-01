// Arquivo cmd_delete_email.cpp
// Criado em 24/03/2018 as 15:41 por Acrisio
// Implementa��o da classe CmdDeleteEmail

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_email.hpp"

#include <cstring>

using namespace stdA;

CmdDeleteEmail::CmdDeleteEmail(bool _waiter) : pangya_db(_waiter), m_uid(0), m_email_id(nullptr), m_count(0u) {
}

CmdDeleteEmail::CmdDeleteEmail(uint32_t _uid, int32_t *_email_id, uint32_t _count, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_email_id(nullptr), m_count(0u) {

	if (_email_id == nullptr || _count == 0u)
		return;

	// Alloc memory
	m_email_id = new int32_t[_count];

	// new count
	m_count = _count;

	// copy
#if defined(_WIN32)
	memcpy_s(m_email_id, _count * sizeof(int32_t), _email_id, _count * sizeof(int32_t));
#elif defined(__linux__)
	memcpy(m_email_id, _email_id, _count * sizeof(int32_t));
#endif
}

CmdDeleteEmail::~CmdDeleteEmail() {

	// free memory
	if (m_email_id != nullptr)
		delete[] m_email_id;

	m_email_id = nullptr;
	m_count = 0u;
}

void CmdDeleteEmail::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {
	// UPDATE n�o usa o result
	// mas caso algum dia eu queira usar o result, depois de deletar um email eu mexo aqui
	return;
}

response* CmdDeleteEmail::prepareConsulta(database& _db) {

	if (m_count > 0u && m_email_id != nullptr) {
		std::string ids = "";

		for (size_t i = 0u; i < m_count; ++i) {
			if (i == 0u)
				ids += std::to_string(m_email_id[i]);
			else
				ids += ", " + std::to_string(m_email_id[i]);
		}

		auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + ids + m_szConsulta[2]);

		checkResponse(r, "nao conseguiu deletar o email(s) do player: " + std::to_string(m_uid));

		return r;

	}else
		throw exception("[CmdDeleteEmail][Error] nao pode deletar Email(s) sem id(s)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 5, 0));

	return nullptr;
}

uint32_t CmdDeleteEmail::getUID() {
	return m_uid;
}

void CmdDeleteEmail::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t* CmdDeleteEmail::getEmailID() {
	return m_email_id;
}

void CmdDeleteEmail::setEmailID(int32_t *_email_id, uint32_t _count) {

	if (_email_id == nullptr || _count == 0u) {

		if (m_email_id != nullptr)
			delete[] m_email_id;

		m_email_id = nullptr;
		m_count = 0u;

		return;
	}

	// realoca se for maior
	if (m_email_id != nullptr && _count > m_count) {

		delete[] m_email_id;

		// Alloc memory
		m_email_id = new int32_t[_count];

	}else if (m_email_id == nullptr) // Alloc memory
		m_email_id = new int32_t[_count];

	// new count
	m_count = _count;

	// copy
#if defined(_WIN32)
	memcpy_s(m_email_id, _count * sizeof(int32_t), _email_id, _count * sizeof(int32_t));
#elif defined(__linux__)
	memcpy(m_email_id, _email_id, _count * sizeof(int32_t));
#endif
}

uint32_t CmdDeleteEmail::getCount() {
	return m_count;
}
