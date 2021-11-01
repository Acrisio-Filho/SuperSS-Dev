// Arquivo packet_func_ms.cpp
// Criado em 29/07/2018 as 14:39 por Acrisio
// Implementa��o da classe packet_func para o Message Server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "packet_func_ms.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/TYPE/stdAType.h"

#include "../Message Server/message_server.hpp"

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).makeFull((_session)->m_key); \
											WSABUF mb = (_packet).getMakedBuf(); \
											try { \
												\
												(_session)->usa(); \
												\
												(_session)->requestSendBuffer(mb.buf, mb.len); \
												\
												if ((_session)->devolve()) \
													sms::ms::getInstance().DisconnectSession((_session)); \
												\
											}catch (exception& e) { \
												\
												if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*n�o pode usa session*/)) \
													if ((_session)->devolve()) \
														sms::ms::getInstance().DisconnectSession((_session)); \
												\
												if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2)) \
													throw; \
											} \

// Verifica se session est� autorizada para executar esse a��o, 
// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!pd._session.m_is_authorized) \
												throw exception("[packet_func::" + std::string((method)) + "][Error] Player[UID=" + std::to_string(pd._session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_MS, 1, 0x5000501)); \

using namespace stdA;

int packet_func::packet012(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestLogin(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet012][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet013(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet013][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote013, nao sei o que esse pacote pede ou faz. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("packet013");
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet013][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet014(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestFriendAndGuildMemberList(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet014][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet016(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestUpdatePlayerLogout(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet016][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet017(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestCheckNickname(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet017][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet018(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestAddFriend(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet018][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet019(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestConfirmFriend(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet019][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet01A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestBlockFriend(pd._session, pd._packet);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet01B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestUnblockFriend(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet01C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestDeleteFriend(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet01D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestUpdatePlayerState(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet01E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestChatFriend(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01E][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet01F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestAssingApelido(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet01F][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet023(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestUpdateChannelPlayerInfo(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet023][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet024(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {
	
		ms->requestNotityPlayerWasInvitedToRoom(pd._session, pd._packet);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet024][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet025(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestChatGuild(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet025][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet028(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		ms->requestInvitPlayerToGuildBattleRoom(pd._session, pd._packet);
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet028][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet029(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("packet029");

		uint32_t player_uid_send = pd._packet->readUint32();
		uint32_t player_uid_receive = pd._packet->readUint32();

		_smp::message_pool::getInstance().push(new message("[packet_func::packet029][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid) 
				+ "] message Player[UID=" + std::to_string(player_uid_send) + "] gift item to player[UID=" + std::to_string(player_uid_receive) + "].", CL_FILE_LOG_AND_CONSOLE));
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet029][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet02A(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02A][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02A, player foi aceito na guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02A][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02A, player foi aceito na guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_ONLY_CONSOLE));
#endif // _DEBUG

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("packet02A");
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02A][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet02B(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02B][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02B, player foi quicado da guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02B][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02B, player foi quicado da guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_ONLY_CONSOLE));
#endif // _DEBUG

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("packet02B");
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02B][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet02C(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02C][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02C, troca a imagem da guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02C][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02C, troca a imagem da guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_ONLY_CONSOLE));
#endif // _DEBUG

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("packet02C");
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02C][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet02D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02D][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02D, troca o nome da guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02D][Log] Player[UID=" + std::to_string(pd._session.m_pi.uid)
				+ "] mandou o Pacote02D, troca o nome da guild, nao faco o tratamento desse pacote ainda. Hex: \n\r"
				+ hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_ONLY_CONSOLE));
#endif // _DEBUG

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("packet02D");
	
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[packet_func::packet02D][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet_svFazNada(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	// Esse pacote � para os pacotes que o server envia para o cliente
	// e n�o precisa de tratamento depois que foi enviado para o cliente

	return 0;
}

int packet_func::packet_as001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2);

	try {

		sms::ms::getInstance().requestAcceptGuildMember(_packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as001][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet_as002(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2);

	try {

		sms::ms::getInstance().requestMemberExitedFromGuild(_packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as002][ErrorSystem] " + e.getFullMessageError()));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet_as003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2);

	try {

		sms::ms::getInstance().requestKickGuildMember(_packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as003][ErrorSystem] " + e.getFullMessageError()));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::MESSAGE_SERVER)
			throw;
	}

	return 0;
}

void packet_func::friend_broadcast(std::map< uint32_t, player* > _m_player, packet& _p, session *_s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[packet_func::friend_broadcast][Error] session *_s is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_MS, 1, 2));

	for (auto& el : _m_player) {

		if (el.second != nullptr && el.second != _s) {	// N�o envia para a mesma session, por que j� enviou l� em cima
			MAKE_SEND_BUFFER(_p, el.second);

			SHOW_DEBUG_PACKET(_p, _debug);

			SHOW_DEBUG_FINAL_PACKET(mb, _debug);
		}
	}

	_m_player.clear();
}

void packet_func::friend_broadcast(std::map< uint32_t, player* > _m_player, std::vector< packet* > _v_p, session *_s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[packet_func::friend_broadcast][Error] session *_s is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_MS, 1, 2));

	for (auto& el : _v_p) {

		if (el != nullptr) {

			for (auto& el2 : _m_player) {

				if (el2.second != nullptr && el2.second != _s) {	// N�o envia para a mesma session, por que j� enviou l� em cima
					MAKE_SEND_BUFFER(*el, el2.second);

					SHOW_DEBUG_PACKET(*el, _debug);

					SHOW_DEBUG_FINAL_PACKET(mb, _debug);
				}
			}

			delete el;
		}else
			_smp::message_pool::getInstance().push(new message("[packet_func::friend_broadcast][Error][WARNING] packet *p is nullptr.", CL_FILE_LOG_AND_CONSOLE));
	}

	_v_p.clear();
	_v_p.shrink_to_fit();

	_m_player.clear();
}

void packet_func::session_send(packet& _p, session *_s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[packet_func::session_send][Error] session *_s is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_MS, 1, 2));

	MAKE_SEND_BUFFER(_p, _s);

	SHOW_DEBUG_PACKET(_p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);
}

void packet_func::session_send(std::vector< packet* > _v_p, session *_s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[packet_func::session_send][Error] session *_p is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_MS, 1, 2));

	for (auto& el : _v_p) {
		if (el != nullptr) {
			MAKE_SEND_BUFFER(*el, _s);

			SHOW_DEBUG_PACKET(*el, _s);

			SHOW_DEBUG_FINAL_PACKET(mb, _s);

			delete el;
		}else
			_smp::message_pool::getInstance().push(new message("[packet_func::session_send][Error][WARNING] packet *p is nullptr.", CL_FILE_LOG_AND_CONSOLE));
	}

	_v_p.clear();
	_v_p.shrink_to_fit();
}
