// Arquivo cmd_insert_ticket_report_data.cpp
// Criado em 22/09/2018 as 17:24 por Acrisio
// Implementa��o da classe CmdInsertTicketReportData

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_insert_ticket_report_data.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdInsertTicketReportData::CmdInsertTicketReportData(bool _waiter) : pangya_db(_waiter), m_id(-1), m_trd{0} {
}

CmdInsertTicketReportData::CmdInsertTicketReportData(int32_t _id, TicketReportInfo::stTicketReportDados& _trd, bool _waiter)
	: pangya_db(_waiter), m_id(_id), m_trd(_trd) {
}

CmdInsertTicketReportData::~CmdInsertTicketReportData() {
}

void CmdInsertTicketReportData::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdInsertTicketReportData::prepareConsulta(database& _db) {

	if (m_id < 0)
		throw exception("[CmdInsertTicketReportData::prepareConsulta][Error] m_id is invalid[VALUE=" + std::to_string(m_id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string finish_date = "null";

	if (SystemTimeToUnix(m_trd.finish_time) > 0)
		finish_date =  _db.makeText(_formatDate(m_trd.finish_time));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_id) + ", " + std::to_string(m_trd.uid) + ", " + std::to_string(m_trd.score) 
			+ ", " + std::to_string((unsigned short)m_trd.medal.ucMedal) + ", " + std::to_string((unsigned short)m_trd.trofel) + ", " + std::to_string(m_trd.pang) 
			+ ", " + std::to_string(m_trd.bonus_pang) + ", " + std::to_string(m_trd.exp) + ", " + std::to_string(m_trd.mascot_typeid) 
			+ ", " + std::to_string((unsigned short)m_trd.flag_item_pang) + ", " + std::to_string((unsigned short)m_trd.premium) 
			+ ", " + std::to_string(m_trd.state) + ", " + finish_date
	);

	checkResponse(r, "nao conseguiu inserir Ticket Report[ID=" + std::to_string(m_id) + "] Dados[UID=" + std::to_string(m_trd.uid) + ", SCORE=" + std::to_string(m_trd.score) + ", MEDAL=" 
			+ std::to_string((unsigned short)m_trd.medal.ucMedal) + ", TROFEL=" + std::to_string((unsigned short)m_trd.trofel) + ", PANG=" + std::to_string(m_trd.pang) + ", BONUS_PANG=" 
			+ std::to_string(m_trd.bonus_pang) + ", EXP=" + std::to_string(m_trd.exp) + ", MASCOT=" + std::to_string(m_trd.mascot_typeid) + ", BOOST_PANG=" 
			+ std::to_string((unsigned short)m_trd.flag_item_pang) + ", PREMIUM=" + std::to_string((unsigned short)m_trd.premium) + ", STATE=" + std::to_string(m_trd.state) + ", FINISH_DATE=" + finish_date + "]");

	return r;
}

int32_t CmdInsertTicketReportData::getId() {
	return m_id;
}

void CmdInsertTicketReportData::setId(int32_t _id) {
	m_id = _id;
}

TicketReportInfo::stTicketReportDados& CmdInsertTicketReportData::getInfo() {
	return m_trd;
}

void CmdInsertTicketReportData::setInfo(TicketReportInfo::stTicketReportDados& _trd) {
	m_trd = _trd;
}
