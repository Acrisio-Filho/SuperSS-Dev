// Arquivo cmd_update_user_info.cpp
// Criado em 08/09/2018 as 12:42 por Acrisio
// Implementa��o da classe CmdUpdateUserInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_user_info.hpp"

using namespace stdA;

CmdUpdateUserInfo::CmdUpdateUserInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ui{0} {
}

CmdUpdateUserInfo::CmdUpdateUserInfo(uint32_t _uid, UserInfoEx& _ui, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ui(_ui) {
}

CmdUpdateUserInfo::~CmdUpdateUserInfo() {
}

void CmdUpdateUserInfo::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateUserInfo::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateUserInfo::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ui.best_drive) + ", " + std::to_string(m_ui.best_long_putt)
				+ ", " + std::to_string(m_ui.best_chip_in) + ", " + std::to_string(m_ui.combo) + ", " + std::to_string(m_ui.all_combo)
				+ ", " + std::to_string(m_ui.tacada) + ", " + std::to_string(m_ui.putt) + ", " + std::to_string(m_ui.tempo)
				+ ", " + std::to_string(m_ui.tempo_tacada) + ", " + std::to_string(m_ui.acerto_pangya) + ", " + std::to_string(m_ui.timeout)
				+ ", " + std::to_string(m_ui.ob) + ", " + std::to_string(m_ui.total_distancia) + ", " + std::to_string(m_ui.hole)
				+ ", " + std::to_string(m_ui.hole_in) + ", " + std::to_string(m_ui.hio) + ", " + std::to_string(m_ui.bunker)
				+ ", " + std::to_string(m_ui.fairway) + ", " + std::to_string(m_ui.albatross) + ", " + std::to_string(m_ui.mad_conduta)
				+ ", " + std::to_string(m_ui.putt_in) + ", " + /*std::to_string(m_ui.exp) + ", " + std::to_string(m_ui.level) exp, level e pang, n�o atualiza aqui, por que ele tem os seus pr�prios
				+ ", " + std::to_string(m_ui.pang) + ", " +*/ std::to_string(m_ui.media_score) + ", " + std::to_string(m_ui.best_score[0])
				+ ", " + std::to_string(m_ui.best_score[1]) + ", " + std::to_string(m_ui.best_score[2]) + ", " + std::to_string(m_ui.best_score[3])
				+ ", " + std::to_string(m_ui.best_score[4]) + ", " + std::to_string(m_ui.best_pang[0]) + ", " + std::to_string(m_ui.best_pang[1])
				+ ", " + std::to_string(m_ui.best_pang[2]) + ", " + std::to_string(m_ui.best_pang[3]) + ", " + std::to_string(m_ui.best_pang[4])
				+ ", " + std::to_string(m_ui.sum_pang) + ", " + std::to_string(m_ui.event_flag) + ", " + std::to_string(m_ui.jogado)
				+ ", " + std::to_string(m_ui.team_game) + ", " + std::to_string(m_ui.team_win) + ", " + std::to_string(m_ui.team_hole)
				+ ", " + std::to_string(m_ui.ladder_point) + ", " + std::to_string(m_ui.ladder_hole) + ", " + std::to_string(m_ui.ladder_win)
				+ ", " + std::to_string(m_ui.ladder_lose) + ", " + std::to_string(m_ui.ladder_draw) + ", " + std::to_string(m_ui.quitado)
				+ ", " + std::to_string(m_ui.skin_pang) + ", " + std::to_string(m_ui.skin_win) + ", " + std::to_string(m_ui.skin_lose)
				+ ", " + std::to_string(m_ui.skin_run_hole) + ", " + std::to_string(m_ui.skin_all_in_count) + ", " + std::to_string(m_ui.disconnect)
				+ ", " + std::to_string(m_ui.jogados_disconnect) + ", " + std::to_string(m_ui.event_value) + ", " + std::to_string(m_ui.skin_strike_point)
				+ ", " + std::to_string(m_ui.sys_school_serie) + ", " + std::to_string(m_ui.game_count_season) + ", " + std::to_string(m_ui.total_pang_win_game)
				+ ", " + std::to_string(m_ui.medal.lucky) + ", " + std::to_string(m_ui.medal.fast) + ", " + std::to_string(m_ui.medal.best_drive)
				+ ", " + std::to_string(m_ui.medal.best_chipin) + ", " + std::to_string(m_ui.medal.best_puttin) + ", " + std::to_string(m_ui.medal.best_recovery)
				+ ", " + std::to_string(m_ui._16bit_nao_sei)
	);

	checkResponse(r, "nao conseguiu atualizar o User Info do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateUserInfo::getUID() {
	return m_uid;
}

void CmdUpdateUserInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

UserInfoEx& CmdUpdateUserInfo::getInfo() {
	return m_ui;
}

void CmdUpdateUserInfo::setInfo(UserInfoEx& _ui) {
	m_ui = _ui;
}
