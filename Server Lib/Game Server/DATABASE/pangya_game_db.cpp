// Arquivo pangya_game_db.h
// Criado em 13/08/2017 por Acrisio
// Implementação da classe pangya_db para o Game Server

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "pangya_game_db.h"
#include "../../Projeto IOCP/UTIL/util_time.h"
#include <map>

#include "../../Projeto IOCP/TYPE/stda_error.h"

using namespace stdA;


pangya_db::pangya_db() : pangya_base_db() {
};

pangya_db::~pangya_db() {
};

player_info pangya_db::getPlayerInfo(int32_t uid) {
	exec_query query("SELECT a.uid, a.id, a.nick, a.password, a.capability, a.IDState, b.level FROM pangya.account a, pangya.user_info b WHERE a.uid = b.uid AND a.uid = " + std::to_string(uid));

	MED_RESULT_READ(&query)

	player_info pi;

	BEGIN_RESULT_READ(7, 1)

	pi.uid = IFNULL(atoi, _result->data[0]);
	if (_result->data[1] != nullptr)
		strcpy_s(pi.id, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(pi.nickname, _result->data[2]);
	if (_result->data[3] != nullptr)
		strcpy_s(pi.pass, _result->data[3]);
	//pi.cap = IFNULL(atoi, _result->data[4]);
	pi.block_flag.setIDState(IFNULL(atoi, _result->data[5]));
	pi.level = (unsigned short)IFNULL(atoi, _result->data[6]);

	END_RESULT_READ("erro ao pegar info do player: " + std::to_string(uid) + ".", 1)

	return pi;
};

std::string pangya_db::getAuthKey(int32_t uid) {
	exec_query query("pangya.ProcGeraAuthKey", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::string s;

	BEGIN_RESULT_READ(1, 5)

	s = std::string(_result->data[0]);

	END_RESULT_READ("erro ao pegar AuthKey do player: " + std::to_string(uid) + ".", 5)

	return s;
};

std::string pangya_db::getAuthKeyLogin(int32_t uid) {
	exec_query query("pangya.ProcGeraAuthKeyLogin", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::string s;

	BEGIN_RESULT_READ(1, 6)

	s = std::string(_result->data[0]);

	END_RESULT_READ("erro ao pegar AuthKeyLogin do player: " + std::to_string(uid) + ".", 6)

	return s;
};

MemberInfoEx pangya_db::getMemberInfo(int32_t uid) {

	exec_query query("pangya.ProcGetUserInfo", std::to_string(uid));

	MED_RESULT_READ(&query)

	MemberInfoEx mi;

	mi.clear();

	BEGIN_RESULT_READ(26, 3);

	if (_result->data[0] != nullptr)
		strcpy_s(mi.id, _result->data[0]);
	mi.uid = IFNULL(atoi, _result->data[1]);
	mi.sexo = (unsigned char)IFNULL(atoi, _result->data[2]);
	mi.do_tutorial = (unsigned char)IFNULL(atoi, _result->data[3]);
	if (_result->data[4] != nullptr)
		strcpy_s(mi.nick_name, _result->data[4]);
	mi.school = IFNULL(atoi, _result->data[5]);
	mi.capability = IFNULL(atoi, _result->data[6]);
	mi.manner_flag = IFNULL(atoi, _result->data[9]);
	if (_result->data[11] != nullptr)
		strcpy_s(mi.guild_name, _result->data[11]);
	mi.guild_uid = IFNULL(atoi, _result->data[12]);
	mi.guild_pang = IFNULL(atoll, _result->data[13]);
	mi.guild_point = IFNULL(atoi, _result->data[14]);
	mi.guild_mark_img_no = 1;		// POR ENQUANTO ESTÁ VALOR ESTÁTICO DEPOIS VOU PEGAR DO DB QUANDO FIZER LÁ DIREITO
	mi.event_1 = (unsigned char)IFNULL(atoi, _result->data[15]);
	mi.event_2 = (unsigned char)IFNULL(atoi, _result->data[16]);
	mi.limit_papel_shop = (unsigned char)IFNULL(atoi, _result->data[17]);
	mi.level = (unsigned char)IFNULL(atoi, _result->data[23]);
	if (_result->data[24] != nullptr)
		strcpy_s(mi.guild_mark_img, _result->data[24]);

	END_RESULT_READ("erro ao pegar o info do player: " + std::to_string(uid) + ".", 3)

	return mi;
};

UserInfo pangya_db::getUserInfo(int32_t uid) {
	exec_query query("pangya.GetInfo_User", std::to_string(uid));

	MED_RESULT_READ(&query)

	UserInfo ui;
	size_t i;

	ui.clear();

	BEGIN_RESULT_READ(67, 4)

	ui.tacada = IFNULL(atoi, _result->data[1]);
	ui.putt = IFNULL(atoi, _result->data[2]);
	ui.tempo = IFNULL(atoi, _result->data[3]);
	ui.tempo_tacada = IFNULL(atoi, _result->data[4]);
	ui.best_drive = (float)IFNULL(atof, _result->data[5]);
	ui.acerto_pangya = IFNULL(atoi, _result->data[6]);
	ui.bunker = (short)IFNULL(atoi, _result->data[7]);
	ui.ob = IFNULL(atoi, _result->data[8]);
	ui.total_distancia = IFNULL(atoi, _result->data[9]);
	ui.hole = IFNULL(atoi, _result->data[10]);
	ui.hole_in = IFNULL(atoi, _result->data[11]);
	ui.hio = IFNULL(atoi, _result->data[12]);
	ui.timeout = IFNULL(atoi, _result->data[13]);
	ui.fairway = IFNULL(atoi, _result->data[14]);
	ui.albatross = IFNULL(atoi, _result->data[15]);
	ui.mad_conduta = IFNULL(atoi, _result->data[16]);
	ui.putt_in = IFNULL(atoi, _result->data[17]);
	ui.best_long_putt = (float)IFNULL(atof, _result->data[18]);
	ui.best_chip_in = (float)IFNULL(atof, _result->data[19]);
	ui.exp = IFNULL(atoi, _result->data[20]);
	ui.level = (unsigned char)IFNULL(atoi, _result->data[21]);
	ui.pang = IFNULL(atoll, _result->data[22]);
	ui.media_score = IFNULL(atoi, _result->data[23]);
	for (i = 0; i < 5; i++)
		ui.best_score[i] = (unsigned char)IFNULL(atoi, _result->data[24 + i]);	// 24 + 5
	for (i = 0; i < 5; i++)
		ui.best_pang[i] = IFNULL(atoll, _result->data[29 + i]);			// 29 + 5
	ui.sum_pang = IFNULL(atoll, _result->data[34]);
	ui.event_flag = (unsigned char)IFNULL(atoi, _result->data[35]);
	ui.jogado = IFNULL(atoi, _result->data[36]);
	ui.quitado = IFNULL(atoi, _result->data[37]);
	ui.skin_pang = IFNULL(atoll, _result->data[38]);
	ui.skin_win = IFNULL(atoi, _result->data[39]);
	ui.skin_lose = IFNULL(atoi, _result->data[40]);
	ui.skin_run_hole = IFNULL(atoi, _result->data[41]);
	ui.skin_strike_point = IFNULL(atoi, _result->data[42]);
	ui.skin_all_in_count = IFNULL(atoi, _result->data[43]);
	ui.all_combo = IFNULL(atoi, _result->data[44]);
	ui.combo = IFNULL(atoi, _result->data[45]);
	ui.team_win = IFNULL(atoi, _result->data[46]);
	ui.team_game = IFNULL(atoi, _result->data[47]);
	ui.team_hole = IFNULL(atoi, _result->data[48]);
	ui.ladder_point = IFNULL(atoi, _result->data[49]);
	ui.ladder_win = IFNULL(atoi, _result->data[50]);
	ui.ladder_lose = IFNULL(atoi, _result->data[51]);
	ui.ladder_draw = IFNULL(atoi, _result->data[52]);
	ui.ladder_hole = IFNULL(atoi, _result->data[53]);
	ui.event_value = (short)IFNULL(atoi, _result->data[54]);
	ui.nao_sei = IFNULL(atoi, _result->data[55]);
	ui.max_jogos_nao_sei = IFNULL(atoi, _result->data[56]);
	ui.jogos_nao_sei = IFNULL(atoi, _result->data[57]);
	ui.game_count_season = IFNULL(atoi, _result->data[58]);
	ui.lucky_medal = IFNULL(atoi, _result->data[60]);
	ui.fast_medal = IFNULL(atoi, _result->data[61]);
	ui.best_drive_medal = IFNULL(atoi, _result->data[62]);
	ui.best_chip_in_medal = IFNULL(atoi, _result->data[63]);
	ui.best_putt_in_medal = IFNULL(atoi, _result->data[64]);
	ui.best_recovery_medal = IFNULL(atoi, _result->data[65]);
	ui._16bit_nao_sei = (short)IFNULL(atoi, _result->data[66]);

	END_RESULT_READ("erro ao pegar User Info do player: " + std::to_string(uid) + ".", 4)

	return ui;
};

TrofelInfo pangya_db::getTrofelInfo(int32_t uid, int season) {
	exec_query query("pangya.ProcGetTrofel", std::to_string(uid) + ", " + std::to_string(season));

	MED_RESULT_READ(&query)

	TrofelInfo ti;
	size_t i;

	ti.clear();

	BEGIN_RESULT_READ(39, 5)

	for (i = 0; i < 3; i++)
		ti.ama_6[i] = (short)IFNULL(atoi, _result->data[0 + i]);	// 0 + 3
	for (i = 0; i < 3; i++)
		ti.ama_5[i] = (short)IFNULL(atoi, _result->data[3 + i]);	// 3 + 3
	for (i = 0; i < 3; i++)
		ti.ama_4[i] = (short)IFNULL(atoi, _result->data[6 + i]);	// 6 + 3
	for (i = 0; i < 3; i++)
		ti.ama_3[i] = (short)IFNULL(atoi, _result->data[9 + i]);	// 9 + 3
	for (i = 0; i < 3; i++)
		ti.ama_2[i] = (short)IFNULL(atoi, _result->data[12 + i]);	// 12 + 3
	for (i = 0; i < 3; i++)
		ti.ama_1[i] = (short)IFNULL(atoi, _result->data[15 + i]);	// 15 + 3
	for (i = 0; i < 3; i++)
		ti.pro_1[i] = (short)IFNULL(atoi, _result->data[18 + i]);	// 18 + 3
	for (i = 0; i < 3; i++)
		ti.pro_2[i] = (short)IFNULL(atoi, _result->data[21 + i]);	// 21 + 3
	for (i = 0; i < 3; i++)
		ti.pro_3[i] = (short)IFNULL(atoi, _result->data[24 + i]);	// 24 + 3
	for (i = 0; i < 3; i++)
		ti.pro_4[i] = (short)IFNULL(atoi, _result->data[27 + i]);	// 27 + 3
	for (i = 0; i < 3; i++)
		ti.pro_5[i] = (short)IFNULL(atoi, _result->data[30 + i]);	// 30 + 3
	for (i = 0; i < 3; i++)
		ti.pro_6[i] = (short)IFNULL(atoi, _result->data[33 + i]);	// 33 + 3
	for (i = 0; i < 3; i++)
		ti.pro_7[i] = (short)IFNULL(atoi, _result->data[36 + i]);	// 36 + 3

	END_RESULT_READ("erro ao pegar Trofel Info do player: " + std::to_string(uid) + ".", 5)

	return ti;
};

UserEquip pangya_db::getUserEquip(int32_t uid) {
	exec_query query("pangya.USP_CHAR_USER_EQUIP", std::to_string(uid));

	MED_RESULT_READ(&query)

	UserEquip ue;
	size_t i;

	ue.clear();

	BEGIN_RESULT_READ(29, 6)

	ue.caddie_id = IFNULL(atoi, _result->data[0]);
	ue.character_id = IFNULL(atoi, _result->data[1]);
	ue.clubset_id = IFNULL(atoi, _result->data[2]);
	ue.ball_typeid = IFNULL(atoi, _result->data[3]);
	for (i = 0; i < 10; i++)
		ue.item_slot[i] = IFNULL(atoi, _result->data[4 + i]);		// 4 + 10
	for (i = 0; i < 6; i++)
		ue.skin_id[i] = IFNULL(atoi, _result->data[14 + i]);		// 14 + 6
	for (i = 0; i < 6; i++)
		ue.skin_typeid[i] = IFNULL(atoi, _result->data[20 + i]);	// 20 + 6
	ue.mascot_id = IFNULL(atoi, _result->data[26]);
	for (i = 0; i < 2; i++)
		ue.poster[i] = IFNULL(atoi, _result->data[27 + i]);		// 27 + 2

	END_RESULT_READ("erro ao pegar Itens Equipados do player: " + std::to_string(uid) + ".", 6)

	return ue;
};

std::vector< MapStatistics > pangya_db::getMapStatistics(int32_t uid, int season) {
	exec_query query("pangya.ProcGetMapStatistics", std::to_string(uid) + ", " + std::to_string(season));

	MED_RESULT_READ(&query)

	MapStatistics ms;
	std::vector< MapStatistics > v_ms;

	BEGIN_RESULT_READ(14, 7)

	ms.clear();

	//ms.tipo = (unsigned char)IFNULL(atoi, _result->data[1]);
	ms.course = (unsigned char)IFNULL(atoi, _result->data[2]);
	ms.tacada = IFNULL(atoi, _result->data[3]);
	ms.putt = IFNULL(atoi, _result->data[4]);
	ms.hole = IFNULL(atoi, _result->data[5]);
	ms.fairway = IFNULL(atoi, _result->data[6]);
	ms.hole_in = IFNULL(atoi, _result->data[7]);
	ms.putt_in = IFNULL(atoi, _result->data[8]);
	ms.total_score = IFNULL(atoi, _result->data[9]);
	ms.best_score = (unsigned char)IFNULL(atoi, _result->data[10]);
	ms.best_pang = IFNULL(atoll, _result->data[11]);
	ms.character_typeid = IFNULL(atoi, _result->data[12]);
	ms.event_score = (unsigned char)IFNULL(atoi, _result->data[13]);

	v_ms.push_back(ms);

	END_RESULT_READ("erro ao pegar map statistics do player: " + std::to_string(uid) + ", season: " + std::to_string(season) + ".", 7)

	return v_ms;
};

std::vector< MapStatistics > pangya_db::getMapStatisticsAssist(int32_t uid, int season) {
	exec_query query("pangya.ProcGetMapStatisticsAssist", std::to_string(uid) + ", " + std::to_string(season));

	MED_RESULT_READ(&query)

	MapStatistics ms;
	std::vector< MapStatistics > v_ms;

	BEGIN_RESULT_READ(14, 36)

	ms.clear();

	//ms.tipo = (unsigned char)IFNULL(atoi, _result->data[1]);
	ms.course = (unsigned char)IFNULL(atoi, _result->data[2]);
	ms.tacada = IFNULL(atoi, _result->data[3]);
	ms.putt = IFNULL(atoi, _result->data[4]);
	ms.hole = IFNULL(atoi, _result->data[5]);
	ms.fairway = IFNULL(atoi, _result->data[6]);
	ms.hole_in = IFNULL(atoi, _result->data[7]);
	ms.putt_in = IFNULL(atoi, _result->data[8]);
	ms.total_score = IFNULL(atoi, _result->data[9]);
	ms.best_score = (unsigned char)IFNULL(atoi, _result->data[10]);
	ms.best_pang = IFNULL(atoll, _result->data[11]);
	ms.character_typeid = IFNULL(atoi, _result->data[12]);
	ms.event_score = (unsigned char)IFNULL(atoi, _result->data[13]);

	v_ms.push_back(ms);

	END_RESULT_READ("erro ao pegar map statistics assist do player: " + std::to_string(uid) + ", season: " + std::to_string(season) + ".", 36)

	return v_ms;
};

std::vector< CharacterInfo > pangya_db::getCharacterInfo(int32_t uid) {
	exec_query query("pangya.USP_CHAR_EQUIP_LOAD_S4", std::to_string(uid));

	MED_RESULT_READ(&query)

	CharacterInfo ce;
	std::vector< CharacterInfo > v_ce;
	size_t i;

	BEGIN_RESULT_READ(81, 8)

	ce.clear();

	ce.id = IFNULL(atoi, _result->data[0]);
	ce._typeid = IFNULL(atoi, _result->data[1]);
	for (i = 0; i < 24; i++)
		ce.parts_id[i] = IFNULL(atoi, _result->data[2 + i]);		// 2 + 24
	for (i = 0; i < 24; i++)
		ce.parts_typeid[i] = IFNULL(atoi, _result->data[26 + i]);	// 26 + 24
	ce.default_hair = (unsigned char)IFNULL(atoi, _result->data[50]);
	ce.default_shirts = (unsigned char)IFNULL(atoi, _result->data[51]);
	ce.gift_flag = (unsigned char)IFNULL(atoi, _result->data[52]);
	for (i = 0; i < 5; i++)
		ce.pcl[i] = (unsigned char)IFNULL(atoi, _result->data[53 + i]);	// 53 + 5
	ce.purchase = (unsigned char)IFNULL(atoi, _result->data[58]);
	for (i = 0; i < 5; i++)
		ce.auxparts[i] = IFNULL(atoi, _result->data[59 + i]);				// 59 + 5
	for (i = 0; i < 4; i++)
		ce.cut_in[i] = IFNULL(atoi, _result->data[64 + i]);					// 64 + 4 Cut-in deveria guarda no db os outros 3 se for msm os 4 que penso q seja, é sim no JP USA os 4
	ce.mastery = IFNULL(atoi, _result->data[68]);
	for (i = 0; i < 4; i++)
		ce.card_character[i] = IFNULL(atoi, _result->data[69 + i]);		// 69 + 4
	for (i = 0; i < 4; i++)
		ce.card_caddie[i] = IFNULL(atoi, _result->data[73 + i]);			// 73 + 4
	for (i = 0; i < 4; i++)
		ce.card_NPC[i] = IFNULL(atoi, _result->data[77 + i]);				// 77 + 4

	v_ce.push_back(ce);

	END_RESULT_READ("erro ao pegar characters equip do player: " + std::to_string(uid) + ".", 8)

	return v_ce;
};

CharacterInfo pangya_db::getCharacterInfoOne(int32_t uid, int32_t character_id) {
	exec_query query("pangya.USP_CHAR_EQUIP_LOAD_S4_ONE", std::to_string(uid) + ", " + std::to_string(character_id));

	MED_RESULT_READ(&query)

	CharacterInfo ce;
	size_t i;

	BEGIN_RESULT_READ(81, 34)

	ce.clear();

	ce.id = IFNULL(atoi, _result->data[0]);
	ce._typeid = IFNULL(atoi, _result->data[1]);
	for (i = 0; i < 24; i++)
		ce.parts_id[i] = IFNULL(atoi, _result->data[2 + i]);		// 2 + 24
	for (i = 0; i < 24; i++)
		ce.parts_typeid[i] = IFNULL(atoi, _result->data[26 + i]);	// 26 + 24
	ce.default_hair = (unsigned char)IFNULL(atoi, _result->data[50]);
	ce.default_shirts = (unsigned char)IFNULL(atoi, _result->data[51]);
	ce.gift_flag = (unsigned char)IFNULL(atoi, _result->data[52]);
	for (i = 0; i < 5; i++)
		ce.pcl[i] = (unsigned char)IFNULL(atoi, _result->data[53 + i]);	// 53 + 5
	ce.purchase = (unsigned char)IFNULL(atoi, _result->data[58]);
	for (i = 0; i < 5; i++)
		ce.auxparts[i] = IFNULL(atoi, _result->data[59 + i]);				// 59 + 5
	for (i = 0; i < 4; i++)
		ce.cut_in[i] = IFNULL(atoi, _result->data[64 + i]);					// 64 + 4 Cut-in deveria guarda no db os outros 3 se for msm os 4 que penso q seja
	ce.mastery = IFNULL(atoi, _result->data[68]);
	for (i = 0; i < 4; i++)
		ce.card_character[i] = IFNULL(atoi, _result->data[69 + i]);			// 69 + 4
	for (i = 0; i < 4; i++)
		ce.card_caddie[i] = IFNULL(atoi, _result->data[73 + i]);			// 73 + 4
	for (i = 0; i < 4; i++)
		ce.card_NPC[i] = IFNULL(atoi, _result->data[77 + i]);				// 77 + 4

	END_RESULT_READ("erro ao pegar character equip do player: " + std::to_string(uid) + ", character id: " + std::to_string(character_id) + ".", 34)

	return ce;
};

std::vector< CaddieInfo > pangya_db::getCaddieInfo(int32_t uid) {
	exec_query query("pangya.ProcGetCaddieInfo", std::to_string(uid));

	MED_RESULT_READ(&query)

	CaddieInfo ci;
	std::vector< CaddieInfo > v_ci;

	BEGIN_RESULT_READ(11, 9)

	ci.clear();

	ci.id = IFNULL(atoi, _result->data[0]);
	ci._typeid = IFNULL(atoi, _result->data[2]);
	ci.parts_typeid = IFNULL(atoi, _result->data[3]);
	ci.level = (unsigned char)IFNULL(atoi, _result->data[4]);
	ci.exp = IFNULL(atoi, _result->data[5]);
	ci.rent_flag = (unsigned char)IFNULL(atoi, _result->data[6]);
	ci.end_date_unix = (short)IFNULL(atoi, _result->data[7]);
	ci.purchase = (unsigned char)IFNULL(atoi, _result->data[8]);
	ci.parts_end_date_unix = (short)IFNULL(atoi, _result->data[9]);
	ci.check_end = (short)IFNULL(atoi, _result->data[10]);

	v_ci.push_back(ci);

	END_RESULT_READ("erro ao pegar caddie info do player: " + std::to_string(uid) + ".", 9)

	return v_ci;
};

ClubSetInfo pangya_db::getClubSetInfo(int32_t uid, int32_t clubset_id) {
	exec_query query("SELECT a.item_id, a.typeid, (a.c0 + a.ClubSet_WorkShop_C0) as c0, \
					  (a.c1 + a.ClubSet_WorkShop_C1) as c1, (a.c2 + a.ClubSet_WorkShop_C2) as c2, \
					  (a.c3 + a.ClubSet_WorkShop_C3) as c3, (a.c4 + a.ClubSet_WorkShop_C4) as c4, \
					  b.c0 as slot_c0, b.c1 as slot_c1, b.c2 as slot_c2, b.c3 as slot_c3, b.c4 as slot_c4 \
					 FROM pangya.pangya_item_warehouse a, pangya.pangya_clubset_enchant b \
					 WHERE a.uid = b.uid AND a.item_id = b.item_id AND a.uid = " + std::to_string(uid) +
					 " AND a.item_id = " + std::to_string(clubset_id));

	MED_RESULT_READ(&query)

	ClubSetInfo csi = {};
	size_t i;

	BEGIN_RESULT_READ(12, 10)

	csi.id = IFNULL(atoi, _result->data[0]);
	csi._typeid = IFNULL(atoi, _result->data[1]);
	for (i = 0; i < 5; i++)
		csi.enchant_c[i] = (short)IFNULL(atoi, _result->data[2 + i]);		// 2 + 5
	for (i = 0; i < 5; i++)
		csi.slot_c[i] = (short)IFNULL(atoi, _result->data[7 + i]);		// 7 + 5

	END_RESULT_READ("erro ao pegar club info ID: " + std::to_string(clubset_id) + " do player: " + std::to_string(uid) + ".", 10)

	return csi;
};

std::vector< MascotInfoEx > pangya_db::getMascotInfo(int32_t uid) {
	exec_query query("pangya.ProcGetMascotInfo", std::to_string(uid));

	MED_RESULT_READ(&query)

	MascotInfoEx mi;
	std::vector< MascotInfoEx > v_mi;

	BEGIN_RESULT_READ(10, 11)

	mi.clear();

	mi.id = IFNULL(atoi, _result->data[0]);
	mi._typeid = IFNULL(atoi, _result->data[2]);
	mi.level = (unsigned char)IFNULL(atoi, _result->data[3]);
	mi.exp = IFNULL(atoi, _result->data[4]);
	mi.flag = (unsigned char)IFNULL(atoi, _result->data[5]);
	if (_result->data[6] != nullptr)
		strcpy_s(mi.message, _result->data[6]);
	mi.tipo = (short)IFNULL(atoi, _result->data[7]);
	mi.is_cash = (unsigned char)IFNULL(atoi, _result->data[8]);
	translateDate(_result->data[9], &mi.data);

	v_mi.push_back(mi);

	END_RESULT_READ("erro ao pegar mascot info do player: " + std::to_string(uid) + ".", 11)

	return v_mi;
};

std::vector< WarehouseItem > pangya_db::getWarehouseItem(int32_t uid) {
	exec_query query("pangya.ProcGetWarehouseItem", std::to_string(uid));

	MED_RESULT_READ(&query)

	WarehouseItem wi;
	std::vector< WarehouseItem > v_wi;
	size_t i;

	BEGIN_RESULT_READ(45, 12)

	wi.clear();

	wi.id = IFNULL(atoi, _result->data[0]);
	wi._typeid = IFNULL(atoi, _result->data[2]);
	wi.ano = IFNULL(atoi, _result->data[3]);
	for (i = 0; i < 5; i++)
		wi.c[i] = (short)IFNULL(atoi, _result->data[4 + i]);			// 4 + 5
	wi.purchase = (unsigned char)IFNULL(atoi, _result->data[9]);
	wi.flag = (unsigned char)IFNULL(atoi, _result->data[11]);
	wi.apply_date = IFNULL(atoll, _result->data[12]);
	wi.end_date = IFNULL(atoll, _result->data[13]);
	wi.type = (unsigned char)IFNULL(atoi, _result->data[14]);
	for (i = 0; i < 4; i++)
		wi.card.character[i] = IFNULL(atoi, _result->data[15 + i]);	// 15 + 4
	for (i = 0; i < 4; i++)
		wi.card.caddie[i] = IFNULL(atoi, _result->data[19 + i]);		// 19 + 4
	for (i = 0; i < 4; i++)
		wi.card.NPC[i] = IFNULL(atoi, _result->data[23 + i]);			// 23 + 4
	wi.clubset_workshop.flag = (short)IFNULL(atoi, _result->data[27]);
	for (i = 0; i < 5; i++)
		wi.clubset_workshop.c[i] = (short)IFNULL(atoi, _result->data[28 + i]);	// 28 + 5
	wi.clubset_workshop.mastery = IFNULL(atoi, _result->data[33]);
	wi.clubset_workshop.recovery_pts = IFNULL(atoi, _result->data[34]);
	wi.clubset_workshop.level = IFNULL(atoi, _result->data[35]);
	wi.clubset_workshop.rank = IFNULL(atoi, _result->data[36]);
	if (_result->data[37] != nullptr)
		strcpy_s(wi.ucc.name, _result->data[37]);
	if (_result->data[38] != nullptr)
		strcpy_s(wi.ucc.idx, _result->data[38]);
	wi.ucc.seq = (short)IFNULL(atoi, _result->data[39]);
	if (_result->data[40] != nullptr)
		strcpy_s(wi.ucc.copier_nick, _result->data[40]);
	wi.ucc.copier = IFNULL(atoi, _result->data[41]);
	wi.ucc.trade = (unsigned char)IFNULL(atoi, _result->data[42]);
	//wi.sd_flag = (unsigned char)IFNULL(atoi, _result->data[43]);
	wi.ucc.status = (unsigned char)IFNULL(atoi, _result->data[44]);

	v_wi.push_back(wi);

	END_RESULT_READ("erro ao pegar WarehouseItem do player: " + std::to_string(uid) + ".", 12)

	return v_wi;
};

std::string pangya_db::GeraWebKey(int32_t uid) {
	exec_query query("pangya.ProcGeraWeblinkKey", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::string webKey;

	BEGIN_RESULT_READ(1, 13)

	webKey = _result->data[0];

	END_RESULT_READ("erro ao gerar Weblink Key para o player: " + std::to_string(uid) + ".", 13)

	return webKey;
};

int32_t pangya_db::addItem(int32_t _uid, CharacterInfo& _item, unsigned short _gift_flag, unsigned short _purchase) {
	exec_query query("pangya.ProcAddCharacter", std::to_string(_uid) + ", " + std::to_string(_item.id) 
			+ ", " + std::to_string(_item._typeid)
			+ ", " + std::to_string((unsigned short)_item.default_hair)
			+ ", " + std::to_string((unsigned short)_item.default_shirts)
			+ ", " + std::to_string(_purchase)
			+ ", " + std::to_string(_gift_flag)
			+ ", " + std::to_string(_item.parts_typeid[0])
			+ ", " + std::to_string(_item.parts_typeid[1])
			+ ", " + std::to_string(_item.parts_typeid[2])
			+ ", " + std::to_string(_item.parts_typeid[3])
			+ ", " + std::to_string(_item.parts_typeid[4])
			+ ", " + std::to_string(_item.parts_typeid[5])
			+ ", " + std::to_string(_item.parts_typeid[6])
			+ ", " + std::to_string(_item.parts_typeid[7])
			+ ", " + std::to_string(_item.parts_typeid[8])
			+ ", " + std::to_string(_item.parts_typeid[9])
			+ ", " + std::to_string(_item.parts_typeid[10])
			+ ", " + std::to_string(_item.parts_typeid[11])
			+ ", " + std::to_string(_item.parts_typeid[12])
			+ ", " + std::to_string(_item.parts_typeid[13])
			+ ", " + std::to_string(_item.parts_typeid[14])
			+ ", " + std::to_string(_item.parts_typeid[15])
			+ ", " + std::to_string(_item.parts_typeid[16])
			+ ", " + std::to_string(_item.parts_typeid[17])
			+ ", " + std::to_string(_item.parts_typeid[18])
			+ ", " + std::to_string(_item.parts_typeid[19])
			+ ", " + std::to_string(_item.parts_typeid[20])
			+ ", " + std::to_string(_item.parts_typeid[21])
			+ ", " + std::to_string(_item.parts_typeid[22])
			+ ", " + std::to_string(_item.parts_typeid[23])
	);

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(1, 61);

	_item.id = IFNULL(atoi, _result->data[0]);

	END_RESULT_READ("erro ao adicionar o character: " + std::to_string(_item._typeid) + ", para o player: " + std::to_string(_uid) + ".", 61);

	return _item.id;
};

int32_t pangya_db::addItem(int32_t _uid, CaddieInfo& _item, unsigned short _gift_flag, unsigned short _purchase) {
	exec_query query("pangya.ProcAddCaddie", std::to_string(_uid) + ", " + std::to_string(_item.id)
		+ ", " + std::to_string(_item._typeid)
		+ ", " + std::to_string(_gift_flag)
		+ ", " + std::to_string(_purchase)
		+ ", " + std::to_string(_item.rent_flag)
		+ ", " + std::to_string(_item.end_date_unix)
	);

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(2, 62);

	_item.id = IFNULL(atoi, _result->data[0]);
	_item.end_date_unix = (unsigned short)IFNULL(atoi, _result->data[1]);

	END_RESULT_READ("error ao add o caddie: " + std::to_string(_item._typeid) + ", para o player: " + std::to_string(_uid) + ".", 62);

	return _item.id;
};

int32_t pangya_db::addItem(int32_t _uid, MascotInfoEx& _item, unsigned short _gift_flag, unsigned short _purchase) {
	return 0;
};

int32_t pangya_db::addItem(int32_t _uid, MyRoomItem& _item, unsigned short _gift_flag, unsigned short _purchase) {
	return 0;
};

int32_t pangya_db::addItem(int32_t _uid, CounterItemInfo& _item, unsigned short _gift_flag, unsigned short _purchase) {
	return 0;
};

int32_t pangya_db::addItem(int32_t _uid, WarehouseItem& _item, unsigned short _flag_iff, unsigned short _gift_flag, unsigned short _purchase) {
	exec_query query("pangya.USP_ADD_ITEM",
			std::to_string(_uid) + ", "
		+ std::to_string(_gift_flag) + ", "
		+ std::to_string(_purchase) + ", "
		+ std::to_string(_item.id) + ", "
		+ std::to_string(_item._typeid) + ", "
		+ std::to_string(_item.type) + ", "
		+ std::to_string(_flag_iff) + ", "
		+ std::to_string(_item.c[4]) + ", "
		+ std::to_string(_item.c[0]) + ", "
		+ std::to_string(_item.c[1]) + ", "
		+ std::to_string(_item.c[2]) + ", "
		+ std::to_string(_item.c[3]) + ", "
		+ std::to_string(_item.c[4]) + ", "
		+ std::to_string(0.f) + ", "
		+ std::to_string(0.f) + ", "
		+ std::to_string(0.f) + ", "
		+ std::to_string(0.f));
		
	MED_RESULT_READ(&query);
		
	int32_t id = 0l;
		
	BEGIN_RESULT_READ(0, 47);
		
	if (num_result == 1)
		_item.id = id = IFNULL(atoi, _result->data[0]);
	else if (num_result == 0 && (_flag_iff == IFF::Part::UCC_BLANK || _flag_iff == IFF::Part::UCC_COPY)) {
		_item.id = IFNULL(atoi, _result->data[0]);
		if (_result->data[1] != nullptr)
			strcpy_s(_item.ucc.idx, _result->data[1]);
		_item.ucc.seq = (unsigned short)IFNULL(atoi, _result->data[2]);
	}
		
	END_RESULT_READ("error ao add item para o player: " + std::to_string(_uid) + ".", 47);
		
	return id;
};

int32_t pangya_db::addItem(int32_t _uid, AchievementInfo& _item, unsigned short _gift_flag, unsigned short _purchase) {
	return 0;
};

int32_t pangya_db::addItem(int32_t _uid, CardInfo& _item, unsigned short _gift_flag, unsigned short _purchase) {
	return 0;
};

int32_t pangya_db::addHairStyle(int32_t _uid, CharacterInfo& _item) {
	exec_query query("UPDATE pangya.pangya_character_information \
		SET default_hair = " + std::to_string((unsigned short)_item.default_hair) 
		+ " WHERE UID = " + std::to_string(_uid) 
		+ " AND item_id = " + std::to_string(_item.id));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 60);

	END_RESULT_READ("erro ao atualizar o hairstyle do character: " + std::to_string(_item.id) + ", do player: " + std::to_string(_uid) + ".", 60);

	return _item.id;
};

int32_t pangya_db::addCaddieItem(int32_t _uid, CaddieInfo& _item) {
	return 0;
};

int32_t pangya_db::addTrofelSpecial(int32_t _uid, uint32_t trofel_typeid) {
	return 0;
};

int32_t pangya_db::addTrofelGrandPrix(int32_t _uid, uint32_t trofel_typeid) {
	return 0;
};

TutorialInfo pangya_db::getTutorialInfo(int32_t uid) {
	exec_query query("pangya.GetTutorial", std::to_string(uid));

	MED_RESULT_READ(&query)

	TutorialInfo ti = { 0 };

	BEGIN_RESULT_READ(3, 14)

	ti.rookie = IFNULL(atoi, _result->data[0]);
	ti.beginner = IFNULL(atoi, _result->data[1]);
	ti.advancer = IFNULL(atoi, _result->data[2]);

	END_RESULT_READ("erro ao pegar tutorial info do player: " + std::to_string(uid) + ".", 14)

	return ti;
};

uint64_t pangya_db::getCookie(int32_t uid) {
	exec_query query("SELECT cookie FROM pangya.user_info WHERE uid = " + std::to_string(uid));

	MED_RESULT_READ(&query)

	uint64_t cookie;

	BEGIN_RESULT_READ(1, 15)

	cookie = IFNULL(atoll, _result->data[0]);

	END_RESULT_READ("erro ao pegar os cookies do player: " + std::to_string(uid) + ".", 15)

	return cookie;

};

CouponGacha pangya_db::getCouponGacha(int32_t uid) {
	exec_query query("SELECT c0 FROM pangya.pangya_item_warehouse WHERE typeid = 0x1A000080 AND uid = " + std::to_string(uid) + ";\
		SELECT c0 FROM pangya.pangya_item_warehouse WHERE typeid = 0x1A000083 AND uid = " + std::to_string(uid));

	MED_RESULT_READ(&query)

	size_t num_query = 0;
	CouponGacha cg;
	cg.clear();

	BEGIN_RESULT_READ(1, 16)

	if (num_query == 0)
		cg.normal_ticket = IFNULL(atoi, _result->data[0]);
	else
		cg.partial_ticket = IFNULL(atoi, _result->data[0]);
		
	num_query++;

	END_RESULT_READ("erro nao conseguiu pegar os coupons gacha do player: " + std::to_string(uid) + ".", 16)

	return cg;
};

std::vector< CourseRewardInfo > pangya_db::getCourseRewardInfo() {
	exec_query query("pangya.ProcGetCourseReward", "");

	MED_RESULT_READ(&query)

	std::vector< CourseRewardInfo > v_cri;
	CourseRewardInfo cri;

	BEGIN_RESULT_READ(2, 17)

	cri.clear();

	cri.course = (unsigned char)IFNULL(atoi, _result->data[0]);
	cri.point = IFNULL(atoi, _result->data[1]);

	v_cri.push_back(cri);

	END_RESULT_READ("erro ao pegar o course info.", 17)

	return v_cri;

};

std::vector< CounterItemInfo > pangya_db::getCounterItemInfo(int32_t uid) {
	exec_query query("pangya.ProcGetCountItem", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::vector< CounterItemInfo > v_cii;
	CounterItemInfo cii;

	BEGIN_RESULT_READ(4, 18)

	cii.clear();

	cii.tipo = (unsigned char)IFNULL(atoi, _result->data[0]);
	cii._typeid = IFNULL(atoi, _result->data[1]);
	cii.id = IFNULL(atoi, _result->data[2]);
	cii.count_num = IFNULL(atoi, _result->data[3]);

	v_cii.push_back(cii);

	END_RESULT_READ("erro ao pegar Counter Item Info do player: " + std::to_string(uid) + ".", 18)

	return v_cii;
};

std::map< size_t, AchievementInfo > pangya_db::getAchievementInfo(int32_t uid) {
	//exec_query query("pangya.ProcGetAchieveType", std::to_string(uid));
	exec_query query("pangya.ProcGetAchievement_New", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::map< size_t, AchievementInfo > map_ai;
	std::map< size_t, AchievementInfo >::iterator i;
	AchievementInfo ai;
	QuestStuffInfoEx qsi;

	BEGIN_RESULT_READ(9, 19)

	ai.clear();
	qsi.clear();

	ai._typeid = IFNULL(atoi, _result->data[1]);

	qsi._typeid = IFNULL(atoi, _result->data[4]);//atol(_result->data[4]);
	qsi.count_typeid = IFNULL(atoi, _result->data[5]);
	qsi.count_id = IFNULL(atoi, _result->data[6]);
	qsi.count_num = IFNULL(atoi, _result->data[7]);
	qsi.clear_date_unix = IFNULL(atoi, _result->data[8]);

	if ((i = map_ai.find(ai._typeid)) == map_ai.end()) {
		ai.tipo = (unsigned char)IFNULL(atoi, _result->data[0]);
		ai.id = IFNULL(atoi, _result->data[2]);
		ai.option = IFNULL(atoi, _result->data[3]);

		ai.v_qsi.push_back(qsi);

		map_ai[ai._typeid] = ai;
	}else
		i->second.v_qsi.push_back(qsi);
		
	//ai.v_qsi = getQuestStuffInfo(ai.id);

	END_RESULT_READ("erro ao pegar achievement info do player: " + std::to_string(uid) + ".", 19)

	return map_ai;//v_ai;
};

std::vector< CardInfo > pangya_db::getCardInfo(int32_t uid) {
	exec_query query("pangya.ProcGetCardInfo", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::vector< CardInfo > v_ci;
	CardInfo ci;

	BEGIN_RESULT_READ(11, 20)

	ci.clear();

	ci.id = IFNULL(atoi, _result->data[0]);
	ci._typeid = IFNULL(atoi, _result->data[2]);
	ci.slot = IFNULL(atoi, _result->data[3]);
	ci.efeito = IFNULL(atoi, _result->data[4]);
	ci.efeito_qntd = IFNULL(atoi, _result->data[5]);
	ci.qntd = IFNULL(atoi, _result->data[6]);
	if (_result->data[7] != nullptr)
		translateDate(_result->data[7], &ci.use_date);
	if (_result->data[8] != nullptr)
		translateDate(_result->data[8], &ci.end_date);
	ci.type = (unsigned char)IFNULL(atoi, _result->data[9]);
	ci.use_yn = (unsigned char)IFNULL(atoi, _result->data[10]);

	v_ci.push_back(ci);

	END_RESULT_READ("erro ao pegar card info do player: " + std::to_string(uid) + ".", 20)

	return v_ci;
};

std::vector< CardEquipInfo > pangya_db::getCardEquipInfo(int32_t uid) {
	exec_query query("pangya.ProcGetCardEquip", std::to_string(uid) + ", 0");	// Card

	MED_RESULT_READ(&query)

	std::vector< CardEquipInfo > v_cei;
	CardEquipInfo cei;

	BEGIN_RESULT_READ(11, 21)

	cei.clear();

	cei._typeid = IFNULL(atoi, _result->data[0]);
	cei.parts_typeid = IFNULL(atoi, _result->data[2]);
	cei.parts_id = IFNULL(atoi, _result->data[3]);
	cei.efeito = IFNULL(atoi, _result->data[4]);
	cei.efeito_qntd = IFNULL(atoi, _result->data[5]);
	cei.slot = IFNULL(atoi, _result->data[6]);
	if (_result->data[7] != nullptr)
		translateDate(_result->data[7], &cei.use_date);
	if (_result->data[8] != nullptr)
		translateDate(_result->data[8], &cei.end_date);
	cei.tipo = IFNULL(atoi, _result->data[9]);
	cei.use_yn = (unsigned char)IFNULL(atoi, _result->data[10]);

	v_cei.push_back(cei);

	END_RESULT_READ("erro ao pegar card equip info do player: " + std::to_string(uid) + ".", 21)

	return v_cei;
};

std::vector< YamUsed > pangya_db::getYamUsed(int32_t uid) {
	exec_query query("pangya.ProcGetCardEquip", std::to_string(uid) + ", 1");	// Yam

	MED_RESULT_READ(&query)

	std::vector< YamUsed > v_yu;
	YamUsed yu;

	BEGIN_RESULT_READ(12, 22)

	yu.clear();

	yu._typeid = IFNULL(atoi, _result->data[0]);
	yu.parts_typeid = IFNULL(atoi, _result->data[2]);
	yu.parts_id = IFNULL(atoi, _result->data[3]);
	yu.efeito = IFNULL(atoi, _result->data[4]);
	yu.efeito_qntd = IFNULL(atoi, _result->data[5]);
	yu.slot = IFNULL(atoi, _result->data[6]);
	if (_result->data[7] != nullptr)
		translateDate(_result->data[7], &yu.use_date);
	if (_result->data[8] != nullptr)
		translateDate(_result->data[8], &yu.end_date);
	yu.tempo.setTime(IFNULL(atoi, _result->data[9]));
	yu.tipo = IFNULL(atoi, _result->data[10]);
	yu.use_yn = (unsigned char)IFNULL(atoi, _result->data[11]);

	v_yu.push_back(yu);

	END_RESULT_READ("erro ao pegar card equip info do player: " + std::to_string(uid) + ".", 22)

	return v_yu;
};

std::vector< TrofelEspecialInfo > pangya_db::getTrofelEspecialInfo(int32_t uid, int season) {
	exec_query query("pangya.ProcGetTrofelSpecial", std::to_string(uid) + ", " + std::to_string(season));

	MED_RESULT_READ(&query)

	std::vector< TrofelEspecialInfo > v_tsi;
	TrofelEspecialInfo tsi;

	BEGIN_RESULT_READ(3, 23)

	tsi.clear();

	tsi.id = IFNULL(atoi, _result->data[0]);
	tsi._typeid = IFNULL(atoi, _result->data[1]);
	tsi.qntd = IFNULL(atoi, _result->data[2]);

	v_tsi.push_back(tsi);

	END_RESULT_READ("erro ao pegar trofel especial info do player: " + std::to_string(uid) + ", na season: " + std::to_string(season) + ".", 23)

	return v_tsi;
};

std::vector< TrofelEspecialInfo > pangya_db::getTrofelGrandPrix(int32_t uid, int season) {
	exec_query query("pangya.ProcGetTrofelGrandPrix", std::to_string(uid) + ", " + std::to_string(season));

	MED_RESULT_READ(&query)

	std::vector< TrofelEspecialInfo > v_tgp;
	TrofelEspecialInfo tgp;

	BEGIN_RESULT_READ(3, 24)

	tgp.clear();

	tgp.id = IFNULL(atoi, _result->data[0]);
	tgp._typeid = IFNULL(atoi, _result->data[1]);
	tgp.qntd = IFNULL(atoi, _result->data[2]);

	v_tgp.push_back(tgp);

	END_RESULT_READ("erro ao pegar trofel grand prix do player: " + std::to_string(uid) + ", na season: " + std::to_string(season) + ".", 24)

	return v_tgp;
};

MyRoomConfig pangya_db::getMyRoomConfig(int32_t uid) {
	exec_query query("SELECT senha, public_lock, [state] FROM pangya.pangya_myroom WHERE UID = " + std::to_string(uid));

	MED_RESULT_READ(&query);

	MyRoomConfig mrc{ 0 };

	BEGIN_RESULT_READ(3, 51);

	if (_result->data[0] != nullptr)
		strcpy_s(mrc.pass, _result->data[0]);
	mrc.public_lock = (unsigned char)IFNULL(atoi, _result->data[1]);
	mrc.allow_enter = (unsigned short)IFNULL(atoi, _result->data[2]);

	END_RESULT_READ("Error ao pegar my room config do player: " + std::to_string(uid) + ".", 51);

	return MyRoomConfig();
};

std::vector< MyRoomItem > pangya_db::getMyRoomItem(int32_t uid) {
	exec_query query("pangya.ProcGetRoom", std::to_string(uid));

	MED_RESULT_READ(&query);

	std::vector< MyRoomItem > v_mri;
	MyRoomItem mri;

	BEGIN_RESULT_READ(9, 52);

	mri.clear();

	mri.id = IFNULL(atoi, _result->data[0]);
	mri._typeid = IFNULL(atoi, _result->data[2]);
	mri.number = (unsigned short)IFNULL(atoi, _result->data[3]);
	mri.location.x = (float)IFNULL(atof, _result->data[4]);
	mri.location.y = (float)IFNULL(atof, _result->data[5]);
	mri.location.z = (float)IFNULL(atof, _result->data[6]);
	mri.location.r = (float)IFNULL(atof, _result->data[7]);
	mri.equiped = (unsigned char)IFNULL(atoi, _result->data[8]);

	v_mri.push_back(mri);

	END_RESULT_READ("Error ao pegar my room item do player: " + std::to_string(uid) + ".", 52);

	return v_mri;
};

uint32_t pangya_db::checkDolfiniLockerPassword(int32_t uid) {
	exec_query query("pangya.CheckDolfiniPassword", std::to_string(uid));

	MED_RESULT_READ(&query);

	uint32_t ret = 0u;

	BEGIN_RESULT_READ(1, 52);

	ret = IFNULL(atoi, _result->data[0]);

	END_RESULT_READ("Error ao verificar a senha do dolfini locker do player: " + std::to_string(uid) + ".", 52);

	return 0;
};

std::vector< MailBox > pangya_db::getEmailNaoLido(int32_t uid) {
	exec_query query("pangya.ProcGetEmailNaoLidaFromMailBox", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::vector< MailBox > v_mb;
	MailBox mb;

	BEGIN_RESULT_READ(18, 25)

	mb.clear();

	mb.id = IFNULL(atoi, _result->data[0]);
	if (_result->data[1] != nullptr)
		strcpy_s(mb.from_id, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(mb.msg, _result->data[2]);
	mb.visit_count = IFNULL(atoi, _result->data[4]);
	mb.lida_yn = (unsigned char)IFNULL(atoi, _result->data[5]);
	mb.item_num = IFNULL(atoi, _result->data[6]);
	mb.item.id = IFNULL(atoi, _result->data[7]);
	mb.item._typeid = IFNULL(atoi, _result->data[8]);
	mb.item.flag_time = (unsigned char)IFNULL(atoi, _result->data[9]);
	mb.item.qntd = IFNULL(atoi, _result->data[10]);
	mb.item.tempo_qntd = IFNULL(atoi, _result->data[11]);
	mb.item.pang = IFNULL(atoll, _result->data[12]);
	mb.item.cookie = IFNULL(atoll, _result->data[13]);
	mb.item.gm_id = IFNULL(atoi, _result->data[14]);
	mb.item.flag_gift = IFNULL(atoi, _result->data[15]);
	if (_result->data[16] != nullptr)
		strcpy_s(mb.item.ucc_img_mark, _result->data[16]);
	mb.item.type = (short)IFNULL(atoi, _result->data[17]);

	v_mb.push_back(mb);

	END_RESULT_READ("erro ao pegar Email Nao Lido do player: " + std::to_string(uid) + ".", 25)

	return v_mb;
};

std::vector< MailBox > pangya_db::getEmailBox(int32_t uid) {
	exec_query query("pangya.ProcGetEmailFromMailBox", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::vector< MailBox > v_mb;
	MailBox mb;

	BEGIN_RESULT_READ(18, 44)

	mb.clear();

	mb.id = IFNULL(atoi, _result->data[0]);
	if (_result->data[1] != nullptr)
		strcpy_s(mb.from_id, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(mb.msg, _result->data[2]);
	mb.visit_count = IFNULL(atoi, _result->data[4]);
	mb.lida_yn = (unsigned char)IFNULL(atoi, _result->data[5]);
	mb.item_num = IFNULL(atoi, _result->data[6]);
	mb.item.id = IFNULL(atoi, _result->data[7]);
	mb.item._typeid = IFNULL(atoi, _result->data[8]);
	mb.item.flag_time = (unsigned char)IFNULL(atoi, _result->data[9]);
	mb.item.qntd = IFNULL(atoi, _result->data[10]);
	mb.item.tempo_qntd = IFNULL(atoi, _result->data[11]);
	mb.item.pang = IFNULL(atoll, _result->data[12]);
	mb.item.cookie = IFNULL(atoll, _result->data[13]);
	mb.item.gm_id = IFNULL(atoi, _result->data[14]);
	mb.item.flag_gift = IFNULL(atoi, _result->data[15]);
	if (_result->data[16] != nullptr)
		strcpy_s(mb.item.ucc_img_mark, _result->data[16]);
	mb.item.type = (short)IFNULL(atoi, _result->data[17]);

	v_mb.push_back(mb);

	END_RESULT_READ("erro ao pegar Email Box do player: " + std::to_string(uid) + ".", 44)

	return v_mb;
};

EmailInfo pangya_db::getEmailInfo(int32_t uid, int32_t email_id) {
	exec_query query("pangya.ProcGetInformationEmail", std::to_string(uid) + ", " + std::to_string(email_id));

	MED_RESULT_READ(&query);

	EmailInfo ei;
	EmailInfo::item item;
	ei.clear();

	BEGIN_RESULT_READ(16, 45);

	if (ei.id == 0) {
		ei.id = IFNULL(atoi, _result->data[0]);
		if (_result->data[1] != nullptr)
			strcpy_s(ei.from_id, _result->data[1]);
		if (_result->data[2] != nullptr)
			strcpy_s(ei.gift_date, _result->data[2]);
		if (_result->data[3] != nullptr)
			strcpy_s(ei.msg, _result->data[3]);
		ei.lida_yn = (unsigned char)IFNULL(atoi, _result->data[4]);
	}
	item.clear();

	item._typeid = IFNULL(atoi, _result->data[6]);

	if (item._typeid != 0) {

		item.id = IFNULL(atoi, _result->data[5]); // ID vem antes de typeid na consulta

		item.flag_time = (unsigned char)IFNULL(atoi, _result->data[7]);
		item.qntd = IFNULL(atoi, _result->data[8]);
		item.tempo_qntd = IFNULL(atoi, _result->data[9]);
		item.pang = IFNULL(atoll, _result->data[10]);
		item.cookie = IFNULL(atoll, _result->data[11]);
		item.gm_id = IFNULL(atoi, _result->data[12]);
		item.flag_gift = IFNULL(atoi, _result->data[13]);
		if (_result->data[14] != nullptr)
			strcpy_s(item.ucc_img_mark, _result->data[14]);
		item.type = (short)IFNULL(atoi, _result->data[15]);

		ei.itens.push_back(item);
	}

	END_RESULT_READ("erro ao pegar Email Info : " + std::to_string(email_id) + "\tdo player: " + std::to_string(uid) + ".", 45);

	return ei;
};

void pangya_db::deleteEmail(int32_t _uid, int32_t* a_email_id, size_t num_email) {
	
	std::string ids = "";

	for (size_t i = 0u; i < num_email; ++i) {
		if (i == 0)
			ids += std::to_string(a_email_id[i]);
		else
			ids += ", " + std::to_string(a_email_id[i]);
	}

	exec_query query("UPDATE pangya.pangya_gift_table SET valid = 0 WHERE uid = " + std::to_string(_uid)
		+ " AND Msg_ID IN(" + ids + ")", exec_query::_UPDATE);

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 48);

	END_RESULT_READ("erro ao deletar email(s) [" + ids + "] do player: " + std::to_string(_uid) + ".", 48);
};

std::vector< stItem > pangya_db::getItemEmailAndAtt(int32_t uid, int32_t email_id) {
	exec_query query("pangya.ProcMoveItemFromMailToArmario", std::to_string(uid) + ", " + std::to_string(email_id));

	MED_RESULT_READ(&query);

	std::vector< stItem > v_item;
	stItem item;

	BEGIN_RESULT_READ(4, 46);

	item.clear();

	item.id = IFNULL(atoi, _result->data[0]);
	item._typeid = IFNULL(atoi, _result->data[1]);
	item.qntd = IFNULL(atoi, _result->data[2]);
	// tem que ajeitar no banco de dados o tipo do tempo que vai add quando tiver
	//ip.tipo_unidade_add = (unsigned char)IFNULL(atoi, _result->data[3]);
	// Tbm tem que ter o gift flag, para saber se foi presente de outro pessoa ou do sistema
	item.STDA_C_ITEM_TIME = (short)IFNULL(atoi, _result->data[3]);

	v_item.push_back(item);

	END_RESULT_READ("error ao pegar itens do email: " + std::to_string(email_id) + " do player: " + std::to_string(uid) + ".", 46);

	return v_item;
};

std::vector< CaddieInfo > pangya_db::getCaddieFerias(int32_t uid) {
	exec_query query("pangya.ProcGetCaddieFerias", std::to_string(uid));

	MED_RESULT_READ(&query)

	CaddieInfo ci;
	std::vector< CaddieInfo > v_ci;

	BEGIN_RESULT_READ(11, 26)

	ci.clear();

	ci.id = IFNULL(atoi, _result->data[0]);
	ci._typeid = IFNULL(atoi, _result->data[2]);
	ci.parts_typeid = IFNULL(atoi, _result->data[3]);
	ci.level = (unsigned char)IFNULL(atoi, _result->data[4]);
	ci.exp = IFNULL(atoi, _result->data[5]);
	ci.rent_flag = (unsigned char)IFNULL(atoi, _result->data[6]);
	ci.end_date_unix = (short)IFNULL(atoi, _result->data[7]);
	ci.purchase = (unsigned char)IFNULL(atoi, _result->data[8]);
	ci.parts_end_date_unix = (short)IFNULL(atoi, _result->data[9]);
	ci.check_end = (short)IFNULL(atoi, _result->data[10]);

	v_ci.push_back(ci);

	END_RESULT_READ("erro ao pegar caddie ferias do player: " + std::to_string(uid) + ".", 26)

	return v_ci;
};

std::vector< MsgOffInfo > pangya_db::getMsgOffInfo(int32_t uid) {
	exec_query query("pangya.ProcGetMsgOff", std::to_string(uid));

	MED_RESULT_READ(&query)

	std::vector< MsgOffInfo > v_moi;
	MsgOffInfo moi;

	BEGIN_RESULT_READ(5, 27)

	moi.clear();

	moi.id = (short)IFNULL(atoi, _result->data[0]);
	moi.from_uid = IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(moi.nick, _result->data[2]);
	if (_result->data[3] != nullptr)
		strcpy_s(moi.msg, _result->data[3]);
	strncpy_s(moi.date, _result->data[4], sizeof(moi.date));

	v_moi.push_back(moi);

	END_RESULT_READ("erro ao pegar msg off info do palyer: " + std::to_string(uid) + ".", 27)

	return v_moi;
};

AttendanceRewardInfo pangya_db::getAttendanceRewardInfo(int32_t uid) {
	exec_query query("pangya.ProcGetAttendanceReward", std::to_string(uid));

	MED_RESULT_READ(&query)

	AttendanceRewardInfo ari = { 0 };

	BEGIN_RESULT_READ(6, 28)

	ari.counter = IFNULL(atoi, _result->data[0]);
	ari.now._typeid = IFNULL(atoi, _result->data[1]);
	ari.now.qntd = IFNULL(atoi, _result->data[2]);
	ari.after._typeid = IFNULL(atoi, _result->data[3]);
	ari.after.qntd = IFNULL(atoi, _result->data[4]);
	ari.login = (unsigned char)IFNULL(atoi, _result->data[5]);

	END_RESULT_READ("erro ao pegar Attendance Reward do player: " + std::to_string(uid) + ".", 28)

	return ari;
};

AttendanceRewardInfo pangya_db::checkAttendanceReward(int32_t uid) {
	exec_query query("pangya.ProcCheckAttendanceReward", std::to_string(uid));

	MED_RESULT_READ(&query)

	AttendanceRewardInfo ari = { 0 };

	BEGIN_RESULT_READ(6, 29)

	ari.counter = IFNULL(atoi, _result->data[0]);
	ari.now._typeid = IFNULL(atoi, _result->data[1]);
	ari.now.qntd = IFNULL(atoi, _result->data[2]);
	ari.after._typeid = IFNULL(atoi, _result->data[3]);
	ari.after.qntd = IFNULL(atoi, _result->data[4]);
	ari.login = (unsigned char)IFNULL(atoi, _result->data[5]);

	END_RESULT_READ("erro ao checar Attendance Reward do player: " + std::to_string(uid) + ".", 29)

	return ari;
};

Last5PlayersGame pangya_db::getLast5PlayersGame(int32_t uid) {
	exec_query query("pangya.ProcGetLastPlayerGame", std::to_string(uid));

	MED_RESULT_READ(&query)

	Last5PlayersGame l5pg = { 0 };
	size_t i;

	BEGIN_RESULT_READ(20, 30)

	for (i = 0; i < 5; i++) {
		l5pg.players[i]._friend = IFNULL(atoi, _result->data[i * 4]);
		if (_result->data[i * 4 + 1] != nullptr)
			strcpy_s(l5pg.players[i].nick, _result->data[i * 4 + 1]);	// Aqui de vez em quando da erro, deve ser aquele lá depois que loga os 2000 bot no release quando sai ele da erro
		if (_result->data[i * 4 + 2] != nullptr)
			strcpy_s(l5pg.players[i].id, _result->data[i * 4 + 2]);
		l5pg.players[i].uid = IFNULL(atoi, _result->data[i * 4 + 3]);
	}

	END_RESULT_READ("erro ao pegar Last Five Players Played with player: " + std::to_string(uid) + ".", 30)

	return l5pg;

};

KeysOfLogin pangya_db::getKeysOfLogin(int32_t uid) {
	exec_query query("pangya.ProcGetAuthKeys", std::to_string(uid));

	MED_RESULT_READ(&query)

	KeysOfLogin kol = { 0 };

	BEGIN_RESULT_READ(3, 31)

		if (_result->data[0] != nullptr)
			strcpy_s(kol.keys[0], _result->data[0]);
	if (_result->data[1] != nullptr)
		strcpy_s(kol.keys[1], _result->data[1]);
	kol.valid = (unsigned char)IFNULL(atoi, _result->data[2]);

	END_RESULT_READ("erro ao pegar auth keys do login do player: " + std::to_string(uid) + ".", 31)

	return kol;
};

PremiumTicket pangya_db::getPremiumTicket(int32_t uid) {
	exec_query query("pangya.ProcGetPremiumTicket", std::to_string(uid));

	MED_RESULT_READ(&query)

	PremiumTicket pt = { 0 };

	BEGIN_RESULT_READ(4, 32)

	pt.id = IFNULL(atoi, _result->data[0]);
	pt._typeid = IFNULL(atoi, _result->data[1]);
	pt.unix_end_date = IFNULL(atoi, _result->data[2]);
	pt.unix_sec_date = IFNULL(atoi, _result->data[3]);

	END_RESULT_READ("erro ao pegar Premium Ticket do player: " + std::to_string(uid) + ".", 32)

	return pt;
};

DailyQuest pangya_db::getDailyQuest(int32_t uid) {
	exec_query query("pangya.ProcGetDailyQuest_New", std::to_string(uid));

	MED_RESULT_READ(&query)

	DailyQuest dq = { 0 };
	size_t i;

	BEGIN_RESULT_READ(5, 33)

	dq.now_date = IFNULL(atoi, _result->data[0]);
	dq.accept_date = IFNULL(atoi, _result->data[1]);
	dq.count = 3;			// POR ENQUANTO O COUNT É ESTÁTICO EM 3, MAS DEPOIS VOU COLOCAR NO DB DIREITO
	for (i = 0; i < 3; i++)
		dq._typeid[i] = IFNULL(atoi, _result->data[2 + i]);

	END_RESULT_READ("erro ao pegar Daily Quest do player: " + std::to_string(uid) + ".", 33)

	return dq;
};

GuildInfo pangya_db::getGuildInfo(int32_t uid, int option) {
	exec_query query("pangya.ProcGetGuildInfo", std::to_string(uid) + ", " + std::to_string(option));

	MED_RESULT_READ(&query)

	GuildInfo gi = { 0 };

	BEGIN_RESULT_READ(12, 35)

	gi.uid = IFNULL(atoi, _result->data[0]);
	if (_result->data[1] != nullptr)
		strcpy_s(gi.name, _result->data[1]);
	gi.index_mark_emblem = 1;		// AQUI ESTÁ ESTÁTICO, MAS DEPOIS VOU AJEITAR ISSO NO BANCO DE DADOS
	gi.pang = IFNULL(atoi, _result->data[7]);
	gi.point = IFNULL(atoi, _result->data[6]);

	END_RESULT_READ("erro ao pegar Guild Info do player: " + std::to_string(uid) + ", com a option : " + std::to_string(option) + ".", 35)

	return gi;
};

std::vector< ServerInfo > pangya_db::getMSNServer() {
	exec_query query("pangya.ProcGetServerList", "1");

	MED_RESULT_READ(&query)

	std::vector< ServerInfo > v_si;
	ServerInfo si;

	BEGIN_RESULT_READ(13, 37)

	si = { 0 };

	if (_result->data[0] != nullptr)
		strcpy_s(si.nome, _result->data[0]);
	si.uid = IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(si.ip, _result->data[2]);
	si.port = atoi(_result->data[3]);
	si.max_user = atoi(_result->data[4]);
	si.curr_user = atoi(_result->data[5]);
	si.propriedade = atoi(_result->data[6]);
	si.angelic_wings_num = atoi(_result->data[7]);
	si.event_flag = atoi(_result->data[8]);
	si.event_map = atoi(_result->data[9]);
	si.img_no = atoi(_result->data[10]);
	si.app_rate = atoi(_result->data[11]);
	si.rate_scratchy = atoi(_result->data[12]);

	v_si.push_back(si);

	END_RESULT_READ("erro ao pegar a List de MSN Servers", 37)

	return v_si;
};

bool pangya_db::checkDailyQuestPlayer(int32_t uid) {
	exec_query query("pangya.ProcCheckPlayerDailyQuest", std::to_string(uid));

	MED_RESULT_READ(&query)

	bool ret = false;

	BEGIN_RESULT_READ(1, 38)

	ret = (IFNULL(atoi, _result->data[0]) == 1);

	END_RESULT_READ("error ao checar daily quest do player: " + std::to_string(uid) + ".", 38)

	return ret;
};

std::vector< RemoveDailyQuest > pangya_db::getOldDailyQuestPlayer(int32_t uid) {
	exec_query query("SELECT ID_ACHIEVEMENT, typeid FROM pangya.achievement_tipo WHERE [OPTION] = 1 AND UID = " + std::to_string(uid));

	MED_RESULT_READ(&query)

	std::vector< RemoveDailyQuest > old_quest;

	BEGIN_RESULT_READ(2, 39)

	old_quest.push_back({ IFNULL(atoi, _result->data[0]), std::strtoul(_result->data[1], nullptr, 10) } );

	END_RESULT_READ("error ao pegar as ultimas quest diaria do player: " + std::to_string(uid) + ".", 39)

	return old_quest;
};

int32_t pangya_db::addDailyQuest(int32_t uid, std::string quest_name, uint32_t quest_typeid, int tipo, int option, int32_t stuff_typeid, uint32_t count_objective) {
	exec_query query("pangya.ProcInsertDailyQuest_New", std::to_string(uid) + ", '" + quest_name + "', " + std::to_string(quest_typeid) + ", " + std::to_string(tipo) + ", "
					+  std::to_string(option) + ", " + std::to_string(stuff_typeid) + ", " + std::to_string(count_objective));

	MED_RESULT_READ(&query)

	int32_t id;

	BEGIN_RESULT_READ(1, 40)

	id = IFNULL(atoi, _result->data[0]);

	END_RESULT_READ("error ao inserir um achievement de daily quest para o player: " + std::to_string(uid) + ".", 40)

	return id;
};

void pangya_db::setDailyQuest(int32_t uid) {
	exec_query query("UPDATE pangya.pangya_daily_quest_player SET today_quest = getdate() WHERE UID = " + std::to_string(uid));

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(0, 41)

	END_RESULT_READ("error ao setar o daily quest data do player: " + std::to_string(uid) + ".", 41)
};

void pangya_db::removeDailyQuest(int32_t uid, std::vector< RemoveDailyQuest >& remove) {
	
	std::string ids = "";

	for (auto i = 0u; i < remove.size(); ++i) {
		if (i == 0)
			ids = std::to_string(remove[i].id);
		else
			ids += ", " + std::to_string(remove[i].id);
	}

	exec_query query("DELETE FROM pangya.achievement_tipo WHERE ID_ACHIEVEMENT IN(" + ids + ");\
					  DELETE FROM pangya.achievement_quest WHERE ID_ACHIEVEMENT IN(" + ids + ")");

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(0, 42)

	END_RESULT_READ("error ao remover daily quest antiga do player: " + std::to_string(uid) + ".", 42)
};

void pangya_db::updateItemEquipSlot(int32_t _uid, int32_t _item_slot[10]) {
	exec_query query("UPDATE pangya.pangya_user_equip SET item_slot_1 = " + std::to_string(_item_slot[0])
		+ ", item_slot_2 = " + std::to_string(_item_slot[1])
		+ ", item_slot_3 = " + std::to_string(_item_slot[2])
		+ ", item_slot_4 = " + std::to_string(_item_slot[3])
		+ ", item_slot_5 = " + std::to_string(_item_slot[4])
		+ ", item_slot_6 = " + std::to_string(_item_slot[5])
		+ ", item_slot_7 = " + std::to_string(_item_slot[6])
		+ ", item_slot_8 = " + std::to_string(_item_slot[7])
		+ ", item_slot_9 = " + std::to_string(_item_slot[8])
		+ ", item_slot_10 = " + std::to_string(_item_slot[9])
		+ " WHERE uid = " + std::to_string(_uid), exec_query::_UPDATE);

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 43);

	END_RESULT_READ("error ao atualizar o Item Slot do player: " + std::to_string(_uid) + ".", 43)
};

void pangya_db::updateCaddieEquiped(int32_t _uid, int32_t _item_id) {
	exec_query query("pangya.USP_FLUSH_CADDIE", std::to_string(_uid) + ", " + std::to_string(_item_id));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 50);

	END_RESULT_READ("error ao atualizar caddie equipado do player: " + std::to_string(_uid) + ".", 50);
};

void pangya_db::updateBallEquiped(int32_t _uid, int32_t _item_typeid) {
	exec_query query("pangya.USP_FLUSH_COMET", std::to_string(_uid) + ", " + std::to_string(_item_typeid));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 50);

	END_RESULT_READ("error ao atualizar ball equipado do player: " + std::to_string(_uid) + ".", 50);
};

void pangya_db::updateClubSetEquiped(int32_t _uid, int32_t _item_id) {
	exec_query query("pangya.USP_FLUSH_CLUB", std::to_string(_uid) + ", " + std::to_string(_item_id));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 50);

	END_RESULT_READ("error ao atualizar clubset equipado do player: " + std::to_string(_uid) + ".", 50);
};

void pangya_db::updateCharacterEquiped(int32_t _uid, int32_t _item_id) {
	exec_query query("pangya.USP_FLUSH_CHARACTER", std::to_string(_uid) + ", " + std::to_string(_item_id));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 50);

	END_RESULT_READ("error ao atualizar character equipado do player: " + std::to_string(_uid) + ".", 50);
};

void pangya_db::updateMascotEquiped(int32_t _uid, int32_t _item_id) {
	exec_query query("pangya.USP_FLUSH_MASCOT", std::to_string(_uid) + ", " + std::to_string(_item_id));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 50);

	END_RESULT_READ("error ao atualizar mascot equipado do player: " + std::to_string(_uid) + ".", 50);
};

void pangya_db::updateCharacterCutinEquiped(int32_t _uid, CharacterInfo& _ci) {
	// Tenho que ajeitar no DB para ele aceitar 4 ID de cutin que o character pode equipar ao mesmo tempo(ACHO)
	exec_query query("pangya.USP_FLUSHCHARACTERCUTIN", std::to_string(_uid) + ", " + std::to_string(_ci.id) 
		+ ", " + std::to_string(_ci.cut_in[0])
		+ ", " + std::to_string(_ci.cut_in[1])
		+ ", " + std::to_string(_ci.cut_in[2])
		+ ", " + std::to_string(_ci.cut_in[3]));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 53);

	END_RESULT_READ("error ao atualizer o character cutin do player: " + std::to_string(_uid) + ".", 53);
};

void pangya_db::updateSkinEquiped(int32_t _uid, UserEquip& _ue) {
	exec_query query("pangya.USP_FLUSH_SKIN", std::to_string(_uid)
		+ ", " + std::to_string(_ue.skin_typeid[0])
		+ ", " + std::to_string(_ue.skin_typeid[1])
		+ ", " + std::to_string(_ue.skin_typeid[2])
		+ ", " + std::to_string(_ue.skin_typeid[3])
		+ ", " + std::to_string(_ue.skin_typeid[4])
		+ ", " + std::to_string(_ue.skin_typeid[5]));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 54);

	END_RESULT_READ("error ao atualizar os skins do player: " + std::to_string(_uid) + ".", 54);
};

void pangya_db::updatePosterEquiped(int32_t _uid, UserEquip& _ue) {
	exec_query query("pangya.USP_FLUSH_EQUIP_POSTER", std::to_string(_uid) + ", " + std::to_string(_ue.poster[0]) + ", " + std::to_string(_ue.poster[1]));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 55);

	END_RESULT_READ("error ao atualizar os poster do player: " + std::to_string(_uid) + ".", 55);
};

void pangya_db::updateCharacterAllPartEquiped(int32_t _uid, CharacterInfo& _ci) {
	
	std::string q = ""; // "|" + std::to_string(_ci._typeid) + "|" + std::to_string(_ci.id);

	q += "|" + std::to_string((unsigned short)_ci.default_hair) + "|" + std::to_string((unsigned short)_ci.default_shirts);
	q += "|" + std::to_string((unsigned short)_ci.gift_flag) + "|" + std::to_string((unsigned short)_ci.purchase);

	auto is = 0u;

	for (is = 0u; is < (sizeof(_ci.parts_typeid) / sizeof(_ci.parts_typeid[0])); ++is)
		q += "|" + std::to_string(_ci.parts_typeid[is]);

	for (is = 0u; is < (sizeof(_ci.parts_id) / sizeof(_ci.parts_id[0])); ++is)
		q += "|" + std::to_string(_ci.parts_id[is]);

	for (is = 0u; is < (sizeof(_ci.auxparts) / sizeof(_ci.auxparts[0])); ++is)
		q += "|" + std::to_string(_ci.auxparts[is]);

	// Depois o Cutin tem que colocar os 4, aqui só está 1 que era o antigo, que eu fiz
	for (is = 0u; is < (sizeof(_ci.cut_in) / sizeof(_ci.cut_in[0])); ++is)
		q += "|" + std::to_string(_ci.cut_in[is]);

	for (is = 0u; is < (sizeof(_ci.pcl) / sizeof(_ci.pcl[0])); ++is)
		q += "|" + std::to_string((unsigned short)_ci.pcl[is]);

	// Mastery Character
	q += "|" + std::to_string(_ci.mastery);

	exec_query query("pangya.USP_CHAR_EQUIP_SAVE_S4", std::to_string(_uid) + ", " + std::to_string(_ci.id) + ", '" + q + "'");

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(0, 56);

	END_RESULT_READ("error ao atualizar character todas as parts do player: " + std::to_string(_uid) + ".", 56);
};

//int32_t pangya_db::addItem(int32_t _uid, stItem& _item, unsigned short _gift_flag, unsigned short _purchase, Location _location) {
//	exec_query query("pangya.USP_ADD_ITEM",
//		  std::to_string(_uid) + ", "
//		+ std::to_string(_gift_flag) + ", "
//		+ std::to_string(_purchase) + ", "
//		+ std::to_string(_item.id) + ", "
//		+ std::to_string(_item._typeid) + ", "
//		+ std::to_string(_item.type) + ", "
//		+ std::to_string(_item.type_iff) + ", "
//		+ std::to_string(_item.c[4]) + ", "
//		+ std::to_string(_item.c[0]) + ", "
//		+ std::to_string(_item.c[1]) + ", "
//		+ std::to_string(_item.c[2]) + ", "
//		+ std::to_string(_item.c[3]) + ", "
//		+ std::to_string(_item.c[4]) + ", "
//		+ std::to_string(_location.x) + ", "
//		+ std::to_string(_location.y) + ", "
//		+ std::to_string(_location.z) + ", "
//		+ std::to_string(_location.r));
//
//	MED_RESULT_READ(&query);
//
//	int32_t id = 0l;
//
//	BEGIN_RESULT_READ(0, 47);
//
//	if (num_result == 1)
//		_item.id = id = IFNULL(atoi, _result->data[0]);
//	else if (num_result == 0 && (_item.type_iff == IFF::Part::UCC_BLANK || _item.type_iff == IFF::Part::UCC_COPY)) {
//		_item.id = IFNULL(atoi, _result->data[0]);
//		if (_result->data[1] != nullptr)
//			strcpy_s(_item.ucc.IDX, _result->data[1]);
//		_item.ucc.seq = IFNULL(atoi, _result->data[2]);
//	}
//
//	END_RESULT_READ("error ao add item para o player: " + std::to_string(_uid) + ".", 47);
//
//	return id;
//};
//
//int32_t pangya_db::addItem(int32_t _uid, std::vector< stItem >& v_item, unsigned short _gift_flag, unsigned short _purchase, Location _location) {
//
//	uint32_t i = 0;
//	
//	for (i; i < v_item.size(); ++i)
//		v_item[i].id = addItem(_uid, v_item[i], _gift_flag, _purchase, _location);
//
//	/*for (i; i < v_item.size(); ++i) {
//		exec_query query("pangya.USP_ADD_ITEM",
//			std::to_string(_uid) + ", "
//			+ std::to_string(_gift_flag) + ", "
//			+ std::to_string(_purchase) + ", "
//			+ std::to_string(v_item[i].id) + ", "
//			+ std::to_string(v_item[i]._typeid) + ", "
//			+ std::to_string(v_item[i].type) + ", "
//			+ std::to_string(v_item[i].c[4]) + ", "
//			+ std::to_string(v_item[i].c[0]) + ", "
//			+ std::to_string(v_item[i].c[1]) + ", "
//			+ std::to_string(v_item[i].c[2]) + ", "
//			+ std::to_string(v_item[i].c[3]) + ", "
//			+ std::to_string(v_item[i].c[4]) + ", "
//			+ std::to_string(_location.x) + ", "
//			+ std::to_string(_location.y) + ", "
//			+ std::to_string(_location.z) + ", "
//			+ std::to_string(_location.r));
//
//		MED_RESULT_READ(&query);
//
//		int32_t id = 0l;
//
//		BEGIN_RESULT_READ(1, 57);
//
//		if (num_result == 0)
//			id = IFNULL(atoi, _result->data[0]);
//
//		v_item[i].id = id;
//
//		END_RESULT_READ("error ao add item para o player: " + std::to_string(_uid) + ".", 57);
//	}*/
//
//	return (int)i;
//};
