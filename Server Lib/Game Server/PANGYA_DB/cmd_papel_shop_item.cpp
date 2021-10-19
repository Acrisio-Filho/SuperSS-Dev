// Arquivo cmd_papel_shop_item.cpp
// Criado em 09/07/2018 as 20:31 por Acrisio
// Implementa��o da classe CmdPapelShopItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_papel_shop_item.hpp"

using namespace stdA;

CmdPapelShopItem::CmdPapelShopItem(bool _waiter) : pangya_db(_waiter), m_ctx_psi() {
}

CmdPapelShopItem::~CmdPapelShopItem() {
}

void CmdPapelShopItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	ctx_papel_shop_item ctx_psi{ 0 };

	ctx_psi._typeid = IFNULL(atoi, _result->data[0]);
	ctx_psi.probabilidade = IFNULL(atoi, _result->data[1]);
	ctx_psi.numero = IFNULL(atoi, _result->data[2]);
	ctx_psi.tipo = PAPEL_SHOP_TYPE((unsigned char)IFNULL(atoi, _result->data[3]));
	ctx_psi.active = (unsigned char)IFNULL(atoi, _result->data[4]);

	m_ctx_psi.push_back(ctx_psi);
}

response* CmdPapelShopItem::prepareConsulta(database& _db) {

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar os papel shop itens");

	return r;
}

std::vector< ctx_papel_shop_item >& CmdPapelShopItem::getInfo() {
	return m_ctx_psi;
}
