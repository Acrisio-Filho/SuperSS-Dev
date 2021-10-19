// Arquivo cmd_insert_box_rare_win_log.cpp
// Criado em 18/07/2018 as 22:12 por Acrisio
// Implementa��o da classe CmdInsertBoxRareWinLog

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_box_rare_win_log.hpp"

using namespace stdA;

CmdInsertBoxRareWinLog::CmdInsertBoxRareWinLog(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_box_typeid(0u), m_ctx_bi{0} {
}

CmdInsertBoxRareWinLog::CmdInsertBoxRareWinLog(uint32_t _uid, uint32_t _box_typeid, ctx_box_item& _ctx_bi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_box_typeid(_box_typeid), m_ctx_bi(_ctx_bi) {
}

CmdInsertBoxRareWinLog::~CmdInsertBoxRareWinLog() {
}

void CmdInsertBoxRareWinLog::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {


	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertBoxRareWinLog::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdInsertBoxRareWinLog::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_box_typeid == 0)
		throw exception("[CmdInsertBoxRareWinLog::prepareConsulta][Error] m_box_typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ctx_bi._typeid == 0)
		throw exception("[CmdInsertBoxRareWinLog::prepareConsulta][Error] m_ctx_bi._typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_box_typeid) + ", " + std::to_string(m_ctx_bi._typeid) 
			+ ", " + std::to_string(m_ctx_bi.qntd) + ", " + std::to_string((unsigned short)m_ctx_bi.raridade)
	);

	checkResponse(r, "nao conseguiu inserir o box[TYPEID=" + std::to_string(m_box_typeid) + "] rare[TYPEID=" + std::to_string(m_ctx_bi._typeid) + ", QNTD=" 
			+ std::to_string(m_ctx_bi.qntd) + ", RARIDADE=" + std::to_string((unsigned short)m_ctx_bi.raridade) + "] win log para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdInsertBoxRareWinLog::getUID() {
	return m_uid;
}

void CmdInsertBoxRareWinLog::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdInsertBoxRareWinLog::getBoxTypeid() {
	return m_box_typeid;
}

void CmdInsertBoxRareWinLog::setBoxTypeid(uint32_t _box_typeid) {
	m_box_typeid = _box_typeid;
}

ctx_box_item& CmdInsertBoxRareWinLog::getInfo() {
	return m_ctx_bi;
}

void CmdInsertBoxRareWinLog::setInfo(ctx_box_item& _ctx_bi) {
	m_ctx_bi = _ctx_bi;
}
