// Arquivo packet_func_ls.cpp
// Criado em 04/06/2017 por Acrisio
// Implementação da classe packet_func

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "packet_func_ls.h"
#include "../../Projeto IOCP/PACKET/packet.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/hex_util.h"
#include "../../Projeto IOCP/THREAD POOL/threadpool.h"

#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../Login Server/login_server.h"

#include "../SESSION/player.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

#include <regex>

#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_id.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_pass.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_server_list.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_login.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_game.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_chat_macro_user.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_add_character.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_update_character_equiped.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_register_logon.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_nick.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_save_nick.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_logon_check.hpp"

#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_verify_ip.hpp"
#include "../PANGYA_DB/cmd_first_login_check.hpp"
#include "../PANGYA_DB/cmd_first_set_check.hpp"
#include "../PANGYA_DB/cmd_create_user.hpp"
#include "../PANGYA_DB/cmd_add_first_login.hpp"
#include "../PANGYA_DB/cmd_add_first_set.hpp"
#include "../PANGYA_DB/cmd_register_player_login.hpp"
#include "../PANGYA_DB/cmd_register_logon_server.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../../Projeto IOCP/UTIL/md5.hpp"

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).makeFull((_session)->m_key); \
											WSABUF mb = (_packet).getMakedBuf(); \
											try { \
												\
												(_session)->usa(); \
												\
												(_session)->requestSendBuffer(mb.buf, mb.len); \
												\
												if ((_session)->devolve()) \
													sls::ls::getInstance().DisconnectSession((_session)); \
												\
											}catch (exception& e) { \
												\
												if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/)) \
													if ((_session)->devolve()) \
														sls::ls::getInstance().DisconnectSession((_session)); \
												\
												if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2)) \
													throw; \
											} \

#define SUCCESS_LOGIN(_from, __arg1, _session) \
	{ \
		(_session)->m_pi.m_state = 1; \
		_smp::message_pool::getInstance().push(new message("[packet_func::" + std::string((_from)) + "][Log] Player logou. [ID=" + std::string((_session)->m_pi.id) + ", UID=" + std::to_string((_session)->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE)); \
 \
		succes_login((__arg1), (_session)); \
	}

// Verifica se session está autorizada para executar esse ação, 
// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!pd._session.m_is_authorized) \
												throw exception("[packet_func::" + std::string((method)) + "][Error] Player[UID=" + std::to_string(pd._session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 1, 0x5000501)); \

using namespace stdA;

int packet_func::packet001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sls::ls::getInstance().requestLogin(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet001][Log][ErrorSystem] " + e.getFullMessageError()));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LOGIN_SERVER)
			throw;
	}

    return 0;
};

int packet_func::packet003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);
	
	std::string auth_key_game = "";

	try {

		uint32_t server_uid = pd._packet->readUint32();

		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][Log] Server UID: " + std::to_string(server_uid)));

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet003");

		// Registra o logon no server_uid do player_uid
		CmdRegisterLogonServer cmd_rls(pd._session.m_pi.uid, server_uid, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_rls, nullptr, nullptr);

		cmd_rls.waitEvent();

		if (cmd_rls.getException().getCodeError() != 0)
			throw cmd_rls.getException();

		CmdAuthKeyGame cmd_auth_key_game(pd._session.m_pi.uid, server_uid, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_auth_key_game, nullptr, nullptr);

		cmd_auth_key_game.waitEvent();

		if (cmd_auth_key_game.getException().getCodeError() != 0)
			throw cmd_auth_key_game.getException();

		auth_key_game = cmd_auth_key_game.getAuthKey();

		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][Log] AuthKeyGame: " + auth_key_game
				+ ", do player: " + std::to_string(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::EXEC_QUERY, 6/*AuthKeyLogin*/))
			throw;
	}

	packet p;
	pacote003(p, &pd._session, auth_key_game);
	session_send(p, &pd._session, 1);

	return 0;
};

int packet_func::packet004(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		ls->requestDownPlayerOnGameServer(pd._session, pd._packet);

	}catch (exception& e) {
		
		packet p;
		pacote00E(p, &pd._session, L"", 12, 500053);
		session_send(p, &pd._session, 1);

		_smp::message_pool::getInstance().push(new message("[packet_func::packet004][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet006(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	std::wstring wnick = L"";
	packet p;

	try {

		wnick = MbToWc(pd._packet->readString());

		_smp::message_pool::getInstance().push(new message(L"[packet_func::packet006][Log] Save Nickname: " + wnick, CL_FILE_LOG_AND_CONSOLE));

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet006");

		CmdSaveNick cmd_sn(pd._session.m_pi.uid, wnick, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_sn, nullptr, nullptr);

		cmd_sn.waitEvent();

		if (cmd_sn.getException().getCodeError() != 0)
			throw cmd_sn.getException();
		
		CmdAddFirstLogin cmd_afl(pd._session.m_pi.uid, 1, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_afl, nullptr, nullptr);

		cmd_afl.waitEvent();

		if (cmd_afl.getException().getCodeError() != 0)
			throw cmd_afl.getException();

		_smp::message_pool::getInstance().push(new message(L"[packet_func::packet006][Log] salvou o nick: " + wnick + L", do player: "
				+ std::to_wstring(pd._session.m_pi.uid) + L" com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		// Aqui colocar para verificar se ele já fez o first set, se não envia o pacote do first set, se não success_login
		CmdFirstSetCheck cmd_fsc(pd._session.m_pi.uid, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_fsc, nullptr, nullptr);

		cmd_fsc.waitEvent();

		if (cmd_fsc.getException().getCodeError() != 0)
			throw cmd_fsc.getException();

		if (!cmd_fsc.getLastCheck()) {	// Verifica se fez o primeiro set do character

			// FIRST_SET
			pacote001(p, &pd._session, 0xD9);
			session_send(p, &pd._session, 1);

			_smp::message_pool::getInstance().push(new message("[packet_func::packet006][Log] Primeira vez que o player escolhe um character padrao. player[UID="
					+ std::to_string(pd._session.m_pi.uid) + ", ID=" + std::string(pd._session.m_pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

		}else
			SUCCESS_LOGIN("packet006", _arg1, &pd._session);

	}catch (exception& e) {

		pacote00E(p, &pd._session, wnick, 1/*UNKNOWN ERROR*/);
		session_send(p, &pd._session, 1);

		_smp::message_pool::getInstance().push(new message("[packet_func::packet006][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet007(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	enum NICK_CHECK : unsigned char {
		SUCCESS,				// Sucesso por trocar o nick por que ele está disponível
		UNKNOWN_ERROR,			// Erro desconhecido, Error ao verificar NICK
		NICK_IN_USE,			// NICKNAME já em uso
		INCORRECT_NICK,			// INCORRET nick, tamanho < 4 ou tem caracteres que não pode
		NOT_ENOUGH_COOKIE,		// Não tem points suficiente
		HAVE_BAD_WORD,			// Tem palavras que não pode no NICK
		ERROR_DB,				// Erro DB
		EMPETY_ERROR,			// Erro Vazio
		EMPETY_ERROR_2,			// ERRO VAZIO 2
		SAME_NICK_USED,			// O Mesmo nick vai ser usado, estou usando para o mesmo que o ID
		EMPETY_ERROR_3,			// ERRO VAZIO 3
		CODE_ERROR_INFO = 12	// CODE  ERROR INFO arquivo iff, o código do erro para mostra no cliente
	};

	NICK_CHECK nc(SUCCESS);
	uint32_t error_info = 0;

	std::wstring wnick = L"";

	try {

		wnick = pd._packet->readWString();

		_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Log] Check Nickname: " + wnick, CL_FILE_LOG_AND_CONSOLE));

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet007");

		if (
#if defined(_WIN32)
			_wcsicmp(
#elif defined(__linux__)
			wcscasecmp(
#endif
				std::wstring(pd._session.m_pi.id, pd._session.m_pi.id + sizeof(pd._session.m_pi.id)).c_str(), wnick.c_str()
			) == 0
		) {
			nc = SAME_NICK_USED;		// NICK igual ao ID, nao pode

			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Error] O nick igual ao ID nao pode. nick: "
					+ wnick + L" Player: " + std::to_wstring(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
		}

		// Pavras que não pode usar
		if (nc == SUCCESS && !(pd._session.m_pi.m_cap & 4) && std::regex_match(wnick.begin(), wnick.end(), std::wregex(L"(.*GM.*)|(.*ADM.*)", std::regex_constants::icase))) {
			nc = HAVE_BAD_WORD;

			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Error] O nick contem palavras inapropriadas: "
					+ wnick + L" Player: " + std::to_wstring(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
		}

		if (nc == SUCCESS && std::regex_match(wnick.begin(), wnick.end(), std::wregex(L".*[ ].*"))) {
			nc = EMPETY_ERROR;

			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Error] O nick contem espaco em branco: "
					+ wnick + L" Player: " + std::to_wstring(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
		}

		if (nc == SUCCESS && wnick.size() < 4 || std::regex_match(wnick.begin(), wnick.end(), std::wregex(L".*[\\^$&,\\?`´~\\|\"@#¨'%*!\\\\].*"))) {
			nc = INCORRECT_NICK;

			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Error] O nick eh menor que 4 letras ou tem caracteres que nao pode: "
					+ wnick + L" Player: " + std::to_wstring(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
		}

		if (nc == SUCCESS) {
			CmdVerifNick cmd_vn(wnick, true/*Waiter*/);

			NormalManagerDB::add(0, &cmd_vn, nullptr, nullptr);

			cmd_vn.waitEvent();

			if (cmd_vn.getException().getCodeError() != 0)
				throw cmd_vn.getException();

			if (cmd_vn.getLastCheck()) {
				nc = NICK_IN_USE;

				_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Error] O nick ja esta em uso: "
						+ wnick + L" Player: " + std::to_wstring(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet007][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PANGYA_DB)
			nc = ERROR_DB;
		else
			nc = UNKNOWN_ERROR;

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message(std::string("[packet_func::packet007][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));

		nc = UNKNOWN_ERROR;
	}

	packet p;
	pacote00E(p, &pd._session, wnick, nc, error_info);
	session_send(p, &pd._session, 1);

	return 0;
};

int packet_func::packet008(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	packet p;

	try {

		uint32_t _typeid = pd._packet->readInt32();
		unsigned short default_hair = pd._packet->readUint8();
		unsigned short default_shirts = pd._packet->readUint8();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] Character Type: " + std::to_string(_typeid), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] Default hair: " + std::to_string(default_hair), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] Default shirts: " + std::to_string(default_shirts), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] Character Type: " + std::to_string(_typeid), CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] Default hair: " + std::to_string(default_hair), CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] Default shirts: " + std::to_string(default_shirts), CL_ONLY_FILE_LOG));
#endif

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet008");

		if (sIff::getInstance().findCharacter(_typeid) == nullptr)
			throw exception("[packet_func::packet008][Error] typeid character: " + std::to_string(_typeid) + " is worng.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 21, 0));

		if (default_hair > 9)
			throw exception("[packet_func::packet008][Error] default_hair: " + std::to_string(default_hair) + " is wrong. character: " + std::to_string(_typeid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 22, 0));

		if (default_shirts != 0)
			throw exception("[packet_func::packet008][Error] default_shirts: " + std::to_string(default_shirts) + " is wrong. character: " + std::to_string(_typeid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 23, 0));

		CharacterInfo ci{ 0 };

		ci.id = -1;
		ci._typeid = _typeid;
		ci.default_hair = (unsigned char)default_hair;
		ci.default_shirts = (unsigned char)default_shirts;

		// Default Parts
		ci.initComboDef();

		CmdAddCharacter cmd_ac(pd._session.m_pi.uid, ci, 0, 1, true/*Waiter*/);
		CmdAddFirstSet cmd_afs(pd._session.m_pi.uid, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_ac, nullptr, nullptr);
		NormalManagerDB::add(0, &cmd_afs, nullptr, nullptr);

		cmd_ac.waitEvent();

		if (cmd_ac.getException().getCodeError() != 0)
			throw cmd_ac.getException();

		// Info Character Add com o Id gerado no banco de dados
		ci = cmd_ac.getInfo();

		cmd_afs.waitEvent();

		if (cmd_afs.getException().getCodeError() != 0)
			throw cmd_afs.getException();

		// Update Character Equipado no banco de dados
		CmdUpdateCharacterEquiped cmd_uce(pd._session.m_pi.uid, ci.id, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_uce, nullptr, nullptr);

		cmd_uce.waitEvent();

		if (cmd_uce.getException().getCodeError() != 0)
			throw cmd_uce.getException();

		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][Log] First Character Set with success! to player: " + std::to_string(pd._session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

		// Ok
		pacote011(p, &pd._session);
		session_send(p, &pd._session, 1);

		// Success Login
		SUCCESS_LOGIN("packet008", _arg1, &pd._session);

	}catch (exception& e) {
		// Erro na hora de salvar o character
		pacote011(p, &pd._session, 1);
		session_send(p, &pd._session, 1);
		pacote00E(p, &pd._session, L"", 12, 500051);
		session_send(p, &pd._session, 1);

		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet00B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sls::ls::getInstance().requestReLogin(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LOGIN_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet_sv003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	// Delete player "Desconnecta player"
	//pp.m_pw->_session_pool.deleteSession(pp.m_session);
	// Parece que ele desconectar sozinho já
	//::shutdown(pp.m_session->m_sock, SD_RECEIVE);

	return 0;
};

int packet_func::packet_sv006(void * _arg1, void * _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	_smp::message_pool::getInstance().push(new message("[packet_func::packet_sv006][Log] Time: " + std::to_string((std::clock() - pd._session.m_time_start) / (double)CLOCKS_PER_SEC), CL_ONLY_FILE_TIME_LOG));

	_smp::message_pool::getInstance().push(new message("[packet_func::packet_sv006][Log] Send SUCCESS LOGIN Time: " + std::to_string((std::clock() - pd._session.m_tick_bot) / (double)CLOCKS_PER_SEC), CL_ONLY_FILE_TIME_LOG));

	return 0;
};

int packet_func::packet_svFazNada(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	// Faz Nada

	return 0;
};

int packet_func::packet_svDisconectPlayerBroadcast(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	/*if (pp.m_pw != nullptr)
		delete pp.m_pw;*/
	// faz nada aqui

	return 0;
};

int packet_func::packet_as001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2);

	try {

		// Log Teste
		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as001][Log] Teste, so para deixar aqui, quando for usar um dia.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as001][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::pacote001(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0x001);

	p.addInt8(option);	// OPTION 1 SENHA OU ID ERRADO

	if (option == 0) {
		p.addString(_session->m_pi.id);
		p.addInt32(_session->m_pi.uid);
		p.addInt32(_session->m_pi.m_cap);
		p.addInt16(_session->m_pi.level);
		p.addInt32(0);
		p.addInt32(0);
		p.addString(_session->m_pi.nickname);
		p.addInt16(0);
	}else if (option == 1)
		p.addInt32(0);	// add 4 bytes vazios
	else if (option == 0xD8) {		// First Login
		p.addInt32(-1);
		p.addInt16(0);
	}else if (option == 0xD9)		// First Set
		p.addInt16(0);

	return 0;
};

int packet_func::pacote002(packet& p, player *_session, std::vector< ServerInfo >& v_element) {
	
	p.init_plain((unsigned short)0x002);

	p.addInt8(v_element.size() & 0xFF);	// 1 Game Server online

	for (size_t i = 0; i < v_element.size(); i++)
		p.addBuffer(&v_element[i], sizeof(ServerInfo));

	return 0;
};

int packet_func::pacote003(packet& p, player *_session, std::string& AuthKeyLogin, int option) {
	
	p.init_plain((unsigned short)0x003);

	p.addInt32(option);

	p.addString(AuthKeyLogin);

	return 0;
};

int packet_func::pacote006(packet& p, player *_session, chat_macro_user& _mu) {
	
	p.init_plain((unsigned short)0x006);

	p.addBuffer(&_mu, sizeof(chat_macro_user));

	return 0;
};

int packet_func::pacote009(packet& p, player *_session, std::vector< ServerInfo >& v_element) {

	p.init_plain((unsigned short)0x009);
	
	p.addInt8(v_element.size() & 0xFF);	// nenhum Msn Server on

	for (size_t i = 0; i < v_element.size(); i++)
		p.addBuffer(&v_element[i], sizeof(ServerInfo));

	return 0;
};

inline int packet_func::pacote00E(packet& p, player *_session, std::string nick, int option, uint32_t error) {
	return pacote00E(p, _session, MbToWc(nick), option, error);
};

int packet_func::pacote00E(packet& p, player *_session, std::wstring nick, int option, uint32_t error) {

	p.init_plain((unsigned short)0x00E);

	p.addInt32(option);

	if (option == 0)
		p.addString(nick);
	else if (option == 12)
		p.addInt32(error);

	return 0;
};

int packet_func::pacote00F(packet& p, player *_session, int option) {

	p.init_plain((unsigned short)0x0F);

	p.addInt8(option);
	
	p.addString(_session->m_pi.id);

	return 0;
};

int packet_func::pacote010(packet& p, player *_session, std::string& AuthKey) {

	p.init_plain((unsigned short)0x10);

	p.addString(AuthKey);

	return 0;
};

int packet_func::pacote011(packet& p, player *_session, int option) {

	p.init_plain((unsigned short)0x11);

	p.addUint16((unsigned short)option);

	return 0;
};

inline void packet_func::succes_login(void* _arg, player *_session, int option) {
	_MAKE_BEGIN_LS(_arg);

	std::vector< ServerInfo > sis;
	std::vector< ServerInfo > msns;
	chat_macro_user _cmu;
	std::string auth_key_login = "";

	/* OPTION
	*  0 PRIMEIRO LOGIN
	*  1 RELOGA DEPOIS QUE CAIU DO GAME SERVER, COM A AUTH KEY
	*/

	try {

		CmdServerList cmd_server_list(CmdServerList::GAME, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_server_list, nullptr, nullptr);

		cmd_server_list.waitEvent();

		if (cmd_server_list.getException().getCodeError() != 0)
			throw cmd_server_list.getException();

		sis = cmd_server_list.getServerList();

		cmd_server_list.setType(CmdServerList::MSN);

		CmdAuthKeyLogin cmd_auth_key_login(_session->m_pi.uid, true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_server_list, nullptr, nullptr);
		NormalManagerDB::add(0, &cmd_auth_key_login, nullptr, nullptr);

		cmd_server_list.waitEvent();
	
		if (cmd_server_list.getException().getCodeError() != 0)
			throw cmd_server_list.getException();

		msns = cmd_server_list.getServerList();

		cmd_auth_key_login.waitEvent();

		if (cmd_auth_key_login.getException().getCodeError() != 0)
			throw cmd_auth_key_login.getException();

		auth_key_login = cmd_auth_key_login.getAuthKey();

		_smp::message_pool::getInstance().push(new message("[packet_func::succes_login][Log] AuthKeyLogin: " + auth_key_login
				+ ", do player: " + std::to_string(_session->m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

		if (option == 0) {
			CmdChatMacroUser cmd_macro_user(_session->m_pi.uid, true/*Waiter*/);

			NormalManagerDB::add(0, &cmd_macro_user, nullptr, nullptr);

			cmd_macro_user.waitEvent();

			if (cmd_macro_user.getException().getCodeError() != 0)
				throw cmd_macro_user.getException();

			_cmu = cmd_macro_user.getMacroUser();
		}

		auto ip = std::string(_session->m_ip);

		// RegisterLogin do Player
		CmdRegisterPlayerLogin cmd_rpl(_session->m_pi.uid, ip, ls->getUID(), true/*Waiter*/);

		NormalManagerDB::add(0, &cmd_rpl, nullptr, nullptr);

		cmd_rpl.waitEvent();

		if (cmd_rpl.getException().getCodeError() != 0)
			throw cmd_rpl.getException();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::succes_login][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::EXEC_QUERY) {
			if (STDA_ERROR_DECODE(e.getCodeError()) != 7/*getServerList*/ && STDA_ERROR_DECODE(e.getCodeError()) != 9/*MacroUser*/
				&& STDA_ERROR_DECODE(e.getCodeError()) != 8/*getMsnList*/ && STDA_ERROR_DECODE(e.getCodeError()) != 5/*AuthKey*/)
				throw;
		}else
			throw;
	}

	packet p;

	pacote010(p, _session, auth_key_login);
	session_send(p, _session, 1);

	if (option == 0) {
		pacote001(p, _session);
		session_send(p, _session, 1);
	}
	
	pacote002(p, _session, sis);
	session_send(p, _session, 1);
	
	pacote009(p, _session, msns);
	session_send(p, _session, 1);
	
	if (option == 0) {
		pacote006(p, _session, _cmu);
		session_send(p, _session, 1);
	}
};

void packet_func::session_send(packet& p, player *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("[packet_func::session_send][Error] session *s is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 1, 2));

	MAKE_SEND_BUFFER(p, s);

	SHOW_DEBUG_PACKET(p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);

	//delete p;
};

void packet_func::session_send(std::vector< packet* > v_p, player *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("[packet_func::session_send][Error] session *s is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 1, 2));

	for (auto i = 0u; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			MAKE_SEND_BUFFER(*v_p[i], s);

			SHOW_DEBUG_PACKET(*v_p[i], _debug);

			SHOW_DEBUG_FINAL_PACKET(mb, _debug);

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("[packet_func::session_send][Error] packet *p is nullptr.", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};
