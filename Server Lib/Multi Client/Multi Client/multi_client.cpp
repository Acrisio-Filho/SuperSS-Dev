// Arquivo mu/lti_client.cpp
// Criado em 19/12/2017 por Acrisio
// Implementação da classe multi_client

#pragma pack(1)

#include <WinSock2.h>
#include "multi_client.h"
#include "../PACKET/packet_func_client.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../SESSION/player.hpp"
#include "../../Projeto IOCP/SOCKET/socket.h"

#include <sstream>

using namespace stdA;

multi_client* smc::mc = nullptr;

multi_client::multi_client(unsigned long num_connection) : 
		client(m_cliente_manager), m_cliente_manager(*this, num_connection * 2, 0), m_num_connection(num_connection) {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[multi_client::multi_client][Error] Multi_client inicializado com error" , CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		packet_func::funcs.addPacketCall(0x000, packet_func::packet000, this);
		packet_func::funcs.addPacketCall(0x001, packet_func::packet001, this);
		packet_func::funcs.addPacketCall(0x002, packet_func::packet002, this);
		packet_func::funcs.addPacketCall(0x003, packet_func::packet003, this);
		packet_func::funcs.addPacketCall(0x006, packet_func::packet006, this);
		packet_func::funcs.addPacketCall(0x009, packet_func::packet009, this);
		packet_func::funcs.addPacketCall(0x010, packet_func::packet010, this);
		packet_func::funcs.addPacketCall(0x03F, packet_func::packet03F, this);
		packet_func::funcs.addPacketCall(0x044, packet_func::packet044, this);
		packet_func::funcs.addPacketCall(0x04D, packet_func::packet04D, this);
		packet_func::funcs.addPacketCall(0x04E, packet_func::packet04E, this);
		packet_func::funcs.addPacketCall(0x0F5, packet_func::packet0F5, this);
		packet_func::funcs.addPacketCall(0x0D7, packet_func::packet0D7, this);
		packet_func::funcs.addPacketCall(0x1A9, packet_func::packet1A9, this);
		packet_func::funcs.addPacketCall(0x1AD, packet_func::packet1AD, this);

		packet_func::funcs.addPacketCall(0x040, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x046, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x047, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x210, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x25D, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x158, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x0B4, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x169, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x13F, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x096, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x181, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x137, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x136, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x138, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x135, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x144, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x0F1, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x21E, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x21D, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x131, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x102, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x072, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x0E1, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x073, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x071, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x070, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x11F, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x101, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x095, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x1F6, packet_func::packet_svFazNada, this);
		packet_func::funcs.addPacketCall(0x096, packet_func::packet_svFazNada, this);

		packet_func::funcs_sv.addPacketCall(0x001, packet_func::packet_sv001, this);
		packet_func::funcs_sv.addPacketCall(0x002, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x003, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x004, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x03D, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x081, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x09E, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0A1, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0A2, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0C1, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0F4, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0FB, packet_func::packet_svFazNada, this);

		// Ponteiro da classe Multi client
		smc::mc = this;

		m_state = GOOD;
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[multi_client::multi_client][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;
	}
};

multi_client::~multi_client() {
	waitAllThreadFinish(INFINITE);
};

void multi_client::start() {

	if (m_num_connection > 0) {
		ClientInfo ci{};

		for (size_t i = 0; i < m_num_connection; ++i) {
			ci.clear();

			strcpy_s(ci.m_id, std::string("nat" + std::to_string(i % 200)).c_str());
			strcpy_s(ci.m_pass, "123456");
			strcpy_s(ci.m_client_version, "SS.R7.989.00");
			ci.m_packet_version = 2019110100; //2016121900;

			try {
				ConnectAndAssoc("127.0.0.1", 10303, ci);
			}catch (exception& e) {
				_smp::message_pool::getInstance().push(new message(e.getFullMessageError() + "\r\nmulti_client::start()"));

				// ID temporario para verificar se connectou com sucesso ou deu erro
				//_cliente->m_uid = ~0;
			}catch (std::exception& e) {
				_smp::message_pool::getInstance().push(new message(std::string(e.what()) + "\r\nmulti_client::start()"));

				// ID temporario para verificar se connectou com sucesso ou deu erro
				//_cliente->m_uid = ~0;
			}catch (...) {
				_smp::message_pool::getInstance().push(new message("Erro desconhecido. multi_client::start()"));

				// ID temporario para verificar se connectou com sucesso ou deu erro
				//_cliente->m_uid = ~0;
			}
		}
	}
	
	client::start();
};

void multi_client::checkClienteOnline() {
	/*for (auto i = 0ul; i < m_cliente_manager; ++i) {
		if (m_clientes[i]->m_uid == ~0) {
			try {
				ConnectAndAssoc("127.0.0.1", 10303, m_clientes[i]);
			}catch (exception& e) {
				_smp::message_pool.push(new message(e.getFullMessageError() + "\r\nmulti_client::checkClienteOnline()"));
			}catch (std::exception& e) {
				_smp::message_pool.push(new message(std::string(e.what()) + "\r\nmulti_client::checkClienteOnline()"));
			}catch (...) {
				_smp::message_pool.push(new message("Erro desconhecido. multi_client::checkClienteOnline()"));
			}
		}
	}*/
};

void multi_client::DisconnectSession(session *_session) {

	if (_session == nullptr)
		throw exception("[multi_client::DisconnectSession][Error] _session is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MULTI_CLIENT, 50, 0));

	player *p = reinterpret_cast< player* >(_session);

	_smp::message_pool::getInstance().push(new message("Player Desconectou. ID: " + std::string(p->m_ci.m_id) + "  UID: " + std::to_string(p->m_ci.m_uid), CL_ONLY_CONSOLE));

	m_cliente_manager.deleteSession(_session);
};

void multi_client::accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) {
	// Faz nada nesse que é cliente, só conecta
	return;
};

std::string multi_client::getSessionID(session *_session) {
	
	if (_session == nullptr)
		throw exception("[multi_client::getSessionID][Error] _session is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MULTI_CLIENT, 25, 0));

	std::string id = "";

	player *p = reinterpret_cast< player* >(_session);

	id = p->m_ci.m_id;

	return id;
};

void multi_client::ConnectAndAssoc(std::string _host, short _port, ClientInfo& _ci) {
	SOCKET _sock = INVALID_SOCKET;
	socket _socket;
	player *_session = nullptr;
	BOOL tcp_nodelay = 1;

	if ((_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		throw exception("Erro nao conseguiu criar o socket. client::ConnectAndAssoc().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENT, 2, WSAGetLastError()));

	if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		_smp::message_pool::getInstance().push(new message("nao conseguiu desabilitar tcp delay(nagle algorithm). client::ConnectAndAssoc().", CL_ONLY_CONSOLE));

	_session = (player*)m_cliente_manager.addSession(_sock, { 0 }, 0);

	std::srand(std::clock());

	m_iocp_io[std::rand() % 16].associaDeviceToPort((ULONG_PTR)_session, (HANDLE)_sock);

	_socket.attach(_sock);
	
	try {
		_socket.connect(_host, _port);

		//_smp::message_pool.push(new message("Cliente connectou com o host com sucesso! Port: " + std::to_string(_port), CL_FILE_TIME_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		UNREFERENCED_PARAMETER(e);

		m_cliente_manager.deleteSession(_session);

		// Relança
		throw;
	}

	_socket.detatch();

	_smp::message_pool::getInstance().push(new message("Connectou com o host: " + _host + ":" + std::to_string(_port), CL_ONLY_CONSOLE));

	_session->m_ci.m_uid = 1;	// Login

	/*strcpy_s(_session->m_ci.m_id, _cc.m_id);
	strcpy_s(_session->m_ci.m_pass, _cc.m_pass);
	strcpy_s(_session->m_ci.m_client_version, _cc.m_client_version);
	_session->m_ci.m_packet_version = _cc.m_packet_version;*/
	_session->m_ci = _ci;

	_session->requestRecvBuffer();
};

DWORD multi_client::send_msg_lobby() {

	try {
		_smp::message_pool::getInstance().push(new message("send_msg_lobby iniciado com sucesso!"));
		timer *_timer = nullptr;

		std::vector< player* > channel_sessions;
		bool send = false;

		while (InterlockedCompareExchange(&m_continue_send_msg, 1, 1)) {
			channel_sessions = m_cliente_manager.getChannelClientes();

			for (auto i = 0ul; i < channel_sessions.size(); ++i) {
				if (channel_sessions[i]->m_ci.m_lobby == 1) {

					std::srand(std::clock() * i * 7);

					if (!send)
						/*packet_func::sendMessage(channel_sessions[i], ((cliente*)channel_sessions[i]->m_client)->m_nickname, "Ola Lobby");
					else */{
						std::vector< DWORD > interval = { 100 };

						if ((_timer = m_timer_mgr.createTimer(500 + (std::rand() % 1500),
									new (timer::timer_param)
									{
										job(packet_func::sendMessage, channel_sessions[i], nullptr),
										m_job_pool
									})) != nullptr)
							send = true;
						else
							_smp::message_pool::getInstance().push(new message("Erro ao criar o Timer em client::send_msg_lobby()", CL_FILE_LOG_AND_CONSOLE));
					}
					

					Sleep(500 + (std::rand() % 1500));
				}

				if (!InterlockedCompareExchange(&m_continue_send_msg, 1, 1))
					break;
			}

			// Sai já criou o time para todos
			break;
		}

		if (_timer != nullptr)
			m_timer_mgr.deleteTimer(_timer);

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "send_msg_lobby() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de send_msg_lobby()..."));

	return 0ul;
};

void multi_client::commandScan() {
	char command[1024];
	
	int ret = 0; // SUCCESS FUNCTION
	std::string s;
	timer *_timer = nullptr;
	std::vector< DWORD > interval = { 30000, 10000, 1000 };
	
	while (ret == 0) {
		std::cin.getline(command, sizeof(command));
	
		std::stringstream ss(command);
	
		while (!ss.eof()) {
			s = "";
	
			ss >> s;
	
			if (s.empty()) {
				_smp::message_pool::getInstance().push(new message("Command empty", CL_ONLY_CONSOLE));
					
				continue;
			}
	
			if (!s.empty() && s.compare("exit") == 0) {
				if (_timer != nullptr)
					m_timer_mgr.deleteTimer(_timer);
	
				return;	// Sai
			}else if (!s.empty() && s.compare("timer") == 0) {
				s = "";
	
				ss >> s;
	
				if (s.empty()) {
					_smp::message_pool::getInstance().push(new message("timer not work without parameters.", CL_ONLY_CONSOLE));
						
					continue;
				}
	
				if (s.compare("start") == 0) {
	
					if (_timer == nullptr) {
						if ((_timer = m_timer_mgr.createTimer(2000,
								new (timer::timer_param)
								{
									job(packet_func::sendMessage, m_cliente_manager.getChannelClientes()[0], nullptr),
									m_job_pool
								}, interval, timer::PERIODIC_INFINITE)) == nullptr)
							_smp::message_pool::getInstance().push(new message("Erro ao criar o Timer em client::scanCommand().", CL_ONLY_CONSOLE));
					}else
						_timer->start();
	
				}else if (s.compare("stop") == 0) {
					if (_timer == nullptr) {
						_smp::message_pool::getInstance().push(new message("timer not creator, execute cmd start first", CL_ONLY_CONSOLE));
	
						continue;
					}
	
					_timer->stop();
				}else if (s.compare("restart") == 0) {
					if (_timer == nullptr) {
						_smp::message_pool::getInstance().push(new message("timer not creator, execute cmd start first", CL_ONLY_CONSOLE));
	
						continue;
					}
	
					_timer->restart();
				}else if (s.compare("pause") == 0) {
					if (_timer == nullptr) {
						_smp::message_pool::getInstance().push(new message("timer not creator, execute cmd start first", CL_ONLY_CONSOLE));
	
						continue;
					}
	
					_timer->pause();
				}else
					_smp::message_pool::getInstance().push(new message("timer unknown parameter \"" + s + "\"", CL_ONLY_CONSOLE));
			}else
				_smp::message_pool::getInstance().push(new message("Unknown Command", CL_ONLY_CONSOLE));
		}
	}
};
