// Arquivo cmd_golden_time_item.cpp
// Criado em 24/10/2020 as 02:50 por Acrisio
// Implementa��o da classe CmdGoldenTimeItem

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_golden_time_item.hpp"

using namespace stdA;

CmdGoldenTimeItem::CmdGoldenTimeItem(uint32_t _id, bool _waiter) : pangya_db(_waiter), m_id(_id), m_item() {
}

CmdGoldenTimeItem::CmdGoldenTimeItem(bool _waiter) : pangya_db(_waiter), m_id(0u), m_item() {
}

CmdGoldenTimeItem::~CmdGoldenTimeItem() {

	if (!m_item.empty()) {
		m_item.clear();
		m_item.shrink_to_fit();
	}
}

void CmdGoldenTimeItem::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	checkColumnNumber(4, (uint32_t)_result->cols);

	stItemReward item{ 0u };

	item._typeid = (uint32_t)IFNULL(atoi, _result->data[0]);
	item.qntd = (uint32_t)IFNULL(atoi, _result->data[1]);
	item.qntd_time = (uint32_t)IFNULL(atoi, _result->data[2]);
	item.rate = (uint32_t)IFNULL(atoi, _result->data[3]);

	m_item.push_back(item);
}

response* CmdGoldenTimeItem::prepareConsulta(database& _db) {
	
	if (m_id == 0u)
		throw exception("[CmdGoldenTimeItem::prepareConsulta][Error] m_id is invalid(" + std::to_string(m_id) + ")", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (!m_item.empty())
		m_item.clear();

	auto r = consulta(_db, m_szConsulta + std::to_string(m_id));

	checkResponse(r, "nao conseguiu pegar o item do Golden Time[ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdGoldenTimeItem::getId() {
	return m_id;
}

void CmdGoldenTimeItem::setId(uint32_t _id) {
	m_id = _id;
}

std::vector< stItemReward >& CmdGoldenTimeItem::getInfo() {
	return m_item;
}
