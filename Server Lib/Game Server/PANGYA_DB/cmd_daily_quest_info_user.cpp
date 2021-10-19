// Arquivo cmd_daily_quest_info_user.cpp
// Criado em 24/03/2018 as 19:58 por Acrisio
// Implementa��o da classe CmdDailyQuesInfoUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_daily_quest_info_user.hpp"

using namespace stdA;

CmdDailyQuestInfoUser::CmdDailyQuestInfoUser(bool _waiter)
		: pangya_db(_waiter), m_uid(0), m_type(GET), m_ok(false), m_dqiu{0} {
}

CmdDailyQuestInfoUser::CmdDailyQuestInfoUser(uint32_t _uid, TYPE _type, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_ok(false), m_dqiu{0} {
}

CmdDailyQuestInfoUser::~CmdDailyQuestInfoUser() {
}

void CmdDailyQuestInfoUser::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	if (m_type == GET) {				// GetDailyQuestInfo
		checkColumnNumber(6, (uint32_t)_result->cols);

		m_dqiu.now_date = IFNULL(atoi, _result->data[0]);
		m_dqiu.accept_date = IFNULL(atoi, _result->data[1]);
		m_dqiu.count = 3;			// POR ENQUANTO O COUNT � EST�TICO EM 3, MAS DEPOIS VOU COLOCAR NO DB DIREITO
		m_dqiu.current_date = IFNULL(atoi, _result->data[2]);
		for (auto i = 0; i < 3; i++)
			m_dqiu._typeid[i] = IFNULL(atoi, _result->data[3 + i]);		// i + 3 (3 + 3) = 6

	}else if (m_type == CHECK) {			// CheckDailyQuestInfo
		m_ok = IFNULL(atoi, _result->data[0]) == 1;
	}
}

response* CmdDailyQuestInfoUser::prepareConsulta(database& _db) {

	m_ok = false;
	m_dqiu.clear();

	response *r = nullptr;

	switch (m_type) {
	case GET:
	default:
		r = procedure(_db, m_szConsulta[0], std::to_string(m_uid));

		checkResponse(r, "nao conseguiu pegar o daily quest info do player: " + std::to_string(m_uid));
		break;
	case CHECK:
		r = procedure(_db, m_szConsulta[1], std::to_string(m_uid));

		checkResponse(r, "nao conseguiu verificar o daily quest info do player: " + std::to_string(m_uid));
		break;
	}

	return r;
}

DailyQuestInfoUser& CmdDailyQuestInfoUser::getInfo() {
	return m_dqiu;
}

void CmdDailyQuestInfoUser::setInfo(DailyQuestInfoUser& _dqiu) {
	m_dqiu = _dqiu;
}

bool CmdDailyQuestInfoUser::check() {
	return m_ok;
}

uint32_t CmdDailyQuestInfoUser::getUID() {
	return m_uid;
}

void CmdDailyQuestInfoUser::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdDailyQuestInfoUser::TYPE CmdDailyQuestInfoUser::getType() {
	return m_type;
}

void CmdDailyQuestInfoUser::setType(TYPE _type) {
	m_type = _type;
}
