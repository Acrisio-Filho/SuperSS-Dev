// Arquivo cmd_save_nick.cpp
// Criado em 01/04/2018 as 14:02 por Acrisio
// Implementa��o da classe CmdSaveNick

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_save_nick.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

CmdSaveNick::CmdSaveNick(bool _waiter) : pangya_db(_waiter), m_uid(0), m_nick() {
}

CmdSaveNick::CmdSaveNick(uint32_t _uid, std::string& _nick, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_nick() {
	setNick(_nick);
}

CmdSaveNick::CmdSaveNick(uint32_t _uid, std::wstring& _nick, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_nick(_nick) {
}

CmdSaveNick::~CmdSaveNick() {
}

void CmdSaveNick::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdSaveNick::prepareConsulta(database& _db) {

	if (m_nick.empty())
		throw exception("[CmdSaveNick::prepareConsulta][Error] Nick invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_wstring(m_uid) + L", " + _db.makeText(m_nick));

	checkResponse(r, L"nao conseguiu atualizar o nick: " + m_nick + L", do player: " + std::to_wstring(m_uid));

	return r;
}

uint32_t CmdSaveNick::getUID() {
	return m_uid;
}

void CmdSaveNick::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::string CmdSaveNick::getNick() {
	return WcToMb(m_nick);
}

std::wstring& CmdSaveNick::getWNick() {
	return m_nick;
}

void CmdSaveNick::setNick(std::string& _nick) {

	auto _wnick = MbToWc(_nick);

	setNick(_wnick);
}

void CmdSaveNick::setNick(std::wstring& _nick) {
	m_nick = _nick;
}
