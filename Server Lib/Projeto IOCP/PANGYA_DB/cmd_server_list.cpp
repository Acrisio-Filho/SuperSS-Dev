// Arquivo cmd_server_list.cpp
// Criado em 17/03/2018 as 22:56 por Acrisio
// Implementação da classe CmdServerList

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_server_list.hpp"

using namespace stdA;

CmdServerList::CmdServerList(bool _waiter) : pangya_db(_waiter), m_type(GAME) {
};

CmdServerList::CmdServerList(TYPE_SERVER _type, bool _waiter) : pangya_db(_waiter), m_type(_type) {
};

CmdServerList::~CmdServerList() {
};

void CmdServerList::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(13, (uint32_t)_result->cols);
    
    ServerInfo si = { 0 };

	if (is_valid_c_string(_result->data[0]))
		STRCPY_TO_MEMORY_FIXED_SIZE(si.nome, sizeof(si.nome), _result->data[0]);
	si.uid = IFNULL(atoi, _result->data[1]);
	if (is_valid_c_string(_result->data[2]))
		STRCPY_TO_MEMORY_FIXED_SIZE(si.ip, sizeof(si.ip), _result->data[2]);
	si.port = IFNULL(atoi, _result->data[3]);
	si.max_user = IFNULL(atoi, _result->data[4]);
	si.curr_user = IFNULL(atoi, _result->data[5]);
	si.propriedade.ulProperty = IFNULL(atoi, _result->data[6]);
	si.angelic_wings_num = IFNULL(atoi, _result->data[7]);
	si.event_flag.usEventFlag = (short)IFNULL(atoi, _result->data[8]);
	si.event_map = (short)IFNULL(atoi, _result->data[9]);
	si.img_no = (short)IFNULL(atoi, _result->data[10]);
	si.app_rate = (short)IFNULL(atoi, _result->data[11]);
	si.unknown = (short)IFNULL(atoi, _result->data[12]);	// Estava o rate_scratchy mas realoquei ele para o ServerInfoEx::Rate

	v_server_list.push_back(si);
};

response* CmdServerList::prepareConsulta(database& _db) {

    v_server_list.clear();
    v_server_list.shrink_to_fit();

    auto r = procedure(_db, m_szConsulta, std::to_string(m_type));

    checkResponse(r, "nao conseguiu pegar o server list");

    return r;
};

CmdServerList::TYPE_SERVER CmdServerList::getType() {
    return m_type;
};

void CmdServerList::setType(TYPE_SERVER _type) {
    m_type = _type;
};

std::vector< ServerInfo >& CmdServerList::getServerList() {
    return v_server_list;
};
