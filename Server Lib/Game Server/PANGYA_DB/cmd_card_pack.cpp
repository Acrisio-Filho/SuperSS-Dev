// Arquivo cmd_card_pack.cpp
// Criado em 01/07/2018 as 00:45 por Acrisio
// Implementa��o da classe CmdCardPack

#if defined(_WIN32)
#pragma pack(1)
#endif

//#include <WinSock2.h>
#include "cmd_card_pack.hpp"

using namespace stdA;

CmdCardPack::CmdCardPack(bool _waiter) : pangya_db(_waiter), m_card_pack() {
}

CmdCardPack::~CmdCardPack() {
}

void CmdCardPack::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(10, (uint32_t)_result->cols);

	CardPack cp{ 0 };

	cp._typeid = IFNULL(atoi, _result->data[0]);

	auto it = m_card_pack.end();

	if ((it = m_card_pack.find(cp._typeid)) != m_card_pack.end())
		it->second.card.push_back({ (uint32_t)IFNULL(atoi, _result->data[7]), (uint32_t)IFNULL(atoi, _result->data[8]), CARD_TYPE(IFNULL(atoi, _result->data[9])) });
	else {	// Make New
		cp.num = IFNULL(atoi, _result->data[1]);
		cp.volume = (unsigned char)IFNULL(atoi, _result->data[2]);
		
		for (auto i = 0u; i < (sizeof(cp.rate.value) / sizeof(cp.rate.value[0])); ++i)
			cp.rate.value[i] = (unsigned short)IFNULL(atoi, _result->data[3 + i]);			// 3 + i, 3 + 4 = 7

		cp.card.push_back({ (uint32_t)IFNULL(atoi, _result->data[7]), (uint32_t)IFNULL(atoi, _result->data[8]), CARD_TYPE(IFNULL(atoi, _result->data[9])) });

		m_card_pack[cp._typeid] = cp;
	}
}

response* CmdCardPack::prepareConsulta(database& _db) {

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiui pegar o(s) Card(s) Pack");

	return r;
}

std::map< uint32_t, CardPack >& CmdCardPack::getCardPack() {
	return m_card_pack;
}
