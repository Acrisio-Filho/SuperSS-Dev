// Arquivo cmd_update_pang.cpp
// Criado em 13/05/2018 as 10:58 por Acrisio
// Implementa��o da classe CmdUpdatePang

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_pang.hpp"

using namespace stdA;

CmdUpdatePang::CmdUpdatePang(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pang(0ull), m_type_update(INCREASE) {
}

CmdUpdatePang::CmdUpdatePang(uint32_t _uid, uint64_t _pang, T_UPDATE_PANG _type_update, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pang(_pang), m_type_update(_type_update) {
}

CmdUpdatePang::~CmdUpdatePang() {
}

void CmdUpdatePang::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// Aqui � update ent�o nao usa o _result e nem o index
	return;
}

response* CmdUpdatePang::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::string(m_type_update == INCREASE ? " + " : " - ") + std::to_string(m_pang) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar o pang[value=" + std::string(m_type_update == INCREASE ? " + " : " - ") + std::to_string(m_pang) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdatePang::getUID() {
	return m_uid;
}

void CmdUpdatePang::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdUpdatePang::getPang() {
	return m_pang;
}

void CmdUpdatePang::setPang(uint64_t _pang) {
	m_pang = _pang;
}

CmdUpdatePang::T_UPDATE_PANG CmdUpdatePang::getTypeUpdate() {
	return m_type_update;
}

void CmdUpdatePang::setTypeUpdate(T_UPDATE_PANG _type_update) {
	m_type_update = _type_update;
}
