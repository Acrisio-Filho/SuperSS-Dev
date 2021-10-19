// Arquivo cmd_update_clubset_workshop.cpp
// Criado em 23/06/2018 as 18:47 por Acrisio
// Implementa��o da classe CmdUpdateClubSetWorkshop

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_clubset_workshop.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdUpdateClubSetWorkshop::CmdUpdateClubSetWorkshop(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_flag(FLAG::F_TRANSFER_MASTERY_PTS), m_wi{0} {
}

CmdUpdateClubSetWorkshop::CmdUpdateClubSetWorkshop(uint32_t _uid, WarehouseItemEx& _wi, FLAG _flag, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_flag(_flag), m_wi(_wi) {
}

CmdUpdateClubSetWorkshop::~CmdUpdateClubSetWorkshop() {
}

void CmdUpdateClubSetWorkshop::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateClubSetWorkshop::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateClubSetWorkShop::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi.id <= 0 || m_wi._typeid == 0)
		throw exception("[CmdUpdateClubSetWorkShop::prepareConsulta][Error] WarehouseItem(ClubSet)[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (sIff::getInstance().getItemGroupIdentify(m_wi._typeid) != iff::CLUBSET)
		throw exception("[CmdUpdateClubSetWorkShop::prepareConsulta][Error] Item[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) + "] nao eh um ClubSet", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_wi.clubset_workshop.level)
			+ ", " + std::to_string(m_wi.clubset_workshop.c[0]) + ", " + std::to_string(m_wi.clubset_workshop.c[1]) + ", " + std::to_string(m_wi.clubset_workshop.c[2])
			+ ", " + std::to_string(m_wi.clubset_workshop.c[3]) + ", " + std::to_string(m_wi.clubset_workshop.c[4]) + ", " + std::to_string(m_wi.clubset_workshop.mastery)
			+ ", " + std::to_string(m_wi.clubset_workshop.rank) + ", " + std::to_string(m_wi.clubset_workshop.recovery_pts) + ", " + std::to_string(m_flag)
	);

	checkResponse(r, "nao conseguiu atualizar ClubSet[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) + "] WorkShop[C0=" + std::to_string(m_wi.clubset_workshop.c[0]) 
			+ ", C1=" + std::to_string(m_wi.clubset_workshop.c[1]) + ", C2=" + std::to_string(m_wi.clubset_workshop.c[2]) + ", C3=" + std::to_string(m_wi.clubset_workshop.c[3]) + ", C4=" 
			+ std::to_string(m_wi.clubset_workshop.c[4]) + ", Level=" + std::to_string(m_wi.clubset_workshop.level) + ", Mastery=" + std::to_string(m_wi.clubset_workshop.mastery) + ", Rank=" 
			+ std::to_string(m_wi.clubset_workshop.rank) + ", Recovery=" + std::to_string(m_wi.clubset_workshop.recovery_pts) + "] Flag=" + std::to_string(m_wi.clubset_workshop.flag) + " do player[UID=" + std::to_string(m_uid) + "]");
	
	return r;
}

uint32_t CmdUpdateClubSetWorkshop::getUID() {
	return m_uid;
}

void CmdUpdateClubSetWorkshop::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdUpdateClubSetWorkshop::FLAG CmdUpdateClubSetWorkshop::getFlag() {
	return m_flag;
}

void CmdUpdateClubSetWorkshop::setFlag(FLAG _flag) {
	m_flag = _flag;
}

WarehouseItemEx& CmdUpdateClubSetWorkshop::getInfo() {
	return m_wi;
}

void CmdUpdateClubSetWorkshop::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
