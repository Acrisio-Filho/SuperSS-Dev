// Arquivo cmd_grand_zodiac_event_info.cpp
// Criado em 26/06/2020 as 13:37 por Acrisio
// Implementa��o da classe CmdGrandZodiacEventInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_grand_zodiac_event_info.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdGrandZodiacEventInfo::CmdGrandZodiacEventInfo(bool _waiter) : pangya_db(_waiter), m_rt() {
}

CmdGrandZodiacEventInfo::~CmdGrandZodiacEventInfo() {

	if (!m_rt.empty()) {
		m_rt.clear();
		m_rt.shrink_to_fit();
	}
}

void CmdGrandZodiacEventInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_reuslt*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	range_time rt{ 0u };

	if (_result->data[0] != nullptr)
		_translateTime(_result->data[0], &rt.m_start);

	if (_result->data[1] != nullptr)
		_translateTime(_result->data[1], &rt.m_end);

	rt.m_type = range_time::eTYPE_MAKE_ROOM((unsigned char)IFNULL(atoi, _result->data[2]));

	m_rt.push_back(rt);
}

response* CmdGrandZodiacEventInfo::prepareConsulta(database& _db) {
	
	if (!m_rt.empty())
		m_rt.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "Nao conseguiu pegar os tempo do Grand Zodiac Event");

	return r;
}

std::vector< range_time >& CmdGrandZodiacEventInfo::getInfo() {
	return m_rt;
}
