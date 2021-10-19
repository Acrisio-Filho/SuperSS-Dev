// Arquivo pool.cpp
// Criado em 14/11/2020 as 10:00 por Acrisio
// Implementa��o da classe Pool

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "pool.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"

#include "../UTIL/logger.hpp"
#include "../TYPE/common.hpp"
#include "../PLAYER/player_manager.hpp"

#include <ctime>
#include "../../Projeto IOCP/UTIL/string_util.hpp"
#include "../../Projeto IOCP/UTIL/hex_util.h"

#if defined(_WIN32)
#include <DbgHelp.h>
#include "../../Projeto IOCP/TYPE/set_se_exception_with_mini_dump.h"

#pragma comment(lib, "DbgHelp.lib")
#endif

// Auth Key
constexpr char AUTH_KEY[] = "7436c482-9268-473f-bb8b-7aea263babc0";

#if defined(_WIN32)
#define BEGIN_THREAD_SETUP(_type_class) DWORD result = 0; \
						   STDA_SET_SE_EXCEPTION \
						   try { \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) { \

#define END_THREAD_SETUP(name_thread)	  } \
							}catch (exception& e) { \
								sLogger::getInstance().Log(e.getFullMessageError(), eTYPE_LOG::TL_ERROR); \
							}catch (std::exception& e) { \
								sLogger::getInstance().Log(e.what(), eTYPE_LOG::TL_ERROR); \
							}catch (...) { \
								sLogger::getInstance().Log(std::string((name_thread)) + " -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", eTYPE_LOG::TL_ERROR); \
							} \
							sLogger::getInstance().Log("Saindo do trabalho->" + std::string((name_thread)), eTYPE_LOG::TL_ERROR); \
						return result;
#elif defined(__linux__)
#define BEGIN_THREAD_SETUP(_type_class) void* result = (void*)0; \
						   try { \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) { \

#define END_THREAD_SETUP(name_thread)	  } \
							}catch (exception& e) { \
								sLogger::getInstance().Log(e.getFullMessageError(), eTYPE_LOG::TL_ERROR); \
							}catch (std::exception& e) { \
								sLogger::getInstance().Log(e.what(), eTYPE_LOG::TL_ERROR); \
							}catch (...) { \
								sLogger::getInstance().Log(std::string((name_thread)) + " -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", eTYPE_LOG::TL_ERROR); \
							} \
							sLogger::getInstance().Log("Saindo do trabalho->" + std::string((name_thread)), eTYPE_LOG::TL_ERROR); \
						return result;
#endif

using namespace stdA;

Pool::Pool() 
	:
#if defined(_WIN32) 
	wsa(), 
#endif
	m_socket(), m_thread(nullptr), m_state(false), m_server_id(0u) {
}

Pool::~Pool() {

	try {
		
		close();

	}catch (exception& e) {
		// Garantir o no exception do destructor
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

int Pool::initialize(FNPROCSCLOG _scLog, FNPROCRESPONSECALLBACK _responseCallBack, const uint32_t _server_id) {
	
	try {

		// Set o ID do server que carregou e inicializou a DLL
		m_server_id = _server_id;

		if (m_state && m_socket.isConnected())
			return 0;

		// Load Virtual Log
		sLogger::getInstance().loadVirtualFunctions(_scLog, _responseCallBack);

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Log] Initializing..", eTYPE_LOG::TL_LOG_SYSTEM);

		if (connect() != 0)
			return 1;

		if (!m_socket.isConnected())
			return 1;

		// Initialized
		m_state = true;

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}

	return 0;
}

void Pool::sendPacket(Packet< ePACKET >& _pckt) {

	try {

		if (!m_socket.isConnected())
			if (connect() == 1)
				sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Error] Nao conseguiu inicializar com o servidor do Smart Calculator", eTYPE_LOG::TL_WARNING);

		if (m_socket.isConnected()) {

			auto snd = _pckt.makePacket();

			if (::send(
#if defined(_WIN32)
				m_socket.m_socket, 
#elif defined(__linux__)
				m_socket.m_socket.fd, 
#endif
				(const char*)snd.data(), (int)snd.size(), 0) == SOCKET_ERROR) {

				m_socket.close();

				sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Error] ao enviar os  dados para o servidor do Smart Calculator. ERROR: " + std::to_string(
#if defined(_WIN32)
					WSAGetLastError()
#elif defined(__linux__)
					errno
#endif
				), eTYPE_LOG::TL_ERROR);
			}
		}

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

// Send Server command
void Pool::sendServerCmd(std::string _cmd) {

	try {

		Packet< ePACKET > pckt(ePACKET::COMMAND_SERVER);

		pckt << _cmd;

		sendPacket(pckt);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

#if defined(_WIN32)
DWORD WINAPI Pool::_translate(LPVOID lpParameter) {
#elif defined(__linux__)
void* Pool::_translate(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(Pool);

	pTP->translate();

	END_THREAD_SETUP("translate");
}

#if defined(_WIN32)
DWORD Pool::translate() {
#elif defined(__linux__)
void* Pool::translate() {
#endif
	
	try {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Log] create thread Translate com sucesso!", eTYPE_LOG::TL_LOG_SYSTEM);

		try {

			unsigned char buff[1024];
			int len = 0u;

			while (m_socket.isConnected()) {

				len = m_socket.read(buff, 1024);

#ifdef _DEBUG
				sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][read] " + hex_util::BufferToHexString(buff, len), eTYPE_LOG::TL_LOG);
#endif // _DEBUG

				if (len == 0u || len == SOCKET_ERROR) {

					m_socket.close();

					if (len == SOCKET_ERROR)
						sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Error] ao enviar os  dados para o servidor do Smart Calculator. ERROR: " + std::to_string(
#if defined(_WIN32)
							WSAGetLastError()
#elif defined(__linux__)
							errno
#endif
						), eTYPE_LOG::TL_ERROR);

					break;
				}

				m_sBuff.insert(m_sBuff.end(), buff, buff + len);

				unsigned short pckt_len = 0;

				while (m_sBuff.size() >= 2 && (pckt_len = *(unsigned short*)m_sBuff.data()) < m_sBuff.size()) {

#ifdef _DEBUG
					sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][packet] " + std::to_string(pckt_len), eTYPE_LOG::TL_LOG);
#endif // _DEBUG

					Packet<ePACKET> pckt(ePACKET(*(m_sBuff.data() + 2)));

					pckt.write(m_sBuff.data() + 3, pckt_len);

					m_sBuff.erase(m_sBuff.begin(), m_sBuff.begin() + (pckt_len + 3));

					switch (pckt.m_tipo) {
					case ePACKET::HAND_SHAKE:
					{
#ifdef _DEBUG
						sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][HAND_SHAKE] " + hex_util::BufferToHexString(pckt.m_data.data(), pckt.m_data.size()), eTYPE_LOG::TL_LOG);
#endif // _DEBUG

						std::string hand_shk;
						pckt >> hand_shk;

						// N�o tem resposta para nenhum player isso � o login do sistema
						
						// S� mostra Log
						sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][HAND_SHAKE][Log] " + hand_shk, eTYPE_LOG::TL_LOG);
						break;
					}
					case ePACKET::REPLY_SERVER: {

						// Resposta para o server ou comandos
						std::string reply_or_cmd;
						uint32_t uid = 0u;

						pckt >> uid;
						pckt >> reply_or_cmd;

						// Envia a resposta ou o comando para o server
						sLogger::getInstance().Response(uid, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR/*Ignora esse o response SERVER*/, reply_or_cmd, eTYPE_RESPONSE::SERVER_RESPONSE);

						break;
					}
					case ePACKET::REPLY_SMART: {

						uint32_t id = 0u;
						pckt >> id;

						auto ctx = sPlayerManager::getInstance().getPlayerCtx(id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR);

						if (ctx != nullptr)
							ctx->getPlayer()->translatePacket(&pckt);
						else
							sLogger::getInstance().Response(id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "System Error", eTYPE_RESPONSE::PLAYER_RESPONSE);
						
						break;
					}
					case ePACKET::REPLY_STADIUM: {

						uint32_t id = 0u;
						pckt >> id;

						auto ctx = sPlayerManager::getInstance().getPlayerCtx(id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM);

						if (ctx != nullptr)
							ctx->getPlayer()->translatePacket(&pckt);
						else
							sLogger::getInstance().Response(id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "System Error", eTYPE_RESPONSE::PLAYER_RESPONSE);

						break;
					}
					} // End Switch
				}
			}

		}catch (exception& e) {

			sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
		}

		if (m_socket.isConnected())
			m_socket.close();

		m_state = false; // Desconectou

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}

#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return 0;
#endif
}

void Pool::close() {

	try {

		if (m_socket.isConnected())
			m_socket.close();

		if (m_thread != nullptr) {

			if (m_thread->isLive()) {

				m_thread->exit_thread();

				m_thread->waitThreadFinish(INFINITE);
			}

			delete m_thread;
		}

		m_thread = nullptr;

		if (!m_sBuff.empty()) {
			m_sBuff.clear();
			m_sBuff.shrink_to_fit();
		}

		// Server ID que carregou a DLL
		m_server_id = 0u;

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

int Pool::connect() {

	try {

#if defined(_WIN32)
		if (m_socket.m_socket != INVALID_SOCKET)
			m_socket.close();

		m_socket.m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
#elif defined(__linux__)
		if (m_socket.m_socket.fd != INVALID_SOCKET)
			m_socket.close();

		m_socket.m_socket.fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
#endif

		m_socket.connect("127.0.0.1", 12345);

#if defined(__linux__)
		// Time connected socket
		clock_gettime(CLOCK_SOCKET_TO_CHECK, &m_socket.m_socket.connect_time);
#endif

		if (m_thread == nullptr)
			m_thread = new thread(1, Pool::_translate, this);
		else if (!m_thread->isLive()) // Religa a thread
			m_thread->init_thread();

		if (!m_socket.isConnected())
			return 1;

		// Request Login
		Packet<ePACKET> pckt(ePACKET::LOGIN);

		// Key
		pckt << AUTH_KEY;
		pckt << m_server_id;

		sendPacket(pckt);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}

	return 0;
}
