// Arquivo packet_func_rs.cpp
// Criado em 15/06/2020 as 15:02 por Acrisio
// Implementa��o da classe packet_func para o Rank Server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "packet_func_rs.hpp"

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../Rank Server/rank_server.hpp"

#include "../../Projeto IOCP/UTIL/hex_util.h"

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).makeFull((_session)->m_key); \
											WSABUF mb = (_packet).getMakedBuf(); \
											try { \
												\
												(_session)->usa(); \
												\
												(_session)->requestSendBuffer(mb.buf, mb.len); \
												\
												if ((_session)->devolve()) \
													srs::rs::getInstance().DisconnectSession((_session)); \
												\
											}catch (exception& e) { \
												\
												if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*n�o pode usa session*/)) \
													if ((_session)->devolve()) \
														srs::rs::getInstance().DisconnectSession((_session)); \
												\
												if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2)) \
													throw; \
											} \

// Verifica se session est� autorizada para executar esse a��o, 
// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�aa o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!pd._session.m_is_authorized) \
												throw exception("[packet_func::" + std::string((method)) + "][Error] Player[UID=" + std::to_string(pd._session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_RS, 1, 0x5000501)); \

using namespace stdA;

// Cliente
int packet_func::packet000(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet000][Log] Packet Hex: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		rs->requestLogin(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet000][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet001][Log] Packet Hex: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		rs->requestPlayerInfo(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet001][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet002(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet002][Log] Packet Hex: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		rs->requestSearchPlayerInRank(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet002][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][Log] Packet Hex: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Rank Server
		//rs->

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet004(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet004][Log] Packet Hex: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Rank Server
		//rs->

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet004][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet005(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[packet_func::packet005][Log] Packet Hex: " + hex_util::BufferToHexString(pd._packet->getBuffer(), pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Rank Server
		//rs->

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet005][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

// Server
int packet_func::packet_svFazNada(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	// Esse pacote � para os pacotes que o server envia para o cliente
	// e n�o precisa de tratamento depois que foi enviado para o cliente

	return 0;
}

// Auth Server
int packet_func::packet_as001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2);

	try {

		// Exemplo se precisar
		//srs::rs::getInstance().DisconnectSession;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet_as001][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::RANK_SERVER)
			throw;
	}

	return 0;
}

// Session
void packet_func::session_send(packet& _p, session* _s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[packet_func::session_send][Error] session *_s is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_RS, 1, 2));

	MAKE_SEND_BUFFER(_p, _s);

	SHOW_DEBUG_PACKET(_p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);
}

void packet_func::session_send(std::vector< packet* > _v_p, session* _s, unsigned char _debug) {

	if (_s == nullptr)
		throw exception("[packet_func::session_send][Error] session *_p is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_RS, 1, 2));

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
