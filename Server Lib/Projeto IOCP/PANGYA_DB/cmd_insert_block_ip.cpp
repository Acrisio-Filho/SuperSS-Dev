// Arquivo cmd_insert_block_ip.cpp
// Criado em 14/08/2019 as 11:37 por Acrisio
// Implementa��o da classe CmdInsertBlockIP

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_block_ip.hpp"

using namespace stdA;

CmdInsertBlockIP::CmdInsertBlockIP(bool _waiter) : pangya_db(_waiter), m_ip(""), m_mask("") {
}

CmdInsertBlockIP::CmdInsertBlockIP(std::string _ip, std::string _mask, bool _waiter)
	: pangya_db(_waiter), m_ip(_ip), m_mask(_mask) {
}

CmdInsertBlockIP::~CmdInsertBlockIP() {
}

void CmdInsertBlockIP::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertBlockIP::prepareConsulta(database& _db) {

	if (m_ip.empty() || m_mask.empty())
		throw exception("[CmdInsertBlockIP::prepareConsulta][Error] m_ip[" + m_ip + "] or m_mask["
			+ m_mask + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, _db.makeText(m_ip) + ", " + _db.makeText(m_mask));

	checkResponse(r, "nao conseguiu inserir um Block IP[IP=" + m_ip + ", MASK=" + m_mask + "]");

	return r;
}

std::string& CmdInsertBlockIP::getIP() {
	return m_ip;
}

void CmdInsertBlockIP::setIP(std::string _ip) {
	
	if (!_ip.empty())
		m_ip = _ip;
}

std::string& CmdInsertBlockIP::getMask() {
	return m_mask;
}

void CmdInsertBlockIP::setMask(std::string _mask) {

	if (!_mask.empty())
		m_mask = _mask;
}
