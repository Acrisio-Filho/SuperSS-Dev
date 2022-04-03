// Arquivo login_server.cpp
// Criado em 18/12/2017 por Acrisio
// Implementa��o da classe login_server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <MSWSock.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "login_server.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/SOCKET/socketserver.h"
#include "../../Projeto IOCP/SOCKET/socket.h"
#include "../PACKET/packet_func_ls.h"

#include "../../Projeto IOCP/PANGYA_DB/cmd_register_logon.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_id.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_pass.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_logon_check.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_insert_block_ip.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_insert_block_mac.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_login_info.hpp"

#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_verify_ip.hpp"
#include "../PANGYA_DB/cmd_first_login_check.hpp"
#include "../PANGYA_DB/cmd_first_set_check.hpp"
#include "../PANGYA_DB/cmd_create_user.hpp"

#include "../../Projeto IOCP/UTIL/md5.hpp"

#include <regex>

#if defined(_WIN32)
#include <WS2tcpip.h>
#elif defined(__linux__)
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif

#define REQUEST_BEGIN(method) if (!_session.getState()) \
									throw exception("[login_server::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1, 0)); \
							  if (_packet == nullptr) \
									throw exception("[login_server::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 6, 0)); \

#define FIRST_LOGIN(_session)	(_session)->m_pi.m_state = 2; \
								packet_func::pacote00F(p, (_session)); \
								packet_func::session_send(p, (_session), 1); \
								packet_func::pacote001(p, (_session), 0xD8); \
								packet_func::session_send(p, (_session), 1); \

#define FIRST_SET(_session) (_session)->m_pi.m_state = 3; \
							packet_func::pacote00F(p, (_session)); \
							packet_func::session_send(p, (_session), 1);	\
							packet_func::pacote001(p, (_session), 0xD9); \
							packet_func::session_send(p, (_session), 1); \

#define SUCCESS_LOGIN(_from, __arg1, _session) \
	{ \
		(_session)->m_pi.m_state = 1; \
		_smp::message_pool::getInstance().push(new message("[login_server::" + std::string((_from)) + "][Log] Player logou. [ID=" + std::string((_session)->m_pi.id) + ", UID=" + std::to_string((_session)->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE)); \
 \
		packet_func::succes_login((__arg1), (_session)); \
	}

// Verifica se session está autorizada para executar esse ação, 
// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!_session.m_is_authorized) \
												throw exception("[login_server::request" + std::string((method)) + "][Error] Player[UID=" + std::to_string(_session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1, 0x5000501)); \


using namespace stdA;

login_server::login_server() : server(m_player_manager, 2, 8, 4), m_player_manager(*this, m_si.max_user),
		m_access_flag(false), m_create_user_flag(false), m_same_id_login_flag(false) {
	
	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[login_server::login_server][Error] Ao iniciar o login server.\n", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		if (!sIff::getInstance().isLoad())
			sIff::getInstance().load();

		config_init();

		packet_func::funcs.addPacketCall(0x01, packet_func::packet001, this);
		packet_func::funcs.addPacketCall(0x03, packet_func::packet003, this);
		packet_func::funcs.addPacketCall(0x04, packet_func::packet004, this);
		packet_func::funcs.addPacketCall(0x06, packet_func::packet006, this);
		packet_func::funcs.addPacketCall(0x07, packet_func::packet007, this);
		packet_func::funcs.addPacketCall(0x08, packet_func::packet008, this);
		packet_func::funcs.addPacketCall(0x0B, packet_func::packet00B, this);

		packet_func::funcs_sv.addPacketCall(0x00, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x01, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x02, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x03, packet_func::packet_sv003, this);
		packet_func::funcs_sv.addPacketCall(0x06, packet_func::packet_sv006, this);
		packet_func::funcs_sv.addPacketCall(0x09, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0E, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0F, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x10, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x11, packet_func::packet_svFazNada, this);

		// Auth Server
		packet_func::funcs_as.addPacketCall(0x01, packet_func::packet_as001, this);

		// Initialized complete
		m_state = INITIALIZED;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::login_server][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;

		// não posso lança exception para fora do contrutor que eles nao chama o destrutor dele antes
	}
};

login_server::~login_server() {

	// Crucial esse aqui para terminar o server sem crashs
	waitAllThreadFinish(INFINITE);

};

bool login_server::getAccessFlag() {
	return m_access_flag;
};

bool login_server::getCreateUserFlag() {
	return m_create_user_flag;
};

bool login_server::canSameIDLogin() {
	return m_same_id_login_flag;
};

void login_server::requestDownPlayerOnGameServer(player& _session, packet *_packet) {
	REQUEST_BEGIN("DownPlayerOnGameServer");

	packet p;

	try {

		// Verifica se session está autorizada para executar esse ação, 
		// se ele não fez o login com o Server ele não pode fazer nada até que ele faça o login
		CHECK_SESSION_IS_AUTHORIZED("DownPlayerOnGameServer");

		// Derruba o player que está logado no game server
		// Se o Auth Server Estiver ligado manda por ele, se não tira pelo banco de dados mesmo
		if (m_unit_connect->isLive()) {

			// [Auth Server] -> Game Server UID = _session.m_pi.m_server_uid;
			m_unit_connect->sendDisconnectPlayer(_session.m_pi.m_server_uid, _session.m_pi.uid);

		}else {
			
			// Auth Server não está online, resolver por aqui mesmo
			CmdRegisterLogon cmd_rl(_session.m_pi.uid, 1/*Option 1 desloga no banco de dados*/, true/*Waiter*/);

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_rl, nullptr, nullptr);

			cmd_rl.waitEvent();

			if (cmd_rl.getException().getCodeError() != 0)
				throw cmd_rl.getException();

			// Loga com sucesso
			packet_func::succes_login(this, &_session);

			_smp::message_pool::getInstance().push(new message("[login_server::requestDownPlayerOnGameServer][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ ", ID=" + std::string(_session.m_pi.id) + "] derrubou o outro do game server[UID=" 
					+ std::to_string(_session.m_pi.m_server_uid) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::requestDownPlayerOnGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Fail Login
		packet_func::pacote00E(p, &_session, L"", 12, (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::LOGIN_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 500053);

		packet_func::session_send(p, &_session, 1);
	}
};

void login_server::authCmdShutdown(int32_t _time_sec) {

	try {

		// Shut down com tempo
		if (m_shutdown == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[login_server::authCmdShutdown][Log] Auth Server requisitou para o server ser desligado em "
					+ std::to_string(_time_sec) + " segundos", CL_FILE_LOG_AND_CONSOLE));

			shutdown_time(_time_sec);

		}else
			_smp::message_pool::getInstance().push(new message("[login_server::authCmdShutdown][WARNING] Auth Server requisitou para o server ser delisgado em "
					+ std::to_string(_time_sec) + " segundos, mas o server ja esta com o timer de shutdown", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::authCmdShutdown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void login_server::authCmdBroadcastNotice(std::string /*_notice*/) {
	// Login Server não usa esse Comando
	return;
};

void login_server::authCmdBroadcastTicker(std::string /*_nickname*/, std::string /*_msg*/) {
	// Login Server não usa esse Comando
	return;
};

void login_server::authCmdBroadcastCubeWinRare(std::string /*_msg*/, uint32_t /*_option*/) {
	// Login Server não usa esse Comando
	return;
};

void login_server::authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) {
	
	// Aqui no Login Server Ainda não usa o force, ele desconecta o player do mesmo jeito
	UNREFERENCED_PARAMETER(_force);

	try {

		auto s = m_player_manager.findPlayer(_player_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[login_server::authCmdDisconnectPlayer][log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(s->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Deconecta o Player
			DisconnectSession(s);

			// UPDATE ON Auth Server
			m_unit_connect->sendConfirmDisconnectPlayer(_req_server_uid, _player_uid);

		}else
			_smp::message_pool::getInstance().push(new message("[login_server::authCmdDisconnectPlayer][WARNING] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(_player_uid) + "], mas nao encontrou ele no server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::authCmdDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void login_server::authCmdConfirmDisconnectPlayer(uint32_t _player_uid) {

	try {

		auto s = m_player_manager.findPlayer(_player_uid);

		if (s != nullptr) {

			// Loga com sucesso
			packet_func::succes_login(this, s);

			_smp::message_pool::getInstance().push(new message("[login_server::authCmdConfirmDisconnectPlayer][Log] Player[UID=" + std::to_string(s->m_pi.uid)
					+ ", ID=" + std::string(s->m_pi.id) + "] derrubou o outro do game server[UID="
					+ std::to_string(s->m_pi.m_server_uid) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		}else
			_smp::message_pool::getInstance().push(new message("[login_server::authCmdConfirmDisconnectPlayer][WARNING] Player[UID=" + std::to_string(_player_uid)
					+ "], nao esta mais conectado com o Login Server para enviar a resposta do desconecta player do game server. Bug", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::authCmdConfirmDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void login_server::authCmdNewMailArrivedMailBox(uint32_t /*_player_uid*/, uint32_t /*_mail_id*/) {
	// Login Server não usa esse Comando
	return;
};

void login_server::authCmdNewRate(uint32_t /*_tipo*/, uint32_t /*_qntd*/) {
	// Login Server não usa esse Comando
	return;
};

void login_server::authCmdReloadGlobalSystem(uint32_t _tipo) {
	// Login Server não usa esse Comando
	return;
}

void login_server::authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) {

	// Login Server ainda não usa esse funcionalidade de pedir o info do player que está conectado em outro server
	UNREFERENCED_PARAMETER(_req_server_uid);

	try {

		auto s = m_player_manager.findPlayer(_aspi.uid);

		if (s != nullptr) {

			//confirmLoginOnOtherServer(*s, _req_server_uid, _aspi);

		}else
			_smp::message_pool::getInstance().push(new message("[login_server::authCmdConfirmSendInfoPlayerOnline][WARNING] Player[UID=" + std::to_string(_aspi.uid) 
					+ "] retorno do confirma login com Auth Server do Server[UID=" + std::to_string(_req_server_uid) + "], mas o palyer nao esta mais conectado.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::authCmdConfirmSendInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void login_server::requestLogin(player& _session, packet *_packet) {
	REQUEST_BEGIN("Login");

	/// Pacote01 Option 0x0F(15) é manutenção
	packet p;

	try {

		// Ler dados do packet de login
		std::string id = _packet->readString();
		std::string password = _packet->readString();
		
		// Option Flag Count
		unsigned char opt_count = _packet->readInt8();

		std::vector< uint64_t > v_opt_unkn;

		for (auto i = 0u; i < opt_count; ++i)
			v_opt_unkn.push_back(_packet->readUint64());

		// MAC Address
		std::string mac_address = _packet->readString();

		//  Verify Id is valid
		if (id.size() < 2 || std::regex_match(id.begin(), id.end(), std::regex(".*[\\^$&,\\?`´~\\|\"@#¨'%*!\\\\].*")))
			throw exception("[login_server::requestLogin][Error] ID(" + id 
					+ ") invalid, less then 2 characters or invalid character include in id.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 252, 0));

		// Password to MD5
		std::string pass_md5 = password;

		try {
						
			md5::processData((unsigned char*)pass_md5.data(), (uint32_t)pass_md5.length());

			pass_md5 = md5::getHash();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			// Relança
			throw;
		}

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("ID : " + id, CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("Senha: " + pass_md5, CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("Option Count : " + std::to_string((unsigned short)opt_count), CL_FILE_LOG_AND_CONSOLE));

		for (auto& el : v_opt_unkn)
			_smp::message_pool::getInstance().push(new message("Option Unknown 8 Bytes : 0x" + hex_util::lltoaToHex((int64_t)el), CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("Mac Address : " + mac_address, CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("ID : " + id, CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("Senha: " + pass_md5, CL_ONLY_FILE_LOG));

		_smp::message_pool::getInstance().push(new message("Option Count : " + std::to_string((unsigned short)opt_count), CL_ONLY_FILE_LOG));

		for (auto& el : v_opt_unkn)
			_smp::message_pool::getInstance().push(new message("Option Unknown 8 Bytes : 0x" + hex_util::lltoaToHex((int64_t)el), CL_ONLY_FILE_LOG));

		_smp::message_pool::getInstance().push(new message("Mac Address : " + mac_address, CL_ONLY_FILE_LOG));
#endif // _DEBUG

		// Termina a parte de ler o dados do packet de login

		_session.m_tick_bot = std::clock();

		if (!haveBanList(_session.m_ip, mac_address)) {	// Verifica se está na list de ips banidos

			CmdVerifyID cmd_verifyId(id, true/*Waiter*/); // ID

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_verifyId, nullptr, nullptr);

			cmd_verifyId.waitEvent();

			if (cmd_verifyId.getException().getCodeError() != 0)
				throw cmd_verifyId.getException();

			if (cmd_verifyId.getUID() > 0) {	// Verifica se o ID existe

				CmdVerifyPass cmd_verifyPass(cmd_verifyId.getUID(), pass_md5, true/*Waiter*/); // PASSWORD

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_verifyPass, nullptr, nullptr);

				cmd_verifyPass.waitEvent();

				if (cmd_verifyPass.getException().getCodeError() != 0)
					throw cmd_verifyPass.getException();

				if (cmd_verifyPass.getLastVerify()) {	// Verifica se a senha bate com a do banco de dados

					CmdPlayerInfo cmd_pi(cmd_verifyId.getUID(), true/*Waiter*/);

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_pi, nullptr, nullptr);

					cmd_pi.waitEvent();

					if (cmd_pi.getException().getCodeError() != 0)
						throw cmd_pi.getException();

					*((player_info*)&_session.m_pi) = cmd_pi.getInfo();
					PlayerInfo& pi = _session.m_pi;

					CmdLogonCheck cmd_lc(pi.uid, true/*Waiter*/);
					CmdFirstLoginCheck cmd_flc(pi.uid, true/*Waiter*/);
					CmdFirstSetCheck cmd_fsc(pi.uid, true/*Waiter*/);

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_lc, nullptr, nullptr);
					snmdb::NormalManagerDB::getInstance().add(0, &cmd_flc, nullptr, nullptr);
					snmdb::NormalManagerDB::getInstance().add(0, &cmd_fsc, nullptr, nullptr);

					cmd_lc.waitEvent();

					if (cmd_lc.getException().getCodeError() != 0)
						throw cmd_lc.getException();

					cmd_flc.waitEvent();

					if (cmd_flc.getException().getCodeError() != 0)
						throw cmd_flc.getException();

					cmd_fsc.waitEvent();

					if (cmd_fsc.getException().getCodeError() != 0)
						throw cmd_fsc.getException();

					// Verifica se tem o mesmo player logado com outro socket
					session *player_logado = HasLoggedWithOuterSocket(_session);

					if (!canSameIDLogin() && player_logado != nullptr) {	// Verifica se ja nao esta logado

						p.init_plain((unsigned short)0x01);

						p.addUint8(0xE2);
						p.addInt32(5100107);

						packet_func::session_send(p, &_session, 0);

						// Disconnect
#if defined(_WIN32)
						::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
						::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

						_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] player[UID=" 
								+ std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + ", IP=" + _session.getIP() + "] ja tem outro Player conectado[UID=" + std::to_string(player_logado->getUID())
								+ ", OID=" + std::to_string(player_logado->m_oid) + ", IP=" + player_logado->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

					}else if (pi.m_state) {	// Verifica se já pediu para logar

						p.init_plain((unsigned short)0x01);

						p.addUint8(0xE2);
						p.addInt32(500010);	// Já esta logado, ja enviei o pacote de logar

						packet_func::session_send(p, &_session, 0);

						if (pi.m_state++ >= 3)	// Ataque, derruba a conexão maliciosa
#if defined(_WIN32)
							::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
							::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

						_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Player ja esta logado, o pacote de logar ja foi enviado, player[UID="
								+ std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

					}else {

						CmdVerifyIP cmd_vi(pi.uid, _session.m_ip, true/*Waiter*/);

						snmdb::NormalManagerDB::getInstance().add(0, &cmd_vi, nullptr, nullptr);

						cmd_vi.waitEvent();

						if (cmd_vi.getException().getCodeError() != 0)
							throw cmd_vi.getException();

						if (!(pi.m_cap & 4) && getAccessFlag() && !cmd_vi.getLastVerify()) {	// Verifica se tem permição para acessar

							p.init_plain((unsigned short)0x01);

							p.addUint8(0xE2);
							p.addInt32(500015);	// Acesso restrito

							packet_func::session_send(p, &_session, 0);

							// Disconnect
#if defined(_WIN32)
							::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
							::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

							_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] acesso restrito para o player [UID=" + std::to_string(pi.uid)
									+ ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

						}else if (pi.block_flag.m_id_state.id_state.ull_IDState != 0) {	// Verifica se está bloqueado

							if (pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_TEMPORARY && (pi.block_flag.m_id_state.block_time == -1 || pi.block_flag.m_id_state.block_time > 0)) {

								auto tempo = pi.block_flag.m_id_state.block_time / 60 / 60/*Hora*/; // Hora

								p.init_plain((unsigned short)0x01);

								p.addUint8(7);
								p.addInt32(pi.block_flag.m_id_state.block_time == -1 || tempo == 0 ? 1/*Menos de uma hora*/ : tempo);	// Block Por Tempo

								// Aqui pode ter uma string com mensagem que o pangya exibe
								//p.addString("ola");

								packet_func::session_send(p, &_session, 0);

								// Disconnect
#if defined(_WIN32)
								::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
								::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Bloqueado por tempo[Time="
										+ (pi.block_flag.m_id_state.block_time == -1 ? std::string("indeterminado") : (std::to_string(pi.block_flag.m_id_state.block_time / 60) 
										+ "min " + std::to_string(pi.block_flag.m_id_state.block_time % 60) + "sec")) 
										+ "]. player [UID=" + std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

							}else if (pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_FOREVER) {

								p.init_plain((unsigned short)0x01);

								p.addUint8(0x0c);		// Acho que seja block permanente, que fala de email
								//p->addInt32(500012);	// Block Permanente

								packet_func::session_send(p, &_session, 0);

								// Disconnect
#if defined(_WIN32)
								::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
								::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Bloqueado permanente. player [UID=" + std::to_string(pi.uid)
										+ ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

							}else if (pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_ALL_IP) {

								// Bloquea todos os IP que o player logar e da error de que a area dele foi bloqueada

								// Add o ip do player para a lista de ip banidos
								snmdb::NormalManagerDB::getInstance().add(1, new CmdInsertBlockIP(_session.m_ip, "255.255.255.255"), login_server::SQLDBResponse, this);

								// Resposta
								p.init_plain((unsigned short)0x01);

								p.addUint8(16);
								p.addInt32(500012);		// Ban por Região;

								packet_func::session_send(p, &_session, 0);

								// Disconnect
#if defined(_WIN32)
								::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
								::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
										+ ", IP=" + std::string(_session.m_ip) + "] Block ALL IP que o player fizer login.", CL_FILE_LOG_AND_CONSOLE));

							}else if (pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_MAC_ADDRESS) {

								// Bloquea o MAC Address que o player logar e da error de que a area dele foi bloqueada

								// Add o MAC Address do player para a lista de MAC Address banidos
								snmdb::NormalManagerDB::getInstance().add(2, new CmdInsertBlockMAC(mac_address), login_server::SQLDBResponse, this);

								// Resposta
								p.init_plain((unsigned short)0x01);

								p.addUint8(16);
								p.addInt32(500012);		// Ban por Região;

								packet_func::session_send(p, &_session, 0);

								// Disconnect
#if defined(_WIN32)
								::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
								::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
										+ ", IP=" + std::string(_session.m_ip) + ", MAC=" + mac_address + "] Block MAC Address que o player fizer login.", CL_FILE_LOG_AND_CONSOLE));

							}else if (!cmd_flc.getLastCheck()) {	// Verifica se fez o primeiro login
							
								// Authorized a ficar online no server por tempo indeterminado
								_session.m_is_authorized = 1u;

								FIRST_LOGIN(&_session);

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Primeira vez que o player loga. player[UID=" + std::to_string(pi.uid)
										+ ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

							}else if (!cmd_fsc.getLastCheck()) {	// Verifica se fez o primeiro set do character
							
								// Authorized a ficar online no server por tempo indeterminado
								_session.m_is_authorized = 1u;

								FIRST_SET(&_session);

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Primeira vez que o player escolhe um character padrao. player[UID="
										+ std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

							}else if (cmd_lc.getLastCheck()) {	// Verifica se já esta logado no game server
						
								// Pega o Server UID para usar depois no packet004, para derrubar do server
								_session.m_pi.m_server_uid = cmd_lc.getServerUID();

								// Já está autorizado a ficar online, o login server só vai derrubar o outro que está online no game server
								// Authorized a ficar online no server por tempo indeterminado
								_session.m_is_authorized = 1u;

								p.init_plain((unsigned short)0x01);

								p.addUint8(4);

								packet_func::session_send(p, &_session, 0);

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Player ja esta logado no game server. player[UID="
										+ std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

							}else if (pi.m_cap & 4) {	// Acesso permtido
						
								// Authorized a ficar online no server por tempo indeterminado
								_session.m_is_authorized = 1u;

								SUCCESS_LOGIN("requestLogin", this, &_session);

								_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] GM logou[UID=" + std::to_string(pi.uid)
										+ ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

							}else {
								
								// Authorized a ficar online no server por tempo indeterminado
								_session.m_is_authorized = 1u;

								SUCCESS_LOGIN("requestLogin", this, &_session);
							}

						}else if (!cmd_flc.getLastCheck()) {	// Verifica se fez o primeiro login
							
							// Authorized a ficar online no server por tempo indeterminado
							_session.m_is_authorized = 1u;
							
							FIRST_LOGIN(&_session);

							_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Primeira vez que o player loga. player[UID=" + std::to_string(pi.uid)
									+ ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

						}else if (!cmd_fsc.getLastCheck()) {	// Verifica se fez o primeiro set do character
							
							// Authorized a ficar online no server por tempo indeterminado
							_session.m_is_authorized = 1u;

							FIRST_SET(&_session);

							_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Primeira vez que o player escolhe um character padrao. player[UID="
									+ std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

						}else if (cmd_lc.getLastCheck()) {	// Verifica se já esta logado no game server

							// Pega o Server UID para usar depois no packet004, para derrubar do server
							_session.m_pi.m_server_uid = cmd_lc.getServerUID();

							// Já está autorizado a ficar online, o login server só vai derrubar o outro que está online no game server
							// Authorized a ficar online no server por tempo indeterminado
							_session.m_is_authorized = 1u;

							p.init_plain((unsigned short)0x01);

							p.addUint8(4);

							packet_func::session_send(p, &_session, 0);

							_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Player ja esta logado no game server. player[UID="
									+ std::to_string(pi.uid) + ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

						}else if (pi.m_cap & 4) {	// Acesso permtido
						
							// Authorized a ficar online no server por tempo indeterminado
							_session.m_is_authorized = 1u;

							SUCCESS_LOGIN("requestLogin", this, &_session);

							_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] GM logou[UID=" + std::to_string(pi.uid)
									+ ", ID=" + std::string(pi.id) + "]", CL_FILE_LOG_AND_CONSOLE));

						}else {

							// Authorized a ficar online no server por tempo indeterminado
							_session.m_is_authorized = 1u;

							SUCCESS_LOGIN("requestLogin", this, &_session);
						}
					}

				}else {

					packet_func::pacote001(p, &_session, 6/* ID ou PW errado*/);
					packet_func::session_send(p, &_session, 1);	// Erro pass

					// Disconnect
#if defined(_WIN32)
					::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
					::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

					_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] senha errada. ID: " + cmd_verifyId.getID()
							+ "  senha: " + pass_md5/*cmd_verifyPass.getPass()*/, CL_FILE_LOG_AND_CONSOLE));
				}

			}else if (!getAccessFlag() && getCreateUserFlag()) {

				// Authorized a ficar online no server por tempo indeterminado
				_session.m_is_authorized = 1u;

				_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Criando um novo usuario[ID=" + cmd_verifyId.getID()
						+ ", PASSWORD=" + pass_md5/*pass*/ + "]", CL_FILE_LOG_AND_CONSOLE));

				auto ip = std::string(_session.getIP());

				CmdCreateUser cmd_cu(cmd_verifyId.getID(), password, ip, getUID(), true/*Waiter*/);

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_cu, nullptr, nullptr);

				cmd_cu.waitEvent();

				if (cmd_cu.getException().getCodeError() != 0)
					throw cmd_cu.getException();

				auto& pi = _session.m_pi;

				pi.uid = cmd_cu.getUID();

				CmdPlayerInfo cmd_pi(pi.uid, true/*Waiter*/);

				snmdb::NormalManagerDB::getInstance().add(0, &cmd_pi, nullptr, nullptr);

				cmd_pi.waitEvent();

				if (cmd_pi.getException().getCodeError() != 0)
					throw cmd_pi.getException();

				*((player_info*)&pi) = cmd_pi.getInfo();

				FIRST_LOGIN(&_session);

				// Log
				_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Conta Criada com sucesso. Player[UID=" + std::to_string(pi.uid)
						+ ", ID=" + pi.id + ", PASSWORD=" + pass_md5/*pi.pass*/ + "]", CL_FILE_LOG_AND_CONSOLE));

			}else {

				packet_func::pacote001(p, &_session, 6/*ID é 2, 6 é o ID ou pw errado*/);
				packet_func::session_send(p, &_session, 1);	// Erro login

				// Disconnect
#if defined(_WIN32)
				::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
				::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

				_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] ID nao existe, ID: " + cmd_verifyId.getID(), CL_FILE_LOG_AND_CONSOLE));
			}

		}else {	// Ban IP/MAC por região

			p.init_plain((unsigned short)0x01);

			p.addUint8(16);
			p.addInt32(500012);		// Ban por Região;

			packet_func::session_send(p, &_session, 0);

			// Disconnect
#if defined(_WIN32)
			::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
			::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

			_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][Log] Block por Regiao o IP/MAC: " + std::string(_session.m_ip) + "/" + mac_address, CL_FILE_LOG_AND_CONSOLE));
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::requestLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::LOGIN_SERVER, 252/*Invalid ID*/)) {

			// Invalid ID
			packet_func::pacote001(p, &_session, 2/*Invlid ID*/);
			packet_func::session_send(p, &_session, 1);

		}else {

			// Unknown Error (System Fail)
			p.init_plain((unsigned short)0x01);

			p.addUint8(0xE2);
			p.addInt32(500050);		// System Error

			packet_func::session_send(p, &_session, 0);
		}

		// Disconnect
#if defined(_WIN32)
		::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
		::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif
	}
};

void login_server::requestReLogin(player& _session, packet *_packet) {
	REQUEST_BEGIN("ReLogin");

	packet p;

	try {

		std::string id = _packet->readString();
		uint32_t server_uid = _packet->readUint32();
		std::string  auth_key_login = _packet->readString();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] ID: " + id, CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] UID: " + std::to_string(server_uid), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] Auth Key Login: " + auth_key_login, CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] ID: " + id, CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] UID: " + std::to_string(server_uid), CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] Auth Key Login: " + auth_key_login, CL_ONLY_FILE_LOG));
#endif

		CmdVerifyID cmd_verifyId(id, true/*Waiter*/); // ID

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_verifyId, nullptr, nullptr);

		cmd_verifyId.waitEvent();

		if (cmd_verifyId.getException().getCodeError() != 0)
			throw cmd_verifyId.getException();

		if (cmd_verifyId.getUID() <= 0)	// Verifica se o ID existe
			throw exception("[login_server::requestReLogin][Error] Player[ID=" + id + "] not found. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1035, 0));

		CmdPlayerInfo cmd_pi(cmd_verifyId.getUID(), true/*Waiter*/);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_pi, nullptr, nullptr);

		cmd_pi.waitEvent();

		if (cmd_pi.getException().getCodeError() != 0)
			throw cmd_pi.getException();

		*((player_info*)&_session.m_pi) = cmd_pi.getInfo();

		if (id.compare(_session.m_pi.id) != 0)
			throw exception("[login_server::requestReLogin][Error] id nao eh igual ao da session[PlayerUID: " + std::to_string(_session.m_pi.uid) + "] { SESSION_ID=" 
					+ std::string(_session.m_pi.id) + ", REQUEST_ID=" + id + " } no match", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1025, 0));

		CmdAuthKeyLoginInfo cmd_akli(_session.m_pi.uid, true/*Waiter*/);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_akli, nullptr, nullptr);

		cmd_akli.waitEvent();

		if (cmd_akli.getException().getCodeError() != 0)
			throw cmd_akli.getException();

		auto& akli = cmd_akli.getInfo();

		if (auth_key_login.compare(akli.key) != 0)
			throw exception("[login_server::requestReLogin][Error] auth login server nao eh igual a do banco de dados da session[PlayerUID: " 
					+ std::to_string(_session.m_pi.uid) + "] AuthKeyLogin: " + std::string(akli.key) + " != " 
					+ auth_key_login, STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1026, 0));

		// Verifica se ele pode logar de novo, verifica as flag do login server
		if (haveBanList(_session.m_ip, "", false/*Não verifica o MAC Address*/))	// Verifica se está na list de ips banidos
			throw exception("[login_server::requestReLogin][Error] auth login server, o player[UID=" 
					+ std::to_string(_session.m_pi.uid) + "] esta na lista de ip banidos.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1027, 0));
		
		CmdVerifyIP cmd_vi(_session.m_pi.uid, _session.m_ip, true/*Waiter*/);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_vi, nullptr, nullptr);

		cmd_vi.waitEvent();

		if (cmd_vi.getException().getCodeError() != 0)
			throw cmd_vi.getException();

		if (!(_session.m_pi.m_cap & 4) && getAccessFlag() && !cmd_vi.getLastVerify()) {	// Verifica se tem permição para acessar

			throw exception("[login_server::requestReLogin][Log] acesso restrito para o player [UID=" + std::to_string(_session.m_pi.uid)
					+ ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1028, 0));

		}else if (_session.m_pi.block_flag.m_id_state.id_state.ull_IDState != 0) {	// Verifica se está bloqueado

			if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_TEMPORARY && (_session.m_pi.block_flag.m_id_state.block_time == -1 || _session.m_pi.block_flag.m_id_state.block_time > 0)) {

				throw exception("[login_server::requestReLogin][Log] Bloqueado por tempo[Time="
						+ (_session.m_pi.block_flag.m_id_state.block_time == -1 ? std::string("indeterminado") : (std::to_string(_session.m_pi.block_flag.m_id_state.block_time / 60)
						+ "min " + std::to_string(_session.m_pi.block_flag.m_id_state.block_time % 60) + "sec"))
						+ "]. player [UID=" + std::to_string(_session.m_pi.uid) + ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1029, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_FOREVER) {

				throw exception("[login_server::requestReLogin][Log] Bloqueado permanente. player [UID=" + std::to_string(_session.m_pi.uid)
						+ ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1030, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_ALL_IP) {

				// Bloquea todos os IP que o player logar e da error de que a area dele foi bloqueada

				// Add o ip do player para a lista de ip banidos
				snmdb::NormalManagerDB::getInstance().add(1, new CmdInsertBlockIP(_session.m_ip, "255.255.255.255"), login_server::SQLDBResponse, this);

				// Resposta
				throw exception("[login_server::requestReLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ ", IP=" + std::string(_session.m_ip) + "] Block ALL IP que o player fizer login.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1031, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_MAC_ADDRESS) {

				// Bloquea o MAC Address que o player logar e da error de que a area dele foi bloqueada

				// Aqui só da error por que não tem como bloquear o MAC Address por que o cliente não fornece o MAC Address nesse pacote
				throw exception("[login_server::requestReLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ ", IP=" + std::string(_session.m_ip) + ", MAC=UNKNOWN] (Esse pacote o cliente nao fornece o MAC Address) Block MAC Address que o player fizer login.", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1032, 0));

			}

		}

		// Passou da verificação com sucesso
		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][Log] player[UID=" + std::to_string(_session.m_pi.uid) + ", ID="
				+ std::string(_session.m_pi.id) + "] relogou com sucesso", CL_FILE_LOG_AND_CONSOLE));

		// Authorized a ficar online no server por tempo indeterminado
		_session.m_is_authorized = 1u;

		packet_func::succes_login(this, &_session, 1/*só passa auth Key Login, Server List, Msn Server List*/);

	}catch (exception& e) {

		// Erro do sistema
		packet_func::pacote00E(p, &_session, L"", 12, 500052);
		packet_func::session_send(p, &_session, 1);

		// Disconnect o player
#if defined(_WIN32)
		::shutdown(_session.m_sock, SD_RECEIVE);
#elif defined(__linux__)
		::shutdown(_session.m_sock.fd, SD_RECEIVE);
#endif

		_smp::message_pool::getInstance().push(new message("[login_server::requestReLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void login_server::shutdown_time(int32_t _time_sec) {

	if (_time_sec <= 0)	// Desliga o Server Imediatemente
		shutdown();
	else {

		job _job(server::end_time_shutdown, this, (void*)0);

		// Se o Shutdown Timer estiver criado descria e cria um novo
		if (m_shutdown != nullptr) {

			// Para o Tempo se ele não estiver parado
			if (m_shutdown->getState() != timer::STOPPED)
				m_shutdown->stop();

			m_timer_mgr.deleteTimer(m_shutdown);
		}

		if ((m_shutdown = m_timer_mgr.createTimer(_time_sec * 1000, new (timer::timer_param){ _job, m_job_pool })) == nullptr)
			throw exception("[login_server::shutdown_time][Error] nao conseguiu criar o timer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 51, 0));
	}
};

void login_server::onAcceptCompleted(session* _session) {

	if (_session == nullptr)
		throw exception("[login_server::onAcceptCompleted][Error] session is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 60, 0));

	if (!_session->getState())
		throw exception("[login_server::onAcceptCompleted][Error] session is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 61, 0));

	if (!_session->isConnected())
		throw exception("[login_server::onAcceptCompleted][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 62, 0));

	packet _packet((unsigned short)0x0);	// Tipo Packet Login Server initial packet no compress e no crypt

	_packet.addInt32((int)_session->m_key);	// key
	_packet.addInt32(m_si.uid);					// Server UID

	_packet.makeRaw();
	WSABUF mb = _packet.getMakedBuf();

	//SHOW_DEBUG_FINAL_PACKET(mb, 1);

	try {
		_session->requestRecvBuffer();
		_session->requestSendBuffer(mb.buf, mb.len);
	}catch (exception& e) {
		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 1))
			throw;
	}
};

void login_server::onDisconnected(session *_session) {

	player *_player = reinterpret_cast< player* >(_session);

	_smp::message_pool::getInstance().push(new message("[login_server::onDisconnected][Log] Player Desconectou. ID: " + std::string(_player->m_pi.id) + "  UID: " + std::to_string(_player->m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

	// Aqui não faz nada, no login server por enquanto
};

void login_server::onHeartBeat() {

	try {

		// Server ainda não está totalmente iniciado
		if (m_state != INITIALIZED)
			return;

		// Tirei o list IP/MAC block daqui e coloquei no monitor no server, por que agora eles são da classe server

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::onHeartBeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void login_server::onStart() {

	try {

		// Não faz nada por enquanto

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[login_server::onStart][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool login_server::checkCommand(std::stringstream& _command) {
	
	std::string s = "";

	_command >> s;

	if (!s.empty() && s.compare("exit") == 0)
		return true;	// Sai
	else if (!s.empty() && s.compare("reload_files") == 0) {

		reload_files();
		
		_smp::message_pool::getInstance().push(new message("Login Server files has been reloaded.", CL_FILE_LOG_AND_CONSOLE));
	
	}else if (!s.empty() && s.compare("reload_socket_config") == 0) {

		// Ler novamento o arquivo de configuração do socket
		if (m_accept_sock != nullptr)
			m_accept_sock->reload_config_file();
		else
			_smp::message_pool::getInstance().push(new message("[login_server::checkCommand][WARNING] m_accept_sock(socket que gerencia os socket que pode aceitar etc) is invalid.", CL_FILE_LOG_AND_CONSOLE));

	}else if (!s.empty() && s.compare("open") == 0) {
		s = "";

		_command >> s;

		if (!s.empty() && s.compare("gm") == 0) {
			m_access_flag = 1;
			_smp::message_pool::getInstance().push(new message("Now only GM and IPs registred can login.", CL_FILE_LOG_AND_CONSOLE));
		}else if (!s.empty() && s.compare("all") == 0) {
			s = "";

			_command >> s;

			if (!s.empty() && s.compare("user") == 0) {
				m_access_flag = 0;
				_smp::message_pool::getInstance().push(new message("Now All user can login.", CL_FILE_LOG_AND_CONSOLE));
			}else
				_smp::message_pool::getInstance().push(new message("Unknown Command: \"open all " + s + "\"", CL_ONLY_CONSOLE));
		}else
			_smp::message_pool::getInstance().push(new message("Unknown Command: \"open " + s + "\"", CL_ONLY_CONSOLE));
	
	}else if (!s.empty() && s.compare("create_user") == 0) {
		s = "";

		_command >> s;

		if (!s.empty() && s.compare("on") == 0) {
			m_create_user_flag = 1;
			_smp::message_pool::getInstance().push(new message("Create User ON", CL_FILE_LOG_AND_CONSOLE));
		}else if (!s.empty() && s.compare("off") == 0) {
			m_create_user_flag = 0;
			_smp::message_pool::getInstance().push(new message("Create User OFF", CL_FILE_LOG_AND_CONSOLE));
		}else
			_smp::message_pool::getInstance().push(new message("Unknown Command: \"create_user " + s + "\"", CL_ONLY_CONSOLE));
	
	}else if (!s.empty() && s.compare("snapshot") == 0) {

		try {
			int *bad_ptr_snapshot = nullptr;
			*bad_ptr_snapshot = 2;
		}catch (exception& e) {
			UNREFERENCED_PARAMETER(e);

			// Log
			_smp::message_pool::getInstance().push(new message("[login_server::checkCommand][Log] Snapshot comando executado.", CL_FILE_LOG_AND_CONSOLE));
		}

	}else
		_smp::message_pool::getInstance().push(new message("Unknown Command: " + s, CL_ONLY_CONSOLE));

	return false;
};

bool login_server::checkPacket(session& _session, packet *_packet) {
	
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
};

void login_server::config_init() {

	// Server Tipo
	m_si.tipo = 0/*Login Server*/;

	m_access_flag = m_reader_ini.readInt("OPTION", "ACCESSFLAG");
	m_create_user_flag = m_reader_ini.readInt("OPTION", "CREATEUSER");

	try {
		m_same_id_login_flag = m_reader_ini.readInt("OPTION", "SAME_ID_LOGIN");
	}catch (exception& e) {
		// Não precisa printar mensagem por que essa opção é de desenvolvimento
		UNREFERENCED_PARAMETER(e);
	}

};

void login_server::reload_files() {
	
	server::config_init();
	config_init();

	sIff::getInstance().reload();
};

void login_server::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[login_server::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[login_server::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_server = reinterpret_cast<server*>(_arg);

	switch (_msg_id) {
	case 1:	// Insert Block IP
	{
		auto cmd_ibi = reinterpret_cast< CmdInsertBlockIP* >(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[login_server::SQLDBResponse][Log] Inseriu Block IP[IP=" + cmd_ibi->getIP() 
				+ ", MASK=" + cmd_ibi->getMask() + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[login_server::SQLDBResponse][Log] Inseriu Block IP[IP=" + cmd_ibi->getIP()
				+ ", MASK=" + cmd_ibi->getMask() + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 2: // Insert Block MAC
	{
		auto cmd_ibm = reinterpret_cast< CmdInsertBlockMAC* >(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[login_server::SQLDBResponse][Log] Inseriu Block MAC[ADDRESS=" + cmd_ibm->getMACAddress() + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[login_server::SQLDBResponse][Log] Inseriu Block MAC[ADDRESS=" + cmd_ibm->getMACAddress() + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 0:
	default:
		break;
	}
};
