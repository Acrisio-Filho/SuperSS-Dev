// Arquivo cmd_update_poster_equiped.cpp
// Criado em 25/03/2018 as 13:01 por Acrisio
// Implementa��o da classe CmdUpdatePosterEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_poster_equiped.hpp"

using namespace stdA;

CmdUpdatePosterEquiped::CmdUpdatePosterEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ue{0} {
}

CmdUpdatePosterEquiped::CmdUpdatePosterEquiped(uint32_t _uid, UserEquip& _ue, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_ue(_ue) {
}

CmdUpdatePosterEquiped::~CmdUpdatePosterEquiped() {
}

void CmdUpdatePosterEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa aqui por que � UPDATE
	return;
}

response* CmdUpdatePosterEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ue.poster[0]) + ", " + std::to_string(m_ue.poster[1]));

	checkResponse(r, "nao conseguiu atualizar o poster[ID=" + std::to_string(m_ue.poster[0]) + ", " + std::to_string(m_ue.poster[1]) + "] equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdatePosterEquiped::getUID() {
	return m_uid;
}

void CmdUpdatePosterEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

UserEquip& CmdUpdatePosterEquiped::getInfo() {
	return m_ue;
}

void CmdUpdatePosterEquiped::setInfo(UserEquip& _ue) {
	m_ue = _ue;
}
