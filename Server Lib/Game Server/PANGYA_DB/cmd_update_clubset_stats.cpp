// Arquivo cmd_update_clubset_stats.cpp
// Criado em 16/06/2018 as 22:02 por Acrisio
// Implementa��o da classe CmdUpdateClubSetStats

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_clubset_stats.hpp"

using namespace stdA;

CmdUpdateClubSetStats::CmdUpdateClubSetStats(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pang(0ull), m_wi{0} {
}

CmdUpdateClubSetStats::CmdUpdateClubSetStats(uint32_t _uid, WarehouseItemEx& _wi, uint64_t _pang, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pang(_pang), m_wi(_wi) {
}

CmdUpdateClubSetStats::~CmdUpdateClubSetStats() {
}

void CmdUpdateClubSetStats::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {
	
	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateClubSetStats::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateClubSetStats][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi.id <= 0 || m_wi._typeid == 0)
		throw exception("[CmdUpdateClubSetStats][Error] WarehouseItem[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_pang)
			+ ", " + std::to_string(m_wi.c[CharacterInfo::S_POWER]) + ", " + std::to_string(m_wi.c[CharacterInfo::S_CONTROL])
			+ ", " + std::to_string(m_wi.c[CharacterInfo::S_ACCURACY]) + ", " + std::to_string(m_wi.c[CharacterInfo::S_SPIN])
			+ ", " + std::to_string(m_wi.c[CharacterInfo::S_CURVE])
	);

	checkResponse(r, "nao conseguiu Atualizar ClubSet[ID=" + std::to_string(m_wi.id) + "] Stats[C0=" + std::to_string(m_wi.c[CharacterInfo::S_POWER]) + ", C1=" 
			+ std::to_string(m_wi.c[CharacterInfo::S_CONTROL]) + ", C2=" + std::to_string(m_wi.c[CharacterInfo::S_ACCURACY]) + ", C3=" + std::to_string(m_wi.c[CharacterInfo::S_SPIN]) + ", C4=" 
			+ std::to_string(m_wi.c[CharacterInfo::S_CURVE]) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateClubSetStats::getUID() {
	return m_uid;
}

void CmdUpdateClubSetStats::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdUpdateClubSetStats::getPang() {
	return m_pang;
}

void CmdUpdateClubSetStats::setPang(uint64_t _pang) {
	m_pang = _pang;
}

WarehouseItemEx& CmdUpdateClubSetStats::getInfo() {
	return m_wi;
}

void CmdUpdateClubSetStats::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
