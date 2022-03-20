// Arquivo cmd_box_info.cpp
// Criado em 15/07/2018 as 22:01 por Acrisio
// Implementa��o da classe CmdBoxInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_box_info.hpp"

using namespace stdA;

CmdBoxInfo::CmdBoxInfo(bool _waiter) : pangya_db(_waiter) {
}

CmdBoxInfo::~CmdBoxInfo() {
}

void CmdBoxInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(14, (uint32_t)_result->cols);

	ctx_box ctx_b{ 0 };
	ctx_box_item ctx_bi{ 0 };

	ctx_b._typeid = (uint32_t)IFNULL(atoi, _result->data[1]);

	auto it = m_box.find(ctx_b._typeid);

	// Box Item
	ctx_bi._typeid = (uint32_t)IFNULL(atoi, _result->data[7]);
	ctx_bi.numero = IFNULL(atoi, _result->data[8]);
	ctx_bi.probabilidade = (uint32_t)IFNULL(atoi, _result->data[9]);
	ctx_bi.qntd = (uint32_t)IFNULL(atoi, _result->data[10]);
	ctx_bi.raridade = BOX_TYPE_RARETY((unsigned char)IFNULL(atoi, _result->data[11]));
	ctx_bi.duplicar = (unsigned char)IFNULL(atoi, _result->data[12]);
	ctx_bi.active = (unsigned char)IFNULL(atoi, _result->data[13]);

	if (it != m_box.end()) {	// J� tem essa box add O Item s�
		// Add o Item a Box
		it->second.item.push_back(ctx_bi);

	}else {	// Ainda n�o tem essa box no map, add ela ao map

		// Inicializa as informa��es da box
		ctx_b.id = IFNULL(atoi, _result->data[0]);
		ctx_b.numero = IFNULL(atoi, _result->data[2]);
		ctx_b.tipo_open = BOX_TYPE_OPEN((unsigned char)IFNULL(atoi, _result->data[3]));
		ctx_b.tipo = BOX_TYPE((unsigned char)IFNULL(atoi, _result->data[4]));
		ctx_b.opened_typeid = (uint32_t)IFNULL(atoi, _result->data[5]);

		if (is_valid_c_string(_result->data[6]))
			STRCPY_TO_MEMORY_FIXED_SIZE(ctx_b.msg, sizeof(ctx_b.msg), _result->data[6]);

		// Add o Item a Box
		ctx_b.item.push_back(ctx_bi);

		// Add a Box ao map
		m_box.insert(std::make_pair(ctx_b._typeid, ctx_b));
	}
}

response* CmdBoxInfo::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao conseguiu pegar info das box para o sistema de box");

	return r;
}

std::map< uint32_t, ctx_box >& CmdBoxInfo::getInfo() {
	return m_box;
}
