// Arquivo cmd_verify_nick.cpp
// Criado em 01/04/2018 as 12:55 por Acrisio
// Implementa��o da classe CmdVerifyNick

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_verify_nick.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

CmdVerifNick::CmdVerifNick(bool _waiter) : pangya_db(_waiter), m_nick(), m_check(false), m_uid(0u) {
}

CmdVerifNick::CmdVerifNick(std::string& _nick, bool _waiter) : pangya_db(_waiter), m_check(false), m_uid(0u) {
	setNick(_nick);
}

CmdVerifNick::CmdVerifNick(std::wstring& _nick, bool _waiter) : pangya_db(_waiter), m_nick(_nick), m_check(false) {
}

CmdVerifNick::~CmdVerifNick() {
}

void CmdVerifNick::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_uid = IFNULL(atoi, _result->data[0]);
	m_check = (m_uid > 0) ? true : false;
}

response* CmdVerifNick::prepareConsulta(database& _db) {

	if (m_nick.empty())
		throw exception("[CmdVerify::prepareConsulta][Error] Nick invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_check = false;
	m_uid = 0u;

	auto r = procedure(_db, m_szConsulta, _db.makeText(m_nick));

	checkResponse(r, L"nao conseguiu verificar se existe o nick: " + m_nick);

	return r;
}

std::string CmdVerifNick::getNick() {
	return WcToMb(m_nick);
}

std::wstring& CmdVerifNick::getWNick() {
	return m_nick;
}

void CmdVerifNick::setNick(std::string& _nick) {
	
	std::wstring ws = MbToWc(_nick);

	setNick(ws);
}

void CmdVerifNick::setNick(std::wstring& _nick) {
	m_nick = _nick;
}

uint32_t stdA::CmdVerifNick::getUID() {
	return m_uid;
}

bool CmdVerifNick::getLastCheck() {
	return m_check;
}
