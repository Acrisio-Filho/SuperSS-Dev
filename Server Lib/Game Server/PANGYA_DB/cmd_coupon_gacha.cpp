// Arquivo cmd_coupon_gacha.cpp
// Criado em 19/03/2018 as 22:14 por Acrisio
// Implementação da classe CmdCouponGacha

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_coupon_gacha.hpp"

using namespace stdA;

CmdCouponGacha::CmdCouponGacha(bool _waiter) : pangya_db(_waiter), m_uid(0), m_cg{0} {
}

CmdCouponGacha::CmdCouponGacha(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_cg{0} {
}

CmdCouponGacha::~CmdCouponGacha() {
}

void CmdCouponGacha::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

    checkColumnNumber(1, (uint32_t)_result->cols);

    if (_index_result == 0)
        m_cg.normal_ticket = IFNULL(atoi, _result->data[0]);
    else if (_index_result == 1)
        m_cg.partial_ticket = IFNULL(atoi, _result->data[0]);
}

response* CmdCouponGacha::prepareConsulta(database& _db) {

    m_cg.clear();

    auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_uid) + "; " + m_szConsulta[1] + std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o(s) coupon(s) gacha do player: " + std::to_string(m_uid));

    return r;
}

CouponGacha& CmdCouponGacha::getCouponGacha() {
    return m_cg;
}

void CmdCouponGacha::setCouponGacha(CouponGacha& _cg) {
    m_cg = _cg;
}

uint32_t CmdCouponGacha::getUID() {
    return m_uid;
}

void CmdCouponGacha::setUID(uint32_t _uid) {
    m_uid = _uid;
}