// Arquivo cmd_member_info.cpp
// Criado em 18/03/2018 as 13:17 por Acrisio
// Implementação da classe CmdMemberInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_member_info.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

#include <memory.h>

using namespace stdA;

CmdMemberInfo::CmdMemberInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_mi{} {
};

CmdMemberInfo::CmdMemberInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_mi{} {
};

CmdMemberInfo::~CmdMemberInfo() {
};

void CmdMemberInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(28, (uint32_t)_result->cols);

	if (is_valid_c_string(_result->data[0]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_mi.id, sizeof(m_mi.id), _result->data[0]);
	
	m_mi.uid = IFNULL(atoi, _result->data[1]);
	m_mi.sexo = (unsigned char)IFNULL(atoi, _result->data[2]);
	m_mi.do_tutorial = (unsigned char)IFNULL(atoi, _result->data[3]);
	
	if (is_valid_c_string(_result->data[4]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_mi.nick_name, sizeof(m_mi.nick_name), _result->data[4]);
	
	m_mi.school = IFNULL(atoi, _result->data[5]);
	m_mi.capability.ulCapability = (uint32_t)IFNULL(atoi, _result->data[6]);
	m_mi.manner_flag = IFNULL(atoi, _result->data[9]);

	if (is_valid_c_string(_result->data[11]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_mi.guild_name, sizeof(m_mi.guild_name), _result->data[11]);
	
	m_mi.guild_uid = IFNULL(atoi, _result->data[12]);
	m_mi.guild_pang = IFNULL(atoll, _result->data[13]);
	m_mi.guild_point = IFNULL(atoi, _result->data[14]);
	m_mi.guild_mark_img_no = (uint32_t)IFNULL(atoi, _result->data[15]); // Guild Idx é o ultilizado no PangYa JP
	m_mi.event_1 = (unsigned char)IFNULL(atoi, _result->data[16]);
	m_mi.event_2 = (unsigned char)IFNULL(atoi, _result->data[17]);

	// 1 Player loga primeira vezes, 2 é o um player que já logou mais de 1x
	m_mi.flag_login_time = 2u;
	
	// Sexo do player
	m_mi.state_flag.stFlagBit.sexo = m_mi.sexo;
	
	m_mi.papel_shop.limit_count = (short)IFNULL(atoi, _result->data[18]);
	m_mi.papel_shop.current_count = (short)IFNULL(atoi, _result->data[22]);
	m_mi.papel_shop.remain_count = (short)IFNULL(atoi, _result->data[23]);

	if (_result->data[24] != nullptr)
		_translateDate(_result->data[24], &m_mi.papel_shop_last_update);
	
	m_mi.level = (unsigned char)IFNULL(atoi, _result->data[25]);

	if (is_valid_c_string(_result->data[26]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_mi.guild_mark_img, sizeof(m_mi.guild_mark_img), _result->data[26]);

    if (m_mi.uid != m_uid)
        throw exception("[CmdMemberInfo::lineResult][Error] UID do member info do player nao e igual ao requisitado. UID Req: " + std::to_string(m_uid) + " != " + std::to_string(m_mi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

};

response* CmdMemberInfo::prepareConsulta(database& _db) {

    m_mi.clear();

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o member info do player: " + std::to_string(m_uid));

	return r;
};

MemberInfoEx& CmdMemberInfo::getInfo() {
    return m_mi;
};

void CmdMemberInfo::setInfo(MemberInfoEx& _mi) {
    m_mi = _mi;
};

uint32_t CmdMemberInfo::getUID() {
    return m_uid;
};

void CmdMemberInfo::setUID(uint32_t _uid) {
    m_uid = _uid;
};
