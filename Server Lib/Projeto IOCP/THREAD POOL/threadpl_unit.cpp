// Arquivo threadpl_unit.cpp
// Criado em 02/12/2018 as 12:19 por Acrisio
// Implementação da classe threadpl_unit

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "threadpl_unit.hpp"
#include "../UTIL/message_pool.h"
#include "../SOCKET/socketserver.h"
#include "../SOCKET/socket.h"
#include "../UTIL/hex_util.h"
#include "../UTIL/exception.h"

#include "../TYPE/stda_error.h"

using namespace stdA;

threadpl_unit::threadpl_unit(size_t _num_thread_workers_io, size_t _num_thread_workers_logical, uint32_t _job_thread_num)
    : threadpool(_num_thread_workers_io, _num_thread_workers_logical, _job_thread_num) {
};

threadpl_unit::~threadpl_unit() {};

void threadpl_unit::translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) {
	switch (operation) {
		case STDA_OT_SEND_RAW_COMPLETED:
			
			if (dwIOsize > 0 && lpBuffer != nullptr) {
		       
				try {
					
					lpBuffer->consume(dwIOsize);

					_session->releaseSend();

				}catch (exception& e) {

					CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

					_session->releaseSend();

					_smp::message_pool::getInstance().push(new message("[threadpl_unit::translate_packet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				
				}catch (std::exception& e) {

					CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

					_session->releaseSend();

					_smp::message_pool::getInstance().push(new message("[threadpl_unit::translate_packet][ErrorSystem] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
				}

			}else {
				_session->releaseSend();

				try {
					
					// getConnectTime pode lançar exception
					if (_session->getConnectTime() <= 0 && _session->getState()) {

						_smp::message_pool::getInstance().push(new message("[threadpl_unit::translate_packet][Error] [STDA_OT_SEND_RAW_COMPLETED] _session[OID=" + std::to_string(_session->m_oid) + "] is not connected.", CL_FILE_LOG_AND_CONSOLE));

						// Ainda não habilitar o disconnect session, vms aguardar as mensagens para ver se vai ter
						//DisconnectSession(_session);
					}
				
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[threadpl_unit::translate_packet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			}

			break;
		case STDA_OT_SEND_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {
				packet_head ph;
				packet *_packet = nullptr;
				int32_t left = 0;

				//_session->releaseSend();	// Erro se colocar ele aqui
				
				do {
					try {
						LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER_UNIT(ph, sizeof(ph), STDA_OT_DISPACH_PACKET_SERVER);
					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED MY class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

						if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CRYPT || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::COMPRESS) {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_WITH_MSG;
						}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 15) ||
								STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 1))  {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_SERVER;

							// Libera o send
							_session->releaseSend();

							//break;
							return;	// Return por que o break só vai sair do while, e esse break é para sair do switch, mas como depois do switch não tem nada, então só terminar a função

						}else {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_SERVER;

							// Libera o send
							_session->releaseSend();

							//break;
							return;	// Return por que o break só vai sair do while, e esse break é para sair do switch, mas como depois do switch não tem nada, então só terminar a função

						}
					}catch (std::exception& e) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED std::class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_SERVER;

						// Libera o send
						_session->releaseSend();

						//break;
						return;	// Return por que o break só vai sair do while, e esse break é para sair do switch, mas como depois do switch não tem nada, então só terminar a função

					}catch (...) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED Exception desconhecida. {SESSION[IP=" + std::string(_session->getIP()) + "]}", CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_SERVER;

						// Libera o send
						_session->releaseSend();

						//break;
						return;	// Return por que o break só vai sair do while, e esse break é para sair do switch, mas como depois do switch não tem nada, então só terminar a função
					}
				} while (dwIOsize > 0);

				// Libera o send
				_session->releaseSend();
			}else {

				// Libera o send
				_session->releaseSend();

				try {
					
					// getConnectTime pode lançar exception
					if (_session->getConnectTime() <= 0 && _session->getState()) {

						_smp::message_pool::getInstance().push(new message("[threadpl_unit::translate_packet][Error] [STDA_OT_SEND_COMPLETED] _session[OID=" + std::to_string(_session->m_oid) + "] is not connected.", CL_FILE_LOG_AND_CONSOLE));

						// Ainda não habilitar o disconnect session, vms aguardar as mensagens para ver se vai ter
						//DisconnectSession(_session);
					}
				
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[threadpl_unit::translate_packet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			}

			break;
		case STDA_OT_RECV_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {
				//_session->m_tick = std::clock();	// Tick time client para o TTL(time to live)

				packet_head ph;
				packet *_packet = nullptr;
				int32_t left = 0;
				
				lpBuffer->addSize(dwIOsize);	// Add o size que recebeu no socket

				do {
					try {
						LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT_UNIT(ph, sizeof(ph), STDA_OT_DISPACH_PACKET_CLIENT);
					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED MY class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

						if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CRYPT || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::COMPRESS) {
							
							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_WITH_MSG;

							CLEAR_BUFFER_LOOP_CLIENT;

							// Shutdown Socket Here
#if defined(_WIN32)
							::shutdown(_session->m_sock, SD_BOTH);
#elif defined(__linux__)
							::shutdown(_session->m_sock.fd, SD_BOTH);
#endif

							break;

						}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 15) ||
							STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 1)) {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_CLIENT;

							// Shutdown Socket Here
#if defined(_WIN32)
							::shutdown(_session->m_sock, SD_BOTH);
#elif defined(__linux__)
							::shutdown(_session->m_sock.fd, SD_BOTH);
#endif

							break;
						}else {
							
							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_CLIENT;

							// Shutdown Socket Here
#if defined(_WIN32)
							::shutdown(_session->m_sock, SD_BOTH);
#elif defined(__linux__)
							::shutdown(_session->m_sock.fd, SD_BOTH);
#endif

							break;
						}
					}catch (std::exception& e) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED std::class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_CLIENT;

						// Shutdown Socket Here
#if defined(_WIN32)
						::shutdown(_session->m_sock, SD_BOTH);
#elif defined(__linux__)
						::shutdown(_session->m_sock.fd, SD_BOTH);
#endif

						break;
					}catch (...) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED Exception desconhecida {SESSION[IP=" + std::string(_session->getIP()) + "]}.", CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_CLIENT;

						// Shutdown Socket Here
#if defined(_WIN32)
						::shutdown(_session->m_sock, SD_BOTH);
#elif defined(__linux__)
						::shutdown(_session->m_sock.fd, SD_BOTH);
#endif

						break;
					}
				} while (dwIOsize > 0);

#if defined(_WIN32)
				// Post new IO Operation - RECV REQUEST
				memset(lpBuffer, 0, sizeof(OVERLAPPED));	// Zera o over pra receber mais
#endif

				//_session->requestRecvBuffer(); // Manda de novo para receber qualquer resposta do cliente
				_session->releaseRecv();
				//postIoOperation(_session, lpBuffer, operation - 1);
			}else {
				if (_session != nullptr /*&& _session->m_sock != INVALID_SOCKET*/)
					DisconnectSession(_session);
			}

			break;
	}
};
