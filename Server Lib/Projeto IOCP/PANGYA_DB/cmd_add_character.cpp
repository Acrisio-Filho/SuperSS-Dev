// Arquivo cmd_add_character.cpp
// Criado em 25/03/2018 as 18:05 por Acrisio
// Implementa��o da classe CmdAddCharacter

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_character.hpp"

using namespace stdA;

CmdAddCharacter::CmdAddCharacter(bool _waiter) : CmdAddItemBase(_waiter), m_ci{0} {
}

CmdAddCharacter::CmdAddCharacter(uint32_t _uid, CharacterInfo& _ci, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
		: CmdAddItemBase(_uid, _purchase, _gift_flag, _waiter), m_ci(_ci) {
}

CmdAddCharacter::~CmdAddCharacter() {
}

void CmdAddCharacter::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_ci.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddCharacter::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id) + ", " + std::to_string(m_ci._typeid)
					+ ", " + std::to_string((unsigned short)m_ci.default_hair) + ", " + std::to_string((unsigned short)m_ci.default_shirts)
					+ ", " + std::to_string((unsigned short)m_purchase) + ", " + std::to_string((unsigned short)m_gift_flag)
					+ ", " + std::to_string(m_ci.parts_typeid[0]) + ", " + std::to_string(m_ci.parts_typeid[1])
					+ ", " + std::to_string(m_ci.parts_typeid[2]) + ", " + std::to_string(m_ci.parts_typeid[3])
					+ ", " + std::to_string(m_ci.parts_typeid[4]) + ", " + std::to_string(m_ci.parts_typeid[5])
					+ ", " + std::to_string(m_ci.parts_typeid[6]) + ", " + std::to_string(m_ci.parts_typeid[7])
					+ ", " + std::to_string(m_ci.parts_typeid[8]) + ", " + std::to_string(m_ci.parts_typeid[9])
					+ ", " + std::to_string(m_ci.parts_typeid[10]) + ", " + std::to_string(m_ci.parts_typeid[11])
					+ ", " + std::to_string(m_ci.parts_typeid[12]) + ", " + std::to_string(m_ci.parts_typeid[13])
					+ ", " + std::to_string(m_ci.parts_typeid[14]) + ", " + std::to_string(m_ci.parts_typeid[15])
					+ ", " + std::to_string(m_ci.parts_typeid[16]) + ", " + std::to_string(m_ci.parts_typeid[17])
					+ ", " + std::to_string(m_ci.parts_typeid[18]) + ", " + std::to_string(m_ci.parts_typeid[19])
					+ ", " + std::to_string(m_ci.parts_typeid[20]) + ", " + std::to_string(m_ci.parts_typeid[21])
					+ ", " + std::to_string(m_ci.parts_typeid[22]) + ", " + std::to_string(m_ci.parts_typeid[23]));

	checkResponse(r, "nao conseguiu adicionar o character[TYPEID=" + std::to_string(m_ci._typeid) + "] para o player: " + std::to_string(m_uid));

	return r;
}

CharacterInfo& CmdAddCharacter::getInfo() {
	return m_ci;
}

void CmdAddCharacter::setInfo(CharacterInfo& _ci) {
	m_ci = _ci;
}
