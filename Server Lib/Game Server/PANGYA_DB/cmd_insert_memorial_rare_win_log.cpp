// Arquivo cmd_insert_memorial_rare_win_log.cpp
// Criado em 22/07/2018 as 14:59 por Acrisio
// Implementa��o da classe CmdInsertMemorialRareWinLog

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_memorial_rare_win_log.hpp"

using namespace stdA;

CmdInsertMemorialRareWinLog::CmdInsertMemorialRareWinLog(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_coin_typeid(0u), m_ci{0} {
}

CmdInsertMemorialRareWinLog::CmdInsertMemorialRareWinLog(uint32_t _uid, uint32_t _coin_typeid, ctx_coin_item_ex& _ci, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_coin_typeid(_coin_typeid), m_ci(_ci) {
}

CmdInsertMemorialRareWinLog::~CmdInsertMemorialRareWinLog() {
}

void CmdInsertMemorialRareWinLog::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertMemorialRareWinLog::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdInsertMemorialRareWinLog::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_coin_typeid == 0)
		throw exception("[CmdInsertMemorialRareWinLog::prepareConsulta][Error] m_coin_typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ci._typeid == 0)
		throw exception("[CmdInsertMemorialRareWinLog::prepareConsulta][Error] m_ci._typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_coin_typeid) + ", " + std::to_string(m_ci._typeid)
			+ ", " + std::to_string(m_ci.qntd) + ", " + std::to_string(m_ci.tipo) + ", " + std::to_string(m_ci.probabilidade)
	);

	checkResponse(r, "nao conseguiu inserir um Memorial Shop[COIN=" + std::to_string(m_coin_typeid) + "] Rare Win[TYPEID=" + std::to_string(m_ci._typeid) + ", QNTD=" 
			+ std::to_string(m_ci.qntd) + ", RARIDADE=" + std::to_string(m_ci.tipo) + "] Log para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdInsertMemorialRareWinLog::getUID() {
	return m_uid;
}

void CmdInsertMemorialRareWinLog::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdInsertMemorialRareWinLog::getCoinTypeid() {
	return m_coin_typeid;
}

void CmdInsertMemorialRareWinLog::setCoinTypeid(uint32_t _coin_typeid) {
	m_coin_typeid = _coin_typeid;
}

ctx_coin_item_ex& CmdInsertMemorialRareWinLog::getInfo() {
	return m_ci;
}

void CmdInsertMemorialRareWinLog::setInfo(ctx_coin_item_ex& _ci) {
	m_ci = _ci;
}
