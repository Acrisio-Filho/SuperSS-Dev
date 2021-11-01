// Arquivo cmd_delete_quest.cpp
// Criado em 11/11/2018 as 13:27 por Acrisio
// Implementa��o da classe CmdDeleteQuest

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_quest.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

CmdDeleteQuest::CmdDeleteQuest(bool _waiter) : pangya_db(_waiter), m_uid(0u), v_id() {
}

CmdDeleteQuest::CmdDeleteQuest(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), v_id(_id) {
}

CmdDeleteQuest::CmdDeleteQuest(uint32_t _uid, std::vector< QuestStuffInfo >& _v_id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), v_id() {

	for (auto& el : _v_id)
		v_id.push_back(el.id);
}

CmdDeleteQuest::~CmdDeleteQuest() {
}

void CmdDeleteQuest::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um DELETE
	return;
}

response* CmdDeleteQuest::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdDeleteQuest::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (v_id.empty())
		throw exception("[CmdDeleteQuest::prepareConsulta][Error] v_quest_id is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 1));

	std::string str_ids = stdA::to_string< std::vector< int32_t >, std::string >(v_id);

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + str_ids + m_szConsulta[2]);

	checkResponse(r, "nao conseguiu deletar Quest[ID = { " + str_ids + " }] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteQuest::getUID() {
	return m_uid;
}

void CmdDeleteQuest::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteQuest::getId() {

	if (!v_id.empty())
		return *v_id.begin();

	return -1;
}

std::vector< int32_t >& CmdDeleteQuest::getIds() {
	return v_id;
}

void CmdDeleteQuest::setId(int32_t _id) {

	if (!v_id.empty())
		v_id.clear();

	v_id.push_back(_id);
}

void CmdDeleteQuest::setId(std::vector< QuestStuffInfo >& _v_id) {

	if (!v_id.empty())
		v_id.clear();

	for (auto& el : _v_id)
		v_id.push_back(el.id);
}
