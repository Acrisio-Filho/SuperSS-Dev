// Arquivo pangya_login_db.cpp
// Criado em 23/07/2017 por Acrisio
// Implementação da classe pangya_db para o Login Server

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "pangya_login_db.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

pangya_db::pangya_db() : pangya_base_db() {
};

pangya_db::~pangya_db() {};

PlayerInfo pangya_db::getPlayerInfo(uint32_t uid) {
    exec_query query("SELECT a.uid, a.id, a.nick, a.password, a.capability, b.level FROM pangya.account a, pangya.user_info b WHERE a.uid = b.uid AND a.uid = " + std::to_string(uid));
	
	MED_RESULT_READ(&query)

	PlayerInfo pi;

    BEGIN_RESULT_READ(6, 1)

	pi.uid = IFNULL(atoi, _result->data[0]);
	if (_result->data[1] != nullptr)
		strcpy_s(pi.id, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(pi.nickname, _result->data[2]);
	if (_result->data[3] != nullptr)
		strcpy_s(pi.pass, _result->data[3]);
	pi.m_cap = IFNULL(atoi, _result->data[4]);
	pi.level = (unsigned short)IFNULL(atoi, _result->data[5]);

	END_RESULT_READ("o info do player uid: " + std::to_string(uid) + ".", 1)

    return pi;
};

uint32_t pangya_db::VerifyID(std::string id) {
	uint32_t uid = 0;

	exec_query query("SELECT uid FROM pangya.account WHERE id = '" + id + "'");

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(1, 3)

	uid = IFNULL(atoi, _result->data[0]);

	END_RESULT_READ("o id do player id: " + id + ".", 3)

	return uid;
};

bool pangya_db::VerifyPass(uint32_t uid, std::string pass) {
	bool b = false;

	exec_query query("pangya.ProcVerifyPass", std::to_string(uid) + ", " + pass);

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(1, 4)

	if (IFNULL(atoi, _result->data[0]) > 0)
		b = true;

	END_RESULT_READ("a senha do player uid: " + std::to_string(uid) + ".", 4)

	return b;
};

std::string pangya_db::getAuthKey(uint32_t uid) {
	std::string s;

	exec_query query("pangya.ProcGeraAuthKey", std::to_string(uid));

	MED_RESULT_READ(&query);

	BEGIN_RESULT_READ(1, 5);

	if (_result->data[0] != nullptr)
		s = std::string(_result->data[0]);

	END_RESULT_READ("a AuthKey. player uid: " + std::to_string(uid) + ".", 5)

	return s;
};

std::string pangya_db::getAuthKeyLogin(uint32_t uid) {
	std::string s;

	exec_query query("pangya.ProcGeraAuthKeyLogin", std::to_string(uid));

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(1, 6)

	if (_result->data[0] != nullptr)
		s = std::string(_result->data[0]);

	END_RESULT_READ("a AuthKeyLogin. player uid: " + std::to_string(uid) + ".", 6)

	return s;
};

//std::vector< ServerInfo > pangya_db::getServerList() {
//	std::vector< ServerInfo > sis;
//	ServerInfo si;
//
//	exec_query query("pangya.ProcGetServerList", "0");
//
//	MED_RESULT_READ(&query)
//
//	BEGIN_RESULT_READ(13, 7)
//
//	si = { 0 };
//
//	if (_result->data[0] != nullptr)
//		strcpy_s(si.nome, _result->data[0]);
//	si.uid = IFNULL(atoi, _result->data[1]);
//	if (_result->data[2] != nullptr)
//		strcpy_s(si.ip, _result->data[2]);
//	si.port = IFNULL(atoi, _result->data[3]);
//	si.max_user = IFNULL(atoi, _result->data[4]);
//	si.curr_user = IFNULL(atoi, _result->data[5]);
//	si.propriedade = IFNULL(atoi, _result->data[6]);
//	si.angelic_wings_num = IFNULL(atoi, _result->data[7]);
//	si.event_flag = (short)IFNULL(atoi, _result->data[8]);
//	si.event_map = (short)IFNULL(atoi, _result->data[9]);
//	si.img_no = (short)IFNULL(atoi, _result->data[10]);
//	si.app_rate = (short)IFNULL(atoi, _result->data[11]);
//	si.rate_scratchy = (short)IFNULL(atoi, _result->data[12]);
//
//	sis.push_back(si);
//
//	END_RESULT_READ("a Lista de Game Servers.", 7)
//
//	return sis;
//};

std::vector< ServerInfo > pangya_db::getMsn() {
	std::vector< ServerInfo > sis;
	ServerInfo si;

	exec_query query("pangya.ProcGetServerList", "1");

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(13, 8)
	
	si = { 0 };

	if (_result->data[0] != nullptr)
		strcpy_s(si.nome, _result->data[0]);
	si.uid = IFNULL(atoi, _result->data[1]);
	if (_result->data[2] != nullptr)
		strcpy_s(si.ip, _result->data[2]);
	si.port = IFNULL(atoi, _result->data[3]);
	si.max_user = IFNULL(atoi, _result->data[4]);
	si.curr_user = IFNULL(atoi, _result->data[5]);
	si.propriedade = IFNULL(atoi, _result->data[6]);
	si.angelic_wings_num = IFNULL(atoi, _result->data[7]);
	si.event_flag = (short)IFNULL(atoi, _result->data[8]);
	si.event_map = (short)IFNULL(atoi, _result->data[9]);
	si.img_no = (short)IFNULL(atoi, _result->data[10]);
	si.app_rate = (short)IFNULL(atoi, _result->data[11]);
	si.rate_scratchy = (short)IFNULL(atoi, _result->data[12]);

	sis.push_back(si);

	END_RESULT_READ("a Lista de Msn Servers.", 8)

	return sis;
};

macro_user pangya_db::getMacroUser(uint32_t uid) {
	macro_user mu;

	exec_query query("pangya.ProcGetMacrosUser", std::to_string(uid));

	MED_RESULT_READ(&query)

	BEGIN_RESULT_READ(9, 9)

	size_t i = 0;

	for (i = 0; i < 9; i++)
		if (_result->data[i] != nullptr)
			strcpy_s(mu.macro[i], _result->data[i]);

	END_RESULT_READ("Macro do usuario. player uid: " + std::to_string(uid) + ".", 9)

	return mu;
};
