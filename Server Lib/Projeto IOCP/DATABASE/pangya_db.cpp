// Arquivo pangya_db.cpp
// Criado em 05/12/2017 por Acrisio
// Implementação da classe pangya_base_db

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "pangya_db.h"

using namespace stdA;

list_fifo_asyc< exec_query > pangya_base_db::m_query_pool;
list_async< exec_query* > pangya_base_db::m_cache_query;

pangya_base_db::pangya_base_db() {};

pangya_base_db::~pangya_base_db() {}

void pangya_base_db::register_server(ServerInfoEx& si) {
	exec_query query("pangya.ProcRegServer_New", std::to_string(si.uid) + ", '" + std::string(si.nome) + "', '" +
		std::string(si.ip) + "', " + std::to_string(si.port) + ", " + std::to_string(si.tipo) + ", " +
		std::to_string(si.max_user) + ", " + std::to_string(si.curr_user) + ", " + std::to_string(si.rate_pang) + ", '" +
		std::string(si.version) + "', '" + std::string(si.version_client) + "', " + std::to_string(si.propriedade) + ", " +
		std::to_string(si.angelic_wings_num) + ", " + std::to_string(si.event_flag) + ", " + std::to_string(si.rate_exp) + ", " +
		std::to_string(si.img_no) + ", " + std::to_string(si.rate_scratchy) + ", " + std::to_string(si.rate_mastery) + ", " +
		std::to_string(si.rate_treasure) + ", " + std::to_string(si.rate_rare_papel) + ", " +
		std::to_string(si.rate_cookie_papel) + ", " + std::to_string(si.rate_chuva));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(1, 2);

	END_RESULT_READ("erro ao registrar o server", 2);
};

std::vector<ServerInfo> pangya_base_db::getServerList() {
	exec_query query("pangya.ProcGetServerList", "0");

	MED_RESULT_READ(&query);

	std::vector< ServerInfo > v_si;
	ServerInfo si;

	BEGIN_RESULT_READ(13, 49);

	si = { 0 };

	if (_result->data[0] != nullptr)
		strcpy_s(si.nome, _result->data[0]);
	si.uid = IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(si.ip, _result->data[2]);
	si.port = IFNULL(atoi, _result->data[3]);
	si.max_user = IFNULL(atoi, _result->data[4]);
	si.curr_user = IFNULL(atoi, _result->data[5]);
	si.propriedade = IFNULL(atoi, _result->data[6]);
	si.angelic_wings_num = IFNULL(atoi, _result->data[7]);
	si.event_flag = (short)IFNULL(atoi, _result->data[8]);
	si.event_map = (short)IFNULL(atoi, _result->data[9]);
	si.img_no = (short)IFNULL(atoi, _result->data[10]);
	si.app_rate = (short)IFNULL(atoi, _result->data[11]);
	si.rate_scratchy = (short)IFNULL(atoi, _result->data[12]);

	v_si.push_back(si);

	END_RESULT_READ("erro ao pegar a Lista dos servidores.", 49);

	return v_si;
};

bool pangya_base_db::compare(exec_query* _query1, exec_query* _query2) {
	return _query1->getQuery().compare(_query2->getQuery()) == 0;
};