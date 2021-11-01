// Arquivo cmd_grand_zodiac_pontos.cpp
// Criado em 25/06/2020 as 16:04 por Acrisio
// Implementa��o da classe CmdGrandZodiacPontos

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_grand_zodiac_pontos.hpp"

using namespace stdA;

CmdGrandZodiacPontos::CmdGrandZodiacPontos(uint32_t _uid, eCMD_GRAND_ZODIAC_TYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pontos(0ull), m_type(_type) {
}

CmdGrandZodiacPontos::CmdGrandZodiacPontos(uint32_t _uid, uint64_t _pontos, eCMD_GRAND_ZODIAC_TYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pontos(_pontos), m_type(_type) {
}

CmdGrandZodiacPontos::CmdGrandZodiacPontos(bool _waiter)
	: pangya_db(_waiter), m_uid(0u), m_pontos(0ull), m_type(eCMD_GRAND_ZODIAC_TYPE::CGZT_GET) {
}

CmdGrandZodiacPontos::~CmdGrandZodiacPontos() {
}

void CmdGrandZodiacPontos::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	if (m_type == eCMD_GRAND_ZODIAC_TYPE::CGZT_GET) {
		
		checkColumnNumber(1, (uint32_t)_result->cols);
		
		m_pontos = (uint64_t)IFNULL(atoll, _result->data[0]);
	}
}

response* CmdGrandZodiacPontos::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdGrandZodiacPontos::prepareConsulta][Error] m_uid(" + std::to_string(m_uid) + ") is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string query = "";

	if (m_type == eCMD_GRAND_ZODIAC_TYPE::CGZT_GET)
		query = m_szConsulta[0] + std::to_string(m_uid);
	else if (m_type == eCMD_GRAND_ZODIAC_TYPE::CGZT_UPDATE)
		query = m_szConsulta[1] + std::to_string(m_pontos) + m_szConsulta[2] + std::to_string(m_uid);

	auto r = consulta(_db, query);

	checkResponse(r, "nao conseguiu " + (m_type == eCMD_GRAND_ZODIAC_TYPE::CGZT_GET 
										? "pegar os pontos do Grand Zodiac" 
										: "atualizar os pontos[" + std::to_string(m_pontos) + "]") 
					+ " do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdGrandZodiacPontos::getUID() {
	return m_uid;
}

uint64_t CmdGrandZodiacPontos::getPontos() {
	return m_pontos;
}

CmdGrandZodiacPontos::eCMD_GRAND_ZODIAC_TYPE& CmdGrandZodiacPontos::getType() {
	return m_type;
}

void CmdGrandZodiacPontos::setUID(uint32_t _uid) {
	m_uid = _uid;
}

void CmdGrandZodiacPontos::setPontos(uint64_t _pontos) {
	m_pontos = _pontos;
}

void CmdGrandZodiacPontos::setType(eCMD_GRAND_ZODIAC_TYPE _type) {
	m_type = _type;
}
