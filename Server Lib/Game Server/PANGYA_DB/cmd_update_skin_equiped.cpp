// Arquivo cmd_update_skin_equiped.cpp
// Criado em 25/03/2018 as 12:45 por Acrisio
// Implementa��o da classe CmdUpdateSkinEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_skin_equiped.hpp"

using namespace stdA;

CmdUpdateSkinEquiped::CmdUpdateSkinEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ue{0} {
}

CmdUpdateSkinEquiped::CmdUpdateSkinEquiped(uint32_t _uid, UserEquip& _ue, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_ue(_ue) {
}

CmdUpdateSkinEquiped::~CmdUpdateSkinEquiped() {
}

void CmdUpdateSkinEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usar aqui por que � UPDATE
	return;
}

response* CmdUpdateSkinEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ue.skin_typeid[0])
					+ ", " + std::to_string(m_ue.skin_typeid[1]) + ", " + std::to_string(m_ue.skin_typeid[2])
					+ ", " + std::to_string(m_ue.skin_typeid[3]) + ", " + std::to_string(m_ue.skin_typeid[4])
					+ ", " + std::to_string(m_ue.skin_typeid[5]));

	checkResponse(r, "nao conseguiu atualizar o skin equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateSkinEquiped::getUID() {
	return m_uid;
}

void CmdUpdateSkinEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

UserEquip& CmdUpdateSkinEquiped::getInfo() {
	return m_ue;
}

void CmdUpdateSkinEquiped::setInfo(UserEquip& _ue) {
	m_ue = _ue;
}
