// Arquivo cmd_get_gift_clubset.cpp
// Criado em 31/05/2018 as 20:19 por Acrisio
// Implementa��o da classe CmdGetGiftClubSet

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_get_gift_clubset.hpp"

using namespace stdA;

CmdGetGiftClubSet::CmdGetGiftClubSet(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_wi{0} {
}

CmdGetGiftClubSet::CmdGetGiftClubSet(uint32_t _uid, WarehouseItemEx& _wi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_wi(_wi) {
}

CmdGetGiftClubSet::~CmdGetGiftClubSet() {
}

void CmdGetGiftClubSet::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(45, (uint32_t)_result->cols);

	m_wi.id = IFNULL(atoi, _result->data[0]);
	
	if (m_wi.id > 0) { // Found

		auto i = 0;

		m_wi.id = IFNULL(atoi, _result->data[0]);
		m_wi._typeid = IFNULL(atoi, _result->data[2]);
		m_wi.ano = IFNULL(atoi, _result->data[3]);
		for (i = 0; i < 5; i++)
			m_wi.c[i] = (short)IFNULL(atoi, _result->data[4 + i]);			// 4 + 5
		m_wi.purchase = (unsigned char)IFNULL(atoi, _result->data[9]);
		m_wi.flag = (unsigned char)IFNULL(atoi, _result->data[11]);
		//m_wi.apply_date = IFNULL(atoll, _result->data[12]);
		//m_wi.end_date = IFNULL(atoll, _result->data[13]);
		
		// Salve local unix date on WarehouseItemEx and System Unix Date on apply_date to send to client
		m_wi.apply_date_unix_local = IFNULL(atoi, _result->data[12]);
		m_wi.end_date_unix_local = IFNULL(atoi, _result->data[13]);

		// Date 
		if (m_wi.apply_date_unix_local > 0)
			m_wi.apply_date = TzLocalUnixToUnixUTC(m_wi.apply_date_unix_local);

		if (m_wi.end_date_unix_local > 0)
			m_wi.end_date = TzLocalUnixToUnixUTC(m_wi.end_date_unix_local);
		
		m_wi.type = (unsigned char)IFNULL(atoi, _result->data[14]);
		for (i = 0; i < 4; i++)
			m_wi.card.character[i] = IFNULL(atoi, _result->data[15 + i]);	// 15 + 4
		for (i = 0; i < 4; i++)
			m_wi.card.caddie[i] = IFNULL(atoi, _result->data[19 + i]);		// 19 + 4
		for (i = 0; i < 4; i++)
			m_wi.card.NPC[i] = IFNULL(atoi, _result->data[23 + i]);			// 23 + 4
		m_wi.clubset_workshop.flag = (short)IFNULL(atoi, _result->data[27]);
		for (i = 0; i < 5; i++)
			m_wi.clubset_workshop.c[i] = (short)IFNULL(atoi, _result->data[28 + i]);	// 28 + 5
		m_wi.clubset_workshop.mastery = IFNULL(atoi, _result->data[33]);
		m_wi.clubset_workshop.recovery_pts = IFNULL(atoi, _result->data[34]);
		m_wi.clubset_workshop.level = IFNULL(atoi, _result->data[35]);
		m_wi.clubset_workshop.rank = IFNULL(atoi, _result->data[36]);
		if (is_valid_c_string(_result->data[37]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_wi.ucc.name, sizeof(m_wi.ucc.name), _result->data[37]);
		if (is_valid_c_string(_result->data[38]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_wi.ucc.idx, sizeof(m_wi.ucc.idx), _result->data[38]);
		m_wi.ucc.seq = (unsigned short)IFNULL(atoi, _result->data[39]);
		if (is_valid_c_string(_result->data[40]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_wi.ucc.copier_nick, sizeof(m_wi.ucc.copier_nick), _result->data[40]);
		m_wi.ucc.copier = IFNULL(atoi, _result->data[41]);
		m_wi.ucc.trade = (unsigned char)IFNULL(atoi, _result->data[42]);
		//m_wi.sd_flag = (unsigned char)IFNULL(atoi, _result->data[43]);
		m_wi.ucc.status = (unsigned char)IFNULL(atoi, _result->data[44]);
	}
}

response* CmdGetGiftClubSet::prepareConsulta(database& _db) {

	if (m_wi.id <= 0 || m_wi._typeid == 0)
		throw exception("[CmdGetGiftClubSet::prepareConsulta][Error] ClubSet[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) + "] is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_wi._typeid));

	checkResponse(r, "nao conseguiu pegar o presente de ClubSet[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) +"] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdGetGiftClubSet::getUID() {
	return m_uid;
}

void CmdGetGiftClubSet::setUID(uint32_t _uid) {
	m_uid = _uid;
}

WarehouseItemEx& CmdGetGiftClubSet::getInfo() {
	return m_wi;
}

void CmdGetGiftClubSet::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
