// Arquivo cmd_add_character_hair_style.cpp
// Criado em 25/03/2018 as 20:18 por Acrisio
// Implementa��o da classe CmdAddCharacterHairStyle

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_character_hair_style.hpp"

using namespace stdA;

CmdAddCharacterHairStyle::CmdAddCharacterHairStyle(bool _waiter) : CmdAddItemBase(_waiter), m_ci{0} {
}

CmdAddCharacterHairStyle::CmdAddCharacterHairStyle(uint32_t _uid, CharacterInfo& _ci, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
		: CmdAddItemBase(_uid, _purchase, _gift_flag, _waiter), m_ci(_ci) {
}

CmdAddCharacterHairStyle::~CmdAddCharacterHairStyle() {
}

void CmdAddCharacterHairStyle::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa aqui por que � um UPDATE
	return;
}

response* CmdAddCharacterHairStyle::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::to_string((unsigned short)m_ci.default_hair) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_ci.id));

	checkResponse(r, "nao consiguiu adicionar o hair style[" + std::to_string((unsigned short)m_ci.default_hair) + "] para o character[ID=" + std::to_string(m_ci.id) + "] do player: " + std::to_string(m_uid));

	return r;
}

CharacterInfo& CmdAddCharacterHairStyle::getInfo() {
	return m_ci;
}

void CmdAddCharacterHairStyle::setInfo(CharacterInfo& _ci) {
	m_ci = _ci;
}
