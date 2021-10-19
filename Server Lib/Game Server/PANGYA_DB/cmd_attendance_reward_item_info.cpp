// Arquivo cmd_attendance_reward_item_info.cpp
// Criado em 14/11/2018 as 22:03 por Acrisio
// Implementa��o da classe CmdAttendanceRewardItemInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_attendance_reward_item_info.hpp"

using namespace stdA;

CmdAttendanceRewardItemInfo::CmdAttendanceRewardItemInfo(bool _waiter) : pangya_db(_waiter) {
}

CmdAttendanceRewardItemInfo::~CmdAttendanceRewardItemInfo() {
}

void CmdAttendanceRewardItemInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	AttendanceRewardItemCtx aric{ 0 };

	aric._typeid = (uint32_t)IFNULL(atoi, _result->data[0]);
	aric.qntd = IFNULL(atoi, _result->data[1]);
	aric.tipo = (unsigned char)IFNULL(atoi, _result->data[2]);

	v_item.push_back(aric);
}

response* CmdAttendanceRewardItemInfo::prepareConsulta(database& _db) {

	if (!v_item.empty())
		v_item.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar os Itens do Attendance Reward");

	return r;
}

std::vector< AttendanceRewardItemCtx >& CmdAttendanceRewardItemInfo::getInfo() {
	return v_item;
}
