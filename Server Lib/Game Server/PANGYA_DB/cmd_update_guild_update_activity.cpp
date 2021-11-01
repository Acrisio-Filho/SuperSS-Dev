// Arquivo cmd_update_guild_update_activity.cpp
// Criado em 30/11/2019 as 17:34 por Acrisio
// Implementa��o da classe CmdUpdateGuildUpdateActivity

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_guild_update_activity.hpp"

using namespace stdA;

CmdUpdateGuildUpdateActiviy::CmdUpdateGuildUpdateActiviy(bool _waiter) : pangya_db(_waiter), m_index(0ull) {
}

CmdUpdateGuildUpdateActiviy::CmdUpdateGuildUpdateActiviy(uint64_t _index, bool _waiter)
	: pangya_db(_waiter), m_index(_index) {
}

CmdUpdateGuildUpdateActiviy::~CmdUpdateGuildUpdateActiviy() {
}

void CmdUpdateGuildUpdateActiviy::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateGuildUpdateActiviy::prepareConsulta(database& _db) {
	
	if (m_index == 0ull)
		throw exception("[CmdUpdateGuildUpdateActivity::prepareConsulta][Error] m_index is invalid(zero).", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, m_szConsulta + std::to_string(m_index));

	checkResponse(r, "nao conseguiu atualizar o guild update activity[INDEX=" + std::to_string(m_index) + "]");

	return r;
}

uint64_t CmdUpdateGuildUpdateActiviy::getIndex() {
	return m_index;
}

void CmdUpdateGuildUpdateActiviy::setIndex(uint64_t _index) {
	m_index = _index;
}
