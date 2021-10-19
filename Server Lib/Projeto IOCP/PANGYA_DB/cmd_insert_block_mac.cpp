// Arquivo cmd_insert_block_mac.cpp
// Criado em 22/08/2019 as 11:51 por Acrisio
// Implementação da classe CmdInsertBlockMAC

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_block_mac.hpp"

using namespace stdA;

CmdInsertBlockMAC::CmdInsertBlockMAC(bool _waiter) : pangya_db(_waiter), m_mac_address("") {
}

CmdInsertBlockMAC::CmdInsertBlockMAC(std::string& _mac_address, bool _waiter)
	: pangya_db(_waiter), m_mac_address(_mac_address) {
}

CmdInsertBlockMAC::~CmdInsertBlockMAC() {
}

void CmdInsertBlockMAC::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// Não usa por que é um INSERT
	return;
}

response* CmdInsertBlockMAC::prepareConsulta(database& _db) {
	
	if (m_mac_address.empty())
		throw exception("[CmdInsertBlockMAC::prepareConsulta][Error] m_mac_address is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, _db.makeText(m_mac_address));

	checkResponse(r, "nao conseguiu inserir o MAC ADDRESS[" + m_mac_address + "] para a lista de MAC bloqueado");

	return r;
}

std::string& CmdInsertBlockMAC::getMACAddress() {
	return m_mac_address;
}

void CmdInsertBlockMAC::setMACAddress(std::string& _mac_address) {
	m_mac_address = _mac_address;
}
