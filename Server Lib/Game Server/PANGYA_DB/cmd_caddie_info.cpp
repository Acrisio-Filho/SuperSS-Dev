// Arquivo cmd_caddie_info.cpp
// Criado em 18/03/2018 as 21:28 por Acrisio
// Implementa��o da classe CmdCaddieInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_caddie_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <algorithm>

using namespace stdA;

CmdCaddieInfo::CmdCaddieInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_type(ALL), m_caddie_id(-1), v_ci() {
}

CmdCaddieInfo::CmdCaddieInfo(uint32_t _uid, TYPE _type, int32_t _caddie_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_caddie_id(_caddie_id), v_ci() {
}

CmdCaddieInfo::~CmdCaddieInfo() {
}

void CmdCaddieInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(11, (uint32_t)_result->cols);

	CaddieInfoEx ci{ 0 };

	// Begin of Initialization Caddie Info Structure
	ci.id = IFNULL(atoi, _result->data[0]);
	ci._typeid = IFNULL(atoi, _result->data[2]);
	ci.parts_typeid = IFNULL(atoi, _result->data[3]);
	ci.level = (unsigned char)IFNULL(atoi, _result->data[4]);
	ci.exp = IFNULL(atoi, _result->data[5]);
	ci.rent_flag = (unsigned char)IFNULL(atoi, _result->data[6]);

	//ci.end_date_unix = (short)IFNULL(atoi, _result->data[7]);
	if (_result->data[7] != nullptr)
		_translateDate(_result->data[7], &ci.end_date);

	ci.purchase = (unsigned char)IFNULL(atoi, _result->data[8]);

	//ci.parts_end_date_unix = (short)IFNULL(atoi, _result->data[9]);
	if (_result->data[9] != nullptr)
		_translateDate(_result->data[9], &ci.end_parts_date);
	
	ci.check_end = (short)IFNULL(atoi, _result->data[10]);
	// End of Initialization Caddie Info Structure

	auto it = v_ci.find(ci.id);

	if (it == v_ci.end() || (v_ci.count(ci.id) == 1 && it->second._typeid != ci._typeid))	// Make New, not have this Caddie
		v_ci.insert(std::make_pair(ci.id, ci));
	else if (v_ci.count(ci.id) > 1) {

		auto er = v_ci.equal_range(ci.id);

		it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
			return _el.second._typeid == ci._typeid;
		});

		// N�o tem um igual add um novo
		if (it == er.second/*End*/) {

			v_ci.insert(std::make_pair(ci.id, ci));

			_smp::message_pool::getInstance().push(new message("[CmdCaddieInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] adicionou Caddie[TYPEID=" 
					+ std::to_string(ci._typeid) + ", ID=" + std::to_string(ci.id) + "], com mesmo id e typeid diferente de outro Caddie que tem no multimap", CL_FILE_LOG_AND_CONSOLE));
		}else
			// Tem um Caddie com o mesmo ID e TYPEID (DUPLICATA)
			_smp::message_pool::getInstance().push(new message("[CmdCaddieInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] tentou adicionar no multimap um Caddie[TYPEID=" 
					+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] com o mesmo ID e TYPEID, DUPLICATA", CL_FILE_LOG_AND_CONSOLE));

	}else // Tem um Caddie com o mesmo ID e TYPEID (DUPLICATA)
		_smp::message_pool::getInstance().push(new message("[CmdCaddieInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] tentou adicionar no multimap um Caddie[TYPEID="
				+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] com o mesmo ID e TYPEID, DUPLICATA", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdCaddieInfo::prepareConsulta(database& _db) {
	
	v_ci.clear();

	auto r = procedure(_db, (m_type == ALL) ? m_szConsulta[0] : ((m_type == ONE) ? m_szConsulta[1] : m_szConsulta[2]), std::to_string(m_uid) + (m_type == ONE ? ", " + std::to_string(m_caddie_id) : std::string()));

	checkResponse(r, "nao conseguiu pegar o caddie info do player: " + std::to_string(m_uid));

	return r;
}

std::multimap< int32_t/*ID*/, CaddieInfoEx >& CmdCaddieInfo::getInfo() {
	return v_ci;
}

uint32_t CmdCaddieInfo::getUID() {
	return m_uid;
}

void CmdCaddieInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdCaddieInfo::TYPE CmdCaddieInfo::getType() {
	return m_type;
}

void CmdCaddieInfo::setType(TYPE _type) {
	m_type = _type;
}

int32_t CmdCaddieInfo::getCaddieID() {
	return m_caddie_id;
}

void CmdCaddieInfo::setCaddieID(int32_t _caddie_id) {
	m_caddie_id = _caddie_id;
}

