// Arquivo cmd_command_info.cpp
// Criado em 02/12/2018 as 22:44 por Acrisio
// Implementa��o da classe CmdCommandInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_command_info.hpp"

using namespace stdA;

CmdCommandInfo::CmdCommandInfo(bool _waiter) : pangya_db(_waiter) {
}

CmdCommandInfo::~CmdCommandInfo() {
}

void CmdCommandInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(11, (uint32_t)_result->cols);

	CommandInfo ci{ 0 };

	ci.idx = (uint32_t)IFNULL(atoi, _result->data[0]);
	ci.id = (uint32_t)IFNULL(atoi, _result->data[1]);

	for (auto i = 0u; i < 5u; ++i)
		ci.arg[i] = (uint32_t)IFNULL(atoi, _result->data[i + 2]);	// 2 + 5

	ci.target = (uint32_t)IFNULL(atoi, _result->data[7]);
	ci.flag = (unsigned short)IFNULL(atoi, _result->data[8]);
	ci.valid = (unsigned char)IFNULL(atoi, _result->data[9]);
	ci.reserveDate = (time_t)IFNULL(atoi, _result->data[10]);

	v_ci.push_back(ci);
}

response* CmdCommandInfo::prepareConsulta(database& _db) {

	if (!v_ci.empty()) {
		v_ci.clear();
		v_ci.shrink_to_fit();
	}

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao conseguiu pegar os comandos do server no banco de dados");

	return r;
}

std::vector< CommandInfo >& CmdCommandInfo::getInfo() {
	return v_ci;
}
