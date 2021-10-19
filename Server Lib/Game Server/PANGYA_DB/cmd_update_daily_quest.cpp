// Arquivo cmd_update_daily_quest.cpp
// Criado em 13/04/2018 as 18:41 por Acrisio
// Implementa��o da classe CmdUpdateDailyQuest

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_daily_quest.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateDailyQuest::CmdUpdateDailyQuest(bool _waiter) : pangya_db(_waiter), m_dqi{0}, m_updated(false) {
}

CmdUpdateDailyQuest::CmdUpdateDailyQuest(DailyQuestInfo& _dqi, bool _waiter) 
	: pangya_db(_waiter), m_dqi(_dqi), m_updated(false) {
}

CmdUpdateDailyQuest::~CmdUpdateDailyQuest() {
}

void CmdUpdateDailyQuest::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	// Update ON DB
	m_updated = IFNULL(atoi, _result->data[0]) == 1 ? true : false;
	
	if (!m_updated) { // Não atualizou, pega os valores atualizados do banco de dados

		for (auto i = 0u; i < 3u; ++i)
			m_dqi._typeid[i] = (uint32_t)IFNULL(atoi, _result->data[1u + i]);	// 1 + 3

		if (_result->data[4] != nullptr)
			_translateDate(_result->data[4], &m_dqi.date);
	}	

	return;
}

response* CmdUpdateDailyQuest::prepareConsulta(database& _db) {

	m_updated = false;

	std::string reg_date = "null";

	if (!isEmpty(m_dqi.date))
		reg_date = _db.makeText(_formatDate(m_dqi.date));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_dqi._typeid[0]) + ", " + std::to_string(m_dqi._typeid[1]) 
			+ ", " + std::to_string(m_dqi._typeid[2]) + ", " + reg_date
	);

	checkResponse(r, "nao conseguiu atualizar o sistema de Daily Quest[" + m_dqi.toString() + "] no banco de dados");
	
	return r;
}

DailyQuestInfo& CmdUpdateDailyQuest::getInfo() {
	return m_dqi;
}

void CmdUpdateDailyQuest::setInfo(DailyQuestInfo& _dqi) {
	m_dqi = _dqi;
}

bool CmdUpdateDailyQuest::isUpdated() {
	return m_updated;
}
