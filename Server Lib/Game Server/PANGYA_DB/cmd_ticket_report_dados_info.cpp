// Arquivo cmd_ticket_report_dados_info.cpp
// Criado em 13/10/2018 as 13:21 por Acrisio
// Implementa��o da classe CmdTicketReportDadosInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_ticket_report_dados_info.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <memory.h>

using namespace stdA;

CmdTicketReportDadosInfo::CmdTicketReportDadosInfo(bool _waiter) : pangya_db(_waiter), m_ticket_report_id(-1), m_trsi{0} {
}

CmdTicketReportDadosInfo::CmdTicketReportDadosInfo(int32_t _ticket_report_id, bool _waiter)
	: pangya_db(_waiter), m_ticket_report_id(_ticket_report_id), m_trsi{0} {
}

CmdTicketReportDadosInfo::~CmdTicketReportDadosInfo() {
}

void CmdTicketReportDadosInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(22, (uint32_t)_result->cols);

	if (m_trsi.id < 0) {
		m_trsi.id = IFNULL(atoi, _result->data[0]);
		
		if (_result->data[3] != nullptr)
			_translateDate(_result->data[3], &m_trsi.date);
	}

	TicketReportScrollInfo::stPlayerDados pd{ 0 };

	pd.tipo = (uint32_t)IFNULL(atoi, _result->data[1]);
	pd.trofel_typeid = (uint32_t)IFNULL(atoi, _result->data[2]);
	pd.uid = (uint32_t)IFNULL(atoi, _result->data[4]);
	pd.score = (char)IFNULL(atoi, _result->data[5]);
	pd.medalha.ucMedal = (unsigned char)IFNULL(atoi, _result->data[6]);
	pd.trofel = (unsigned char)IFNULL(atoi, _result->data[7]);
	pd.pang = (uint64_t)IFNULL(atoll, _result->data[8]);
	pd.bonus_pang = (uint64_t)IFNULL(atoll, _result->data[9]);
	pd.exp = (uint32_t)IFNULL(atoi, _result->data[10]);
	pd.mascot_typeid = (uint32_t)IFNULL(atoi, _result->data[11]);
	pd.state = (unsigned char)IFNULL(atoi, _result->data[12]);
	pd.item_boost = (unsigned char)IFNULL(atoi, _result->data[13]);
	pd.premium_user = (unsigned char)IFNULL(atoi, _result->data[14]);
	pd.level = (uint32_t)IFNULL(atoi, _result->data[15]);
	if (is_valid_c_string(_result->data[16]))
		STRCPY_TO_MEMORY_FIXED_SIZE(pd.id, sizeof(pd.id), _result->data[16]);
	if (is_valid_c_string(_result->data[17]))
		STRCPY_TO_MEMORY_FIXED_SIZE(pd.nickname, sizeof(pd.nickname), _result->data[17]);
	pd.guild_uid = (uint32_t)IFNULL(atoi, _result->data[18]);
	if (is_valid_c_string(_result->data[19]))
		STRCPY_TO_MEMORY_FIXED_SIZE(pd.guild_mark_img, sizeof(pd.guild_mark_img), _result->data[19]);
	
	pd.mark_index = (uint32_t)IFNULL(atoi, _result->data[20]); // Guild Emblem Index � usado no JP

	if (_result->data[21] != nullptr)
		_translateDate(_result->data[21], &pd.finish_time);

	// Add ao vector do Ticket Report Scroll Info
	m_trsi.v_players.push_back(pd);

	if (m_trsi.id < 0)
		throw exception("[CmdTicketReportDadosInfo::lineResult][Error] m_trsi[request_id=" + std::to_string(m_ticket_report_id) + ", return_id=" 
				+ std::to_string(m_trsi.id) + "] is wrong not match.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

}

response* CmdTicketReportDadosInfo::prepareConsulta(database& _db) {

	if (m_ticket_report_id < 0)
		throw exception("[CmdTicketReportDadosInfo::prepareConsulta][Error] m_ticket_report_id[VALUE=" 
				+ std::to_string(m_ticket_report_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_trsi.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_ticket_report_id));

	checkResponse(r, "nao conseguiu pegar os dados do Ticket Report[ID=" + std::to_string(m_ticket_report_id) + "]");

	return r;
}

int32_t CmdTicketReportDadosInfo::getTicketReportId() {
	return m_ticket_report_id;
}

void CmdTicketReportDadosInfo::setTicketReportId(int32_t _ticket_report_id) {
	m_ticket_report_id = _ticket_report_id;
}

TicketReportScrollInfo& CmdTicketReportDadosInfo::getInfo() {
	return m_trsi;
}

void CmdTicketReportDadosInfo::setInfo(TicketReportScrollInfo& _trsi) {
	m_trsi = _trsi;
}
