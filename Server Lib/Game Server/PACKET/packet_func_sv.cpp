// Arquivo packet_func_sv.cpp
// Criado em 13/08/2017 por Acrisio
// Implementação da classe packet_func para o Game Server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "packet_func_sv.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/PACKET/packet.h"
#include "../../Projeto IOCP/THREAD POOL/threadpool.h"
#include "../../Projeto IOCP/UTIL/hex_util.h"
#include "../../Projeto IOCP/UTIL/util_time.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <algorithm>

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../Game Server/game_server.h"

#include "../GAME/item_manager.h"

#include <ctime>
#include <regex>

#include "../../Projeto IOCP/PANGYA_DB/cmd_server_list.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_login_info.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_game.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_update_auth_key_login.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_nick.hpp"

#include "../UTIL/mgr_achievement.hpp"
#include "../UTIL/mgr_daily_quest.hpp"
#include "../UTIL/sys_achievement.hpp"

#include "../GAME/treasure_hunter_system.hpp"

#include "../GAME/mail_box_manager.hpp"

#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_member_info.hpp"
#include "../PANGYA_DB/cmd_caddie_info.hpp"
#include "../PANGYA_DB/cmd_mascot_info.hpp"
#include "../PANGYA_DB/cmd_warehouse_item.hpp"
#include "../PANGYA_DB/cmd_tutorial_info.hpp"
#include "../PANGYA_DB/cmd_cookie.hpp"
#include "../PANGYA_DB/cmd_coupon_gacha.hpp"
#include "../PANGYA_DB/cmd_treasure_hunter_info.hpp"
#include "../PANGYA_DB/cmd_achievement_info.hpp"
#include "../PANGYA_DB/cmd_card_info.hpp"
#include "../PANGYA_DB/cmd_card_equip_info.hpp"
#include "../PANGYA_DB/cmd_item_buff_info.hpp"
#include "../PANGYA_DB/cmd_my_room_config.hpp"
#include "../PANGYA_DB/cmd_my_room_item.hpp"
#include "../PANGYA_DB/cmd_dolfini_locker_info.hpp"
#include "../PANGYA_DB/cmd_mail_box_info.hpp"
#include "../PANGYA_DB/cmd_email_info.hpp"
#include "../PANGYA_DB/cmd_delete_email.hpp"
#include "../PANGYA_DB/cmd_item_left_from_email.hpp"
#include "../PANGYA_DB/cmd_msg_off_info.hpp"
#include "../PANGYA_DB/cmd_attendance_reward_info.hpp"
#include "../PANGYA_DB/cmd_last_player_game_info.hpp"
#include "../PANGYA_DB/cmd_premium_ticket_info.hpp"
#include "../PANGYA_DB/cmd_daily_quest_info_user.hpp"
#include "../PANGYA_DB/cmd_old_daily_quest_info.hpp"
#include "../PANGYA_DB/cmd_add_daily_quest.hpp"
#include "../PANGYA_DB/cmd_delete_daily_quest.hpp"
#include "../PANGYA_DB/cmd_gera_web_key.hpp"
#include "../PANGYA_DB/cmd_check_achievement.hpp"

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).makeFull((_session)->m_key); \
						 WSABUF mb = (_packet).getMakedBuf(); \
						 try { \
							\
							(_session)->usa(); \
							\
							(_session)->requestSendBuffer(mb.buf, mb.len); \
							\
							if ((_session)->devolve()) \
								sgs::gs::getInstance().DisconnectSession((_session)); \
							\
						 }catch (exception& e) { \
							\
							if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/)) \
								if ((_session)->devolve()) \
									sgs::gs::getInstance().DisconnectSession((_session)); \
							\
							if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2)) \
								throw; \
						 } \

// Verifica se session está autorizada para executar esse ação, 
// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!pd._session.m_is_authorized) \
												throw exception("[packet_func::" + std::string((method)) + "][Error] Player[UID=" + std::to_string(pd._session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0x7000501)); \

using namespace stdA;

int packet_func::packet002(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		gs->requestLogin(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet002][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestChat(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet004(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet004][Log] Hex:\n\r" + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Enter Channel, channel ID
		gs->requestEnterChannel(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet004][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet006(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishGame(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet006][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
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

	std::wstring nick = L"";

	unsigned char opt = 0u;
	unsigned char error = 2u;

	MemberInfoEx mi;

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet007");

		opt = pd._packet->readUint8();

		if (opt != 0)
			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][WARNING] Player[UID=" + std::to_wstring(pd._session.m_pi.uid) 
					+ L"] Pediu para Check Nickname: " + nick + L", [OPT=" + std::to_wstring(opt) + L"] diferente de 0.", CL_FILE_LOG_AND_CONSOLE));

		nick = pd._packet->readWString();

		_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Log] Player[UID=" + std::to_wstring(pd._session.m_pi.uid) 
				+ L"] Pediu para Check Nickname: " + nick, CL_FILE_LOG_AND_CONSOLE));

		if (nc == SUCCESS && std::regex_match(nick.begin(), nick.end(), std::wregex(L".*[ ].*"))) {
			nc = EMPETY_ERROR;

			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Log] Player[UID=" + std::to_wstring(pd._session.m_pi.uid) 
					+ L"] Pediu para verificar o nick contem espaco em branco: " + nick, CL_FILE_LOG_AND_CONSOLE));
		}

		if (nc == SUCCESS && nick.size() < 4 || std::regex_match(nick.begin(), nick.end(), std::wregex(L".*[\\^$&,\\?`´~\\|\"@#¨'%*!\\\\].*"))) {
			nc = INCORRECT_NICK;

			_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Log] Player[UID=" + std::to_wstring(pd._session.m_pi.uid) 
					+ L"] Pediu para verificar o nick eh menor que 4 letras ou tem caracteres que nao pode: " + nick, CL_FILE_LOG_AND_CONSOLE));
		}

		if (nc == SUCCESS) {
			CmdVerifNick cmd_vn(nick, true);	// Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_vn, nullptr, nullptr);

			cmd_vn.waitEvent();

			if (cmd_vn.getException().getCodeError() != 0)
				throw cmd_vn.getException();

			if (cmd_vn.getLastCheck()) {
				nc = NICK_IN_USE;

				error = (nc == NICK_IN_USE && cmd_vn.getUID() != 0 ? 0 : 2);

				CmdMemberInfo cmd_mi(cmd_vn.getUID(), true);	// Waiter

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_mi, nullptr, nullptr);

				cmd_mi.waitEvent();

				if (cmd_mi.getException().getCodeError() != 0)
					throw cmd_mi.getException();

				mi = cmd_mi.getInfo();

				_smp::message_pool::getInstance().push(new message(L"[packet_func::packet007][Log] Player[UID=" + std::to_wstring(pd._session.m_pi.uid) + L"] Pediu para verificar o nick ja esta em uso: " + nick, CL_FILE_LOG_AND_CONSOLE));
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

	try {

		packet p((unsigned short)0xA1);

		p.addUint8(error);

		if (error == 0 && nc == NICK_IN_USE) {
			p.addInt32(mi.uid);

			p.addBuffer(&mi, sizeof(MemberInfo));	// esse aqui no antigo enviar sem o número da sala
		}

		session_send(p, &pd._session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet007][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet008(void * _arg1, void * _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);
	
	try {

		channel *_channel = gs->findChannel(pd._session.m_pi.channel);

		if (_channel != nullptr)
			_channel->requestMakeRoom(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet008][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet009(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *_channel = gs->findChannel(pd._session.m_pi.channel);

		if (_channel != nullptr)
			_channel->requestEnterRoom(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet009][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet00A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *_channel = gs->findChannel(pd._session.m_pi.channel);

		if (_channel != nullptr)
			_channel->requestChangeInfoRoom(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet00B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *_channel = gs->findChannel(pd._session.m_pi.channel);

		if (_channel != nullptr)
			_channel->requestChangePlayerItemChannel(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet00C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *_channel = gs->findChannel(pd._session.m_pi.channel);

		// Bloquear para ver se funciona o sync do entra depois no camp, 
		// mesmo que o outro(0x9D) chama primeiro esse(0x0C) é mais rápido para verificar se o player está em uma sala
		pd._session.lockSync();

		if (_channel != nullptr)
			_channel->requestChangePlayerItemRoom(pd._session, pd._packet);

		// Bloquear para ver se funciona o sync do entra depois no camp, 
		// mesmo que o outro(0x9D) chama primeiro esse(0x0C) é mais rápido para verificar se o player está em uma sala
		pd._session.unlockSync();

	}catch (exception& e) {

		// Bloquear para ver se funciona o sync do entra depois no camp, 
		// mesmo que o outro(0x9D) chama primeiro esse(0x0C) é mais rápido para verificar se o player está em uma sala
		pd._session.unlockSync();

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet00D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangePlayerStateReadyRoom(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet00E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestStartGame(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00E][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet00F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExitRoom(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet00F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet010(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangePlayerTeamRoom(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet010][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet011(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishLoadHole(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet011][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet012(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet12][Log] request Player[UID=" + std::to_string(pd._session.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (c != nullptr)
			c->requestInitShot(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet012][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet013(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeMira(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet013][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet014(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeStateBarSpace(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet014][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}
	
	return 0;
};

int packet_func::packet015(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActivePowerShot(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet015][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet016(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeClub(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet016][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet017(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUseActiveItem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet017][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet018(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeStateTypeing(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet018][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet019(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestMoveBall(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet019][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet01A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestInitHole(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet01B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet1B][Log] request Player[UID=" + std::to_string(pd._session.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (c != nullptr)
			c->requestSyncShot(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet01C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishShot(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet01D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestBuyItemShop(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet01F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestGiftItemShop(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer excpetion que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet020(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangePlayerItemMyRoom(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet020][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet022(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestStartTurnTime(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet022][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet026(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestKickPlayerOfRoom(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet026][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::CHANNEL, 1))		// Diferente do erro 1 de channel
			throw;
	}

	return 0;
};

int packet_func::packet029(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCheckInvite(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet029][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet02A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestPrivateMessage(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet02A][ErrorSystem] " + e.getFullMessageError() , CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet02D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestShowInfoRoom(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet02D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::CHANNEL, 1))		// Diferente do erro 1 de channel
			throw;
	}

	return 0;
};

int packet_func::packet02F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestPlayerInfo(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet02F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet030(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUnOrPauseGame(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet030][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet031(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishHoleData(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet031][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet032(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangePlayerStateAFKRoom(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet032][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet033(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestExceptionClientMessage(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet033][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet034(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishCharIntro(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet034][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet035(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestTeamFinishHole(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet035][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet036(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestReplyContinueVersus(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet036][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet037(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestLastPlayerFinishVersus(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet037][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet039(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestPayCaddieHolyDay(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet039][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet03A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestPlayerReportChatGame(pd._session, pd._packet);
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet03A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet03C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestTranslateSubPacket(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet03C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet03D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCookie(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet03D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet03E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestEnterSpyRoom(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet03E][ErrorSystem] " + e.getCodeError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet041(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExecCCGIdentity(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet041][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet042(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestInitShotArrowSeq(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet042][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet043(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->sendServerListAndChannelListToSession(pd._session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet043][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet047(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->sendRankServer(pd._session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet047][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet048(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestLoadGamePercent(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet048][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet04A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveReplay(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet04A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet04B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetStatsUpdate(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet04B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet04F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeStateChatBlock(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet04F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet054(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChatTeam(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet054][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet055(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestChangeWhisperState(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet055][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet057(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestCommandNoticeGM(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet057][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet05C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->sendDateTimeToSession(pd._session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet05C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet060(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExecCCGDestroy(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet060][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet061(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			_smp::message_pool::getInstance().push(new message("[packet_func::packet061][Log] player[UID=" + std::to_string(pd._session.m_pi.uid) 
					+ "] tentou desconectar um player, mas o server ja faz o tratamento do packet08F do comando GM.", CL_FILE_LOG_AND_CONSOLE));

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet061");

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet061][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
}

int packet_func::packet063(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *c = gs->findChannel(pd._session.m_pi.channel);
		
		if (c != nullptr)
			c->requestPlayerLocationRoom(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet063][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::CHANNEL, 1))		// Diferente do erro 1 de channel
			throw;
	}

	return 0;
};

int packet_func::packet064(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestDeleteActiveItem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet064][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet065(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveBooster(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet065][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet066(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestSendTicker(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet066][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet067(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestQueueTicker(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet067][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet069(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestChangeChatMacroUser(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet069][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet06B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestSetNoticeBeginCaddieHolyDay(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet06B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet073(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeMascotMessage(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet73][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet074(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCancelEditSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet074][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet075(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCloseSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet075][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet076(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenEditSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet076][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet077(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestViewSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet077][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet078(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCloseViewSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet078][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet079(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeNameSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet079][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet07A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestVisitCountSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet07A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet07B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto r = gs->findChannel(pd._session.m_pi.channel);

		if (r != nullptr)
			r->requestPangSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet07B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet07C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet07C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet07D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestBuyItemSaleShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet07D][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet081(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet081][Log] Hex:\n\r" + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestEnterLobby(pd._session, pd._packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet081][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet082(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet082][Log] Hex:\n\r" + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExitLobby(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet082][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::CHANNEL, 1))		// Diferente do erro 1 de channel
			throw;
	}

	return 0;
};

int packet_func::packet083(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet083][Log] Hex:\n\r" + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		gs->requestEnterOtherChannelAndLobby(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet083][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet088(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestCheckGameGuardAuthAnswer(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet088][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet08B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet08B");
		
		CmdServerList cmd_sl(CmdServerList::MSN, true);	// waitable

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_sl, nullptr, nullptr);

		cmd_sl.waitEvent();

		if (cmd_sl.getException().getCodeError() != 0)
			_smp::message_pool::getInstance().push(new message("[packet_func::packet08B][Error] " + cmd_sl.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		auto v_si = cmd_sl.getServerList();
		//std::vector< ServerInfo > v_si = pangya_db::getMSNServer();

		packet p;

		pacote0FC(p, &pd._session, v_si);
		session_send(p, &pd._session, 0);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet08B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet08F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestCommonCmdGM(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet08F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet098(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenPapelShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet098][ErrorSystem] " + e.getFullMessageError() , CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0A1(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestEnterWebLinkState(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0A1][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0A2(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExitedFromWebGuild(pd._session, pd._packet);
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0A2][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0AA(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUseTicketReport(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0AA][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}
	
	return 0;
};

int packet_func::packet0AB(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenTicketReportScroll(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0AB][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0AE(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestMakeTutorial(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0AE][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0B2(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenBoxMyRoom(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0B2][ErrorSystem] " + e.getFullMessageError() , CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0B4(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Esse pacote é que o player aceitou convite do player entrou na sala saiu e relogou, ai manda esse pacote com o número da sala
		/*_smp::message_pool::getInstance().push(new message("[packet_func::packet0B4][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid) 
				+ "] mandou o Pacote0B4 mas nao sei o que ele pede ou faz ainda. Hex: \n\r" 
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));*/

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0B4");

		unsigned char option = pd._packet->readUint8();
		unsigned short numero_sala = pd._packet->readUint16();

		// Log
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0B4][Log][Option=" + std::to_string((unsigned short)option) 
				+ "] Player[UID=" + std::to_string(pd._session.m_pi.uid) + "] foi convidado por um player aceitou o pedido saiu da sala[NUMERO=" 
				+ std::to_string(numero_sala) + "] e relogou.", CL_FILE_LOG_AND_CONSOLE));
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0B4][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet09C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet09C(Last5Player)");

		// Last 5 Player Game Info
		packet p;

		pacote10E(p, &pd._session, pd._session.m_pi.l5pg);
		session_send(p, &pd._session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet09C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet09D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		// Bloquear para ver se funciona o sync do entra depois no camp, 
		// mesmo que o outro(0x9D) chama primeiro esse(0x0C) é mais rápido para verificar se o player está em uma sala
		pd._session.lockSync();

		if (c != nullptr)
			c->requestEnterGameAfterStarted(pd._session, pd._packet);

		// Bloquear para ver se funciona o sync do entra depois no camp, 
		// mesmo que o outro(0x9D) chama primeiro esse(0x0C) é mais rápido para verificar se o player está em uma sala
		pd._session.unlockSync();

	}catch (exception& e) {

		// Bloquear para ver se funciona o sync do entra depois no camp, 
		// mesmo que o outro(0x9D) chama primeiro esse(0x0C) é mais rápido para verificar se o player está em uma sala
		pd._session.unlockSync();

		_smp::message_pool::getInstance().push(new message("[packet_func::packet09D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet09E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUpdateGachaCoupon(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet09E][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0B5(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0B5(MyrRoomHouseInfo)");

		uint32_t from_uid, to_uid;

		from_uid = pd._packet->readUint32();
		to_uid = pd._packet->readUint32();

		packet p((unsigned short)0x12B);

		// Aqui o player só pode pedir para entrar no dele mesmo
		if (from_uid == to_uid && pd._session.m_pi.mrc.allow_enter) {	// Isso tinha no season 4, agora nos season posteriores tiraram isso
			p.addUint32(1);	// option;

			p.addUint32(to_uid);

			p.addBuffer(&pd._session.m_pi.mrc, sizeof(MyRoomConfig));
		}else {
			p.addUint32(0);

			p.addUint32(to_uid);
		}

		session_send(p, &pd._session, 0);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0B5][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet0B7(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0B7(InfoPlayerMyRoom)");

		// @@!! Ajeitar para pegar a função estática da sala que initializa o Player Room Info
		PlayerRoomInfoEx pri;

		// Player Room Info Init
		pri.oid = pd._session.m_oid;
#if defined(_WIN32)
		memcpy_s(pri.nickname, sizeof(pri.nickname), pd._session.m_pi.nickname, sizeof(pri.nickname));
		memcpy_s(pri.guild_name, sizeof(pri.guild_name), pd._session.m_pi.gi.name, sizeof(pri.guild_name));
#elif defined(__linux__)
		memcpy(pri.nickname, pd._session.m_pi.nickname, sizeof(pri.nickname));
		memcpy(pri.guild_name, pd._session.m_pi.gi.name, sizeof(pri.guild_name));
#endif
		pri.position = 0;	// posição na sala
		pri.capability = pd._session.m_pi.m_cap;
		pri.title = pd._session.m_pi.ue.m_title;

		if (pd._session.m_pi.ei.char_info != nullptr)
			pri.char_typeid = pd._session.m_pi.ei.char_info->_typeid;

#if defined(_WIN32)
		memcpy_s(pri.skin, sizeof(pri.skin), pd._session.m_pi.ue.skin_typeid, sizeof(pri.skin));
#elif defined(__linux__)
		memcpy(pri.skin, pd._session.m_pi.ue.skin_typeid, sizeof(pri.skin));
#endif
		pri.skin[4] = 0;		// Aqui tem que ser zero, se for outro valor não mostra a imagem do character equipado

		pri.state_flag.uFlag.stFlagBit.master = 1;
		pri.state_flag.uFlag.stFlagBit.ready = 1;	// Sempre está pronto(ready) o master

		pri.state_flag.uFlag.stFlagBit.sexo = pd._session.m_pi.mi.sexo;

		// Só faz calculo de Quita rate depois que o player
		// estiver no level Beginner E e jogado 50 games
		if (pd._session.m_pi.level >= 6 && pd._session.m_pi.ui.jogado >= 50) {
			float rate = pd._session.m_pi.ui.getQuitRate();

			if (rate < GOOD_PLAYER_ICON)
				pri.state_flag.uFlag.stFlagBit.azinha = 1;
			else if (rate >= QUITER_ICON_1 && rate < QUITER_ICON_2)
				pri.state_flag.uFlag.stFlagBit.quiter_1 = 1;
			else if (rate >= QUITER_ICON_2)
				pri.state_flag.uFlag.stFlagBit.quiter_2 = 1;
		}

		pri.level = pd._session.m_pi.mi.level;

		if (pd._session.m_pi.ei.char_info != nullptr && pd._session.m_pi.ui.getQuitRate() < GOOD_PLAYER_ICON)
			pri.icon_angel = pd._session.m_pi.ei.char_info->AngelEquiped();
		else
			pri.icon_angel = 0u;

		pri.ucUnknown_0A = 10;	// 0x0A dec"10" _session.m_pi.place
		pri.guild_uid = pd._session.m_pi.gi.uid;
#if defined(_WIN32)
		memcpy_s(pri.guild_mark_img, sizeof(pri.guild_mark_img), pd._session.m_pi.gi.mark_emblem, sizeof(pri.guild_mark_img));
#elif defined(__linux__)
		memcpy(pri.guild_mark_img, pd._session.m_pi.gi.mark_emblem, sizeof(pri.guild_mark_img));
#endif
		pri.guild_mark_index = pd._session.m_pi.gi.index_mark_emblem;
		pri.uid = pd._session.m_pi.uid;
		pri.state_lounge = pd._session.m_pi.state_lounge;
		pri.usUnknown_flg = 0;
		pri.state = pd._session.m_pi.state;
		pri.location = { pd._session.m_pi.location.x, pd._session.m_pi.location.z, pd._session.m_pi.location.r };
		pri.shop = { 0 };

		if (pd._session.m_pi.ei.mascot_info != nullptr)
			pri.mascot_typeid = pd._session.m_pi.ei.mascot_info->_typeid;

		pri.flag_item_boost = pd._session.m_pi.checkEquipedItemBoost();
		pri.ulUnknown_flg = 0;
		//pri.id_NT não estou usando ainda
		//pri.ucUnknown106
		pri.convidado = 0;	// Flag Convidado, [Não sei bem por que os que entra na sala normal tem valor igual aqui, já que é flag de convidado waiting], Valor constante da sala para os players(ACHO)
		pri.avg_score = pd._session.m_pi.ui.getMediaScore();
		//pri.ucUnknown3

		if (pd._session.m_pi.ei.char_info != nullptr)
			pri.ci = *pd._session.m_pi.ei.char_info;

		packet p((unsigned short)0x168);	// Character Equipado

		p.addBuffer(&pri, sizeof(PlayerRoomInfoEx));

		session_send(p, &pd._session, 0);

		p.init_plain((unsigned short)0x12D);	// Itens do Myroom, Mala, Email, sofa, teto chao, e poster, "NESSA SEASON, SÓ USA POSTER"

		p.addUint32(1);	// Option

		p.addUint16((unsigned short)pd._session.m_pi.v_mri.size());

		for (auto i = 0u; i < pd._session.m_pi.v_mri.size(); ++i)
			p.addBuffer(&pd._session.m_pi.v_mri[i], sizeof(MyRoomItem));

		session_send(p, &pd._session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0B7][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet0B9(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		gs->requestUCCSystem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0B9][ErrorSystem]", CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet0BA(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = sgs::gs::getInstance().findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestInvite(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0BA][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0BD(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUseCardSpecial(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0BD][ErrorSytem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0C1(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0C1(UpdatePlace)");

		pd._session.m_pi.place = pd._packet->readUint8();	// Att place(lugar)

		// Update Location Player on DB
		pd._session.m_pi.updateLocationDB();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0C1][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet0C9(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		gs->requestUCCWebKey(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0C9][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet0CA(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenCardPack(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0CA][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0CB(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishGame(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0CB][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0CC(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCheckDolfiniLockerPass(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0CC][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0CD(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestDolfiniLockerItem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0CD][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0CE(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestAddDolfiniLockerItem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0CE][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0CF(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestRemoveDolfiniLockerItem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0CF][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0D0(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestMakePassDolfiniLocker(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D0][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0D1(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeDolfiniLockerPass(pd._session, pd._packet);

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D1][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0D2(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeDolfiniLockerModeEnter(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D2][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0D3(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0D3(CheckDolfiniLocker)");

		uint32_t check = 0u;

		check = pd._session.m_pi.df.isLocker();

		packet p((unsigned short)0x170);

		p.addUint32(0);	// option
		p.addUint32(check);

		session_send(p, &pd._session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D3][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet0D4(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUpdateDolfiniLockerPang(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D4][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0D5(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestDolfiniLockerPang(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D5][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0D8(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestUseItemBuff(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0D8][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0DE(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Player não pode ver a message privada que o player mandou, avisa para o server
		/*_smp::message_pool::getInstance().push(new message("[packet_func::packet0DE][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid) 
				+ "] mandou o Pacote0DE mas nao sei o que ele pede ou faz ainda. Hex: \n\r" 
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));*/

		// Envia mensagem para o player que enviou o MP que o player não pode ver a mensagem
		gs->requestNotifyNotDisplayPrivateMessageNow(pd._session, pd._packet);
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet0DE][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::packet0E5(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveCutin(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0E5][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0E6(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExtendRental(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0E6][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0E7(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestDeleteRental(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0E7][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0EB(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		channel *c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestPlayerStateCharacterLounge(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0EB][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)	// Por Hora relança qualquer exception que não seja do channel
			throw;
	}

	return 0;
};

int packet_func::packet0EC(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCometRefill(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0EC][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0EF(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenBoxMail(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0EF][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet0F4(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		// TTL = Time To Live
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Atualiza Time Bot do player: " + std::to_string(pd._session.m_pi.uid) + ", time anterior: " + std::to_string((int64_t)(((std::clock() - pd._session.m_tick_bot) / (double)CLOCKS_PER_SEC) * 1000)) + "ms", CL_ONLY_FILE_LOG));
#else
		if (((std::clock() - pd._session.m_tick_bot) / (double)CLOCKS_PER_SEC) >= ((gs->getBotTTL() + (gs->getBotTTL() / 4)/*TTL time + 1/4 dele*/) / 1000.0))
			_smp::message_pool::getInstance().push(new message("[Warning][Bot Or Lag] Atualiza Time Bot do player: " + std::to_string(pd._session.m_pi.uid) + ", time anterior: " + std::to_string((int64_t)(((std::clock() - pd._session.m_tick_bot) / (double)CLOCKS_PER_SEC) * 1000)) + "ms", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0F4");

		pd._session.m_tick_bot = std::clock();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0F4][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet0FB(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	packet p;

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0FB(WebKey)");
		
		CmdGeraWebKey cmd_gwk(pd._session.m_pi.uid, true);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_gwk, nullptr, nullptr);

		cmd_gwk.waitEvent();

		if (cmd_gwk.getException().getCodeError() != 0)
			throw cmd_gwk.getException();

		auto webKey = cmd_gwk.getKey();

		pacote1AD(p, &pd._session, webKey, 1);
		session_send(p, &pd._session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0FB][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error Reply
		p.init_plain((unsigned short)0x1AD);

		p.addUint32(0);	// Error
		p.addUint16(0);	// size Key

		session_send(p, &pd._session, 1);
	}

	return 0;
};

int packet_func::packet0FE(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// @@!! Depois tenho que descobrir o que seja isso que ele passa e o que recebe do cliente(OPTION)
		_smp::message_pool::getInstance().push(new message("VALUE - (OPTION): " + std::to_string(pd._packet->readInt32()), CL_ONLY_FILE_LOG));

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet0FE");

		packet p;

		pacote1B1(p, &pd._session);
		session_send(p, &pd._session, 0);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet0FE][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet119(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	packet p;

	try {

		gs->requestChangeServer(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet119][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
}
int packet_func::packet126(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenLegacyTikiShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet126][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw; // Relança
	}

	return 0;
};

int packet_func::packet127(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestPointLegacyTikiShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet127][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw; // Relança
	}

	return 0;
};

int packet_func::packet128(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExchangeTPByItemLegacyTikiShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet128][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw; // Relança
	}

	return 0;
};

int packet_func::packet129(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExchangeItemByTPLegacyTikiShop(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet129][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw; // Relança
	}

	return 0;
};

int packet_func::packet12C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestFinishGame(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet12C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet12D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestReplyInitialValueGrandZodiac(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet12D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet12E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestMarkerOnCourse(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet12E][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet12F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet12F][Log] request Player[UID=" + std::to_string(pd._session.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (c != nullptr)
			c->requestShotEndData(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet12F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet130(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	packet p;

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestLeavePractice(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet130][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet131(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestLeaveChipInPractice(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet131][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet137(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestStartFirstHoleGrandZodiac(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet137][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet138(void* _arg1, void *_arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveWing(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet138][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet140(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet140(requestEnterShop)");

		packet p((unsigned short)0x20E);

		p.addUint32(0);
		p.addUint32(0);	// Não sei pode ACHO "ser Value acho, ou erro, pode ser dizendo que o shop esta bloqueado"

		session_send(p, &pd._session, 0);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet140][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet141(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestChangeWindNextHoleRepeat(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet141][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet143(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestOpenMailBox(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet143][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet144(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestInfoMail(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet144][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet145(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestSendMail(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet145][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet146(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet146][Log] Request player[UID=" + std::to_string(pd._session.m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (c != nullptr)
			c->requestTakeItemFomMail(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet146][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet147(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestDeleteMail(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet147][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet14B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestPlayPapelShop(pd._session, pd._packet);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet14B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet151(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestDailyQuest(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet151][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet152(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestAcceptDailyQuest(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet152][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}
	
	return 0;
};


int packet_func::packet153(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	
	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestTakeRewardDailyQuest(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet153][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}
	
	return 0;
};

int packet_func::packet154(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestLeaveDailyQuest(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet154][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet155(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestLoloCardCompose(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet155][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet156(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveAutoCommand(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet156][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet157(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	packet p;

	try {

		uint32_t uid = pd._packet->readUint32();

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet157][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid) + "].\tPlayer Achievement request uid: " + std::to_string(uid), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[packet_func::packet157][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid) + "].\tPlayer Achievement request uid: " + std::to_string(uid), CL_ONLY_FILE_LOG));
#endif // _DEBUG

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("packet157(requestAchievementInfo)");

		MgrAchievement* mgr_achievement = nullptr;
		player *s = nullptr;

		if (pd._session.m_pi.uid == uid)	// O player solicitou o próprio achievement info
			mgr_achievement = &pd._session.m_pi.mgr_achievement;
		else if ((s = gs->findPlayer(uid)) != nullptr)	// O player solicitou o achievement info de outro player online
			mgr_achievement = &s->m_pi.mgr_achievement;
		else {	// O player solicitou o achievement info de outro player off-line

			MgrAchievement mgr_achievement;

			mgr_achievement.initAchievement(uid);

			mgr_achievement.sendAchievementGuiToPlayer(pd._session);

			return 0;
		}

		if (mgr_achievement == nullptr) {
			pacote22C(p, &pd._session, 1);
			session_send(p, &pd._session, 1);	// Error
		}else
			mgr_achievement->sendAchievementGuiToPlayer(pd._session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet157][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		pacote22C(p, &pd._session, 1);
		session_send(p, &pd._session, 1);	// Error
	}

	return 0;
};

int packet_func::packet158(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCadieCauldronExchange(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet158][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet15C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActivePaws(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet15C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet15D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveRing(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet15D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet164(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopUpLevel(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet164][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
}

int packet_func::packet165(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopUpLevelConfirm(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet165][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
}

int packet_func::packet166(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopUpLevelCancel(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet166][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet167(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopUpRank(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet167][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet168(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopUpRankTransformConfirm(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet168][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet169(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopUpRankTransformCancel(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet169][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet16B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopRecoveryPts(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet16B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet16C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetWorkShopTransferMasteryPts(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet16C][ErrorSystem] ", CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet16D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestClubSetReset(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet16D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet16E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCheckAttendanceReward(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet16E][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet16F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestAttendanceRewardLoginCount(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet16F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet171(void* _arg1, void *_arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveEarcuff(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet171][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet172(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// !@ Log
		_smp::message_pool::getInstance().push(new message("[packet_func::packet172][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] request open Event Workshop 2013.", CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("[packet_func::packet172][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "]. Packet raw: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet172][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet176(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet176][Log] Hex:\n\r" + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestEnterLobbyGrandPrix(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet176][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet177(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet177][Log] Hex:\n\r" + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExitLobbyGrandPrix(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet177][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet179(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestEnterRoomGrandPrix(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet179][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet17A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestExitRoomGrandPrix(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet17A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet17F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestPlayMemorial(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet17F][ErrorSystem] " + e.getFullMessageError() , CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet180(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveGlove(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet180][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet181(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveRingGround(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet181][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet184(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestToggleAssist(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet184][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet185(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveAssistGreen(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet185][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet187(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCharacterMasteryExpand(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet187][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError() != STDA_ERROR_TYPE::CHANNEL))
			throw;
	}

	return 0;
};

int packet_func::packet188(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCharacterStatsUp(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet188][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet189(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCharacterStatsDown(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet189][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet18A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCharacterCardEquip(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet18A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet18B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCharacterCardEquipWithPatcher(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet18B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet18C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestCharacterRemoveCard(pd._session, pd._packet);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet18C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet18D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestTikiShopExchangeItem(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet18D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet192(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// !@ Log
		_smp::message_pool::getInstance().push(new message("[packet_func::packet192][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] request open Event Arin 2014.", CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("[packet_func::packet192][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "]. Packet raw: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet192][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet196(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveRingPawsRainbowJP(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet196][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet197(void* _arg1, void *_arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveRingPowerGagueJP(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet197][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet198(void* _arg1, void *_arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveRingMiracleSignJP(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet198][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet199(void* _arg1, void *_arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestActiveRingPawsRingSetJP(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet199][ErrorSystem]", CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet_sv4D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
		
		std::string str_tmp = "Time: " + std::to_string((std::clock() - pd._session.m_time_start) / (double)CLOCKS_PER_SEC);

		pd._session.m_time_start = std::clock();

		_smp::message_pool::getInstance().push(new message(str_tmp, CL_ONLY_FILE_TIME_LOG));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_sv4D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet_sv055(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		auto c = gs->findChannel(pd._session.m_pi.channel);

		if (c != nullptr)
			c->requestInitShotSended(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_sv055][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::CHANNEL)
			throw;
	}

	return 0;
};

int packet_func::packet_svRequestInfo(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	/*if (++pd._session.m_pi.ri.show == 12/*Show Info*//*)
		pacote089(p, &pd._session, pd._session.m_pi.ri.uid, pd._session.m_pi.ri.season);
		session_send(p, pd._session);
	*/
	return 0;
};

int packet_func::packet_sv22D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	packet p;

	// Já enviou o pacote22D com os achievement todo envia o pacote para abrir o GUI
	pacote22C(p, &pd._session);
	//session_send(p, &pd._session, 0);
	_smp::message_pool::getInstance().push(new message("Packet ID: " + std::to_string(pd._packet->getTipo()) + " has send.", CL_ONLY_FILE_LOG));

	return 0;
};

int packet_func::packet_svFazNada(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	// Esse pacote é para os pacotes que o server envia para o cliente
	// e não precisa de tratamento depois que foi enviado para o cliente

	return 0;
};

int packet_func::packet_svDisconectPlayerBroadcast(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		channel *_channel = gs->findChannel(pd._session.m_pi.channel);

		if (_channel != nullptr)
			_channel->leaveChannel(pd._session);

		if (_arg2 != nullptr)
			delete (ParamDispatch*)_arg2;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_svDisconectPlayerBroadcast][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet_as001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2);

	try {

#if INTPTR_MAX == INT64_MAX && MY_GG_SRV_LIB == 0
		sgs::gs::getInstance().authCmdKeyLoginGGAuthServer(_packet);
#elif INTPTR_MAX == INT32_MAX || MY_GG_SRV_LIB == 1
		// !@ Teste
		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as001][Log] Teste, ver se esta funcionando.", CL_FILE_LOG_AND_CONSOLE));
#else
#error Unknown pointer size or missing size macros!
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as001][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME_SERVER)
			throw;
	}

	return 0;
};

int packet_func::pacote040(packet& p, player *_session, PlayerInfo *pi, std::string msg, unsigned char option) {

	if ((option == 0 || option == 0x80) && pi == nullptr)
		throw exception("Error PlayerInfo *pi is nullptr. packet_func::pacote040()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x40);

	p.addInt8(option);

	if (option == 0 || option == 0x80) {
		p.addString(pi->nickname);
		p.addString(msg);
	}

	return 0;
};

int packet_func::pacote044(packet& p, player *_session, ServerInfoEx& _si, int option, PlayerInfo *pi, int valor) {
	if (option == 0 && pi == nullptr)
		throw exception("Erro PlayerInfo *pi is nullptr. packet_func::pacote044()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x44);

	p.addInt8(option & 0xFF);	// Option
	
	if (option == 0)
		principal(p, pi, _si);
	else if (option == 1)
		p.addInt8(0);
	else if (option == 0xD3)
		p.addInt8(0);
	else if (option == 0xD2)
		p.addInt32(valor);

	return 0;
};

int packet_func::pacote046(packet& p, player *_session, std::vector< PlayerCanalInfo > v_element, int option) {

	size_t elements = v_element.size();

	if (elements * sizeof(PlayerCanalInfo) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x46);
		p.addInt8((unsigned char)option);
		p.addInt8((unsigned char)elements);

		for (auto i = 0u; i < v_element.size(); i++)
			p.addBuffer(&v_element[i], sizeof(PlayerCanalInfo));

		return 1;
	}else {
		MAKE_BEGIN_SPLIT_PACKET(0x46, _session, sizeof(PlayerCanalInfo), MAX_BUFFER_PACKET);

		p.addInt8((unsigned char)option);
		p.addInt8((unsigned char)((total > por_packet) ? por_packet : total));

		MAKE_MID_SPLIT_PACKET_VECTOR(sizeof(PlayerCanalInfo));

		MAKE_END_SPLIT_PACKET(1);
	}

	return 0;
};

int packet_func::pacote047(packet& p, std::vector< RoomInfo > v_element, int option) {

	std::vector< packet* > v_p;
	p.init_plain((unsigned short)0x47);

	p.addUint8((unsigned char)((option == 0 ) ? v_element.size() : 1));				// count;
	p.addInt8((unsigned char)option);
	p.addInt16(-1);					// Não sei bem, mas sempre peguei esse pacote com -1 aqui

	for (auto i = 0u; i < v_element.size(); ++i)
		p.addBuffer(&v_element[i] , sizeof(RoomInfo));
	
	return 1;
};

int packet_func::pacote048(packet& p, player *_session, std::vector< PlayerRoomInfoEx > v_element, int option) {

	if ((option & 0xFF) == 2) {	// exit player
		p.init_plain((unsigned short)0x48);
		p.addUint8((unsigned char)option);
		p.addInt16(-1);

		p.addInt32(_session->m_oid);

		return 1;
	}else if ((option & 0xFF) == 7) {
		size_t elements = v_element.size();

		MAKE_BEGIN_SPLIT_PACKET(0x48, _session, (option & 0x100 ? sizeof(PlayerRoomInfo) : sizeof(PlayerRoomInfoEx)), sizeof(PlayerRoomInfoEx));

		p.addUint8((unsigned char)option);
		p.addInt16(-1);

		if ((option & 0xFF) == 0 || (option & 0xFF) == 5)
			p.addInt8((unsigned char)((total > por_packet) ? por_packet : total));
		else if ((option & 0xFF) == 7)
			p.addUint8((unsigned char)elements);
		else if ((option & 0xFF) == 3)
			p.addInt32(_session->m_oid);

		MAKE_MID_SPLIT_PACKET_VECTOR((option & 0x100 ? sizeof(PlayerRoomInfo) : sizeof(PlayerRoomInfoEx)));

		p.addUint8(0);	// Final list de PlayerRoomInfo

		MAKE_END_SPLIT_PACKET(1);
	}else {
		size_t elements = v_element.size();

		if (elements * (option & 0x100 ? sizeof(PlayerRoomInfo) : sizeof(PlayerRoomInfoEx)) < (MAX_BUFFER_PACKET - 100)) {
			p.init_plain((unsigned short)0x48);
			p.addUint8((unsigned char)option);
			p.addInt16(-1);

			if ((option & 0xFF) == 0 || (option & 0xFF) == 5)
				p.addInt8((unsigned char)elements);
			else if ((option & 0xFF) == 3)
				p.addInt32(_session->m_oid);

			for (auto i = 0u; i < v_element.size(); i++)
				p.addBuffer(&v_element[i], (option & 0x100 ? sizeof(PlayerRoomInfo) : sizeof(PlayerRoomInfoEx)));

			p.addUint8(0);	// Final list de PlayerRoomInfo

			return 1;
		}else {
			MAKE_BEGIN_SPLIT_PACKET(0x48, _session, (option & 0x100 ? sizeof(PlayerRoomInfo) : sizeof(PlayerRoomInfoEx)), MAX_BUFFER_PACKET);

			if ((option & 0xFF) == 0 && index != 0u)
				p.addUint8(5u);	// Option 5 é para add os players aos que já tem na sala
			else
				p.addUint8((unsigned char)option);

			p.addInt16(-1);

			if ((option & 0xFF) == 0 || (option & 0xFF) == 5)
				p.addInt8((unsigned char)((total > por_packet) ? por_packet : total));
			else if ((option & 0xFF) == 3)
				p.addInt32(_session->m_oid);

			MAKE_MID_SPLIT_PACKET_VECTOR((option & 0x100 ? sizeof(PlayerRoomInfo) : sizeof(PlayerRoomInfoEx)));

			p.addUint8(0);	// Final list de PlayerRoomInfo

			MAKE_END_SPLIT_PACKET(1);
		}
	}

	return 0;
};

int packet_func::pacote049(packet& p, room* _room, int option) {

	if (_room == nullptr)
		throw exception("Error _room is nullptr. EM packet_func::pacote049()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 3, 0));

	p.init_plain((unsigned short)0x49);

	p.addInt16((short)option);

	p.addBuffer((RoomInfo*)_room->getInfo(), sizeof(RoomInfo));

	return 0;
};

int packet_func::pacote04A(packet& p, RoomInfoEx& _ri, int option) {
	
	p.init_plain((unsigned short)0x4A);

	p.addInt16((short)option);		// pode ser valor constante da sala ou o número, ainda não descobri, sempre passa -1 des vezes que vi

	// Tem que ser o tipo_show, por que ele é o que o cliente quer,
	// o tipo(real) só server conhece para poder fazer o jogo direito 
	p.addUint8(_ri.tipo_show);
	p.addUint8(_ri.course);
	p.addUint8(_ri.qntd_hole);
	p.addUint8(_ri.modo);

	if (_ri.modo == RoomInfo::M_REPEAT) {
		p.addUint8(_ri.hole_repeat);
		p.addUint32(_ri.fixed_hole);
	}

	p.addUint32(_ri.natural.ulNaturalAndShortGame);
	p.addUint8(_ri.max_player);
	p.addInt8(_ri._30s);		// constante 30 de pangya
	p.addUint8((unsigned char)_ri.state_flag);
	p.addUint32(_ri.time_vs);
	p.addUint32(_ri.time_30s);
	p.addUint32(_ri.trofel);

	p.addUint8(_ri.senha_flag);	// Senha Flag

	p.addString(_ri.nome);

	return 0;
};

int packet_func::pacote04B(packet& p, player *_session, unsigned char _type, int error, int _valor) {
	
	if (_session == nullptr)
		throw exception("Error _session is nullptr. Em packet_func::pacote04B()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));
	
	if (!_session->getState())
		throw exception("Error player nao esta mais connectado. Em packet_func::pacote04B()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 2, 0));

	p.init_plain((unsigned short)0x4B);

	p.addInt32(error);

	if (error == 0) {
		p.addUint8(_type);

		p.addUint32(_session->m_oid);

		switch (_type) {
		case 1: // Caddie
			if (_session->m_pi.ei.cad_info != nullptr)
				p.addBuffer(_session->m_pi.ei.cad_info->getInfo(), sizeof(CaddieInfo));
			else
				p.addZeroByte(sizeof(CaddieInfo));
			break;
		case 2:	// Ball(Comet)
			if (_session->m_pi.ei.comet != nullptr)
				p.addUint32(_session->m_pi.ei.comet->_typeid);
			else
				p.addZeroByte(sizeof(uint32_t));
			break;
		case 3: // ClubSet
			p.addBuffer(&_session->m_pi.ei.csi, sizeof(ClubSetInfo));
			break;
		case 4: // Character
			if (_session->m_pi.ei.char_info != nullptr)
				p.addBuffer(_session->m_pi.ei.char_info, sizeof(CharacterInfo));
			else
				p.addZeroByte(sizeof(CharacterInfo));
			break;
		case 5:	// Mascot
			if (_session->m_pi.ei.mascot_info != nullptr)
				p.addBuffer((MascotInfo*)_session->m_pi.ei.mascot_info, sizeof(MascotInfo));
			else
				p.addZeroByte(sizeof(MascotInfo));
			break;
		case 6:	// Itens Active 1 = Jester big cabeça, 2 = Hermes velocidade x2, 3 = Twilight Fogos na cabeça
		{
			p.addInt32(_valor);

			if (_valor == ChangePlayerItemRoom::stItemEffectLounge::TYPE_EFFECT::TE_TWILIGHT/*Twilight*/)
				p.addInt32(1);	// Ativa Fogos
			else {

				if (_session->m_pi.ei.char_info != nullptr) {
					auto it = (_session->m_pi.ei.char_info == nullptr) ? _session->m_pi.mp_scl.end() : _session->m_pi.mp_scl.find(_session->m_pi.ei.char_info->id);

					if (it == _session->m_pi.mp_scl.end()) {

						_smp::message_pool::getInstance().push(new message("[channel::pacote04B][Error] player[UID=" + std::to_string(_session->m_pi.uid)
								+ "] nao tem os estados do character na lounge. Criando um novo para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						// Add New State Character Lounge
						auto pair = _session->m_pi.mp_scl.insert(std::make_pair(_session->m_pi.ei.char_info->id, StateCharacterLounge{}));

						it = pair.first;
					}

					switch (_valor) {
					case ChangePlayerItemRoom::stItemEffectLounge::TYPE_EFFECT::TE_BIG_HEAD:	// Jester (Big head)
						p.addFloat(it->second.scale_head);
						break;
					case ChangePlayerItemRoom::stItemEffectLounge::TYPE_EFFECT::TE_FAST_WALK:	// Hermes (Velocidade x2)
						p.addFloat(it->second.walk_speed);
						break;
					}
				}
			}

			break;
		}
		case 7:	// Player game
			// Nada Aqui
			break;
		default:
			throw exception("Error tipo desconhecido. Em packet_func::pacote04B()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 3, 0));
		}
	}

	return 0;
};

int packet_func::pacote04C(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0x4C);

	p.addInt16((short)option);

	return 0;
};

int packet_func::pacote04D(packet& p, player *_session, std::vector< channel* >& v_element, int option) {
	
	p.init_plain((unsigned short)0x4D);

	p.addInt8((unsigned char)v_element.size());

	for (size_t i = 0; i < v_element.size(); ++i)
		p.addBuffer(v_element[i]->getInfo(), sizeof(ChannelInfo));

	return 0;
};

int packet_func::pacote04E(packet& p, player *_session, int option, int _codeErrorInfo) {
	
	/* Option Values
	* 1 Sucesso
	* 2 Channel Full
	* 3 Nao encontrou canal
	* 4 Nao conseguiu pegar informções do canal
	* 6 ErrorCode Info
	*/

	p.init_plain((unsigned short)0x4E);

	p.addInt8((unsigned char)option);

	if (_codeErrorInfo != 0)
		p.addInt32(_codeErrorInfo);

	return 0;
};

int packet_func::pacote06B(packet& p, player *_session, PlayerInfo *pi, unsigned char type, unsigned char err_code) {
	
	if (pi == nullptr)
		throw exception("Erro PlayerInfo *pi is nullptr. packet_func::pacote06B()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x6B);

	p.addInt8(err_code);		// Error Code, 4 Sucesso, diferente é erro
	p.addInt8(type);

	if (err_code == 4) {
		switch (type) {
		case 0:	// Character Equipado Com os Parts Equipado
			if (_session->m_pi.ei.char_info != nullptr)
				p.addBuffer(_session->m_pi.ei.char_info, sizeof(CharacterInfo));
			else
				p.addZeroByte(sizeof(CharacterInfo));
			break;
		case 1:	// Caddie Equipado
			if (_session->m_pi.ei.cad_info != nullptr)
				p.addUint32(_session->m_pi.ei.cad_info->id);
			else
				p.addZeroByte(sizeof(uint32_t));
			break;
		case 2: // Itens Equipáveis
			p.addBuffer(pi->ue.item_slot, sizeof(pi->ue.item_slot));
			break;
		case 3:	// Ball e Clubset Equipado
			if (_session->m_pi.ei.comet != nullptr) // Ball
				p.addUint32(_session->m_pi.ei.comet->_typeid);
			else
				p.addZeroByte(sizeof(uint32_t));
			p.addUint32(_session->m_pi.ei.csi.id);	// ClubSet ID
			break;
		case 4:	// Skins
			p.addBuffer(&_session->m_pi.ue.skin_typeid, sizeof(_session->m_pi.ue.skin_typeid));
			break;
		case 5:	// Only Chracter Equipado
			if (_session->m_pi.ei.char_info != nullptr)
				p.addUint32(_session->m_pi.ei.char_info->id);
			else
				p.addZeroByte(sizeof(uint32_t));
			break;
		case 8:	// Mascot Equipado
			if (_session->m_pi.ei.mascot_info != nullptr)
				p.addBuffer((MascotInfo*)_session->m_pi.ei.mascot_info, sizeof(MascotInfo));
			else
				p.addZeroByte(sizeof(MascotInfo));
			break;
		case 9:	// Character Cutin Equipado
			if (_session->m_pi.ei.char_info != nullptr) {
				p.addUint32(_session->m_pi.ei.char_info->id);
				p.addBuffer(_session->m_pi.ei.char_info->cut_in, sizeof(_session->m_pi.ei.char_info->cut_in));
			}else
				p.addZeroByte(20);
			break;
		case 10:	// Poster Equipado
			p.addBuffer(&_session->m_pi.ue.poster, sizeof(_session->m_pi.ue.poster));
			break;
		}
	}

	return 0;
};

int packet_func::pacote070(packet& p, player *_session, std::multimap< int32_t/*ID*/, CharacterInfo >& v_element, int option) {

	size_t elements = v_element.size();

	if (elements * sizeof(CharacterInfo) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x70);
		p.addInt16((short)elements);
		p.addInt16((short)elements);

		for (auto i = v_element.begin(); i != v_element.end(); i++)
			p.addBuffer(&i->second, sizeof(CharacterInfo));

		return 1;
	}else {
		MAKE_BEGIN_SPLIT_PACKET(0x70, _session, sizeof(CharacterInfo), MAX_BUFFER_PACKET)

		MAKE_MED_SPLIT_PACKET(0);

		MAKE_MID_SPLIT_PACKET_MAP(sizeof(CharacterInfo));

		MAKE_END_SPLIT_PACKET(1);
	}

	return 0;
};

int packet_func::pacote071(packet& p, player *_session, std::multimap< int32_t/*ID*/, CaddieInfoEx >& v_element, int option) {
	
	size_t elements = v_element.size();

	if (elements * sizeof(CaddieInfo) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x71);
		p.addInt16((short)elements);
		p.addInt16((short)elements);

		for (auto i = v_element.begin(); i != v_element.end(); i++)
			p.addBuffer(i->second.getInfo(), sizeof(CaddieInfo));

		return 1;
	}else {
		MAKE_BEGIN_SPLIT_PACKET(0x71, _session, sizeof(CaddieInfo), MAX_BUFFER_PACKET);

		MAKE_MED_SPLIT_PACKET(0);

		for (i = 0; i < por_packet && index < elements && _it != v_element.end(); i++, index++, ++_it)
			p.addBuffer(_it->second.getInfo(), sizeof(CaddieInfo));

		MAKE_END_SPLIT_PACKET(1);
	}

	return 0;
};

int packet_func::pacote072(packet& p, player *_session, UserEquip ue, int option) {
	
	p.init_plain((unsigned short)0x72);

	p.addBuffer(&ue, sizeof(UserEquip));

	return 0;
};

int packet_func::pacote073(packet& p, player *_session, std::multimap< int32_t/*ID*/, WarehouseItemEx >& v_element, int option) {

	size_t elements = v_element.size();

	if (elements * sizeof(WarehouseItem) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x73);
		p.addInt16((short)elements);
		p.addInt16((short)elements);

		for (auto i = v_element.begin(); i != v_element.end(); ++i)
			p.addBuffer(&i->second, sizeof(WarehouseItem));

		return 1;
	}else {
		MAKE_BEGIN_SPLIT_PACKET(0x73, _session, sizeof(WarehouseItem), MAX_BUFFER_PACKET);

		MAKE_MED_SPLIT_PACKET(0);

		MAKE_MID_SPLIT_PACKET_MAP(sizeof(WarehouseItem));

		MAKE_END_SPLIT_PACKET(0);	// not display on debug, por que quando tem muitos itens aumenta o tamanho do arquivo de log
	}

	return 0;
};

int packet_func::pacote089(packet& p, player *_session, uint32_t _uid, unsigned char season, uint32_t err_code) {
	
	p.init_plain((unsigned short)0x89);

	p.addInt32(err_code);
	p.addInt8(season);
	p.addInt32(_uid);

	return 0;
};

int packet_func::pacote095(packet& p, player *_session, unsigned short sub_tipo, int option, PlayerInfo *pi) {
	
	p.init_plain((unsigned short)0x95);

	p.addInt16(sub_tipo);

	if (sub_tipo == 0x102)
		p.addInt8((unsigned char)option);
	else if (sub_tipo == 0x111) {
		p.addInt32(option);

		if (pi == nullptr) {
			//delete p;

			throw exception("Erro PlayerInfo *pi is nullptr. packet_func::pacote095()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));
		}

		p.addInt64(pi->ui.pang);
	}

	return 0;
};

int packet_func::pacote096(packet& p, player *_session, PlayerInfo *pi) {
	if (pi == nullptr)
		throw exception("Erro PlayerInfo *pi is nullptr. packet_func::pacote096()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x96);

	p.addQWord(&pi->cookie);

	return 0;
};

int packet_func::pacote09A(packet& p, player *_session, PlayerInfo *pi) {
	if (pi == nullptr)
		throw exception("Erro PlayerInfo *pi is nullptr, packet_func::pacote09A()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x9A);

	p.addUint32(pi->m_cap.ulCapability);

	return 0;
};

int packet_func::pacote09F(packet& p, player * _session, std::vector< ServerInfo >& v_server, std::vector< channel* >& v_channel) {
	
	p.init_plain((unsigned short)0x9F);

	size_t i = 0u;

	p.addInt8((unsigned char)v_server.size());

	for (i = 0u; i < v_server.size(); ++i)
		p.addBuffer(&v_server[i], sizeof(ServerInfo));

	p.addInt8((unsigned char)v_channel.size());

	for (i = 0u; i < v_channel.size(); ++i)
		p.addBuffer(v_channel[i]->getInfo(), sizeof(ChannelInfo));

	return 0;
};

int packet_func::pacote0AA(packet& p, player *_session, std::vector< stItem >& v_item) {

	if (_session == nullptr || !_session->getState())
		throw exception("Error player nao esta conectado. Em packet_func::pacote0AA()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 50, 0));

	if (v_item.size() > 0) {
		p.init_plain((unsigned short)0xAA);

		p.addUint16((unsigned short)v_item.size());	// Count, ele só manda de 1 msm não manda todos, não sei por que

		for (auto i = 0u; i < v_item.size(); ++i) {

			p.addUint32(v_item[i]._typeid);
			p.addUint32(v_item[i].id);
			p.addUint16(v_item[i].STDA_C_ITEM_TIME);
			p.addUint8(v_item[i].flag_time);
			p.addUint16((unsigned short)v_item[i].stat.qntd_dep);
			p.addBuffer(&v_item[i].date.date.sysDate[1], sizeof(SYSTEMTIME));
			p.addBuffer(&v_item[i].ucc.IDX, sizeof(v_item[i].ucc.IDX));

			// Aqui é a reflexão desse pacote, usa no ticket report
			if (v_item[i]._typeid == 0x1A000042) {
				p.addUint16(v_item[i].STDA_C_ITEM_TICKET_REPORT_ID_HIGH);
				p.addUint16(v_item[i].STDA_C_ITEM_TICKET_REPORT_ID_LOW);

				p.addBuffer(&v_item[i].date.date.sysDate[1], sizeof(SYSTEMTIME));
			}
		}

		p.addUint64(_session->m_pi.ui.pang);
		p.addUint64(_session->m_pi.cookie);
	}

	return 0;
};

int packet_func::pacote0B2(packet& p, player *_session, std::vector< MsgOffInfo >& v_element, int option) {
	
	p.init_plain((unsigned short)0xB2);

	p.addInt32(2);		// Não sei bem o que é, mas pode ser uma opção

	p.addInt32(option);

	p.addInt32((uint32_t)v_element.size());

	for (std::vector< MsgOffInfo >::iterator i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(MsgOffInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(MsgOffInfo));
#endif

	return 0;
};

int packet_func::pacote0B4(packet& p, player *_session, std::vector< TrofelEspecialInfo >& v_element, int option) {
	
	p.init_plain((unsigned short)0xB4);

	p.addInt16((short)option);

	p.addInt8((unsigned char)v_element.size());

	for (std::vector< TrofelEspecialInfo >::iterator i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(TrofelEspecialInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(TrofelEspecialInfo));
#endif

	return 0;
};

int packet_func::pacote0D4(packet& p, player *_session, std::multimap< int32_t/*ID*/, CaddieInfoEx >& v_element) {
	
	p.init_plain((unsigned short)0xD4);

	p.addInt32((uint32_t)v_element.size());

	for (auto i = v_element.begin(); i != v_element.end(); i++)
		p.addBuffer(i->second.getInfo(), sizeof(CaddieInfo));

	return 0;
};

int packet_func::pacote0E1(packet& p, player *_session, std::multimap< int32_t/*ID*/, MascotInfoEx >& v_element, int option) {
	
	p.init_plain((unsigned short)0xE1);

	p.addInt8(v_element.size() & 0xFF);

	for (auto i = v_element.begin(); i != v_element.end(); i++)
		p.addBuffer(&i->second, sizeof(MascotInfo));

	return 0;
};

int packet_func::pacote0F1(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0xF1);

	p.addInt8((unsigned char)option);

	return 0;
};

int packet_func::pacote0F5(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0xF5);

	return 0;
};

int packet_func::pacote0F6(packet& p, player *_session) {
	p.init_plain((unsigned short)0xF6);

	return 0;
};

int packet_func::pacote0FC(packet& p, player *_session, std::vector< ServerInfo >& v_si) {
	
	p.init_plain((unsigned short)0xFC);

	p.addInt8((unsigned char)v_si.size());

	for (std::vector< ServerInfo >::iterator i = v_si.begin(); i != v_si.end(); ++i)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(ServerInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(ServerInfo));
#endif

	return 0;
};

int packet_func::pacote101(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0x101);

	p.addInt8((unsigned char)option);

	return 0;
};

int packet_func::pacote102(packet& p, player *_session, PlayerInfo *pi) {
	
	if (pi == nullptr)
		throw exception("[packet_func::pacote12][Error] PlayerInfo *pi is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x102);

	p.addInt32(pi->cg.normal_ticket);
	p.addInt32(pi->cg.partial_ticket);

	p.addInt64(pi->ui.pang);
	p.addInt64(pi->cookie);

	return 0;
};

int packet_func::pacote10E(packet& p, player *_session, Last5PlayersGame& l5pg) {
	
	p.init_plain((unsigned short)0x10E);

	p.addBuffer(&l5pg, sizeof(Last5PlayersGame));

	return 0;
};

int packet_func::pacote11F(packet& p, player *_session, PlayerInfo *pi, short tipo) {
	if (pi == nullptr)
		throw exception("Erro PlayerInfo *pi is nullptr. packet_func::pacote11F()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	p.init_plain((unsigned short)0x11F);

	p.addInt16(tipo);

	p.addBuffer(&pi->TutoInfo, sizeof(TutorialInfo));

	return 0;
};

int packet_func::pacote12E(packet& p, player *_session, WarehouseItemEx *wi, int state, int option) {
	if (wi == nullptr)
		throw exception("Erro WarehouseItemEx *wi is nullptr, packet_func::pacote12E()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 2, 0));

	p.init_plain((unsigned short)0x12E);

	p.addInt8((unsigned char)option);
	p.addInt32(wi->_typeid);
	p.addString(wi->ucc.idx);
	p.addInt8((unsigned char)state);

	p.addBuffer(wi, sizeof(WarehouseItem));

	return 0;
};

int packet_func::pacote131(packet& p, int option) {
	
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	p.init_plain((unsigned short)0x131);

	p.addInt8((unsigned char)option);

	p.addInt8((unsigned char)MS_NUM_MAPS);

	/*p.addBuffer(TreasureHunterSystem::getAllCoursePoint(), sizeof(TreasureHunterInfo) * MS_NUM_MAPS);*/
	p.addBuffer(sTreasureHunterSystem::getInstance().getAllCoursePoint(), sizeof(TreasureHunterInfo) * MS_NUM_MAPS);

	return 0;
};

int packet_func::pacote135(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0x135);

	return 0;
};

int packet_func::pacote136(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0x136);

	return 0;
};

int packet_func::pacote137(packet& p, player *_session, std::vector< CardEquipInfoEx >& v_element) {
	
	p.init_plain((unsigned short)0x137);

	p.addInt16((short)v_element.size());

	for (auto i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(CardEquipInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(CardEquipInfo));
#endif

	return 0;
};

int packet_func::pacote138(packet& p, player *_session, std::vector< CardInfo >& v_element, int option) {
	
	p.init_plain((unsigned short)0x138);

	p.addInt32(option);

	p.addInt16((short)v_element.size());

	for (std::vector< CardInfo >::iterator i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(CardInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(CardInfo));
#endif

	return 0;
};

int packet_func::pacote13F(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0x13F);

	p.addInt8((unsigned char)option);

	return 0;
};

int packet_func::pacote144(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0x144);

	p.addInt8((unsigned char)option);

	return 0;
};

int packet_func::pacote156(packet& p, player *_session, uint32_t _uid, UserEquip& _ue, unsigned char season) {
	
	p.init_plain((unsigned short)0x156);

	p.addInt8(season);
	p.addInt32(_uid);
	p.addBuffer(&_ue, sizeof(UserEquip));

	return 0;
};

int packet_func::pacote157(packet& p, player *_session, MemberInfoEx& _mi, unsigned char season) {
	
	p.init_plain((unsigned short)0x157);

	p.addInt8(season);
	
	p.addInt32(_mi.uid);
	p.addInt16(_mi.sala_numero);

	p.addBuffer(&_mi, sizeof(MemberInfo));
	
	p.addInt32(_mi.uid);
	p.addInt32(_mi.guild_point);

	return 0;
};

int packet_func::pacote158(packet& p, player *_session, uint32_t _uid, UserInfo& _ui, unsigned char season) {
	
	p.init_plain((unsigned short)0x158);

	p.addInt8(season);

	p.addInt32(_uid);

	p.addBuffer(&_ui, sizeof(UserInfo));

	return 0;
};

int packet_func::pacote159(packet& p, player *_session, uint32_t _uid, TrofelInfo& ti, unsigned char season) {
	
	p.init_plain((unsigned short)0x159);

	p.addInt8(season);
	p.addInt32(_uid);

	p.addBuffer(&ti, sizeof(TrofelInfo));

	return 0;
};

int packet_func::pacote15A(packet& p, player *_session, uint32_t _uid, std::vector< TrofelEspecialInfo >& v_tei, unsigned char season) {
	
	p.init_plain((unsigned short)0x15A);

	p.addInt8(season);
	p.addInt32(_uid);
	
	p.addInt16((unsigned short)v_tei.size());

	for (std::vector< TrofelEspecialInfo >::iterator i = v_tei.begin(); i != v_tei.end(); ++i)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(TrofelEspecialInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(TrofelEspecialInfo));
#endif

	return 0;
};

int packet_func::pacote15B(packet& p, player *_session, uint32_t _uid, unsigned char season) {
	
	p.init_plain((unsigned short)0x15B);

	p.addInt8(season);
	p.addInt32(_uid);
	p.addInt16(0);	// Aqui é o count de algum info do player que eu ainda não sei

	return 0;
};

int packet_func::pacote15C(packet& p, player *_session, uint32_t _uid, std::vector< MapStatistics >& v_ms, std::vector< MapStatistics >& v_msa, unsigned char season) {
	
	p.init_plain((unsigned short)0x15C);

	std::vector< MapStatistics >::iterator i;

	p.addInt8(season);
	p.addInt32(_uid);

	p.addInt32((uint32_t)v_ms.size());	// Count Normal

	for (i = v_ms.begin(); i != v_ms.end(); ++i)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(MapStatistics));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(MapStatistics));
#endif

	p.addInt32((uint32_t)v_msa.size());	// Count Assist

	for (i = v_msa.begin(); i != v_msa.end(); ++i)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(MapStatistics));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(MapStatistics));
#endif

	return 0;
};

int packet_func::pacote15D(packet& p, player *_session, uint32_t _uid, GuildInfo& _gi) {
	
	p.init_plain((unsigned short)0x15D);

	p.addInt32(_uid);
	p.addBuffer(&_gi, sizeof(GuildInfo));

	return 0;
};

int packet_func::pacote15E(packet& p, player *_session, uint32_t _uid, CharacterInfo& _ci) {
	
	p.init_plain((unsigned short)0x15E);

	p.addInt32(_uid);

	p.addBuffer(&_ci, sizeof(CharacterInfo));

	return 0;
};

int packet_func::pacote169(packet& p, player *_session, TrofelInfo& ti, int option) {
	
	p.init_plain((unsigned short)0x169);

	p.addInt8((unsigned char)option);

	p.addBuffer(&ti, sizeof(TrofelInfo));

	return 0;
};

int packet_func::pacote181(packet& p, player *_session, std::vector< ItemBuffEx >& v_element, int option) {
	
	p.init_plain((unsigned short)0x181);

	p.addInt32(option);

	if (option == 0) {
		p.addInt8((unsigned char)v_element.size());

		for (auto i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
			p.addBuffer(i._Ptr, sizeof(ItemBuff));
#elif defined(__linux__)
			p.addBuffer(&(*i), sizeof(ItemBuff));
#endif

	}else if (option == 2) {
		p.addInt32((uint32_t)v_element.size());

		for (auto i = v_element.begin(); i != v_element.end(); i++) {
			p.addInt32(i->_typeid);

#if defined(_WIN32)
			p.addBuffer(i._Ptr, sizeof(ItemBuff));
#elif defined(__linux__)
			p.addBuffer(&(*i), sizeof(ItemBuff));
#endif
		}
	}else
		p.addInt8(0);

	return 0;
};

int packet_func::pacote1A9(packet& p, player *_session, int32_t ttl_milliseconds/*time to live*/, int option) {
	
	p.init_plain((unsigned short)0x1A9);

	p.addInt8((unsigned char)option);

	p.addInt32(ttl_milliseconds);

	return 0;
};

int packet_func::pacote1AD(packet& p, player *_session, std::string webKey, int option) {
	
	p.init_plain((unsigned short)0x1AD);

	p.addInt32(option);

	if (webKey.empty())
		p.addInt16(0);
	else
		p.addString(webKey);

	return 0;
};

int packet_func::pacote1B1(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0x1B1);

	p.addInt32(0x0132DC55);
	p.addInt8(0x19);
	p.addZeroByte(6);
	p.addInt16(0x2211);
	p.addZeroByte(17);
	p.addInt8(0x11);
	p.addInt16(0);

	return 0;
};

int packet_func::pacote1D4(packet& p, player *_session, std::string _AuthKeyLogin, int option) {
	
	p.init_plain((unsigned short)0x1D4);

	p.addInt32(option);

	if (option == 0 && !_AuthKeyLogin.empty())
		p.addString(_AuthKeyLogin);

	return 0;
};

int packet_func::pacote210(packet& p, player *_session, std::vector< MailBox >& v_element, int option) {
	
	p.init_plain((unsigned short)0x210);

	p.addInt32(option);

	p.addInt32((uint32_t)v_element.size());

	for (auto i = 0u; i < v_element.size(); ++i)
		p.addBuffer(&v_element[i], sizeof(MailBox));

	return 0;
};

int packet_func::pacote211(packet& p, player *_session, std::vector< MailBox > v_element, int32_t pagina, int32_t paginas, int error) {
	
	p.init_plain((unsigned short)0x211);

	p.addInt32(error);

	if (error == 0) {
		p.addInt32(pagina);
		p.addInt32(paginas);
		p.addInt32((uint32_t)v_element.size());

		for (auto i = 0u; i < v_element.size(); ++i)
			p.addBuffer(&v_element[i], sizeof(MailBox));
	}

	return 0;
};

int packet_func::pacote212(packet& p, player *_session, EmailInfo& ei, int error) {
	
	p.init_plain((unsigned short)0x212);

	p.addInt32(error);

	if (error == 0) {
		p.addInt32(ei.id);
		p.addString(ei.from_id);
		p.addString(ei.gift_date);
		p.addString(ei.msg);
		p.addInt8(ei.lida_yn); // Flag que mostra o item, 1 mostra, 0 não mostra

		p.addInt32((uint32_t)ei.itens.size());

		for (auto i = 0u; i < ei.itens.size(); ++i)
			p.addBuffer(&ei.itens[i], sizeof(EmailInfo::item));
	}

	return 0;
};

int packet_func::pacote214(packet& p, player *_session, int error) {
	
	p.init_plain((unsigned short)0x214);

	p.addInt32(error);

	return 0;
};

int packet_func::pacote215(packet& p, player *_session, std::vector< MailBox > v_element, int32_t pagina, int32_t paginas, int error) {
	
	p.init_plain((unsigned short)0x215);

	p.addInt32(error);

	if (error == 0) {
		p.addInt32(pagina);
		p.addInt32(paginas);
		p.addInt32((uint32_t)v_element.size());

		for (auto i = 0u; i < v_element.size(); ++i)
			p.addBuffer(&v_element[i], sizeof(MailBox));
	}

	return 0;
};

int packet_func::pacote216(packet& p, player *_session, std::vector< stItem >& v_item, int option) {
	
	p.init_plain((unsigned short)0x216);

	p.addInt32((const int)GetSystemTimeAsUnix()/*std::time(nullptr)*/);

	if (v_item.size() > 0) {
		p.addInt32((uint32_t)v_item.size());

		for (auto i = v_item.begin(); i != v_item.end(); ++i) {
			
			// Begin Base Item
			p.addUint8(i->type);
			p.addUint32(i->_typeid);
			p.addUint32(i->id);
			p.addUint32(i->flag_time);
			p.addBuffer(&i->stat, sizeof(stItem::item_stat));
			p.addUint32((i->flag_time == 0) ? i->STDA_C_ITEM_QNTD : i->STDA_C_ITEM_TIME);
			p.addBuffer(i->c, sizeof(i->c));
			// End Base Item

			if (i->type == 2) {
				try {
					p.addString(i->ucc.IDX);
				}catch (exception& e) {
					if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PACKET && STDA_ERROR_DECODE(e.getCodeError()) == 3/*String Vazia*/)
						p.addInt16(0);
					else
						throw;
				}

				p.addInt32(i->ucc.status);
				p.addInt32(i->ucc.seq);
				//p.addBuffer(i->unknown2, sizeof(i->unknown2));
				p.addZeroByte(5);	// É o Unknown de cima
			}
		}
	}else
		p.addInt32(option);

	return 0;
};

int packet_func::pacote21D(packet& p, player *_session, std::vector< CounterItemInfo >& v_element, int option) {
	
	size_t elements = v_element.size();

	if (elements * sizeof(CounterItemInfo) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x21D);
		p.addInt32(option);

		p.addInt32((uint32_t)elements);
		p.addInt32((uint32_t)elements);

		for (std::vector< CounterItemInfo >::iterator i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
			p.addBuffer(i._Ptr, sizeof(CounterItemInfo));
#elif defined(__linux__)
			p.addBuffer(&(*i), sizeof(CounterItemInfo));
#endif

		return 1;
	}else {
		MAKE_BEGIN_SPLIT_PACKET(0x21D, _session, sizeof(CounterItemInfo), MAX_BUFFER_PACKET);

		p.addInt32(option);

		MAKE_MED_SPLIT_PACKET(1);

		MAKE_MID_SPLIT_PACKET_VECTOR(sizeof(CounterItemInfo));

		MAKE_END_SPLIT_PACKET(1);
	}

	return 0;
};

int packet_func::pacote21E(packet& p, player *_session, std::multimap< uint32_t, AchievementInfoEx >& v_element, int option) {
	
	CounterItemInfo *cii = nullptr;

	size_t elements = v_element.size();

	if (elements * ((sizeof(AchievementInfo) + 4) + (sizeof(QuestStuffInfo) * 9)) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x21E);
		p.addInt32(option);

		p.addInt32((uint32_t)elements);
		p.addInt32((uint32_t)elements);

		for (auto i = v_element.begin(); i != v_element.end(); i++) {
			p.addInt8(i->second.active);
			p.addInt32(i->second._typeid);
			p.addInt32(i->second.id);
			p.addInt32(i->second.status);
			p.addInt32((uint32_t)i->second.v_qsi.size());

			for (auto ii = i->second.v_qsi.begin(); ii != i->second.v_qsi.end(); ii++) {
				p.addInt32(ii->_typeid);

				if (ii->counter_item_id > 0 && (cii = i->second.findCounterItemById(ii->counter_item_id)) != nullptr) {
					p.addInt32(cii->_typeid);
					p.addInt32(cii->id);
				}else // não tem o counter id e nem o typeid
					p.addZeroByte(8);

				p.addInt32(ii->clear_date_unix);
			}
		}

		return 1;
	}else {
		auto map_i = v_element.begin();

		size_t por_packet = ((MAX_BUFFER_PACKET - 100) > (sizeof(AchievementInfo) + 4 + (sizeof(QuestStuffInfo) * 9))) ? (MAX_BUFFER_PACKET - 100) / ((sizeof(AchievementInfo) + 4) + (sizeof(QuestStuffInfo) * 10)) : 1;
		size_t i = 0, index = 0, total = elements;
		
		for (; map_i != v_element.end(); total -= por_packet) {
			p.init_plain((unsigned short)0x21E);

			p.addInt32(option);

			p.addInt32((uint32_t)total);
			p.addInt32((uint32_t)((total > por_packet) ? por_packet : total));

			for (i = 0; i < por_packet && map_i != v_element.end(); i++, map_i++) {
				p.addInt8(map_i->second.active);
				p.addInt32(map_i->second._typeid);
				p.addInt32(map_i->second.id);
				p.addInt32(map_i->second.status);
				p.addInt32((uint32_t)map_i->second.v_qsi.size());

				for (auto ii = map_i->second.v_qsi.begin(); ii != map_i->second.v_qsi.end(); ii++) {
					p.addInt32(ii->_typeid);

					if (ii->counter_item_id > 0 && (cii = map_i->second.findCounterItemById(ii->counter_item_id)) != nullptr) {
						p.addInt32(cii->_typeid);
						p.addInt32(cii->id);
					}else // não tem o counter id e nem o typeid
						p.addZeroByte(8);

					p.addInt32(ii->clear_date_unix);
				}
			}

			session_send(p, _session, 1);

		}	// FIM MAKE_SPLIT_PACKET
	}

	return 0;
};

int packet_func::pacote225(packet& p, player *_session, DailyQuestInfoUser& _dq, std::vector< RemoveDailyQuestUser > _delete_quest, int option) {
	
	p.init_plain((unsigned short)0x225);

	p.addInt32(option);

	if (option == 0) {
		//p.addInt32(_dq.now_date);		// Antigo, a quest atual fica no "current" e não no "now", deprecated member
		//p.addInt32(_dq.current_date);
		//p.addInt32(_dq.accept_date);
		
		// Convert to UTC send to client
		p.addInt32((int)TzLocalUnixToUnixUTC(_dq.current_date));
		p.addInt32((int)TzLocalUnixToUnixUTC(_dq.accept_date));

		p.addInt32(_dq.count);
		p.addBuffer(_dq._typeid, sizeof(_dq._typeid[0]) * _dq.count);

		p.addInt32((uint32_t)_delete_quest.size());

		for (auto it = _delete_quest.begin(); it != _delete_quest.end(); ++it)
			p.addInt32(it->id);
	}

	return 0;
};

int packet_func::pacote226(packet& p, player *_session, std::vector< AchievementInfoEx >& v_element, int option) {
	
	p.init_plain((unsigned short)0x226);

	p.addInt32(option);

	if (option == 0) {
		if (!v_element.empty()) {
			CounterItemInfo *cii = nullptr;

			p.addInt32((uint32_t)v_element.size());

			for (auto i = v_element.begin(); i != v_element.end(); ++i) {
				p.addInt8(i->active);
				p.addInt32(i->_typeid);
				p.addInt32(i->id);
				p.addInt32(i->status);
				p.addInt32((uint32_t)i->v_qsi.size());

				for (auto ii = i->v_qsi.begin(); ii != i->v_qsi.end(); ii++) {
					p.addInt32(ii->_typeid);

					if (ii->counter_item_id > 0 && (cii = i->findCounterItemById(ii->counter_item_id)) != nullptr) {
						p.addInt32(cii->_typeid);
						p.addInt32(cii->id);
					}else // não tem o counter id e nem o typeid
						p.addZeroByte(8);

					p.addInt32(ii->clear_date_unix);
				}
			}
		}
	}else
		p.addInt32(0);

	return 0;
};


int packet_func::pacote227(packet& p, player *_session, std::vector< AchievementInfoEx >& v_element, int option) {
	
	p.init_plain((unsigned short)0x227);

	p.addInt32(option);

	if (!v_element.empty()) {

		p.addInt32((uint32_t)v_element.size());

		for (auto& el : v_element)
			p.addInt32(el.id);
	}else
		p.addInt32(0);
	
	return 0;
};

int packet_func::pacote228(packet& p, player *_session, std::vector< AchievementInfoEx >& v_element, int option) {
	
	p.init_plain((unsigned short)0x228);

	p.addInt32(option);

	if (option == 0) {
		if (!v_element.empty()) {
			p.addInt32((uint32_t)v_element.size());
	
			for (auto& el : v_element)
				p.addInt32(el.id);
		}
	}

	return 0;
};

int packet_func::pacote22C(packet& p, player *_session, int option) {
	
	p.init_plain((unsigned short)0x22C);

	p.addInt32(option);

	return 0;
};

int packet_func::pacote22D(packet& p, player *_session, std::multimap< uint32_t, AchievementInfoEx >& v_element, int option) {
	
	CounterItemInfo *cii = nullptr;

	size_t elements = v_element.size();

	if (elements * ((sizeof(AchievementInfo) + 4) + (sizeof(QuestStuffInfo) * 9)) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x22D);
		p.addInt32(option);

		p.addInt32((uint32_t)elements);
		p.addInt32((uint32_t)elements);

		for (auto i = v_element.begin(); i != v_element.end(); i++) {
			p.addInt32(i->second._typeid);
			p.addInt32(i->second.id);
			p.addInt32((uint32_t)i->second.v_qsi.size());

			for (auto ii = i->second.v_qsi.begin(); ii != i->second.v_qsi.end(); ii++) {
				p.addInt32(ii->_typeid);

				if (ii->counter_item_id > 0 && (cii = i->second.findCounterItemById(ii->counter_item_id)) != nullptr)
					p.addInt32(cii->value);
				else // não tem o counter id e nem o typeid
					p.addZeroByte(4);

				p.addInt32(ii->clear_date_unix);
			}
		}

		return 1;
	}else {
		auto map_i = v_element.begin();

		size_t por_packet = ((MAX_BUFFER_PACKET - 100) > (sizeof(AchievementInfo) + 4 + (sizeof(QuestStuffInfo) * 10)) ? (MAX_BUFFER_PACKET - 100) / ((sizeof(AchievementInfo) + 4) + (sizeof(QuestStuffInfo) * 9)) : 1);
		size_t i = 0, index = 0, total = elements;

		for (; map_i != v_element.end(); total -= por_packet) {
			p.init_plain((unsigned short)0x22D);

			p.addInt32(option);

			p.addInt32((uint32_t)total);
			p.addInt32((uint32_t)((total > por_packet) ? por_packet : total));

			for (i = 0; i < por_packet && map_i != v_element.end(); i++, map_i++) {
				p.addInt32(map_i->second._typeid);
				p.addInt32(map_i->second.id);
				p.addInt32((uint32_t)map_i->second.v_qsi.size());

				for (auto ii = map_i->second.v_qsi.begin(); ii != map_i->second.v_qsi.end(); ii++) {
					p.addInt32(ii->_typeid);

					if (ii->counter_item_id > 0 && (cii = map_i->second.findCounterItemById(ii->counter_item_id)) != nullptr)
						p.addInt32(cii->value);
					else // não tem o counter id e nem o typeid
						p.addZeroByte(4);

					p.addInt32(ii->clear_date_unix);
				}
			}

			session_send(p, _session, 1);

		}	// FIM MAKE_SPLIT_PACKET
	}

	return 0;
};

int packet_func::pacote248(packet& p, player *_session, AttendanceRewardInfo& ari, int option) {
	
	p.init_plain((unsigned short)0x248);

	p.addInt32(option);

	p.addBuffer(&ari, sizeof(AttendanceRewardInfo));

	return 0;
};

int packet_func::pacote249(packet& p, player *_session, AttendanceRewardInfo& ari, int option) {
	
	p.init_plain((unsigned short)0x249);

	p.addInt32(option);

	p.addBuffer(&ari, sizeof(AttendanceRewardInfo));

	return 0;
};

int packet_func::pacote257(packet& p, player *_session, uint32_t _uid, std::vector< TrofelEspecialInfo >& v_tegi, unsigned char season) {
	
	p.init_plain((unsigned short)0x257);

	p.addInt8(season);
	p.addInt32(_uid);

	p.addInt16((unsigned short)v_tegi.size());

	for (std::vector< TrofelEspecialInfo >::iterator i = v_tegi.begin(); i != v_tegi.end(); ++i)
#if defined(_WIN32)
		p.addBuffer(i._Ptr, sizeof(TrofelEspecialInfo));
#elif defined(__linux__)
		p.addBuffer(&(*i), sizeof(TrofelEspecialInfo));
#endif

	return 0;
};

int packet_func::pacote25D(packet& p, player *_session, std::vector< TrofelEspecialInfo >& v_element, int option) {
	
	size_t elements = v_element.size();

	if (elements * sizeof(TrofelEspecialInfo) < (MAX_BUFFER_PACKET - 100)) {
		p.init_plain((unsigned short)0x25D);
		p.addInt8((unsigned char)option);

		p.addInt32((uint32_t)elements);
		p.addInt32((uint32_t)elements);

		for (std::vector< TrofelEspecialInfo >::iterator i = v_element.begin(); i != v_element.end(); i++)
#if defined(_WIN32)
			p.addBuffer(i._Ptr, sizeof(TrofelEspecialInfo));
#elif defined(__linux__)
			p.addBuffer(&(*i), sizeof(TrofelEspecialInfo));
#endif

		return 1;
	}else {
		MAKE_BEGIN_SPLIT_PACKET(0x25D, _session, sizeof(TrofelEspecialInfo), MAX_BUFFER_PACKET);

		p.addInt8((unsigned char)option);

		MAKE_MED_SPLIT_PACKET(1);

		MAKE_MID_SPLIT_PACKET_VECTOR(sizeof(TrofelEspecialInfo));

		MAKE_END_SPLIT_PACKET(1);
	}

	return 0;
};

int packet_func::pacote26D(packet& p, player *_session, uint32_t _unix_end_date) {
	
	p.init_plain((unsigned short)0x26D);

	p.addInt32(_unix_end_date);

	return 0;
};

int packet_func::principal(packet& p, PlayerInfo *pi, ServerInfoEx& _si) {
	if (pi == nullptr)
		throw exception("Erro PlayerInfo *pi is nullptr. packet_func::principal()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 0));

	size_t st_i = 0;
	SYSTEMTIME si = { 0 };

	// Clinte Versao
	p.addString(_si.version_client);

	// member info
	p.addInt16(pi->mi.sala_numero);
	p.addBuffer(&pi->mi, sizeof(MemberInfo));

	// User Info
	p.addUint32(pi->uid);
	p.addBuffer(&pi->ui, sizeof(UserInfo));

	// Trofel Info
	p.addBuffer(&pi->ti_current_season, sizeof(TrofelInfo));

	// User Equip
	p.addBuffer(&pi->ue, sizeof(UserEquip));

	// Map Statistics Normal
	for (st_i = 0; st_i < MS_NUM_MAPS; st_i++)
		p.addBuffer(&pi->a_ms_normal[st_i], sizeof(MapStatistics));

	// Map Statistics Natural
	for (st_i = 0; st_i < MS_NUM_MAPS; st_i++)
		p.addBuffer(&pi->a_ms_natural[st_i], sizeof(MapStatistics));

	// Map Statistics Grand Prix
	for (st_i = 0; st_i < MS_NUM_MAPS; st_i++)
		p.addBuffer(&pi->a_ms_grand_prix[st_i], sizeof(MapStatistics));

	for (size_t j = 0; j < 9/*season's*/; j++)
		// Map Statistics Normal
		for (st_i = 0; st_i < MS_NUM_MAPS; st_i++)
			p.addBuffer(&pi->aa_ms_normal_todas_season[j][st_i], sizeof(MapStatistics));

	// Character Info(CharEquip)
	if (pi->ei.char_info != nullptr)
		p.addBuffer(pi->ei.char_info, sizeof(CharacterInfo));
	else
		p.addZeroByte(sizeof(CharacterInfo));

	// Caddie Info
	if (pi->ei.cad_info != nullptr)
		p.addBuffer(pi->ei.cad_info->getInfo(), sizeof(CaddieInfo));
	else
		p.addZeroByte(sizeof(CaddieInfo));

	// Club Set Info
	p.addBuffer(&pi->ei.csi, sizeof(ClubSetInfo));

	// Mascot Info
	if (pi->ei.mascot_info != nullptr)
		p.addBuffer((MascotInfo*)pi->ei.mascot_info, sizeof(MascotInfo));
	else
		p.addZeroByte(sizeof(MascotInfo));

	// Date Atual
	translateDateLocal(0, &si);	// Local

	p.addBuffer(&si, sizeof(si));

	// Config do Server
	//p.addInt16(pi->mi.state_flag.ucByte[1]);			// (STATE DO PLAYER COM SEXO, ICON COR E ETC) Não sei bem o que é, no JP nas vezes que loguei ele passa 2
	p.addUint16(pi->mi.flag_login_time);				// Esse é o valor 2 que o JP passa, 1 primeira vez que loga, 2 ele já logou uma ou mais vezes
	p.addBuffer(&pi->mi.papel_shop, sizeof(pi->mi.papel_shop));
	p.addInt32(0); // estava 2 aqui, mas com nova conta é 0 no JP							// Novo no JP, tbm não sei o que é
	p.addUint64(/*_si.flag.ullFlag |*/ pi->block_flag.m_flag.ullFlag);			// Flag do server, de block os sistemas
	p.addUint32(0);						// Aqui é a quantidade de vezes que logou
	p.addInt32(_si.propriedade.ulProperty);	// Property do server

	return 0;
};

// Metôdos de auxílio de criação de pacotes
void packet_func::channel_broadcast(channel& _channel, packet& p, unsigned char _debug) {
	
	std::vector< player* > channel_session = _channel.getSessions();	//gs->getSessionPool().getChannelSessions(s->m_channel);

	for (auto i = 0u; i < channel_session.size(); ++i) {
		MAKE_SEND_BUFFER(p, channel_session[i]);

		SHOW_DEBUG_PACKET(p, _debug);
		
		SHOW_DEBUG_FINAL_PACKET(mb, _debug);
	}
		
	//delete p;
};

void packet_func::channel_broadcast(channel& _channel, std::vector< packet* > v_p, unsigned char _debug) {
	//MAKE_BEGIN_SERVER(_arg);

	std::vector< player* > channel_session;

	for (auto i = 0u; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			channel_session = _channel.getSessions();	//gs->getSessionPool().getChannelSessions(s->m_channel);

			for (auto ii = 0u; ii < channel_session.size(); ++ii) {
				MAKE_SEND_BUFFER(*v_p[i], channel_session[ii]);

				SHOW_DEBUG_PACKET(*v_p[i], _debug);

				SHOW_DEBUG_FINAL_PACKET(mb, _debug);
			}

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::channel_broadcast()", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};

void packet_func::lobby_broadcast(channel& _channel, packet& p, unsigned char _debug) {
	
	std::vector< player* > channel_session = _channel.getSessions();	//gs->getSessionPool().getChannelSessions(s->m_channel);

	for (auto i = 0u; i < channel_session.size(); ++i) {
		if (channel_session[i]->m_pi.mi.sala_numero == -1) {	// Apenas quem está na lobby, sem ser em uma sala
			MAKE_SEND_BUFFER(p, channel_session[i]);

			SHOW_DEBUG_PACKET(p, _debug);

			SHOW_DEBUG_FINAL_PACKET(mb, _debug);
		}
	}

	//delete p;
};

void packet_func::room_broadcast(room& _room, packet& p, unsigned char _debug) {
	
	std::vector< player* > room_session = _room.getSessions(nullptr, false/*without invited*/);

	for (auto i = 0u; i < room_session.size(); ++i) {
		MAKE_SEND_BUFFER(p, room_session[i]);

		SHOW_DEBUG_PACKET(p, _debug);

		SHOW_DEBUG_FINAL_PACKET(mb, _debug);
	}

	//delete p;
};

void packet_func::room_broadcast(room& _room, std::vector< packet* > v_p, unsigned char _debug) {
	//MAKE_BEGIN_SERVER(_arg);

	std::vector< player* > room_session;

	for (auto i = 0u; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			room_session = _room.getSessions(nullptr, false/*without invited*/);

			for (auto ii = 0u; ii < room_session.size(); ++ii) {
				MAKE_SEND_BUFFER(*v_p[i], room_session[ii]);

				SHOW_DEBUG_PACKET(*v_p[i], _debug);

				SHOW_DEBUG_FINAL_PACKET(mb, _debug);
			}

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::room_broadcast()", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};

void packet_func::game_broadcast(Game& _game, packet& p, unsigned char _debug) {

	auto game_session = _game.getSessions();

	for (auto i = 0u; i < game_session.size(); ++i) {
		MAKE_SEND_BUFFER(p, game_session[i]);

		SHOW_DEBUG_PACKET(p, _debug);

		SHOW_DEBUG_FINAL_PACKET(mb, _debug);
	}
};

void packet_func::game_broadcast(Game& _game, std::vector< packet* > v_p, unsigned char _debug) {

	std::vector< player* > game_session;

	for (auto i = 0u; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			game_session = _game.getSessions();

			for (auto ii = 0u; ii < game_session.size(); ++ii) {
				MAKE_SEND_BUFFER(*v_p[i], game_session[ii]);

				SHOW_DEBUG_PACKET(*v_p[i], _debug);

				SHOW_DEBUG_FINAL_PACKET(mb, _debug);
			}

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::room_broadcast()", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};

void packet_func::vector_send(packet& _p, std::vector< session* > _v_s, unsigned char _debug) {

	for (auto& el : _v_s) {
		MAKE_SEND_BUFFER(_p, el);

		SHOW_DEBUG_PACKET(_p, _debug);

		SHOW_DEBUG_FINAL_PACKET(mb, _debug);
	}
};

void packet_func::vector_send(packet& _p, std::vector< player* > _v_s, unsigned char _debug) {

	for (auto& el : _v_s) {
		MAKE_SEND_BUFFER(_p, el);

		SHOW_DEBUG_PACKET(_p, _debug);

		SHOW_DEBUG_FINAL_PACKET(mb, _debug);
	}
};

void packet_func::vector_send(std::vector< packet* > _v_p, std::vector< player* > _v_s, unsigned char _debug) {

	for (auto& el : _v_p) {
		if (el != nullptr) {
			for (auto& el2 : _v_s) {
				MAKE_SEND_BUFFER(*el, el2);

				SHOW_DEBUG_PACKET(*el, _debug);

				SHOW_DEBUG_FINAL_PACKET(mb, _debug);
			}

			delete el;
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::room_broadcast()", CL_FILE_LOG_AND_CONSOLE));
	}

	_v_p.clear();
	_v_p.shrink_to_fit();
};

void packet_func::vector_send(std::vector< packet* > _v_p, std::vector< session* > _v_s, unsigned char _debug) {

	for (auto& el : _v_p) {
		if (el != nullptr) {
			for (auto& el2 : _v_s) {
				MAKE_SEND_BUFFER(*el, el2);

				SHOW_DEBUG_PACKET(*el, _debug);

				SHOW_DEBUG_FINAL_PACKET(mb, _debug);
			}

			delete el;
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::room_broadcast()", CL_FILE_LOG_AND_CONSOLE));
	}

	_v_p.clear();
	_v_p.shrink_to_fit();
};

void packet_func::session_send(packet& p, session *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 2));
	
	MAKE_SEND_BUFFER(p, s);

	SHOW_DEBUG_PACKET(p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);

	//delete p;
};

void packet_func::session_send(std::vector< packet* > v_p, session *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_SV, 1, 2));

	for (auto i = 0u; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			MAKE_SEND_BUFFER(*v_p[i], s);

			SHOW_DEBUG_PACKET(*v_p[i], _debug);

			SHOW_DEBUG_FINAL_PACKET(mb, _debug);

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::session_send()", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};
