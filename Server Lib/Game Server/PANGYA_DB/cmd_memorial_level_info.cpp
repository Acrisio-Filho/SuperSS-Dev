// Arquivo cmd_memorial_level_info.cpp
// Criado em 22/07/2018 as 12:13 por Acrisio
// Implementa��o da classe CmdMemorialLevelInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_memorial_level_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdMemorialLevelInfo::CmdMemorialLevelInfo(bool _waiter) : pangya_db(_waiter), m_level() {
}

CmdMemorialLevelInfo::~CmdMemorialLevelInfo() {
}

void CmdMemorialLevelInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	ctx_memorial_level ml{ 0 };

	ml.level = (uint32_t)IFNULL(atoi, _result->data[0]);

	auto it = m_level.find(ml.level);

	if (it == m_level.end()) {	// N�o tem esse memorial level no map, add ele
		ml.gacha_number = (uint32_t)IFNULL(atoi, _result->data[1]);

		m_level[ml.level] = ml;

	}else // J� tem esse memorial level, no map
		_smp::message_pool::getInstance().push(new message("[CmdMemorialLevelInfo::lineResult][WARNING] ja tem esse memorial level[value=" + std::to_string(ml.level) + "] no map. Bug", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdMemorialLevelInfo::prepareConsulta(database& _db) {

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar Memorial Level Info");

	return r;
}

std::map< uint32_t, ctx_memorial_level >& CmdMemorialLevelInfo::getInfo() {
	return m_level;
}
