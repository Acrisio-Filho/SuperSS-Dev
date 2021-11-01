// Arquivo gg_auth_server.cpp
// Criado em 02/02/2021 as 16:54 por Acrisio
// Implementa��o da classe gg_auth_server

#pragma pack(1)
#include <WinSock2.h>
#include "gg_auth_server.hpp"

#include "../PACKET/packet_func_gg_as.hpp"
#include "../../Projeto IOCP/UTIL/md5.hpp"

#include <ctime>

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
											throw exception("[gg_auth_server::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GG_AUTH_SERVER, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
								if (_packet == nullptr) \
									throw exception("[gg_auth_server::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GG_AUTH_SERVER, 6, 0)); \

// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!_session.m_is_authorized) \
												throw exception("[gg_auth_server::request" + std::string((method)) + "][Error] Player[UID=" + std::to_string(_session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GG_AUTH_SERVER, 1, 0x7000501)); \

using namespace stdA;

gg_auth_server::gg_auth_server() : unit(m_player_manager, 2/*DB Instance*/, 2/*Thread Jobs*/)
	, m_player_manager(*this, m_si.max_user), m_game_guard(nullptr) {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[gg_auth_server::gg_auth_server][Error] Ao iniciar o Game Guard Auth Server.\n", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		// Inicializa o Unit_Connect, que conecta com o Auth Server
		m_unit_connect = new unit_auth_server_connect(*this, m_si);

		config_init();

		// Game Guard
		m_game_guard = new GGAuth(m_si.max_user * 2000/*Limit de player por server*/);

		packet_func::funcs.addPacketCall(0x01, packet_func::packet001, this);
		packet_func::funcs.addPacketCall(0x02, packet_func::packet002, this);
		packet_func::funcs.addPacketCall(0x03, packet_func::packet003, this);
		packet_func::funcs.addPacketCall(0x04, packet_func::packet004, this);
		packet_func::funcs.addPacketCall(0x05, packet_func::packet005, this);
		packet_func::funcs.addPacketCall(0x06, packet_func::packet006, this);
		/*packet_func::funcs.addPacketCall(0x07, packet_func::packet007, this);*/

		packet_func::funcs_sv.addPacketCall(0x00, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x01, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x02, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x03, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x04, packet_func::packet_svFazNada, this);
		/*packet_func::funcs_sv.addPacketCall(0x05, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x06, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x07, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x08, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x09, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0A, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0B, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0C, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0D, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0E, packet_func::packet_svFazNada, this);*/

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::gg_auth_server][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;

		// n�o posso lan�a exception para fora do contrutor que eles nao chama o destrutor dele antes
	}
}

gg_auth_server::~gg_auth_server() {

	// Destroy o Unit Connect que conecta com o Auth Server
	destroy_unit();

	if (m_game_guard != nullptr)
		delete m_game_guard;

	// Crucial esse aqui para terminar o server sem crashs
	waitAllThreadFinish(INFINITE);
}

void gg_auth_server::requestLoginFirstStep(player& _session, packet *_packet) {
	REQUEST_BEGIN("LoginFirstStep");

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestLoginFirstStep][Log] Hex: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif

		std::srand((unsigned long)std::clock());

		unsigned long uid = _packet->readUint32();

		// Gera Auth Key
		_session.m_pi.uid = uid;
		_session.m_pi.auth_key = (unsigned long)(std::clock() + std::rand());

		// Send Key Para o Auth Server Enviar para o Server que quer logar com o Game Guard Auth Server
		// Para o Auth Server
		packet p((unsigned short)0x01);

		p.addUint32(_session.m_pi.auth_key);

		sgg_as::getInstance().sendCommandToOtherServerWithAuthServer(p, uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestLoginFirstStep][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::requestLoginSecondStep(player& _session, packet *_packet) {
	REQUEST_BEGIN("LoginSecondStep");

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestLoginSecondStep][Log] Hex: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif

		std::string key = _packet->readString();

		unsigned long crypt_auth_key = _session.m_pi.uid ^ _session.m_pi.auth_key;

		std::string check_key = std::to_string(crypt_auth_key);

		try {
						
			md5::processData((unsigned char*)check_key.data(), (unsigned long)check_key.length());

			check_key = md5::getHash();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestLoginSecondStep][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			// Relan�a
			throw;
		}

		if (check_key.compare(key) != 0)
			throw exception("[gg_auth_server::requestLoginSecondStep][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] Auth Key(" + std::to_string(_session.m_pi.auth_key) + ") not match[PRIVATE=" 
					+ check_key  + ", PUBLIC=" + key + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GG_AUTH_SERVER, 10, 0));
		
		// Sucesso Login
		_session.m_is_authorized = 1u;	// Autorizado a ficar connectado, por bastante tempo

		// Log
		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestLoginSecondStep][Log] Server[UID=" 
				+ std::to_string(_session.m_pi.uid) + "] Logou com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		// Para o Player
		packet p((unsigned short)0x01);

		p.addUint32(_session.m_oid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestLoginSecondStep][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::requestAddPlayerGameGuardCtx(player& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("AddPlayerGameGuardCtx");

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("AddPlayerGameGuardCtx");

		unsigned long uid = _packet->readUint32();

		_session.addPlayerToGameGuard(uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestAddPlayerGameGuardCtx][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Reply Error
		p.init_plain((unsigned short)0x04);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::GG_AUTH_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1ul);

		packet_func::session_send(p, &_session, 1);
	}
}

void gg_auth_server::requestDeletePlayerGameGuardCtx(player& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("DeletePlayerGameGuardCtx");

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("DeletePlayerGameGuardCtx");

		unsigned long uid = _packet->readUint32();

		_session.removePlayerToGameGuard(uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestDeletePlayerGameGuardCtx][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Reply Error
		p.init_plain((unsigned short)0x04);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::GG_AUTH_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1ul);

		packet_func::session_send(p, &_session, 1);
	}
}

void gg_auth_server::requestGetAuthQueryPlayer(player& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("GetAuthQueryPlayer");

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("GetAuthQueryPlayer");

		p.init_plain((unsigned short)0x02);

		unsigned long uid = _packet->readUint32();

		auto s = _session.getPlayerGameGuard(uid);

		if (s == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestGetAuthQueryPlayer][Error] Nao encontrou o Player[SOCKET=" + std::to_string(uid) + "] no map.", CL_FILE_LOG_AND_CONSOLE));

			p.addUint32(1235); // Error
			p.addUint32(uid);
			p.addZeroByte(sizeof(GG_AUTH_DATA));

			packet_func::session_send(p, &_session, 1);
		
		}else {

			unsigned long error = s->m_csa.GetAuthQuery();

			p.addUint32(error);
			p.addUint32(uid);
			p.addBuffer(&s->m_csa.m_AuthQuery, sizeof(GG_AUTH_DATA));
		}

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestGetAuthQueryPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Reply Error
		p.init_plain((unsigned short)0x02);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::GG_AUTH_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1ul);

		p.addInt32(-1);  // Socket id(uid)
		p.addZeroByte(sizeof(GG_AUTH_DATA));

		packet_func::session_send(p, &_session, 1);
	}
}

void gg_auth_server::requestCheckAuthAnswerPlayer(player& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("requestCheckAuthAnswerPlayer");

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("requestCheckAuthAnswerPlayer");

		GG_AUTH_DATA data;

		p.init_plain((unsigned short)0x03);

		unsigned long uid = _packet->readUint32();
		
		_packet->readBuffer(&data, sizeof(GG_AUTH_DATA));

		auto s = _session.getPlayerGameGuard(uid);

		if (s == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestGetAuthQueryPlayer][Error] Nao encontrou o Player[SOCKET=" + std::to_string(uid) + "] no map.", CL_FILE_LOG_AND_CONSOLE));

			p.addUint32(1235); // Error
			p.addUint32(uid);

			packet_func::session_send(p, &_session, 1);
		
		}else {

			// Copy
			memcpy_s(&s->m_csa.m_AuthAnswer, sizeof(GG_AUTH_DATA), &data, sizeof(GG_AUTH_DATA));

			unsigned long error = s->m_csa.CheckAuthAnswer();

			p.addUint32(error);
			p.addUint32(uid);
		}

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::requestCheckAuthAnswerPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Reply Error
		p.init_plain((unsigned short)0x03);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::GG_AUTH_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1ul);

		p.addInt32(-1); // Socket id(uid)

		packet_func::session_send(p, &_session, 1);
	}
}

void gg_auth_server::onAcceptCompleted(session* _session) {

	if (_session == nullptr)
		throw exception("[gg_auth_server::onAcceptCompleted][Error] session is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 60, 0));

	if (!_session->getState())
		throw exception("[gg_auth_server::onAcceptCompleted][Error] session is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 61, 0));

	if (!_session->isConnected())
		throw exception("[gg_auth_server::onAcceptCompleted][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 62, 0));

	packet _packet((unsigned short)0x00);	// Tipo Packet Game Guard Auth Server initial packet no compress e no crypt

	_packet.addInt32((long)_session->m_key);	// key
	_packet.addInt32(m_si.uid);					// Server UID

	_packet.makeRaw();
	WSABUF mb = _packet.getMakedBuf();

	//SHOW_DEBUG_FINAL_PACKET(mb, 1);

	try {
		_session->requestRecvBuffer();
		_session->requestSendBuffer(mb.buf, mb.len, true/*Raw*/);
	}catch (exception& e) {
		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 1))
			throw;
	}
}

void gg_auth_server::onDisconnected(session* _session) {

	player *_player = reinterpret_cast< player* >(_session);

	_smp::message_pool::getInstance().push(new message("[gg_auth_server::onDisconnected][Log] Player Desconectou. ID: " + std::string(_player->m_pi.id) + "  UID: " + std::to_string(_player->m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

	// Aqui n�o faz nada, no login server por enquanto
}

void gg_auth_server::onHeartBeat() {
	
	// Faz nada por enquanto
}

void gg_auth_server::onStart() {

	try {

		// Start Unit Connect for Try Connection with Auth Server
		if (m_unit_connect != nullptr)
			m_unit_connect->start();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::onStart][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool gg_auth_server::checkCommand(std::stringstream& _command) {
	
	std::string s = "";

	_command >> s;

	if (!s.empty() && s.compare("exit") == 0)
		return true;	// Sai
	else if (!s.empty() && s.compare("reload_files") == 0) {
		reload_files();
		_smp::message_pool::getInstance().push(new message("Game Guard Auth Server files has been reloaded.", CL_FILE_LOG_AND_CONSOLE));
	}else if (!s.empty() && s.compare("reload_socket_config") == 0) {

		// Ler novamento o arquivo de configura��o do socket
		if (m_accept_sock != nullptr)
			m_accept_sock->reload_config_file();
		else
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::checkCommand][WARNING] m_accept_sock(socket que gerencia os socket que pode aceitar etc) is invalid.", CL_FILE_LOG_AND_CONSOLE));

	}else if (!s.empty() && s.compare("snapshot") == 0) {

		try {
			int *bad_ptr_snapshot = nullptr;
			*bad_ptr_snapshot = 2;
		}catch (exception& e) {
			UNREFERENCED_PARAMETER(e);

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::checkCommand][Log] Snapshot comando executado.", CL_FILE_LOG_AND_CONSOLE));
		}

	}else
		_smp::message_pool::getInstance().push(new message("Unknown Command: " + s, CL_ONLY_CONSOLE));

	return false;
}

bool gg_auth_server::checkPacket(session& _session, packet* _packet) {
	
	///--------------- INICIO CHECK PACKET SESSION
	if (/*(std::clock() - _session.m_check_packet.tick) <= CHK_PCKT_INTERVAL_LIMIT /* Dentro do Interval */
		/*&& _session.m_check_packet.packet_id == _packet->getTipo() /* Mesmo pacote */
		/*&& _session.m_check_packet.count++ >= CHK_PCKT_COUNT_LIMIT /* deu o limit de count DERRUBA a session */
		_session.m_check_packet.checkPacketId(_packet->getTipo()) && _session.m_check_packet.incrementCount() >= CHK_PCKT_COUNT_LIMIT) {

		_smp::message_pool::getInstance().push(new message("[WARNING] Tentativa de DDoS ataque com pacote ID: (0x" + hex_util::lltoaToHex(_packet->getTipo()) + ") " + std::to_string(_packet->getTipo()) + ". IP=" + std::string(_session.m_ip), CL_FILE_LOG_AND_CONSOLE));

		DisconnectSession(&_session);

		return false;
	}
	///--------------- FIM CHECK PACKET SESSION

	return true;
}

void gg_auth_server::shutdown_time(int32_t _time_sec) {

	if (_time_sec <= 0)	// Desliga o Server Imediatemente
		shutdown();
	else {

		job _job(unit::end_time_shutdown, this, (void*)0);

		// Se o Shutdown Timer estiver criado descria e cria um novo
		if (m_shutdown != nullptr) {

			// Para o Tempo se ele n�o estiver parado
			if (m_shutdown->getState() != timer::STOPPED)
				m_shutdown->stop();

			m_timer_mgr.deleteTimer(m_shutdown);
		}

		m_shutdown = nullptr;

		if ((m_shutdown = m_timer_mgr.createTimer(_time_sec * 1000, new (timer::timer_param){ _job, m_job_pool })) == nullptr)
			throw exception("[gg_auth_server::shutdown_time][Error] nao conseguiu criar o timer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 51, 0));
	}
}

void gg_auth_server::config_init() {

	// Server Tipo
	m_si.tipo = 6/*Game Guard Auth Server*/;
}

void gg_auth_server::reload_files() {

	unit::config_init();
	config_init();
}

void gg_auth_server::destroy_unit() {

	if (m_unit_connect != nullptr)
		delete m_unit_connect;

	m_unit_connect = nullptr;
}

void gg_auth_server::authCmdShutdown(int32_t _time_sec) {

	try {

		// Shut down com tempo
		if (m_shutdown == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdShutdown][Log] Auth Server requisitou para o server ser desligado em "
					+ std::to_string(_time_sec) + " segundos", CL_FILE_LOG_AND_CONSOLE));

			shutdown_time(_time_sec);

		}else
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdShutdown][WARNING] Auth Server requisitou para o server ser delisgado em "
					+ std::to_string(_time_sec) + " segundos, mas o server ja esta com o timer de shutdown", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdShutdown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::authCmdBroadcastNotice(std::string /*_notice*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdBroadcastTicker(std::string /*_nickname*/, std::string /*_msg*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdBroadcastCubeWinRare(std::string /*_msg*/, uint32_t /*_option*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) {

	// Aqui no Login Server Ainda n�o usa o force, ele desconecta o player do mesmo jeito
	UNREFERENCED_PARAMETER(_force);

	try {

		auto s = m_player_manager.findPlayer(_player_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdDisconnectPlayer][log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(s->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Deconecta o Player
			DisconnectSession(s);

			// UPDATE ON Auth Server
			m_unit_connect->sendConfirmDisconnectPlayer(_req_server_uid, _player_uid);

		}else
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdDisconnectPlayer][WARNING] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(_player_uid) + "], mas nao encontrou ele no server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::authCmdConfirmDisconnectPlayer(uint32_t /*_player_uid*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdNewMailArrivedMailBox(uint32_t /*_player_uid*/, uint32_t /*_mail_id*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdNewRate(uint32_t /*_tipo*/, uint32_t /*_qntd*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdReloadGlobalSystem(uint32_t /*_tipo*/) {
	// Game Guard Auth n�o usa esse Comando
	return;
}

void gg_auth_server::authCmdInfoPlayerOnline(uint32_t _req_server_uid, uint32_t _player_uid) {

	try {

		auto s = m_session_manager.findSessionByUID(_player_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdInfoPlayerOnline][log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu o info do Player[UID=" + std::to_string(s->getUID()) + "]", CL_FILE_LOG_AND_CONSOLE));

			AuthServerPlayerInfo aspi(s->getUID(), s->getID(), s->getIP());

			// UPDATE ON Auth Server
			m_unit_connect->sendInfoPlayerOnline(_req_server_uid, aspi);

		}else {

			// Log
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdInfoPlayerOnline][Log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu info do Player[UID=" + std::to_string(_player_uid) + "], mas nao encontrou ele no server.", CL_FILE_LOG_AND_CONSOLE));

			// UPDATE ON Auth Server
			m_unit_connect->sendInfoPlayerOnline(_req_server_uid, AuthServerPlayerInfo(_player_uid));
		}

	}catch (exception& e) {

		// UPDATE ON Auth Server - Error reply
		m_unit_connect->sendInfoPlayerOnline(_req_server_uid, AuthServerPlayerInfo(_player_uid));

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) {

	// Game Guard Auth Server ainda n�o usa esse funcionalidade de pedir o info do player que est� conectado em outro server
	UNREFERENCED_PARAMETER(_req_server_uid);

	try {

		auto s = m_player_manager.findPlayer(_aspi.uid);

		if (s != nullptr) {

			//confirmLoginOnOtherServer(*s, _req_server_uid, _aspi);

		}else
			_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdConfirmSendInfoPlayerOnline][WARNING] Player[UID=" + std::to_string(_aspi.uid) 
					+ "] retorno do confirma login com Auth Server do Server[UID=" + std::to_string(_req_server_uid) + "], mas o palyer nao esta mais conectado.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdConfirmSendInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::authCmdSendCommandToOtherServer(packet& _packet) {

	try {

		func_arr::func_arr_ex* func = nullptr;

		unsigned long req_server_uid = _packet.readUint32();
		unsigned short command_id = _packet.readUint16();

		try {
		
			func = packet_func_base::funcs_as.getPacketCall(command_id);

			if (func != nullptr && func->execCmd(&_packet))
				throw exception("[gg_auth_server::authCmdSendCommandToOtherServer][Error] Ao tratar o Comando. ID: " + std::to_string(command_id)
						+ "(0x" + hex_util::ltoaToHex(command_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GG_AUTH_SERVER, 5000, 0));
	
		}catch (exception& e) {

			if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
				_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendCommandToOtherServer][Log] Comando ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: " 
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendCommandToOtherServer][Log] Comando ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: "
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_ONLY_FILE_LOG));
	#endif
			}else
				throw;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::authCmdSendReplyToOtherServer(packet& _packet) {

	try {

		func_arr::func_arr_ex* func = nullptr;

		unsigned long req_server_uid = _packet.readUint32();
		unsigned short command_id = _packet.readUint16();

		try {
		
			func = packet_func_base::funcs_as.getPacketCall(command_id);

			if (func != nullptr && func->execCmd(&_packet))
				throw exception("[gg_auth_server::authCmdSendReplyToOtherServer][Error] Ao tratar o Comando. ID: " + std::to_string(command_id)
						+ "(0x" + hex_util::ltoaToHex(command_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GG_AUTH_SERVER, 5001, 0));
	
		}catch (exception& e) {

			if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
				_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendReplyToOtherServer][Log] Reply ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: " 
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendReplyToOtherServer][Log] Reply ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: "
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_ONLY_FILE_LOG));
	#endif
			}else
				throw;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::authCmdSendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::sendCommandToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) {

	try {

		// Envia o comando para o outro server com o Auth Server
		m_unit_connect->sendCommandToOtherServer(_send_server_uid_or_type, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::sendCommandToOtherServerWithAuthServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void gg_auth_server::sendReplyToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) {

	try {

		// Envia a resposta para o outro server com o Auth Server
		m_unit_connect->sendReplyToOtherServer(_send_server_uid_or_type, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[gg_auth_server::sendReplyToOtherServerWithAuthServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}
