// Arquivo packet_func_gg_as.cpp
// Criado em 02/02/2021 as 17:54 por Acrisio
// Implementação da classe packet_func

#pragma pack(1)
#include <WinSock2.h>

#include "packet_func_gg_as.hpp"
#include "../../Projeto IOCP/PACKET/packet.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/hex_util.h"

#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../GG Auth Server/gg_auth_server.hpp"

using namespace stdA;

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).makeFull((_session)->m_key); \
											 WSABUF mb = (_packet).getMakedBuf(); \
											try { \
												\
												(_session)->usa(); \
												\
												(_session)->requestSendBuffer(mb.buf, mb.len); \
												\
												if ((_session)->devolve()) \
													sgg_as::getInstance().DisconnectSession((_session)); \
												\
											}catch (exception& e) { \
												\
												if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/)) \
													if ((_session)->devolve()) \
														sgg_as::getInstance().DisconnectSession((_session)); \
												\
												if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2)) \
													throw; \
											} \

int packet_func::packet001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sgg_as::getInstance().requestLoginFirstStep(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet001][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GG_AUTH_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet002(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sgg_as::getInstance().requestLoginSecondStep(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet002][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GG_AUTH_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sgg_as::getInstance().requestAddPlayerGameGuardCtx(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet003][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GG_AUTH_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet004(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sgg_as::getInstance().requestDeletePlayerGameGuardCtx(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet004][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GG_AUTH_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet005(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sgg_as::getInstance().requestGetAuthQueryPlayer(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet005][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GG_AUTH_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet006(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2);

	try {

		sgg_as::getInstance().requestCheckAuthAnswerPlayer(pd._session, pd._packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[packet_func::packet006][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GG_AUTH_SERVER)
			throw;
	}

	return 0;
}

int packet_func::packet_svFazNada(void* _arg1, void* _arg2) {
	// Faz nada
	return 0;
}

void packet_func::session_send(packet& p, player *s, unsigned char _debug) {

	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_GG_AS, 1, 2));

	MAKE_SEND_BUFFER(p, s);

	SHOW_DEBUG_PACKET(p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);

	//delete p;
}

void packet_func::session_send(std::vector< packet* > v_p, player *s, unsigned char _debug) {

	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_GG_AS, 1, 2));

	for (auto i = 0ul; i < v_p.size(); ++i) {
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
}

void packet_func::vector_send(packet& p, std::vector< player* > _v_s, unsigned char _debug) {

	if (_v_s.empty())
		throw exception("[packet_func::vector_send][Error] vector session is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_GG_AS, 1, 3));

	for (auto& s : _v_s) {

		if (s == nullptr)
			throw exception("[packet_func::vector_send][Error] session *s is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_GG_AS, 1, 2));

		MAKE_SEND_BUFFER(p, s);

		SHOW_DEBUG_PACKET(p, _debug);

		SHOW_DEBUG_FINAL_PACKET(mb, _debug);
	}
}

void packet_func::vector_send(std::vector< packet* > _v_p, std::vector< player* > _v_s, unsigned char _debug) {

	if (_v_s.empty())
		throw exception("[packet_func::vector_send][Error] vector session is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_GG_AS, 1, 3));

	for (auto& el : _v_p) {

		if (el != nullptr) {

			// Sessions
			for (auto& s : _v_s) {

				if (s == nullptr)
					throw exception("[packet_func::vector_send][Error] session *s is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_GG_AS, 1, 2));

				MAKE_SEND_BUFFER(*el, s);

				SHOW_DEBUG_PACKET(*el, _debug);

				SHOW_DEBUG_FINAL_PACKET(mb, _debug);
			}

			// delete Packet
			delete el;
		}else
			_smp::message_pool::getInstance().push(new message("[packet_func::vector_send][Error] packet *p is nullptr.", CL_FILE_LOG_AND_CONSOLE));
	}

	_v_p.clear();
	_v_p.shrink_to_fit();
}
