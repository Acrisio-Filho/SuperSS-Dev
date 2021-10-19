// Arquivo cmd_dolfini_locker_info.cpp
// Criado em 22/03/2018 as 21:19 por Acrisio
// Implementa��o da classe CmdDolfiniLockerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_dolfini_locker_info.hpp"

using namespace stdA;

CmdDolfiniLockerInfo::CmdDolfiniLockerInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_df{} {
}

CmdDolfiniLockerInfo::CmdDolfiniLockerInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_df{} {
}

CmdDolfiniLockerInfo::~CmdDolfiniLockerInfo() {
}

void CmdDolfiniLockerInfo::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	uint32_t uid_req = 0u;

	if (_index_result == 0) {	// Primeira consulta retorna o info do dolfini locker 

		checkColumnNumber(4, (uint32_t)_result->cols);

		uid_req = IFNULL(atoi, _result->data[0]);
		if (_result->data[1] != nullptr)
#if defined(_WIN32)
			memcpy_s(m_df.pass, sizeof(m_df.pass), _result->data[1], sizeof(m_df.pass));
#elif defined(__linux__)
			memcpy(m_df.pass, _result->data[1], sizeof(m_df.pass));
#endif
		m_df.pang = IFNULL(atoll, _result->data[2]);
		m_df.locker = (unsigned char)IFNULL(atoi, _result->data[3]);

	}else if (_index_result == 1) {	// Segunda consulta retorna os itens guardado no dolfini locker
		
		checkColumnNumber(10, (uint32_t)_result->cols);

		DolfiniLockerItem dli{ 0 };

		dli.item.id = IFNULL(atoi, _result->data[0]);
		uid_req = IFNULL(atoi, _result->data[1]);
		dli.item._typeid = IFNULL(atoi, _result->data[2]);
		if (_result->data[3] != nullptr)
#if defined(_WIN32)
			memcpy_s(dli.item.sd_name, sizeof(dli.item.sd_name), _result->data[3], sizeof(dli.item.sd_name));
#elif defined(__linux__)
			memcpy(dli.item.sd_name, _result->data[3], sizeof(dli.item.sd_name));
#endif
			//strcpy_s(dli.item.sd_name, _result->data[3]);
		if (_result->data[4] != nullptr)
#if defined(_WIN32)
			memcpy_s(dli.item.sd_idx, sizeof(dli.item.sd_idx), _result->data[4], sizeof(dli.item.sd_idx));
#elif defined(__linux__)
			memcpy(dli.item.sd_idx, _result->data[4], sizeof(dli.item.sd_idx));
#endif
			//strcpy_s(dli.item.sd_idx, _result->data[4]);
		dli.item.sd_seq = (short)IFNULL(atoi, _result->data[5]);
		if (_result->data[6] != nullptr)
#if defined(_WIN32)
			memcpy_s(dli.item.sd_copier_nick, sizeof(dli.item.sd_copier_nick), _result->data[6], sizeof(dli.item.sd_copier_nick));
#elif defined(__linux__)
			memcpy(dli.item.sd_copier_nick, _result->data[6], sizeof(dli.item.sd_copier_nick));
#endif
			//strcpy_s(dli.item.sd_copier_nick, _result->data[6]);
		dli.item.sd_status = (unsigned char)IFNULL(atoi, _result->data[7]);
		dli.index = IFNULL(atoll, _result->data[8]);
		dli.item.qntd = IFNULL(atoi, _result->data[9]);	// DOLFINI_LOCKER_FLAG, mas � quantidade

		m_df.v_item.push_back(dli);
	}

	if (uid_req != m_uid)
		throw exception("[CmdDolfiniLockerInfo::lineResult][Error] O dolfini info requerido retornou um uid diferente. UID_req: " + std::to_string(m_uid) + " != " + std::to_string(uid_req), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdDolfiniLockerInfo::prepareConsulta(database& _db) {

	m_df.clear();

	auto r = procedure(_db, m_szConsulta[0], std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o dolfini locker info do player: " + std::to_string(m_uid));

	auto r2 = procedure(_db, m_szConsulta[1], std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o dolfini locker item(ns) do player: " + std::to_string(m_uid));

	// add second result_set to response
	for (auto i = 0u; i < r2->getNumResultSet(); i++)
		r->addResultSet(r2->getResultSetAt(i));

	return r;
}

DolfiniLocker& CmdDolfiniLockerInfo::getInfo() {
	return m_df;
}

uint32_t CmdDolfiniLockerInfo::getUID() {
	return m_uid;
}

void CmdDolfiniLockerInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
