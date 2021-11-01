// Arquivo cmd_coin_cube_info.cpp
// Criado em 16/10/2020 as 18:54 por Acrisio
// Implementa��o da classe CmdCoinCubeInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_coin_cube_info.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdCoinCubeInfo::CmdCoinCubeInfo(bool _waiter) : pangya_db(_waiter), m_course_info() {
}

CmdCoinCubeInfo::~CmdCoinCubeInfo() {

	if (!m_course_info.empty())
		m_course_info.clear();
}

void CmdCoinCubeInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	unsigned char course_id = (unsigned char)IFNULL(atoi, _result->data[0]);
	bool active = IFNULL(atoi, _result->data[1]) == 1;

	auto it = m_course_info.find(course_id);

	if (it != m_course_info.end()) {

		// Log
		_smp::message_pool::getInstance().push(new message("[CmdCoinCubeInfo::lineResult][WARNING] ja tem o course[ID=" 
				+ std::to_string((unsigned short)course_id) + "] no map, ele esta[ACTIVE=" + std::to_string((unsigned short)m_course_info[course_id]) 
				+ "] vai ser trocado pelo[ACTIVE=" + std::to_string((unsigned short)active) + "].", CL_FILE_LOG_AND_CONSOLE));

		m_course_info[course_id] = active;

	}else {

		auto ret = m_course_info.insert(std::make_pair(course_id, active));

		if (!ret.second && ret.first == m_course_info.end())
			_smp::message_pool::getInstance().push(new message("[CmdCoinCubeInfo::lineResult][WARNING] nao conseguiu adicionar o course[ID=" 
					+ std::to_string((unsigned short)course_id) + ", ACTIVE=" + std::to_string((unsigned short)active) + "] no map<>.", CL_FILE_LOG_AND_CONSOLE));
	}
}

response* CmdCoinCubeInfo::prepareConsulta(database& _db) {
	
	if (!m_course_info.empty())
		m_course_info.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar coin cube info dos course");

	return r;
}

std::map< unsigned char, bool >& CmdCoinCubeInfo::getInfo() {
	return m_course_info;
}
