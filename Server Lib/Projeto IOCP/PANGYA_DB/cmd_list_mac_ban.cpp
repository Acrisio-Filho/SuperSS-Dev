// Arquivo cmd_list_mac_ban.cpp
// Criado em 22/08/2019 as 10:20 por Acrisio
// Implementação da classe CmdListMacBan

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_list_mac_ban.hpp"

using namespace stdA;

CmdListMacBan::CmdListMacBan(bool _waiter) : pangya_db(_waiter) {
}

CmdListMacBan::~CmdListMacBan() {
}

void CmdListMacBan::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	if (is_valid_c_string(_result->data[0]))
		v_list_mac_ban.push_back(_result->data[0]);
}

response* CmdListMacBan::prepareConsulta(database& _db) {
	
	v_list_mac_ban.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu recuberar a lista de MAC Address");

	return r;
}

std::vector< std::string >& CmdListMacBan::getList() {
	return v_list_mac_ban;
}
