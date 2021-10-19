// Arquivo cmd_golden_time_info.cpp
// Criado em 24/10/2020 as 02:22 por Acrisio
// Implementa��o da classe CmdGoldenTimeInfo

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_golden_time_info.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdGoldenTimeInfo::CmdGoldenTimeInfo(bool _waiter) : pangya_db(_waiter), m_gt() {
}

CmdGoldenTimeInfo::~CmdGoldenTimeInfo() {

	if (!m_gt.empty()) {
		m_gt.clear();
		m_gt.shrink_to_fit();
	}

}

void CmdGoldenTimeInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(6, (uint32_t)_result->cols);

	stGoldenTime gt{ 0u };

	gt.id = (uint32_t)IFNULL(atoi, _result->data[0]);
	gt.type = stGoldenTime::eTYPE((unsigned char)IFNULL(atoi, _result->data[1]));

	if (_result->data[2] != nullptr)
		_translateDate(_result->data[2], &gt.date[0]);

	if (_result->data[3] != nullptr)
		_translateDate(_result->data[3], &gt.date[1]);

	gt.rate_of_players = (uint32_t)IFNULL(atoi, _result->data[4]);
	gt.is_end = IFNULL(atoi, _result->data[5]) == 1 ? true : false;

	m_gt.push_back(gt);
}

response* CmdGoldenTimeInfo::prepareConsulta(database& _db) {
	
	if (!m_gt.empty())
		m_gt.clear();

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao conseguiu pegar o Golden Time Info");

	return r;
}

std::vector< stGoldenTime >& CmdGoldenTimeInfo::getInfo() {
	return m_gt;
}
