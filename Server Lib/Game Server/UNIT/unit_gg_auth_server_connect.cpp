// Arquivo unit_gg_auth_server_connect.cpp
// Criado em 02/02/2021 as 23:48 por Acrisio
// Implementação da classe unit_gg_auth_server_connect

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "unit_gg_auth_server_connect.hpp"

#define REQUEST_BEGIN(method) if (!_session.getState()) \
									throw exception("[unit_gg_auth_server_connect::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 1, 0)); \
							  if (_packet == nullptr) \
									throw exception("[unit_gg_auth_server_connect::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 6, 0)); \

#define BEGIN_PACKET_INIT(_num) funcs.addPacketCall((_num), [](void* _arg1, void* _arg2) { \
	ParamDispatchAS pd = *reinterpret_cast< ParamDispatchAS* >(_arg2); \
	unit_gg_auth_server_connect *uc = reinterpret_cast< unit_gg_auth_server_connect* >(_arg1); \
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

unit_gg_auth_server_connect::unit_gg_auth_server_connect(IUnitGGAuthServer& _owner_server, ServerInfoEx& _si)
	: unit_connect_base(_si), m_owner_server(_owner_server) {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::unit_gg_auth_server_connect][Error] na inicializacao unit gg auth server connect", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		// Inicializar Config do arquivo ini
		config_init();

		/// ---------- Packets ---------

		// Packet000
		BEGIN_PACKET_INIT(0x00);

		uc->requestFirstPacketKey(pd._session, pd._packet);

		END_PACKET_INIT("000");

		// Packet001
		BEGIN_PACKET_INIT(0x01);

		uc->requestAskLogin(pd._session, pd._packet);

		END_PACKET_INIT("001");

		// Packet002
		BEGIN_PACKET_INIT(0x2);

		uc->requestGetQuery(pd._session, pd._packet);

		END_PACKET_INIT("002");

		// Packet003
		BEGIN_PACKET_INIT(0x3);

		uc->requestCheckAnswer(pd._session, pd._packet);

		END_PACKET_INIT("003");

		/*// Packet004
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

		END_PACKET_INIT("00E");*/

		/// ----------- Pacotes -----------

		INIT_PACKET_SERVER_FAZ_NADA(0x01);

		// Pacote002
		INIT_PACKET_SERVER_FAZ_NADA(0x02);

		// Pacote003
		INIT_PACKET_SERVER_FAZ_NADA(0x3);

		// Pacote004
		INIT_PACKET_SERVER_FAZ_NADA(0x4);

		// Pacote005
		INIT_PACKET_SERVER_FAZ_NADA(0x5);

		// Pacote006
		INIT_PACKET_SERVER_FAZ_NADA(0x6);

		/*// Pacote007
		INIT_PACKET_SERVER_FAZ_NADA(0x7);*/

		// Initialized complete
		m_state = INITIALIZED;

	}catch (exception& e) {

		m_state = STATE::FAILURE;

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::unit_gg_auth_server_connect][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

unit_gg_auth_server_connect::~unit_gg_auth_server_connect() {
}

void unit_gg_auth_server_connect::requestFirstPacketKey(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("FirstPacketKey");

	try {

		_session.m_key = (unsigned char)_packet->readUint32();
		_session.m_pi.uid = m_session.m_si.uid;
		
		auto server_guid = _packet->readUint32();

		packet p((unsigned short)0x01);

		p.addUint32(_session.m_pi.uid);

		packet_func_as::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::requestFirstPacketKey][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::requestAskLogin(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("AskLogin");

	try {

		int32_t oid = _packet->readInt32();

		if (oid > -1) {

			_session.m_oid = oid;

			// Log
			_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::requestAskLogin][Log] Logou com o Game Guard Auth Server[OID=" + std::to_string(_session.m_oid) + "]", CL_FILE_LOG_AND_CONSOLE));
		}else
			_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::requestAskLogin][Log] Nao conseguiu logar com o Game Guard Auth Server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::requestAskLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::requestGetQuery(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("GetQuery");

	try {

		GG_AUTH_DATA data;

		uint32_t error = _packet->readUint32();
		uint32_t socket_id = _packet->readUint32();

		_packet->readBuffer(&data, sizeof(GG_AUTH_DATA));

		m_owner_server.ggAuthCmdGetQuery(error, socket_id, data);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::requestGetQuery][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::requestCheckAnswer(UnitPlayer& _session, packet *_packet) {
	REQUEST_BEGIN("CheckAnswer");

	try {

		uint32_t error = _packet->readUint32();
		uint32_t socket_id = _packet->readUint32();

		m_owner_server.ggAuthCmdCheckAnswer(error, socket_id);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::requestCheckAnswer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::sendHaskKeyLogin(std::string _key) {

	if (!isLive())
		throw exception("[unit_gg_auth_server_connect::sendHaskKeyLogin][Error] Nao pode enviar o hash key para o Game Guard Auth Server, por que nao esta conectado com ele.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 50, 0));

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::sendHaskKeyLogin][Log] Send Hash Key(" + _key + ") to Game Guard Auth Server.", CL_FILE_LOG_AND_CONSOLE));

		packet p((unsigned short)0x2);

		p.addString(_key);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::sendHaskKeyLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::sendCreateUser(uint32_t _socket_id) {

	if (!isLive())
		throw exception("[unit_gg_auth_server_connect::sendCreateUser][Error] Nao pode enviar o pedido para criar User Game Guard Auth do player para o Game Guard Auth Server, por que nao esta conectado com ele.",
			STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 50, 0));

	try {

		packet p((unsigned short)0x03);

		p.addUint32(_socket_id);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::sendCreateUser][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::sendDeleteUser(uint32_t _socket_id) {

	if (!isLive())
		throw exception("[unit_gg_auth_server_connect::sendDeleteUser][Error] Nao pode enviar o pedido para deletar User Game Guard Auth do player para o Game Guard Auth Server, por que nao esta conectado com ele.",
			STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 50, 0));

	try {

		packet p((unsigned short)0x04);

		p.addUint32(_socket_id);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::sendDeleteUser][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::sendGetQuery(uint32_t _socket_id) {

	if (!isLive())
		throw exception("[unit_gg_auth_server_connect::sendGetQuery][Error] Nao pode enviar o pedido de Auth Query do player para o Game Guard Auth Server, por que nao esta conectado com ele.",
			STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 50, 0));

	try {

		packet p((unsigned short)0x05);

		p.addUint32(_socket_id);

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::sendGetQuery][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::sendCheckAnswer(uint32_t _socket_id, GG_AUTH_DATA& _data) {

	if (!isLive())
		throw exception("[unit_gg_auth_server_connect::sendCheckAnswer][Error] Nao pode enviar o pedido de verificacao do Auth Answer do player para o Game Guard Auth Server, por que nao esta conectado com ele.",
			STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_GG_AUTH_SERVER_CONNECT, 50, 0));

	try {

		packet p((unsigned short)0x06);

		p.addUint32(_socket_id);

		p.addBuffer(&_data, sizeof(GG_AUTH_DATA));

		packet_func_as::session_send(p, &m_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::sendCheckAnswer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::onHeartBeat() {

	// Aqui Faz a verificação se está connectado com o Game Guard auth server
	try {

		if (m_state != STATE::INITIALIZED)
			return;

		DWORD errCode = 0u;

		if (!m_session.isConnected() 
#if defined(_WIN32)
			&& hEventTryConnect != INVALID_HANDLE_VALUE && (errCode = WaitForSingleObject(hEventTryConnect, 0)) == WAIT_OBJECT_0
#elif defined(__linux__)
			&& hEventTryConnect != nullptr && (errCode = hEventTryConnect->wait(0)) == WAIT_OBJECT_0
#endif
		)
			ConnectAndAssoc();
		else if (errCode != 0 && errCode != WAIT_TIMEOUT)
			_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::onHeartBeat][Error] tentou esperar pelo evento de tentar conectar com o Game Guard Auth Server, mas deu error. Error Code: "
					+ std::to_string(
#if defined(_WIN32)
						GetLastError()
#elif defined(__linux__)
						errno
#endif
					), CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::onHearBeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::onConnected() {

	try {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::onConnected][Log] Connectou com o Game Guard Auth Server: " + m_unit_ctx.ip + ":" + std::to_string(m_unit_ctx.port), CL_ONLY_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::onConnected][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::onDisconnect() {

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::onDisconnect][Log] Desconectou do Game Guard Auth Server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_gg_auth_server_connect::onDisconnect][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void unit_gg_auth_server_connect::config_init() {

	m_reader_ini.init();

	m_unit_ctx.ip = m_reader_ini.readString("GGAUTHSERVER", "IP");
	m_unit_ctx.port = m_reader_ini.readInt("GGAUTHSERVER", "PORT");

	// Carregou com sucesso
	m_unit_ctx.state = true;
}
