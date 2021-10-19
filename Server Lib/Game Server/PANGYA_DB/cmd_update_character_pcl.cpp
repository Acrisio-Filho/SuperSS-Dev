// Arquivo cmd_update_character_pcl.cpp
// Criado em 16/06/2018 as 19:27 por Acrisio
// Implementa��o da classe CmdUpdateCharacterPCL

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_character_pcl.hpp"

using namespace stdA;

CmdUpdateCharacterPCL::CmdUpdateCharacterPCL(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ci{0} {
}

CmdUpdateCharacterPCL::CmdUpdateCharacterPCL(uint32_t _uid, CharacterInfo& _ci, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ci(_ci) {
}

CmdUpdateCharacterPCL::~CmdUpdateCharacterPCL() {
}

void CmdUpdateCharacterPCL::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCharacterPCL::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateCharacterPCL::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ci.id <= 0 || m_ci._typeid == 0)
		throw exception("[CmdUpdateCharacterPCL::prepareConsulta][Error] CharacterInfo[TYPEID=" + std::to_string(m_ci._typeid) + ", ID=" + std::to_string(m_ci.id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id) + ", " + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_POWER]) 
			+ ", " + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_CONTROL]) + ", " + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_ACCURACY]) 
			+ ", " + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_SPIN]) + ", " + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_CURVE])
	);

	checkResponse(r, "nao conseguiu atualizar o Character[TYPEID=" + std::to_string(m_ci._typeid) + ", ID=" + std::to_string(m_ci.id) + "] PCL[c0=" 
			+ std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_POWER]) + ", c1=" + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_CONTROL]) + ", c2=" 
			+ std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_ACCURACY]) + ", c3=" + std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_SPIN]) + ", c4=" 
			+ std::to_string((unsigned short)m_ci.pcl[CharacterInfo::S_CURVE]) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateCharacterPCL::getUID() {
	return m_uid;
}

void CmdUpdateCharacterPCL::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CharacterInfo& CmdUpdateCharacterPCL::getInfo() {
	return m_ci;
}

void CmdUpdateCharacterPCL::setInfo(CharacterInfo& _ci) {
	m_ci = _ci;
}
