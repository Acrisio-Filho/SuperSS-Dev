// Arquivo cmd_user_info.cpp
// Criado em 18/03/2018 as 13:39 por Acrisio
// Implementação da classe CmdUserInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_user_info.hpp"

using namespace stdA;

CmdUserInfo::CmdUserInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ui{0} {
};

CmdUserInfo::CmdUserInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_ui{0} {
};

CmdUserInfo::~CmdUserInfo() {
};

void CmdUserInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(68, (uint32_t)_result->cols);

    auto i = 0u;

    m_ui.tacada = IFNULL(atoi, _result->data[1]);
	m_ui.putt = IFNULL(atoi, _result->data[2]);
	m_ui.tempo = IFNULL(atoi, _result->data[3]);
	m_ui.tempo_tacada = IFNULL(atoi, _result->data[4]);
	m_ui.best_drive = (float)IFNULL(atof, _result->data[5]);
	m_ui.acerto_pangya = IFNULL(atoi, _result->data[6]);
	m_ui.bunker = (short)IFNULL(atoi, _result->data[7]);
	m_ui.ob = IFNULL(atoi, _result->data[8]);
	m_ui.total_distancia = IFNULL(atoi, _result->data[9]);
	m_ui.hole = IFNULL(atoi, _result->data[10]);
	m_ui.hole_in = IFNULL(atoi, _result->data[11]);
	m_ui.hio = IFNULL(atoi, _result->data[12]);
	m_ui.timeout = IFNULL(atoi, _result->data[13]);
	m_ui.fairway = IFNULL(atoi, _result->data[14]);
	m_ui.albatross = IFNULL(atoi, _result->data[15]);
	m_ui.mad_conduta = IFNULL(atoi, _result->data[16]);
	m_ui.putt_in = IFNULL(atoi, _result->data[17]);
	m_ui.best_long_putt = (float)IFNULL(atof, _result->data[18]);
	m_ui.best_chip_in = (float)IFNULL(atof, _result->data[19]);
	m_ui.exp = IFNULL(atoi, _result->data[20]);
	m_ui.level = (unsigned char)IFNULL(atoi, _result->data[21]);
	m_ui.pang = IFNULL(atoll, _result->data[22]);
	m_ui.media_score = IFNULL(atoi, _result->data[23]);
	for (i = 0; i < 5; i++)
		m_ui.best_score[i] = (unsigned char)IFNULL(atoi, _result->data[24 + i]);	// 24 + 5
	for (i = 0; i < 5; i++)
		m_ui.best_pang[i] = IFNULL(atoll, _result->data[29 + i]);			// 29 + 5
	m_ui.sum_pang = IFNULL(atoll, _result->data[34]);
	m_ui.event_flag = (unsigned char)IFNULL(atoi, _result->data[35]);
	m_ui.jogado = IFNULL(atoi, _result->data[36]);
	m_ui.quitado = IFNULL(atoi, _result->data[37]);
	m_ui.skin_pang = IFNULL(atoll, _result->data[38]);
	m_ui.skin_win = IFNULL(atoi, _result->data[39]);
	m_ui.skin_lose = IFNULL(atoi, _result->data[40]);
	m_ui.skin_run_hole = IFNULL(atoi, _result->data[41]);
	m_ui.skin_strike_point = IFNULL(atoi, _result->data[42]);
	m_ui.skin_all_in_count = IFNULL(atoi, _result->data[43]);
	m_ui.all_combo = IFNULL(atoi, _result->data[44]);
	m_ui.combo = IFNULL(atoi, _result->data[45]);
	m_ui.team_win = IFNULL(atoi, _result->data[46]);
	m_ui.team_game = IFNULL(atoi, _result->data[47]);
	m_ui.team_hole = IFNULL(atoi, _result->data[48]);
	m_ui.ladder_point = IFNULL(atoi, _result->data[49]);
	m_ui.ladder_win = IFNULL(atoi, _result->data[50]);
	m_ui.ladder_lose = IFNULL(atoi, _result->data[51]);
	m_ui.ladder_draw = IFNULL(atoi, _result->data[52]);
	m_ui.ladder_hole = IFNULL(atoi, _result->data[53]);
	m_ui.event_value = (short)IFNULL(atoi, _result->data[54]);
	m_ui.disconnect = IFNULL(atoi, _result->data[55]);
	m_ui.sys_school_serie = IFNULL(atoi, _result->data[56]);
	m_ui.jogados_disconnect = IFNULL(atoi, _result->data[57]);
	m_ui.game_count_season = IFNULL(atoi, _result->data[58]);
	m_ui.medal.lucky = IFNULL(atoi, _result->data[60]);
	m_ui.medal.fast = IFNULL(atoi, _result->data[61]);
	m_ui.medal.best_drive = IFNULL(atoi, _result->data[62]);
	m_ui.medal.best_chipin = IFNULL(atoi, _result->data[63]);
	m_ui.medal.best_puttin = IFNULL(atoi, _result->data[64]);
	m_ui.medal.best_recovery = IFNULL(atoi, _result->data[65]);
	m_ui._16bit_nao_sei = (short)IFNULL(atoi, _result->data[66]);
	m_ui.total_pang_win_game = (uint32_t)IFNULL(atoi, _result->data[67]);
};

response* CmdUserInfo::prepareConsulta(database& _db) {

    m_ui.clear();

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o user info do player: " + std::to_string(m_uid));

	return r;
};

UserInfoEx& CmdUserInfo::getInfo() {
    return m_ui;
};

void CmdUserInfo::setInfo(UserInfoEx& _ui) {
    m_ui = _ui;
};

uint32_t CmdUserInfo::getUID() {
    return m_uid;
};

void CmdUserInfo::setUID(uint32_t _uid) {
    m_uid = _uid;   
};
