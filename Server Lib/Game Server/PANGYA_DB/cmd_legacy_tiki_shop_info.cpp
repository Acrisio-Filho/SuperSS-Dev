// Arquivo cmd_legacy_tiki_shop_info.cpp
// Criado em 26/10/2020 as 15:21 por Acrisio
// Implementa��o da classe CmdLegacyTikiShopInfo

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_legacy_tiki_shop_info.hpp"

using namespace stdA;

CmdLegacyTikiShopInfo::CmdLegacyTikiShopInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_tiki_pts(0ull) {
}

CmdLegacyTikiShopInfo::CmdLegacyTikiShopInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_tiki_pts(0ull) {
}

CmdLegacyTikiShopInfo::~CmdLegacyTikiShopInfo() {
}

void CmdLegacyTikiShopInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_tiki_pts = (uint64_t)IFNULL(atoll, _result->data[0]);
}

response* CmdLegacyTikiShopInfo::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdLegacyTikiShopInfo::prepareConsulta][Error] m_uid is invalind(" + std::to_string(m_uid) + ")", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_tiki_pts = 0ull;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "Nao conseguiu pegar o Legacy Tiki Points do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdLegacyTikiShopInfo::getUID() {
	return m_uid;
}

void CmdLegacyTikiShopInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdLegacyTikiShopInfo::getInfo() {
	return m_tiki_pts;
}
