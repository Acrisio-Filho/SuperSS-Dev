// Arquivo cmd_verify_id.cpp
// Criado em 17/03/2018 as 20:15 por Acrisio
// Implementação da classe CmdVerifyID

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_verify_id.hpp"

using namespace stdA;

CmdVerifyID::CmdVerifyID(bool _waiter) : pangya_db(_waiter), m_id(""), m_uid(0) {
};

CmdVerifyID::CmdVerifyID(std::string _id, bool _waiter) : pangya_db(_waiter), m_id(_id), m_uid(0) {
};

CmdVerifyID::~CmdVerifyID() {
};

void CmdVerifyID::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	// Aqui faz as coisas
	m_uid = IFNULL(atoi, _result->data[0]);
	std::string id_req((is_valid_c_string(_result->data[1])) ? _result->data[1] : "");
	// Fim

	if (m_id.compare(id_req) != 0)
		throw exception("[CmdVerifyID::lineResult][Error] ID do player info nao e igual ao requisitado. ID Req: " + m_id + " != " + id_req, STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdVerifyID::prepareConsulta(database& _db) {

	m_uid = 0u;

	auto r = procedure(_db, m_szConsulta, _db.makeText(m_id));

	checkResponse(r, "nao conseguiu pegar o uid do player pelo ID: " + m_id);

	return r;
};

std::string& CmdVerifyID::getID() {
    return m_id;
};

void CmdVerifyID::setID(std::string _id) {
    m_id = _id;
};

uint32_t CmdVerifyID::getUID() {
    return m_uid;
};
