// Arquivo cmd_insert_spinning_cube_super_rare_win_broadcast.cpp
// Criado em 18/07/2018 as 22:44 por Acrisio
// Implementa��o da classe CmdInsertSpinningCubeSuperRareWinBroadcast

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_spinning_cube_super_rare_win_broadcast.hpp"

using namespace stdA;

CmdInsertSpinningCubeSuperRareWinBroadcast::CmdInsertSpinningCubeSuperRareWinBroadcast(bool _waiter) : pangya_db(_waiter), m_message(""), m_opt(0u) {
}

CmdInsertSpinningCubeSuperRareWinBroadcast::CmdInsertSpinningCubeSuperRareWinBroadcast(std::string& _message, unsigned char _opt, bool _waiter)
	: pangya_db(_waiter), m_message(_message), m_opt(_opt) {
}

CmdInsertSpinningCubeSuperRareWinBroadcast::~CmdInsertSpinningCubeSuperRareWinBroadcast() {
}

void CmdInsertSpinningCubeSuperRareWinBroadcast::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {


	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertSpinningCubeSuperRareWinBroadcast::prepareConsulta(database& _db) {

	if (m_message.empty())
		throw exception("[CmdInsertSpinningCubeSuperRareWinBroadcast::prepareConsulta][Error] m_message is invalid(empty)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, _db.makeText(m_message) + ", " + std::to_string((unsigned short)m_opt));

	checkResponse(r, "nao conseguiu inserir Spinning Cube Super Rare Win Broadcast[MSG=" + m_message +", OPT=" + std::to_string((unsigned short)m_opt) + "]");

	return r;
}

std::string& CmdInsertSpinningCubeSuperRareWinBroadcast::getMessage() {
	return m_message;
}

void CmdInsertSpinningCubeSuperRareWinBroadcast::setMessage(std::string& _message) {
	m_message = _message;
}

unsigned char CmdInsertSpinningCubeSuperRareWinBroadcast::getOpt() {
	return m_opt;
}

void CmdInsertSpinningCubeSuperRareWinBroadcast::setOpt(unsigned char _opt) {
	m_opt = _opt;
}
