// Arquivo auth_server.cpp
// Criado em 02/12/2018 as 13:24 por Acrisio
// Implementa��o da classe auth_server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <MSWSock.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "auth_server.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/SOCKET/socketserver.h"
#include "../../Projeto IOCP/SOCKET/socket.h"
#include "../PACKET/packet_func_as.h"

#if defined(_WIN32)
#include <WS2tcpip.h>
#elif defined(__linux__)
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif

#include "../PANGYA_DB/cmd_command_info.hpp"
#include "../PANGYA_DB/cmd_shutdown_info.hpp"
#include "../PANGYA_DB/cmd_notice_info.hpp"
#include "../PANGYA_DB/cmd_ticker_info.hpp"
#include "../PANGYA_DB/cmd_update_command.hpp"

#include "../PANGYA_DB/cmd_update_guild_ranking.hpp"
#include "../PANGYA_DB/cmd_guild_ranking_update_time.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_server_key.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_update_auth_server_key.hpp"

#define REQUEST_BEGIN(method) if (!_session.getState()) \
									throw exception("[auth_server::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 1, 0)); \
							  if (_packet == nullptr) \
									throw exception("[auth_server::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 6, 0)); \

// Verifica se session est� autorizada para executar esse a��o, 
// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!_session.m_is_authorized) \
												throw exception("[auth_server::request" + std::string((method)) + "][Error] Player[UID=" + std::to_string(_session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 1, 0x7000501)); \

using namespace stdA;

auth_server::auth_server() : unit(m_player_manager, 4/*DB Instance Num*/, 4/*Job Thread Num*/), 
	m_player_manager(*this, m_si.max_user), m_guild_ranking_time{0} {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[auth_server::auth_server][Error] Ao iniciar o auth server.\n", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		config_init();

		packet_func::funcs.addPacketCall(0x01, packet_func::packet001, this);
		packet_func::funcs.addPacketCall(0x02, packet_func::packet002, this);
		packet_func::funcs.addPacketCall(0x03, packet_func::packet003, this);
		packet_func::funcs.addPacketCall(0x04, packet_func::packet004, this);
		packet_func::funcs.addPacketCall(0x05, packet_func::packet005, this);
		packet_func::funcs.addPacketCall(0x06, packet_func::packet006, this);
		packet_func::funcs.addPacketCall(0x07, packet_func::packet007, this);

		packet_func::funcs_sv.addPacketCall(0x00, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x01, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x02, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x03, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x04, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x05, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x06, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x07, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x08, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x09, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0A, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0B, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0C, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0D, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x0E, packet_func::packet_svFazNada, this);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::auth_server][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;

		// n�o posso lan�a exception para fora do contrutor que eles nao chama o destrutor dele antes
	}
}

auth_server::~auth_server() {

	// Crucial esse aqui para terminar o server sem crashs
	waitAllThreadFinish(INFINITE);
}

void auth_server::requestDisconnectPlayer(player& _session, packet *_packet) {
	REQUEST_BEGIN("DisconnectPlayer");

	packet p;

	uint32_t player_uid = 0u;
	uint32_t server_uid = 0u;

	try {

		player_uid = _packet->readUint32();
		server_uid = _packet->readUint32();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("DisconnectPlayer");

		auto s = m_player_manager.findPlayer(server_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[auth_server::requestDisconnectPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para o outro Server[UID="
					+ std::to_string(server_uid) + "] desconectar o Player[UID=" + std::to_string(player_uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Envia para o outro server o comando para desconectar o player
			p.init_plain((unsigned short)0x6);

			p.addUint32(player_uid);
			p.addUint32(_session.m_pi.uid);	// Quem pediu para disconectar o player
			p.addUint8(0u);					// 1 Ativado, Flag que forca a desconecta mesmo se o server tiver outras regras

			packet_func::session_send(p, s, 1);

		}else {

			// n�o encontrou o Server para enviar o comando para deconectar o player
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[auth_server::requestDisconnectPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para o outro Server[UID="
					+ std::to_string(server_uid) + "] desconectar o Player[UID=" + std::to_string(player_uid) + "], mas nao encontrou o Server.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[auth_server::requestDisconnectPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para o outro Server[UID="
					+ std::to_string(server_uid) + "] desconectar o Player[UID=" + std::to_string(player_uid) + "], mas nao encontrou o Server.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			// Ent�o retorn para o Cliente que pediu para desconectar o player,
			// para ele continuar sua execu��o e deixar que o server deconecte o player quando ele logar
			p.init_plain((unsigned short)0x7);

			p.addUint32(player_uid);

			packet_func::session_send(p, &_session, 1);

		}

	}catch (exception& e) {

		// Ent�o retorn para o Cliente que pediu para desconectar o player,
		// para ele continuar sua execu��o e deixar que o server deconecte o player quando ele logar
		p.init_plain((unsigned short)0x7);

		p.addUint32(player_uid);

		packet_func::session_send(p, &_session, 1);

		_smp::message_pool::getInstance().push(new message("[auth_server::requestDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void auth_server::requestConfirmDisconnectPlayer(player& _session, packet *_packet) {
	REQUEST_BEGIN("ConfirmDisconnectPlayer");

	try {

		uint32_t player_uid = _packet->readUint32();
		uint32_t server_uid = _packet->readUint32();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("ConfirmDisconnectPlayer");

		if (server_uid != m_si.uid) {
			
			// N�o foi o Auth Server que pediu para disconectar esse usu�rio, procura o cliente e manda a resposta para ele
			auto s = m_player_manager.findPlayer(server_uid);

			if (s != nullptr) {

				// Log
				_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmDisconnectPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para enviar a confirmacao para o Server[UID="
						+ std::to_string(server_uid) + "] que o Player[UID=" + std::to_string(player_uid) + "] foi deconectado.", CL_FILE_LOG_AND_CONSOLE));

				packet p((unsigned short)0x7);

				p.addUint32(player_uid);

				packet_func::session_send(p, s, 1);
			
			}else
				_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmDisconnectPlayer][WARNING] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para enviar a confirmacao para o Server[UID="
						+ std::to_string(server_uid) + "] que o Player[UID=" + std::to_string(player_uid) + "] foi desconectado, mas o server nao esta conectado.", CL_FILE_LOG_AND_CONSOLE));

		}else
			_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmDisconnectPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para enviar a confirmacao para o Server[UID="
					+ std::to_string(server_uid) + "](Auth Server) que o Player[UID=" + std::to_string(player_uid) + "] foi desconectado.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void auth_server::requestInfoPlayer(player& _session, packet *_packet) {
	REQUEST_BEGIN("InfoPlayer");

	packet p;

	uint32_t server_uid = 0u;
	uint32_t player_uid = 0u;

	try {

		server_uid = _packet->readUint32();
		player_uid = _packet->readUint32();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("InfoPlayer");

		auto s = m_player_manager.findPlayer(server_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[auth_server::requestInfoPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para o outro Server[UID="
					+ std::to_string(server_uid) + "] o Info do Player[UID=" + std::to_string(player_uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Envia para o outro server o comando para desconectar o player
			p.init_plain((unsigned short)0xB);

			p.addUint32(_session.m_pi.uid);	// Server UID request (quem pediu o info do player)
			p.addUint32(player_uid);

			packet_func::session_send(p, s, 1);

		}else {

			// n�o encontrou o Server para enviar o comando pedinfo o info do player
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[auth_server::requestInfoPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para o outro Server[UID="
					+ std::to_string(server_uid) + "] o info do Player[UID=" + std::to_string(player_uid) + "], mas nao encontrou o Server.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[auth_server::requestInfoPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para o outro Server[UID="
					+ std::to_string(server_uid) + "] o info do Player[UID=" + std::to_string(player_uid) + "], mas nao encontrou o Server.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			// Ent�o retorna para o Cliente que pediu o info do player, 
			// dizendo que o player n�o foi encontrado online por que o server n�o foi encontrado online no Auth Server
			p.init_plain((unsigned short)0xC);

			p.addUint32(server_uid);
			p.addInt32(-1);	// Error n�o encontrou o server para enviar o request

			p.addUint32(player_uid);

			packet_func::session_send(p, &_session, 1);

		}

	}catch (exception& e) {

		// Ent�o retorna para o Cliente que pediu o info do player, 
		// dizendo que o player n�o foi encontrado online por que o teve algum Exception no Auth Server
		p.init_plain((unsigned short)0xC);

		p.addUint32(server_uid);
		p.addInt32(-1);	// Error n�o encontrou o server para enviar o request

		p.addUint32(player_uid);

		packet_func::session_send(p, &_session, 1);

		_smp::message_pool::getInstance().push(new message("[auth_server::requestInfoPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void auth_server::requestConfirmSendInfoPlayer(player& _session, packet *_packet) {
	REQUEST_BEGIN("ConfirmSendInfoPlayer");

	try {

		AuthServerPlayerInfo aspi{ 0 };

		uint32_t req_server_uid = _packet->readUint32();
		
		aspi.option = _packet->readInt32();
		aspi.uid = _packet->readUint32();

		if (aspi.option == 1) {

			aspi.id = _packet->readString();
			aspi.ip = _packet->readString();

		}

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("ConfirmSendInfoPlayer");

		if (req_server_uid != m_si.uid) {
			
			// N�o foi o Auth Server que pediu para disconectar esse usu�rio, procura o cliente e manda a resposta para ele
			auto s = m_player_manager.findPlayer(req_server_uid);

			if (s != nullptr) {

				// Log
				_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmSendInfoPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para enviar a confirmacao para o Server[UID="
						+ std::to_string(req_server_uid) + "] do info do Player[UID=" + std::to_string(aspi.uid) + "].", CL_FILE_LOG_AND_CONSOLE));

				// Resposta
				packet p((unsigned short)0xC);

				p.addUint32(_session.m_pi.uid);	// Server UID (Quem pediu para enviar a confirma��o do info do player)
				p.addInt32(aspi.option);
				p.addUint32(aspi.uid);

				if (aspi.option == 1) {

					p.addString(aspi.id);
					p.addString(aspi.ip);

				}

				packet_func::session_send(p, s, 1);
			
			}else
				_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmSendInfoPlayer][WARNING] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para enviar a confirmacao para o Server[UID="
						+ std::to_string(req_server_uid) + "] do info do Player[UID=" + std::to_string(req_server_uid) + "], mas o server nao esta conectado.", CL_FILE_LOG_AND_CONSOLE));

		}else
			_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmSendInfoPlayer][Log] o Server[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para enviar a confirmacao para o Server[UID="
					+ std::to_string(req_server_uid) + "](Auth Server) do info do Player[UID=" + std::to_string(req_server_uid) + "].", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::requestConfirmSendInfoPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void auth_server::requestSendCommandToOtherServer(player& _session, packet *_packet) {
	REQUEST_BEGIN("SendCommandToOtherServer");

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("SendCommandToOtherServer");

		CommandOtherServerHeaderEx cosh{ 0 };

		_packet->readBuffer(&cosh, sizeof(CommandOtherServerHeader));

		unsigned short command_buff_size = 0u;

		if ((sizeof(CommandOtherServerHeader) + 2/*Packet ID*/) < _packet->getSize())
			command_buff_size = (unsigned short)(_packet->getSize() - (sizeof(CommandOtherServerHeader) + 2/*Packet ID*/));

		auto s = m_player_manager.findPlayer(cosh.send_server_uid_or_type);

		if (s == nullptr) {

			auto v_s = m_player_manager.findPlayerByTypeExcludeUID(cosh.send_server_uid_or_type, _session.m_pi.uid);

			if (!v_s.empty()) {

				// Inicializa o Buffer do comando
				if (command_buff_size > 0) {

					cosh.command.init(command_buff_size);

					if (!cosh.command.is_good())
						throw exception("[auth_server::requestSendCommandToOtherServer][Error] Server[UID=" + std::to_string(_session.m_pi.uid)
								+ "] pediu para enviar o command[ID=" + std::to_string(cosh.command_id)
								+ "] para o outro Server[UID/TYPE=" + std::to_string(cosh.send_server_uid_or_type)
								+ "], mas nao conseguiu alocar memoria para o comando buff[size=" + std::to_string(command_buff_size) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 3501, 0));

					// Ler o comando buffer do _packet, para enviar para o outro Server
					_packet->readBuffer(cosh.command.buff, cosh.command.size);

				}

				// Log
				_smp::message_pool::getInstance().push(new message("[auth_server::requestSendCommandToOtherServer][Log] Server[UID=" + std::to_string(_session.m_pi.uid)
						+ "] pediu para enviar o command[ID=" + std::to_string(cosh.command_id)
						+ "] para o outro Server[UID/TYPE=" + std::to_string(cosh.send_server_uid_or_type) + "]", CL_FILE_LOG_AND_CONSOLE));

				// Envia para todos o Server do mesmo tipo excluindo quem pediu para enviar o comando
				p.init_plain((unsigned short)0x0D);

				// Quem pediu para enviar esse comando para o outro Server
				p.addUint32(_session.m_pi.uid);

				// Comando ID
				p.addUint16(cosh.command_id);

				if (command_buff_size > 0 && cosh.command.size > 0) {

					// Comando buffer
					p.addBuffer(cosh.command.buff, cosh.command.size);

				}else
					p.addUint16(0u);	// Comando buffer � vazio

				packet_func::vector_send(p, v_s, 1);

			}else
				throw exception("[auth_server::requestSendCommandToOtherServer][WARNING] Server[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] pediu para enviar o command[ID=" + std::to_string(cosh.command_id) 
						+ "] para o outro Server[UID/TYPE=" + std::to_string(cosh.send_server_uid_or_type) 
						+ "], mas nao encontrou ele.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 3500, 0));
		
		}else {

			// Inicializa o Buffer do comando
			if (command_buff_size > 0) {
				
				cosh.command.init(command_buff_size);

				if (!cosh.command.is_good())
					throw exception("[auth_server::requestSendCommandToOtherServer][Error] Server[UID=" + std::to_string(_session.m_pi.uid)
							+ "] pediu para enviar o command[ID=" + std::to_string(cosh.command_id)
							+ "] para o outro Server[UID/TYPE=" + std::to_string(cosh.send_server_uid_or_type)
							+ "], mas nao conseguiu alocar memoria para o comando buff[size=" + std::to_string(command_buff_size) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 3501, 0));

				// Ler o comando buffer do _packet, para enviar para o outro Server
				_packet->readBuffer(cosh.command.buff, cosh.command.size);

			}

			// Log
			_smp::message_pool::getInstance().push(new message("[auth_server::requestSendCommandToOtherServer][Log] Server[UID=" + std::to_string(_session.m_pi.uid)
					+ "] pediu para enviar o command[ID=" + std::to_string(cosh.command_id)
					+ "] para o outro Server[UID/TYPE=" + std::to_string(cosh.send_server_uid_or_type) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Envia para o Server
			p.init_plain((unsigned short)0x0D);

			// Quem pediu para enviar esse comando para o outro Server
			p.addUint32(_session.m_pi.uid);

			// Comando ID
			p.addUint16(cosh.command_id);

			if (command_buff_size > 0 && cosh.command.size > 0) {

				// Comando buffer
				p.addBuffer(cosh.command.buff, cosh.command.size);

			}else
				p.addUint16(0u);	// Comando buffer � vazio

			packet_func::session_send(p, s, 1);
		}
		

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::requestSendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void auth_server::requestSendReplyToOtherServer(player& _session, packet *_packet) {
	REQUEST_BEGIN("SendReplyToOtherServer");

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("SendReplyToOtherServer");

		CommandOtherServerHeaderEx cosh{ 0 };

		_packet->readBuffer(&cosh, sizeof(CommandOtherServerHeader));

		unsigned short command_buff_size = 0u;

		if ((sizeof(CommandOtherServerHeader) + 2/*Packet ID*/) < _packet->getSize())
			command_buff_size = (unsigned short)(_packet->getSize() - (sizeof(CommandOtherServerHeader) + 2/*Packet ID*/));

		auto s = m_player_manager.findPlayer(cosh.send_server_uid_or_type);

		if (s == nullptr)
			throw exception("[auth_server::requestSendReplyToOtherServer][WARNING] Server[UID=" + std::to_string(_session.m_pi.uid)
					+ "] pediu para enviar a resposta[ID=" + std::to_string(cosh.command_id)
					+ "] para o outro Server[UID=" + std::to_string(cosh.send_server_uid_or_type)
					+ "], mas nao encontrou ele.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 3502, 0));

		// Inicializa o Buffer do comando
		if (command_buff_size > 0) {
				
			cosh.command.init(command_buff_size);

			if (!cosh.command.is_good())
				throw exception("[auth_server::requestSendReplyToOtherServer][Error] Server[UID=" + std::to_string(_session.m_pi.uid)
						+ "] pediu para enviar a resposta[ID=" + std::to_string(cosh.command_id)
						+ "] para o outro Server[UID=" + std::to_string(cosh.send_server_uid_or_type)
						+ "], mas nao conseguiu alocar memoria para o comando buff[size=" + std::to_string(command_buff_size) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 3503, 0));

			// Ler o comando buffer do _packet, para enviar para o outro Server
			_packet->readBuffer(cosh.command.buff, cosh.command.size);

		}

		// Log
		_smp::message_pool::getInstance().push(new message("[auth_server::requestSendReplyToOtherServer][Log] Server[UID=" + std::to_string(_session.m_pi.uid)
				+ "] pediu para enviar a resposta[ID=" + std::to_string(cosh.command_id)
				+ "] para o outro Server[UID=" + std::to_string(cosh.send_server_uid_or_type) + "]", CL_FILE_LOG_AND_CONSOLE));

		// Envia para o Server
		p.init_plain((unsigned short)0x0E);

		// Quem pediu para enviar esse comando para o outro Server
		p.addUint32(_session.m_pi.uid);

		// Comando ID
		p.addUint16(cosh.command_id);

		if (command_buff_size > 0 && cosh.command.size > 0) {

			// Comando buffer
			p.addBuffer(cosh.command.buff, cosh.command.size);

		}else
			p.addUint16(0u);	// Comando buffer � vazio

		packet_func::session_send(p, s, 1);


	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::requestSendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void auth_server::requestAuthenticPlayer(player& _session, packet *_packet) {
	REQUEST_BEGIN("AuthenticPlayer");

	try {

		_session.m_pi.tipo = _packet->readUint32();	// Tipo do server
		_session.m_pi.uid = _packet->readUint32();	// UID

#if defined(_WIN32)
		memcpy_s(_session.m_pi.id, sizeof(_session.m_pi.id), _packet->readString().c_str(), sizeof(_session.m_pi.id));
#elif defined(__linux__)
		memcpy(_session.m_pi.id, _packet->readString().c_str(), sizeof(_session.m_pi.id));
#endif

		std::string key = _packet->readString();

		// Passa para o nickname o id
#if defined(_WIN32)
		memcpy_s(_session.m_pi.nickname, sizeof(_session.m_pi.nickname), _session.m_pi.id, sizeof(_session.m_pi.nickname));
#elif defined(__linux__)
		memcpy(_session.m_pi.nickname, _session.m_pi.id, sizeof(_session.m_pi.nickname));
#endif

		CmdAuthServerKey cmd_ask(_session.m_pi.uid, true);	// Waiter

		NormalManagerDB::add(0, &cmd_ask, nullptr, nullptr);

		cmd_ask.waitEvent();

		if (cmd_ask.getException().getCodeError() != 0)
			throw cmd_ask.getException();

		auto ask = cmd_ask.getInfo();

		if (!ask.checkKey(key))
			throw exception("[auth_server::requestAuthenticPlayer][Error] SERVER[UID=" + std::to_string(_session.m_pi.uid) + "] key[KEY=" + key + "] is not valid. Key[KEY="
					+ std::string(ask.key) + ", VALID=" + std::to_string((unsigned short)ask.valid)
					+ "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_AS, 350, 0));

		ask.valid = 0;	// J� usou a chave atualiza no banco de dados

		// Update Auth Server Key of Server
		NormalManagerDB::add(2, new CmdUpdateAuthServerKey(ask), auth_server::SQLDBResponse, this);

		// Logou com sucesso [Por Hora vou deixar assim]
		_session.m_is_authorized = 1u;	// Autorizado a ficar connectado, por bastante tempo

		// Log
		_smp::message_pool::getInstance().push(new message("[auth_server::requestAuthenticPlayer][Log] Server[TIPO=" + std::to_string(_session.m_pi.tipo) + ", NAME="
				+ std::string(_session.m_pi.nickname) + ", UID=" + std::to_string(_session.m_pi.uid) + ", ONLINE_ID=" + std::to_string(_session.m_oid)
				+ ", IP=" + std::string(_session.m_ip) + "] WITH KEY[" + key + "] conectou-se com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		// UPDATE TO CLIENTE
		packet p((unsigned short)0x1);

		p.addUint32(_session.m_oid);	// OID

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::requestAuthenticPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Log
		_smp::message_pool::getInstance().push(new message("[auth_server::requestAuthenticPlayer][Error] desconectando session[OID=" + std::to_string(_session.m_oid)
				+ "], por que mandou alguns dados errado no packet de login. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		
		DisconnectSession(&_session);
	}
}

void auth_server::onAcceptCompleted(session* _session) {

	if (_session == nullptr)
		throw exception("[auth_server::onAcceptCompleted][Error] session is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 60, 0));

	if (!_session->getState())
		throw exception("[auth_server::onAcceptCompleted][Error] session is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 61, 0));

	if (!_session->isConnected())
		throw exception("[auth_server::onAcceptCompleted][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 62, 0));

	packet _packet((unsigned short)0x0);	// Tipo Packet Auth Server initial packet no compress e no crypt

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
}

void auth_server::onDisconnected(session* _session) {

	player *_player = reinterpret_cast< player* >(_session);

	_smp::message_pool::getInstance().push(new message("[auth_server::onDisconnected][Log] Player Desconectou. ID: " + std::string(_player->m_pi.id) + "  UID: " + std::to_string(_player->m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

	// Aqui n�o faz nada, no login server por enquanto
}

void auth_server::onHeartBeat() {

	SYSTEMTIME local{ 0 };

	try {
		
		// Check Commands
		CmdCommandInfo cmd_ci(true);	// Waiter

		NormalManagerDB::add(0, &cmd_ci, nullptr, nullptr);

		cmd_ci.waitEvent();

		if (cmd_ci.getException().getCodeError() != 0)
			throw cmd_ci.getException();

		translateCmd(cmd_ci.getInfo());

		// Guild Ranking Update
		try {

			// Verifica se j� pegou a hora do Guild Ranking se n�o pega no banco de dados
			if (isEmpty(m_guild_ranking_time)) {

				CmdGuildRankingUpdateTime cmd_grut(true);	// Waiter

				NormalManagerDB::add(0, &cmd_grut, nullptr, nullptr);

				cmd_grut.waitEvent();

				if (cmd_grut.getException().getCodeError() != 0)
					throw cmd_grut.getException();

				m_guild_ranking_time = cmd_grut.getTime();

				// Log
				_smp::message_pool::getInstance().push(new message("[auth_server::onHeartBeat][Log] Inicializou o Update Time do Guild Ranking[" 
						+ _formatDate(m_guild_ranking_time) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
			}

			// Local Time para verificar
			GetLocalTime(&local);

			// Verifica se � um novo dia e atualiza o Guild Ranking
			if (m_guild_ranking_time.wYear < local.wYear || m_guild_ranking_time.wMonth < local.wMonth || m_guild_ranking_time.wDay < local.wDay) {
				
				_smp::message_pool::getInstance().push(new message("[auth_server::onHearBeat][Log] Atualizando o Guild Ranking...", CL_FILE_LOG_AND_CONSOLE));

				NormalManagerDB::add(3, new CmdUpdateGuildRanking(), auth_server::SQLDBResponse, this);

				// atualiza a hora do ranking do server
				GetLocalTime(&m_guild_ranking_time);
			}

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[auth_server::onHeartBeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}

	}catch (exception& e) {
		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PANGYA_DB, 6))
			throw;
	}
}

void auth_server::onStart() {

	try {

		// N�o faz nada por enquanto

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::onStart][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

#define FIND_TARGET_AND_SEND(_target, _p, _msg) { \
	/*Find Server By UID, se n�o Encontrar, procura por TIPO*/ \
	auto s = (player*)m_player_manager.findSessionByUID((_target)); \
\
	if (s == nullptr) { \
\
		auto v_s = m_player_manager.findPlayerByType((_target)); \
\
		if (!v_s.empty()) { \
\
			/* Log */ \
			_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Send " + std::string((_msg)) + " For Server[UID=" + std::to_string((_target)) + "]", CL_FILE_LOG_AND_CONSOLE)); \
\
			packet_func::vector_send(p, v_s, 1); \
\
		}else \
			_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] Nao encontrou o SERVER[UID/TIPO=" + std::to_string((_target)) \
					+ "], para enviar o comando de " + std::string((_msg)) + " para ele.", CL_FILE_LOG_AND_CONSOLE)); \
\
	}else { \
\
		/* Log */ \
		_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Send " + std::string((_msg)) + " For Server[UID=" + std::to_string(el.target) + "]", CL_FILE_LOG_AND_CONSOLE)); \
\
		packet_func::session_send(p, s, 1); \
\
	} \
} \

void auth_server::translateCmd(std::vector< CommandInfo >& _v_ci) {

	try {

		for (auto& el : _v_ci) {

			// Ainda n�o chegou na date reservada, pula esse comando
			if (std::time(nullptr) < el.reserveDate)
				continue;

			switch (el.id) {
			case COMMAND_ID::BROADCAST_NOTICE:
			{
				CmdNoticeInfo cmd_ni(el.idx, true);	// Waiter

				NormalManagerDB::add(0, &cmd_ni, nullptr, nullptr);

				cmd_ni.waitEvent();

				if (cmd_ni.getException().getCodeError() != 0) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] " + cmd_ni.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				auto msg = cmd_ni.getInfo();

				if (msg.empty()) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] msg is empty. Comando[" + el.toString() + "]", CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				// Send Msg
				packet p((unsigned short)0x3);

				p.addString(msg);

				FIND_TARGET_AND_SEND(el.target, p, "Broadcast Notice[MESSAGE=" + msg + "]");

				break;
			}
			case COMMAND_ID::BROADCAST_TICKER:
			{

				CmdTickerInfo cmd_ti(el.idx, true);	// Waiter

				NormalManagerDB::add(0, &cmd_ti, nullptr, nullptr);

				cmd_ti.waitEvent();

				if (cmd_ti.getException().getCodeError() != 0) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] " + cmd_ti.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				auto ti = cmd_ti.getInfo();

				if (!ti.isValid()) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] Ticker Info is invalid [MSG="
							+ ti.msg + ", NICK=" + ti.nick + "]. Comando[" + el.toString() + "]", CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				// Send Ticker
				packet p((unsigned short)0x4);

				p.addString(ti.nick);
				p.addString(ti.msg);

				// Find Server By UID, se n�o Encontrar, procura por TIPO
				auto s = (player*)m_player_manager.findSessionByUID(el.target);

				if (s == nullptr) {
				
					// Exclui do vector de server para enviar, o server que gerou o ticker, ele nao precisa que envie de novo para ele
					auto v_s = m_player_manager.findPlayerByTypeExcludeUID(el.target, el.arg[1]/*SERVER UID*/);
					
					if (!v_s.empty()) {
						
						// Log
						_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Send Ticker[MESSAGE=" + ti.msg + ", NICK=" + ti.nick + "] For Server[UID=" + std::to_string(el.target) + "]", CL_FILE_LOG_AND_CONSOLE));
						
						packet_func::vector_send(p, v_s, 1);
					
					}else
						_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] Nao encontrou o SERVER[UID/TIPO=" + std::to_string(el.target)
								+ "], para enviar o comando de Ticker[MESSAGE=" + ti.msg + ", NICK=" + ti.nick + "] para ele.", CL_FILE_LOG_AND_CONSOLE));
				
				}else {

					// Log 
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Send Ticker[MESSAGE=" + ti.msg + ", NICK=" + ti.nick + "] For Server[UID=" + std::to_string(el.target) + "]", CL_FILE_LOG_AND_CONSOLE));

					packet_func::session_send(p, s, 1);
				}

				//FIND_TARGET_AND_SEND(el.target, p, "Ticker[MESSAGE=" + ti.msg + ", NICK=" + ti.nick + "]");

				break;
			}
			case COMMAND_ID::BROADCAST_CUBE_WIN:
			{
				CmdNoticeInfo cmd_ni(el.idx, true);	// Waiter

				NormalManagerDB::add(0, &cmd_ni, nullptr, nullptr);

				cmd_ni.waitEvent();

				if (cmd_ni.getException().getCodeError() != 0) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] " + cmd_ni.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				auto msg = cmd_ni.getInfo();

				if (msg.empty()) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] msg is empty. Comando[" + el.toString() + "]", CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				// Send Broadcast Notice Cube Win Rare
				packet p((unsigned short)0x5);

				p.addUint32(el.arg[1]);	// Option
				p.addString(msg);

				FIND_TARGET_AND_SEND(el.target, p, "Broadcast Notice Cube Win Rare[MESSAGE=" + msg + ", OPTION=" + std::to_string(el.arg[1]) + "]");

				break;
			}
			case COMMAND_ID::NEW_ITEM_NOTICE:
			{

				// Update Command on DB
				el.valid = 0u;

				NormalManagerDB::add(1, new CmdUpdateCommand(el), auth_server::SQLDBResponse, this);

				// Send New Mail Arrived in MailBox
				packet p((unsigned short)0x8);

				p.addUint32(el.arg[0]);	// Player UID
				p.addUint32(el.arg[1]);	// Msg Id

				FIND_TARGET_AND_SEND(el.target, p, "New Mail Arrived In MailBox[PLAYER=" + std::to_string(el.arg[0]) 
						+ ", MSG_ID=" + std::to_string(el.arg[1]) + "]");

				break;
			}
			case COMMAND_ID::NEW_RATE:
			{
				// Update Command on DB
				el.valid = 0u;

				NormalManagerDB::add(1, new CmdUpdateCommand(el), auth_server::SQLDBResponse, this);

				// Send New Rate to Server
				packet p((unsigned short)0x9);

				p.addUint32(el.arg[0]);		// Tipo Rate
				p.addUint32(el.arg[1]);		// Quantidade (amount)

				FIND_TARGET_AND_SEND(el.target, p, "New Rate[TIPO=" + std::to_string(el.arg[0]) + ", QNTD=" + std::to_string(el.arg[1]) + "]");

				break;
			}
			case COMMAND_ID::ADM_KICK_FROM_WEBSITE:
			{
				// Update Command on DB
				el.valid = 0u;

				NormalManagerDB::add(1, new CmdUpdateCommand(el), auth_server::SQLDBResponse, this);

				// Send Disconnect Player
				packet p((unsigned short)0x6);

				p.addUint32(el.arg[0]);		// Playe UID
				p.addUint32(m_si.uid);		// Quem pediu para desconectar o jogador
				p.addUint8(1u);				// 1 Ativado, Flag que forca a desconecta mesmo se o server tiver outras regras

				FIND_TARGET_AND_SEND(el.target, p, "ADM Website Disconnect User[UID=" + std::to_string(el.arg[0]) + "]");

				break;
			}
			case COMMAND_ID::SHUTDOWN:
			{
				CmdShutdownInfo cmd_si(el.idx, true);	// Waiter

				NormalManagerDB::add(0, &cmd_si, nullptr, nullptr);

				cmd_si.waitEvent();

				if (cmd_si.getException().getCodeError() != 0) {
					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Error] " + cmd_si.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

					// Trata os outros comandos
					continue;
				}

				auto time_sec = cmd_si.getInfo();

				// Send Time Shutdown
				packet p((unsigned short)0x2);

				p.addUint32(time_sec);

				// Verifica se � o Auth Server, se for envia para todos o tempo, e desliga o Auth Server Tamb�m
				if (el.target == m_si.uid || el.target == m_si.tipo) {

					packet_func::vector_send(p, m_player_manager.getAllPlayer(), 1);

					_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Comando de Desligar o Auth Server. Desligando o Server em "
							+ std::to_string(time_sec) + " segundos", CL_FILE_LOG_AND_CONSOLE));

					if (time_sec <= 0)
#if defined(_WIN32)
						Sleep(5000);	// Espera 5 segundos para da tempo de enviar para todos os server conectados
#elif defined(__linux__)
						usleep(5000000);	// Espera 5 segundos para da tempo de enviar para todos os server conectados
#endif

					// Shutdown With Time
					shutdown_time(time_sec);

				}else
					FIND_TARGET_AND_SEND(el.target, p, "Shutdown[TIME=" + std::to_string(time_sec) + "]");

				break;
			}
			case COMMAND_ID::RELOAD_SYSTEM:
			{
				// Update Command on DB
				el.valid = 0u;

				NormalManagerDB::add(1, new CmdUpdateCommand(el), auth_server::SQLDBResponse, this);

				// Send Disconnect Player
				packet p((unsigned short)0x0A);

				p.addUint32(el.arg[0]);

				FIND_TARGET_AND_SEND(el.target, p, "Reload System[TYPE=" + std::to_string(el.arg[0]) + "]");

				break;

			}	// END COMMAND_ID::RELOAD_SYSTEM
			default:
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Comando[" + el.toString() + "]", CL_FILE_LOG_AND_CONSOLE));
#else
				_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][Log] Comando[" + el.toString() + "]", CL_ONLY_FILE_LOG));
#endif // _DEBUG
			} // END SWITCH
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[auth_server::translateCmd][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool auth_server::checkCommand(std::stringstream& _command) {
	
	std::string s = "";

	_command >> s;

	if (!s.empty() && s.compare("exit") == 0)
		return true;	// Sai
	else if (!s.empty() && s.compare("reload_files") == 0) {
		reload_files();
		_smp::message_pool::getInstance().push(new message("Auth Server files has been reloaded.", CL_FILE_LOG_AND_CONSOLE));
	}else if (!s.empty() && s.compare("reload_socket_config") == 0) {

		// Ler novamento o arquivo de configura��o do socket
		if (m_accept_sock != nullptr)
			m_accept_sock->reload_config_file();
		else
			_smp::message_pool::getInstance().push(new message("[auth_server::checkCommand][WARNING] m_accept_sock(socket que gerencia os socket que pode aceitar etc) is invalid.", CL_FILE_LOG_AND_CONSOLE));

	}else if (!s.empty() && s.compare("shutdown") == 0) {

		uint32_t time_sec = 0u;

		s = "";

		_command >> s;

		if (!s.empty())
			time_sec = atoi(s.c_str());

		if ((int)time_sec < 0)
			time_sec = 0u;

		// Manda Shutdown para todos os server conectados depois sai
		
		// Send Time Shutdown
		packet p((unsigned short)0x2);

		p.addUint32(time_sec);

		packet_func::vector_send(p, m_player_manager.getAllPlayer(), 1);

		_smp::message_pool::getInstance().push(new message("[auth_server::checkCmmand][Log] Comando de Desligar o Auth Server. Desligando o Server em "
				+ std::to_string(time_sec) + " segundos", CL_FILE_LOG_AND_CONSOLE));

		// Shutdown With Time
		shutdown_time((time_sec > 0 ? time_sec : 5)); // Espera 5 segundos para da tempo de enviar para todos os server conectados;

	}else if (!s.empty() && s.compare("snapshot") == 0) {

		try {
			int *bad_ptr_snapshot = nullptr;
			*bad_ptr_snapshot = 2;
		}catch (exception& e) {
			UNREFERENCED_PARAMETER(e);

			// Log
			_smp::message_pool::getInstance().push(new message("[auth_server::checkCommand][Log] Snapshot comando executado.", CL_FILE_LOG_AND_CONSOLE));
		}

	}else
		_smp::message_pool::getInstance().push(new message("Unknown Command: " + s, CL_ONLY_CONSOLE));

	return false;
}

bool auth_server::checkPacket(session& _session, packet *_packet) {
	
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

void auth_server::config_init() {

	// Server Tipo
	m_si.tipo = 5/*Auth Server*/;
}

void auth_server::reload_files() {

	unit::config_init();
	config_init();
}

void auth_server::shutdown_time(int32_t _time_sec) {

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
			throw exception("[auth_server::shutdown_time][Error] nao conseguiu criar o timer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::AUTH_SERVER, 51, 0));
	}
}

void auth_server::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[auth_server::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[auth_server::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_server = reinterpret_cast< auth_server* >(_arg);

	switch (_msg_id) {
	case 1:	// Update Command
	{
		auto cmd_uc = reinterpret_cast< CmdUpdateCommand* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[auth_server::SQLDBResponse][Log] Atualizou o Command[" + cmd_uc->getInfo().toString() + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 2:	// Update Auth Server Key
	{
		auto cmd_uask = reinterpret_cast< CmdUpdateAuthServerKey* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[auth_server::SQLDBResponse][Log] Atualizou Auth Server Key[SERVER_UID="
				+ std::to_string(cmd_uask->getInfo().server_uid) + ", KEY=" + std::string(cmd_uask->getInfo().key) + ", VALID=" 
				+ std::to_string((unsigned short)cmd_uask->getInfo().valid) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 3: // Update Guild Ranking
	{

		_smp::message_pool::getInstance().push(new message("[auth_server::SQLDBResponse][Log] Atualizou o Guild Ranking com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
}
