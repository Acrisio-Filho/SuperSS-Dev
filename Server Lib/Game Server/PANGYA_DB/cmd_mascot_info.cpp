// Arquivo cmd_mascot_info.cpp
// Criado em 18/03/2018 as 21:49 por Acrisio
// Implementa��o da classe CmdMascotInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_mascot_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include <algorithm>

using namespace stdA;

CmdMascotInfo::CmdMascotInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_type(ALL), m_mascot_id(-1), v_mi() {
}

CmdMascotInfo::CmdMascotInfo(uint32_t _uid, TYPE _type, int32_t _mascot_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_mascot_id(_mascot_id), v_mi() {
}

CmdMascotInfo::~CmdMascotInfo() {
}

void CmdMascotInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_reuslt*/) {

	checkColumnNumber(10, (uint32_t)_result->cols);

	MascotInfoEx mi{};

	mi.id = IFNULL(atoi, _result->data[0]);
	mi._typeid = IFNULL(atoi, _result->data[2]);
	mi.level = (unsigned char)IFNULL(atoi, _result->data[3]);
	mi.exp = IFNULL(atoi, _result->data[4]);
	mi.flag = (unsigned char)IFNULL(atoi, _result->data[5]);
	if (is_valid_c_string(_result->data[6]))
		STRCPY_TO_MEMORY_FIXED_SIZE(mi.message, sizeof(mi.message), _result->data[6]);
	mi.tipo = (short)IFNULL(atoi, _result->data[7]);
	mi.is_cash = (unsigned char)IFNULL(atoi, _result->data[8]);
	_translateDate(_result->data[9], &mi.data);

	auto it = v_mi.find(mi.id);

	if (it == v_mi.end() || (v_mi.count(mi.id) == 1 && it->second._typeid != mi._typeid))
		v_mi.insert(std::make_pair(mi.id, mi));
	else if (v_mi.count(mi.id) > 1) {

		auto er = v_mi.equal_range(mi.id);

		it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
			return _el.second._typeid == mi._typeid;
		});

		// N�o tem um igual add um novo
		if (it == er.second/*End*/) {

			v_mi.insert(std::make_pair(mi.id, mi));

			_smp::message_pool::getInstance().push(new message("[CmdMascotInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] adicionou Mascot[TYPEID=" 
					+ std::to_string(mi._typeid) + ", ID=" + std::to_string(mi.id) + "], com mesmo id e typeid diferente de outro Mascot que tem no multimap", CL_FILE_LOG_AND_CONSOLE));
		}else
			// Tem um Mascot com o mesmo ID e TYPEID (DUPLICATA)
			_smp::message_pool::getInstance().push(new message("[CmdMascotInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] tentou adicionar no multimap um Mascot[TYPEID=" 
					+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] com o mesmo ID e TYPEID, DUPLICATA", CL_FILE_LOG_AND_CONSOLE));

	}else // Tem um Mascot com o mesmo ID e TYPEID (DUPLICATA)
		_smp::message_pool::getInstance().push(new message("[CmdMascotInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] tentou adicionar no multimap um Mascot[TYPEID="
				+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] com o mesmo ID e TYPEID, DUPLICATA", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdMascotInfo::prepareConsulta(database& _db) {
	
	v_mi.clear();
	//v_mi.shrink_to_fit();

	auto r = procedure(_db, (m_type == ALL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + (m_type == ONE ? ", " + std::to_string(m_mascot_id) : std::string()));

	checkResponse(r, "nao conseguiu pegar o(s) mascot(s) do player: " + std::to_string(m_uid));

	return r;
}

std::multimap< int32_t/*ID*/, MascotInfoEx >& CmdMascotInfo::getInfo() {
	return v_mi;
}

uint32_t CmdMascotInfo::getUID() {
	return m_uid;
}

void CmdMascotInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdMascotInfo::TYPE CmdMascotInfo::getType() {
	return m_type;
}

void CmdMascotInfo::setType(TYPE _type) {
	m_type = _type;
}

int32_t CmdMascotInfo::getMascotID() {
	return m_mascot_id;
}

void CmdMascotInfo::setMascotID(int32_t _mascot_id) {
	m_mascot_id = _mascot_id;
}
