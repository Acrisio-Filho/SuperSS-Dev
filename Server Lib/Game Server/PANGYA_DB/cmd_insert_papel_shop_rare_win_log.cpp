// Arquivo cmd_insert_papel_shop_rare_win_log.cpp
// Criado em 09/07/2018 as 22:13 por Acrisio
// Implementa��o da classe CmdInsertPapelShopRareWinLog

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_papel_shop_rare_win_log.hpp"

using namespace stdA;

CmdInsertPapelShopRareWinLog::CmdInsertPapelShopRareWinLog(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ctx_psb{0} {
}

CmdInsertPapelShopRareWinLog::CmdInsertPapelShopRareWinLog(uint32_t _uid, ctx_papel_shop_ball& _ctx_psb, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ctx_psb(_ctx_psb) {
}

CmdInsertPapelShopRareWinLog::~CmdInsertPapelShopRareWinLog() {
}

void CmdInsertPapelShopRareWinLog::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertPapelShopRareWinLog::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdInsertPapelShopRareWinLog::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ctx_psb.ctx_psi._typeid == 0)
		throw exception("[CmdInsertPapelShopRareWinLog::prepareConsulta][Error] m_ctx_psb.ctx_psi._typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ctx_psb.ctx_psi._typeid) + ", " + std::to_string(m_ctx_psb.qntd)
			+ ", " + std::to_string(m_ctx_psb.color) + ", " + std::to_string(m_ctx_psb.ctx_psi.probabilidade)
	);

	checkResponse(r, "nao conseguiu adicionar o Log de Rare Win[TYPEID=" + std::to_string(m_ctx_psb.ctx_psi._typeid) + ", QNTD=" 
			+ std::to_string(m_ctx_psb.qntd) + ", COLOR=" + std::to_string(m_ctx_psb.color) + ", PROBABILIDADE=" + std::to_string(m_ctx_psb.ctx_psi.probabilidade) + "] do Papel Shop para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdInsertPapelShopRareWinLog::getUID() {
	return m_uid;
}

void CmdInsertPapelShopRareWinLog::setUID(uint32_t _uid) {
	m_uid = _uid;
}

ctx_papel_shop_ball& CmdInsertPapelShopRareWinLog::getInfo() {
	return m_ctx_psb;
}

void CmdInsertPapelShopRareWinLog::setInfo(ctx_papel_shop_ball& _ctx_psb) {
	m_ctx_psb = _ctx_psb;
}
