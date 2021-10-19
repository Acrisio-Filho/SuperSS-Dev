// Arquivo threadpl_client.cpp
// Criado em 03/12/2017 por Acrisio
// Implementação da classe threadpl_client

// Tem que ter o pack aqui se não da erro na hora da allocação do HEAP
#if defined(_WIN32)
#pragma pack(1)
#endif

#include <WinSock2.h>
#include "threadpl_client.h"
#include "../UTIL/message_pool.h"
#include "../UTIL/exception.h"
#include "../UTIL/hex_util.h"
#include "../SOCKET/socket.h"

#include "../TYPE/stda_error.h"

#include <ctime>

using namespace stdA;

threadpl_client::threadpl_client(size_t _num_thread_workers_io, size_t _num_thread_workers_logical) 
    : threadpool(_num_thread_workers_io, _num_thread_workers_logical) {
};

threadpl_client::~threadpl_client() {};

void threadpl_client::translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) {
	switch (operation) {
		case STDA_OT_SEND_RAW_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {
		        lpBuffer->consume(dwIOsize);

				_session->releaseSend();
			}

			break;
		case STDA_OT_SEND_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {

				packet_head_client phc;
				packet *_packet = nullptr;
				int32_t left = 0;

				do {
					try {
						LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT_INVERT(phc, sizeof(phc), STDA_OT_DISPACH_PACKET_SERVER);
					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED MY class exception: " + e.getFullMessageError(), CL_ONLY_CONSOLE));

						if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CRYPT || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::COMPRESS) {
							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_WITH_MSG_CLIENT;
						}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 15) ||
							STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 1)) {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_CLIENT;

							break;
						}
					}catch (std::exception& e) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED std::class exception: " + std::string(e.what()), CL_ONLY_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_CLIENT;

						break;
					}catch (...) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED Exception desconhecida.", CL_ONLY_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_CLIENT;

						break;
					}
				} while (dwIOsize > 0);

				_session->releaseSend();
			}

			break;
		case STDA_OT_RECV_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {
				packet_head ph;
				packet *_packet = nullptr;
				int32_t left = 0;

				lpBuffer->addSize(dwIOsize);	// Add o size que recebeu no socket

				do {
					try {
						LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER_INVERT(ph, sizeof(ph), STDA_OT_DISPACH_PACKET_CLIENT);
					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED MY class exception: " + e.getFullMessageError(), CL_ONLY_CONSOLE));

						if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CRYPT || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::COMPRESS) {
							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_WITH_MSG_CLIENT;
						}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 15) ||
							STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 1)) {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_SERVER;

							break;
						}
					}catch (std::exception& e) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED std::class exception: " + std::string(e.what()), CL_ONLY_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_SERVER;

						break;
					}catch (...) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED Exception desconhecida.", CL_ONLY_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_SERVER;

						break;
					}
				} while (dwIOsize > 0);

				// Post new IO Operation - RECV REQUEST
				memset(lpBuffer, 0, sizeof(OVERLAPPED));	// Zera o over pra receber mais

				postIoOperation(_session, lpBuffer, operation - 1);
				
			}else {
				if (_session != nullptr && _session->m_sock != INVALID_SOCKET)
					DisconnectSession(_session);
			}

			break;
	}
};
