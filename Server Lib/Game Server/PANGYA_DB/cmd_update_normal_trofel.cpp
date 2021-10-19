// Arquivo cmd_update_normal_trofel.cpp
// Criado em 23/09/2018 as 12:33 por Acrisio
// Implementação da classe CmdUpdateNormalTrofel

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_normal_trofel.hpp"

using namespace stdA;

CmdUpdateNormalTrofel::CmdUpdateNormalTrofel(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ti{0} {
}

CmdUpdateNormalTrofel::CmdUpdateNormalTrofel(uint32_t _uid, TrofelInfo& _ti, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ti(_ti) {
}

CmdUpdateNormalTrofel::~CmdUpdateNormalTrofel() {
}

void CmdUpdateNormalTrofel::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdUpdateNormalTrofel::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdateNormalTrofel::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ti.ama_6_a_1[5][0]) + ", " + std::to_string(m_ti.ama_6_a_1[5][1]) + ", " + std::to_string(m_ti.ama_6_a_1[5][2])
						+ ", " + std::to_string(m_ti.ama_6_a_1[4][0]) + ", " + std::to_string(m_ti.ama_6_a_1[4][1]) + ", " + std::to_string(m_ti.ama_6_a_1[4][2])
						+ ", " + std::to_string(m_ti.ama_6_a_1[3][0]) + ", " + std::to_string(m_ti.ama_6_a_1[3][1]) + ", " + std::to_string(m_ti.ama_6_a_1[3][2])
						+ ", " + std::to_string(m_ti.ama_6_a_1[2][0]) + ", " + std::to_string(m_ti.ama_6_a_1[2][1]) + ", " + std::to_string(m_ti.ama_6_a_1[2][2])
						+ ", " + std::to_string(m_ti.ama_6_a_1[1][0]) + ", " + std::to_string(m_ti.ama_6_a_1[1][1]) + ", " + std::to_string(m_ti.ama_6_a_1[1][2])
						+ ", " + std::to_string(m_ti.ama_6_a_1[0][0]) + ", " + std::to_string(m_ti.ama_6_a_1[0][1]) + ", " + std::to_string(m_ti.ama_6_a_1[0][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[0][0]) + ", " + std::to_string(m_ti.pro_1_a_7[0][1]) + ", " + std::to_string(m_ti.pro_1_a_7[0][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[1][0]) + ", " + std::to_string(m_ti.pro_1_a_7[1][1]) + ", " + std::to_string(m_ti.pro_1_a_7[1][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[2][0]) + ", " + std::to_string(m_ti.pro_1_a_7[2][1]) + ", " + std::to_string(m_ti.pro_1_a_7[2][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[3][0]) + ", " + std::to_string(m_ti.pro_1_a_7[3][1]) + ", " + std::to_string(m_ti.pro_1_a_7[3][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[4][0]) + ", " + std::to_string(m_ti.pro_1_a_7[4][1]) + ", " + std::to_string(m_ti.pro_1_a_7[4][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[5][0]) + ", " + std::to_string(m_ti.pro_1_a_7[5][1]) + ", " + std::to_string(m_ti.pro_1_a_7[5][2])
						+ ", " + std::to_string(m_ti.pro_1_a_7[6][0]) + ", " + std::to_string(m_ti.pro_1_a_7[6][1]) + ", " + std::to_string(m_ti.pro_1_a_7[6][2])
	);

	checkResponse(r, "player[UID=" + std::to_string(m_uid) + "] nao consiguiu atualizar o Trofel Normal[AMA_1_G=" + std::to_string(m_ti.ama_6_a_1[5][0]) + ", AMA_1_S=" + std::to_string(m_ti.ama_6_a_1[5][1]) + ", AMA_1_B="
			+ std::to_string(m_ti.ama_6_a_1[5][2]) + ", AMA_2_G=" + std::to_string(m_ti.ama_6_a_1[4][0]) + ", AMA_2_S=" + std::to_string(m_ti.ama_6_a_1[4][1]) + ", AMA_2_B=" + std::to_string(m_ti.ama_6_a_1[4][2]) + ", AMA_3_G="
			+ std::to_string(m_ti.ama_6_a_1[3][0]) + ", AMA_3_S=" + std::to_string(m_ti.ama_6_a_1[3][1]) + ", AMA_3_B=" + std::to_string(m_ti.ama_6_a_1[3][2]) + ", AMA_4_G=" + std::to_string(m_ti.ama_6_a_1[2][0]) + ", AMA_4_S="
			+ std::to_string(m_ti.ama_6_a_1[2][1]) + ", AMA_4_B=" + std::to_string(m_ti.ama_6_a_1[2][2]) + ", AMA_5_G=" + std::to_string(m_ti.ama_6_a_1[1][0]) + ", AMA_5_S=" + std::to_string(m_ti.ama_6_a_1[1][1]) + ", AMA_5_B="
			+ std::to_string(m_ti.ama_6_a_1[1][2]) + ", AMA_6_G=" + std::to_string(m_ti.ama_6_a_1[0][0]) + ", AMA_6_S=" + std::to_string(m_ti.ama_6_a_1[0][1]) + ", AMA_6_B=" + std::to_string(m_ti.ama_6_a_1[0][2]) + ", PRO_1_G="
			+ std::to_string(m_ti.pro_1_a_7[0][0]) + ", PRO_1_S=" + std::to_string(m_ti.pro_1_a_7[0][1]) + ", PRO_1_B=" + std::to_string(m_ti.pro_1_a_7[0][2]) + ", PRO_2_G=" + std::to_string(m_ti.pro_1_a_7[1][0]) + ", PRO_2_S="
			+ std::to_string(m_ti.pro_1_a_7[1][1]) + ", PRO_2_B=" + std::to_string(m_ti.pro_1_a_7[1][2]) + ", PRO_3_G=" + std::to_string(m_ti.pro_1_a_7[2][0]) + ", PRO_3_S=" + std::to_string(m_ti.pro_1_a_7[2][1]) + ", PRO_3_B="
			+ std::to_string(m_ti.pro_1_a_7[2][2]) + ", PRO_4_G=" + std::to_string(m_ti.pro_1_a_7[3][0]) + ", PRO_4_S=" + std::to_string(m_ti.pro_1_a_7[3][1]) + ", PRO_4_B=" + std::to_string(m_ti.pro_1_a_7[3][2]) + ", PRO_5_G="
			+ std::to_string(m_ti.pro_1_a_7[4][0]) + ", PRO_5_S=" + std::to_string(m_ti.pro_1_a_7[4][1]) + ", PRO_5_B=" + std::to_string(m_ti.pro_1_a_7[4][2]) + ", PRO_6_G=" + std::to_string(m_ti.pro_1_a_7[5][0]) + ", PRO_6_S="
			+ std::to_string(m_ti.pro_1_a_7[5][1]) + ", PRO_6_B=" + std::to_string(m_ti.pro_1_a_7[5][2]) + ", PRO_7_G=" + std::to_string(m_ti.pro_1_a_7[6][0]) + ", PRO_7_S=" + std::to_string(m_ti.pro_1_a_7[6][1]) + ", PRO_7_B="
			+ std::to_string(m_ti.pro_1_a_7[6][2]) + "] do player");

	return r;
}

uint32_t CmdUpdateNormalTrofel::getUID() {
	return m_uid;
}

void CmdUpdateNormalTrofel::setUID(uint32_t _uid) {
	m_uid = _uid;
}

TrofelInfo& CmdUpdateNormalTrofel::getInfo() {
	return m_ti;
}

void CmdUpdateNormalTrofel::setInfo(TrofelInfo& _ti) {
	m_ti = _ti;
}
