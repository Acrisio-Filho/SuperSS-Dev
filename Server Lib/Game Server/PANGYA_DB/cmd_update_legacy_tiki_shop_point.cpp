// Arquivo cmd_update_lagacy_tiki_shop_point.cpp
// Criado em 26/10/2020 as 15:33 por Acrisio
// Implementa��o da classe CmdUpdateLegacyTikiShopPoint

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_update_legacy_tiki_shop_point.hpp"

using namespace stdA;

CmdUpdateLegacyTikiShopPoint::CmdUpdateLegacyTikiShopPoint(uint32_t _uid, uint64_t _tiki_pts, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_tiki_shop_point(_tiki_pts) {
}

CmdUpdateLegacyTikiShopPoint::CmdUpdateLegacyTikiShopPoint(bool _waiter) : pangya_db(_waiter), m_uid(0ull), m_tiki_shop_point(0ull) {
}

CmdUpdateLegacyTikiShopPoint::~CmdUpdateLegacyTikiShopPoint() {
}

void CmdUpdateLegacyTikiShopPoint::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateLegacyTikiShopPoint::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdUpdateLegacyTikiShopPoint::prepareConsulta][Error] m_uid is invalid(" + std::to_string(m_uid) + ")", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_tiki_shop_point) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "Nao conseguiu atualizar o Legacy Tiki Shop Point[POINT=" + std::to_string(m_tiki_shop_point) 
			+ "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateLegacyTikiShopPoint::getUID() {
	return m_uid;
}

void CmdUpdateLegacyTikiShopPoint::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdUpdateLegacyTikiShopPoint::getTikiShopPoint() {
	return m_tiki_shop_point;
}

void CmdUpdateLegacyTikiShopPoint::setTikiShopPoint(uint64_t _tiki_pts) {
	m_tiki_shop_point = _tiki_pts;
}
