// Arquivo cmd_update_chat_macro_user.cpp
// Criado em 28/07/2018 as 20:43 por Acrisio
// Implementa��o da classe CmdUpdateChatMacroUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_chat_macro_user.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <string>

using namespace stdA;

CmdUpdateChatMacroUser::CmdUpdateChatMacroUser(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_cmu{0} {
}

CmdUpdateChatMacroUser::CmdUpdateChatMacroUser(uint32_t _uid, chat_macro_user& _cmu, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_cmu(_cmu) {
}

CmdUpdateChatMacroUser::~CmdUpdateChatMacroUser() {
}

void CmdUpdateChatMacroUser::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateChatMacroUser::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateChatMacroUser::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	// Verifiy  and Encode Characters of not display of Encode Standard of C
	auto m0 = verifyAndEncode(m_cmu.macro[0]);
	auto m1 = verifyAndEncode(m_cmu.macro[1]);
	auto m2 = verifyAndEncode(m_cmu.macro[2]);
	auto m3 = verifyAndEncode(m_cmu.macro[3]);
	auto m4 = verifyAndEncode(m_cmu.macro[4]);
	auto m5 = verifyAndEncode(m_cmu.macro[5]);
	auto m6 = verifyAndEncode(m_cmu.macro[6]);
	auto m7 = verifyAndEncode(m_cmu.macro[7]);
	auto m8 = verifyAndEncode(m_cmu.macro[8]);

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m0) + ", " + _db.makeText(m1) + ", " + _db.makeText(m2) + ", " 
				+ _db.makeText(m3) + ", " + _db.makeText(m4) + ", " + _db.makeText(m5) + ", " + _db.makeText(m6) + ", " + _db.makeText(m7) + ", " + _db.makeText(m8)
	);

	checkResponse(r, "nao conseguiu atualizar Chat Macro[M1=" + m0 + ", M2=" + m1 + ", M3=" + m2 + ", M4=" + m3 + ", M5=" 
			+ m4 + ", M6=" + m5 + ", M7=" + m6 + ", M8=" + m7 + ", M9=" + m8 + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateChatMacroUser::getUID() {
	return m_uid;
}

void CmdUpdateChatMacroUser::setUID(uint32_t _uid) {
	m_uid = _uid;
}

chat_macro_user& CmdUpdateChatMacroUser::getInfo() {
	return m_cmu;
}

void CmdUpdateChatMacroUser::setInfo(chat_macro_user& _cmu) {
	m_cmu = _cmu;
}
