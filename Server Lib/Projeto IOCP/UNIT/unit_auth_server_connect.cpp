// Arquivo unit_auth_server_connect.cpp
// Criado em 02/02/2021 as 23:43 por Acrisio
// Implementação da classe unit_auth_server_connect

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "unit_auth_server_connect.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_new_auth_server_key.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#define REQUEST_BEGIN(method) if (!_session.getState()) \
									throw exception("[unit_auth_server_connect::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 1, 0)); \
							  if (_packet == nullptr) \
									throw exception("[unit_auth_server_connect::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 6, 0)); \

#define BEGIN_PACKET_INIT(_num) funcs.addPacketCall((_num), [](void* _arg1, void* _arg2) { \
	ParamDispatchAS pd = *reinterpret_cast< ParamDispatchAS* >(_arg2); \
	unit_auth_server_connect *uc = reinterpret_cast< unit_auth_server_connect* >(_arg1); \
\
	try { \

#define END_PACKET_INIT(_num) }catch (exception& e) { \
\
		_smp::message_pool::getInstance().push(new message("[packet_func::packet" + std::string((_num)) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); \
	} \
\
	return 0; \
}, this); \

#define INIT_PACKET_SERVER_FAZ_NADA(_num) funcs_sv.addPacketCall((_num), [](void* _arg1, void* _arg2) { \
		/* Não faz nada */ \
	return 0; \
}, this); \

using namespace stdA;

unit_auth_server_connect::unit_auth_server_connect(IUnitAuthServer& _owner_server, ServerInfoEx& _si)
	: unit_connect_base(_si), m_owner_server(_owner_server) {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::unit_auth_server_connect][Error] na inicializacao unit auth server connect", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		// Inicializar Config do arquivo ini
		config_init();

		/// ---------- Packets ---------

		// Packet000
		BEGIN_PACKET_INIT(0x0);

		uc->requestFirstPacketKey(pd._session, pd._packet);

		END_PACKET_INIT("000");

		// Packet001
		BEGIN_PACKET_INIT(0x1);

		uc->requestAskLogin(pd._session, pd._packet);

		END_PACKET_INIT("001");

		// Packet002
		BEGIN_PACKET_INIT(0x2);

		uc->requestShutdownServer(pd._session, pd._packet);

		END_PACKET_INIT("002");

		// Packet003
		BEGIN_PACKET_INIT(0x3);

		uc->requestBroadcastNotice(pd._session, pd._packet);

		END_PACKET_INIT("003");

		// Packet004
		BEGIN_PACKET_INIT(0x4);

		uc->requestBroadcastTicker(pd._session, pd._packet);

		END_PACKET_INIT("004");

		// Packet005
		BEGIN_PACKET_INIT(0x5);

		uc->requestBroadcastCubeWinRare(pd._session, pd._packet);

		END_PACKET_INIT("005");

		// Packet006
		BEGIN_PACKET_INIT(0x6);

		uc->requestDisconnectPlayer(pd._session, pd._packet);

		END_PACKET_INIT("006");

		// Packet007
		BEGIN_PACKET_INIT(0x7);

		uc->requestConfirmDisconnectPlayer(pd._session, pd._packet);

		END_PACKET_INIT("007");

		// Packet008
		BEGIN_PACKET_INIT(0x8);

		uc->requestNewMailArrivedMailBox(pd._session, pd._packet);

		END_PACKET_INIT("008");

		// Packet009
		BEGIN_PACKET_INIT(0x9);

		uc->requestNewRate(pd._session, pd._packet);

		END_PACKET_INIT("009");

		// Packet00A
		BEGIN_PACKET_INIT(0xA);

		uc->requestReloadSystem(pd._session, pd._packet);

		END_PACKET_INIT("00A");

		// Packet00B
		BEGIN_PACKET_INIT(0xB);

		uc->requestInfoPlayerOnline(pd._session, pd._packet);

		END_PACKET_INIT("00B");

		// Packet00C
		BEGIN_PACKET_INIT(0xC);

		uc->requestConfirmSendInfoPlayerOnline(pd._session, pd._packet);

		END_PACKET_INIT("00C");

		// Packet00D
		BEGIN_PACKET_INIT(0xD);

		uc->requestSendCommandToOtherServer(pd._session, pd._packet);

		END_PACKET_INIT("00D");

		// Packet00E
		BEGIN_PACKET_INIT(0xE);

		uc->requestSendReplyToOtherServer(pd._session, pd._packet);

		END_PACKET_INIT("00E");

		/// ----------- Pacotes -----------

		INIT_PACKET_SERVER_FAZ_NADA(0x1);

		// Pacote002
		INIT_PACKET_SERVER_FAZ_NADA(0x2);

		// Pacote003
		INIT_PACKET_SERVER_FAZ_NADA(0x3);

		// Pacote004
		INIT_PACKET_SERVER_FAZ_NADA(0x4);

		// Pacote005
		INIT_PACKET_SERVER_FAZ_NADA(0x5);

		// Pacote005
		INIT_PACKET_SERVER_FAZ_NADA(0x6);

		// Pacote005
		INIT_PACKET_SERVER_FAZ_NADA(0x7);

		// Initialized complete
		m_state = INITIALIZED;

	}catch (exception& e) {

		m_state = STATE::FAILURE;

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::unit_auth_server_connect][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

unit_auth_server_connect::~unit_auth_server_connect() {
}

void unit_auth_server_connect::requestFirstPacketKey(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("FirstPacketKey");

	try {

		_session.m_key = (unsigned char)_packet->readUint32();
		
		auto server_guid = _packet->readUint32();

		CmdNewAuthServerKey cmd_nask(_session.m_si.uid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_nask, nullptr, nullptr);

		cmd_nask.waitEvent();

		if (cmd_nask.getException().getCodeError() != 0)
			throw cmd_nask.getException();

		// Resposta para o Auth Server
		packet p((unsigned short)0x1);

		p.addUint32(_session.m_si.tipo);
		p.addUint32(_session.m_si.uid);
		p.addString(_session.m_si.nome);
		p.addString(cmd_nask.getInfo());

		packet_func_as::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestFirstPacketKey][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestAskLogin(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("AskLogin");

	try {

		int32_t oid = _packet->readInt32();

		if (oid > -1) {

			_session.m_oid = oid;

			// Log
			_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestAskLogin][Log] Logou com o Auth Server[OID=" + std::to_string(_session.m_oid) + "]", CL_FILE_LOG_AND_CONSOLE));
		}else
			_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestAskLogin][Log] Nao conseguiu logar com o Auth Server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestAskLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestShutdownServer(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("ShutdownServer");

	try {

		// Time In Seconds for Shutdown
		int32_t time = _packet->readInt32();

		m_owner_server.authCmdShutdown(time);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestShutdownServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestBroadcastNotice(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("BroadcastNotice");

	try {

		auto notice = _packet->readString();

		m_owner_server.authCmdBroadcastNotice(notice);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestBroadcastNotice][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestBroadcastTicker(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("BroadcastTicker");

	try {

		auto nickname = _packet->readString();
		auto msg = _packet->readString();

		m_owner_server.authCmdBroadcastTicker(nickname, msg);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestBroadcastTicker][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestBroadcastCubeWinRare(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("BroadcastCubeWinRare");

	try {

		uint32_t option = _packet->readUint32();
		auto msg = _packet->readString();

		m_owner_server.authCmdBroadcastCubeWinRare(msg, option);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestBroadcastCubeWinRare][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestDisconnectPlayer(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("DisconnectPlayer");

	try {

		uint32_t player_uid = _packet->readUint32();
		uint32_t server_uid = _packet->readUint32();
		unsigned char force = _packet->readUint8();			// Flag que força a disconectar o usuário

		m_owner_server.authCmdDisconnectPlayer(server_uid, player_uid, force);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestConfirmDisconnectPlayer(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("ConfirmDisconnectPlayer");

	try {

		uint32_t player_uid = _packet->readUint32();

		m_owner_server.authCmdConfirmDisconnectPlayer(player_uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestConfirmDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestNewMailArrivedMailBox(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("NewMailArrivedMailBox");

	try {

		uint32_t player_uid = _packet->readUint32();
		uint32_t mail_id = _packet->readUint32();

		m_owner_server.authCmdNewMailArrivedMailBox(player_uid, mail_id);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestNewMailArrivedMailBox][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestNewRate(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("NewRate");

	try {

		uint32_t tipo = _packet->readUint32();
		uint32_t qntd = _packet->readUint32();

		m_owner_server.authCmdNewRate(tipo, qntd);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestNewRate][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestReloadSystem(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("ReloadSystem");

	try {

		uint32_t sistema = _packet->readUint32();

		m_owner_server.authCmdReloadGlobalSystem(sistema);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestReloadSystem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestInfoPlayerOnline(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("GetInfoPlayerOnline");

	try {

		uint32_t req_server_uid = _packet->readUint32();
		uint32_t player_uid = _packet->readUint32();

		m_owner_server.authCmdInfoPlayerOnline(req_server_uid, player_uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestConfirmSendInfoPlayerOnline(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("ConfirmSendInfoOnline");

	try {

		AuthServerPlayerInfo aspi{ 0 };

		uint32_t req_server_uid = _packet->readUint32();
		
		aspi.option = _packet->readInt32();
		aspi.uid = _packet->readUint32();

		if (aspi.option == 1) {

			aspi.id = _packet->readString();
			aspi.ip = _packet->readString();

		}

		m_owner_server.authCmdConfirmSendInfoPlayerOnline(req_server_uid, aspi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestConfirmSendInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestSendCommandToOtherServer(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("SendCommandToOtherServer");

	try {

		m_owner_server.authCmdSendCommandToOtherServer(*_packet);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestSendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::requestSendReplyToOtherServer(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("SendReplyToOtherServer");

	try {

		m_owner_server.authCmdSendReplyToOtherServer(*_packet);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::requestSendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::sendConfirmDisconnectPlayer(uint32_t _server_uid, uint32_t _player_uid) {

	if (!isLive())
		throw exception("[unit_auth_server_connect::sendConfirmDisconnectPlayer][Error] Nao pode enviar o comando confirm disconnect player para o Auth Server, por que nao esta conectado com ele.",
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 50, 0));

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendConfirmDisconnectPlayer][Log] Send Confirm Disconnect Player[UID=" + std::to_string(_player_uid) 
				+ "] para o Auth Server enviar a resposta para o Server[UID=" + std::to_string(_server_uid) + "]", CL_FILE_LOG_AND_CONSOLE));

		packet p((unsigned short)0x3);

		p.addUint32(_player_uid);
		p.addUint32(_server_uid);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendConfirmDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::sendDisconnectPlayer(uint32_t _server_uid, uint32_t _player_uid) {

	if (!isLive())
		throw exception("[unit_auth_server_connect::sendDisconnectPlayer][Error] Nao pode enviar o comando disconnect player para o Auth Server, por que nao esta conectado com ele.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 50, 0));

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendDisconnectPlayer][Log] Send Disconnect Player[UID=" + std::to_string(_player_uid) 
				+ "] para o Auth Server enviar para o Server[UID=" + std::to_string(_server_uid) + "]", CL_FILE_LOG_AND_CONSOLE));

		packet p((unsigned short)0x2);

		p.addUint32(_player_uid);
		p.addUint32(_server_uid);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::sendInfoPlayerOnline(uint32_t _server_uid, AuthServerPlayerInfo _aspi) {

	if (!isLive())
		throw exception("[unit_auth_server_connect::sendInfoPlayerOnline][Error] Nao pode enviar o comando disconnect player para o Auth Server, por que nao esta conectado com ele.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 50, 0));

	try {

		// Log
		if (_aspi.option == 1)
			_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendInfoPlayerOnline][Log] Send Info Player[UID=" + std::to_string(_aspi.uid)
					+ "], para o Auth Server enviar para o Server[UID=" + std::to_string(_server_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
		else
			_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendInfoPlayerOnline][Log] Error nao encontrou o Player[UID=" + std::to_string(_aspi.uid)
					+ "] online para enviar o info dele para o Auth Server enviar para o Server[UID=" + std::to_string(_server_uid) + "]", CL_FILE_LOG_AND_CONSOLE));

		packet p((unsigned short)0x5);

		p.addUint32(_server_uid);
		p.addInt32(_aspi.option);
		p.addUint32(_aspi.uid);

		if (_aspi.option == 1) {

			p.addString(_aspi.id);
			p.addString(_aspi.ip);
		}

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::getInfoPlayerOnline(uint32_t _server_uid, uint32_t _player_uid) {

	if (!isLive())
		throw exception("[unit_auth_server_connect::getInfoPlayerOnline][Error] Nao pode enviar o comando disconnect player para o Auth Server, por que nao esta conectado com ele.",
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 50, 0));

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::getInfoPlayerOnline][Log] Pede o Info do Player[UID=" + std::to_string(_player_uid) 
				+ "] Online para o Auth Server pedir para o Server[UID=" + std::to_string(_server_uid) + "]", CL_FILE_LOG_AND_CONSOLE));

		packet p((unsigned short)0x4);

		p.addUint32(_server_uid);
		p.addUint32(_player_uid);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::getInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::sendCommandToOtherServer(uint32_t _server_uid, packet& _packet) {

	if (!isLive())
		throw exception("[unit_auth_server_connect::sendCommandToOtherServer][Error] Nao pode enviar o comando[ID=" + std::to_string(_packet.getTipo()) 
				+ "] para o outro server[UID=" + std::to_string(_server_uid)  + "] com o Auth Server, por que nao esta conectado com ele.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 50, 0));

	try {

		// Ler o Command ID para verificar se está tudo ok
		_packet.readUint16();

		if (_packet.getSize() < 2)
			throw exception("[unit_auth_server_connect::sendCommandToOtherServer][Error] Tentou enviar o comando[ID=" + std::to_string(_packet.getTipo())
					+ "] para o outro server[UID=" + std::to_string(_server_uid) + "] com o Auth Server, mas o packet eh invalido nao tem nem o id.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 1000, 0));

		unsigned short command_buff_size = (unsigned short)(_packet.getSize() - 2u);

		CommandOtherServerHeaderEx cosh{ 0 };

		cosh.send_server_uid_or_type = _server_uid;
		cosh.command_id = _packet.getTipo();

		// Inicializa comando buffer
		cosh.command.init(command_buff_size);

		if (!cosh.command.is_good())
			throw exception("[unit_auth_server_connect::sendCommandToOtherServer][Error] Tentou enviar a reposta[ID=" + std::to_string(_packet.getTipo())
					+ "] para o outro server[UID=" + std::to_string(_server_uid)
					+ "] com o Auth Server, mas nao conseguiu allocar memoria para o command buffer. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 1001, 0));

		_packet.readBuffer(cosh.command.buff, cosh.command.size);

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendCommandToOtherServer][Log] Envia comando[ID=" + std::to_string(_packet.getTipo())
				+ "] para outro server[UID=" + std::to_string(_server_uid) + "] com o Auth Server.", CL_FILE_LOG_AND_CONSOLE));

		// Envia o comando para o Auth Server enviar para o outro server
		packet p((unsigned short)0x06);

		p.addBuffer(&cosh, sizeof(CommandOtherServerHeader));

		if (command_buff_size > 0 && cosh.command.size > 0)
			p.addBuffer(cosh.command.buff, cosh.command.size);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::sendReplyToOtherServer(uint32_t _server_uid, packet& _packet) {

	if (!isLive())
		throw exception("[unit_auth_server_connect::sendReplyToOtherServer][Error] Nao pode enviar a resposta[ID=" + std::to_string(_packet.getTipo()) 
				+ "] para o outro server[UID=" + std::to_string(_server_uid) + "] com o Auth Server, por que nao esta conectado com ele.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 50, 0));

	try {


		// Ler a Resposta ID para verificar se está tudo ok
		_packet.readUint16();

		if (_packet.getSize() < 2)
			throw exception("[unit_auth_server_connect::sendReplyToOtherServer][Error] Tentou enviar a reposta[ID=" + std::to_string(_packet.getTipo()) 
					+ "] para o outro server[UID=" + std::to_string(_server_uid) + "] com o Auth Server, mas o packet eh invalido nao tem nem o id.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 1000, 0));

		unsigned short command_buff_size = (unsigned short)(_packet.getSize() - 2u);

		CommandOtherServerHeaderEx cosh{ 0 };

		cosh.send_server_uid_or_type = _server_uid;
		cosh.command_id = _packet.getTipo();
		
		// Inicializa comando buffer
		cosh.command.init(command_buff_size);

		if (!cosh.command.is_good())
			throw exception("[unit_auth_server_connect::sendReplyToOtherServer][Error] Tentou enviar a reposta[ID=" + std::to_string(_packet.getTipo())
					+ "] para o outro server[UID=" + std::to_string(_server_uid) 
					+ "] com o Auth Server, mas nao conseguiu allocar memoria para o command buffer. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_AUTH_SERVER_CONNECT, 1001, 0));

		_packet.readBuffer(cosh.command.buff, cosh.command.size);

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendReplyToOtherServer][Log] Envia resposta[ID=" + std::to_string(_packet.getTipo())
				+ "] para o outro server[UID=" + std::to_string(_server_uid) + "] com o Auth Server.", CL_FILE_LOG_AND_CONSOLE));

		// Envia a resposta para o Auth Server enviar para o outro server
		packet p((unsigned short)0x07);

		p.addBuffer(&cosh, sizeof(CommandOtherServerHeader));

		if (command_buff_size > 0 && cosh.command.size > 0)
			p.addBuffer(cosh.command.buff, cosh.command.size);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::sendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::onHeartBeat() {

	// Aqui Faz a verificação se está connectado com o auth server
	try {

		if (m_state != STATE::INITIALIZED)
			return;

		DWORD errCode = 0u;

#if defined(_WIN32)
		if (!m_session.isConnected() && (errCode = WaitForSingleObject(hEventTryConnect, 0)) == WAIT_OBJECT_0)
			ConnectAndAssoc();
		else if (errCode != 0 && errCode != WAIT_TIMEOUT)
			_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onHeartBeat][Error] tentou esperar pelo evento de tentar conectar com o Auth Server, mas deu error. Error Code: "
					+ std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
		if (!m_session.isConnected() && (hEventTryConnect != nullptr && (errCode = hEventTryConnect->wait(0)) == WAIT_OBJECT_0))
			ConnectAndAssoc();
		else if (errCode != 0 && errCode != WAIT_TIMEOUT)
			_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onHeartBeat][Error] tentou esperar pelo evento de tentar conectar com o Auth Server, mas deu error. Error Code: "
					+ std::to_string(errno), CL_FILE_LOG_AND_CONSOLE));
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onHearBeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::onConnected() {

	try {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onConnected][Log] Connectou com o Auth Server: " + m_unit_ctx.ip + ":" + std::to_string(m_unit_ctx.port), CL_ONLY_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onConnected][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::onDisconnect() {

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onDisconnect][Log] Desconectou do Auth Server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_auth_server_connect::onDisconnect][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_auth_server_connect::config_init() {

	m_reader_ini.init();

	m_unit_ctx.ip = m_reader_ini.readString("AUTHSERVER", "IP");
	m_unit_ctx.port = m_reader_ini.readInt("AUTHSERVER", "PORT");

	// Carregou com sucesso
	m_unit_ctx.state = true;
}
