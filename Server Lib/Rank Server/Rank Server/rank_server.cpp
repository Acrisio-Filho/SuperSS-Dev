// Arquivo rank_server.cpp
// Criado em 15/06/2020 as 14:48 por Acrisio
// Implementa��o da classe rank_server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "rank_server.hpp"

#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/TYPE/stdAType.h"

#include "../PACKET/packet_func_rs.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_update_rank_registry.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_insert_block_ip.hpp"

#include "../PANGYA_DB/cmd_rank_config_info.hpp"

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[rank_server::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[rank_server::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 6, 0)); \

// Verifica se session est� autorizada para executar esse a��o, 
// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!_session.m_is_authorized) \
												throw exception("[rank_server::request" + std::string((method)) + "][Error] Player[UID=" + std::to_string(_session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1, 0x5000501)); \

using namespace stdA;

rank_server::rank_server() : server(m_player_manager, 1, 4, 4), m_player_manager(*this, m_si.max_user),
	m_rank_registry_manager(), m_refresh_time(), m_sync_update_time_refresh(0u) {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[rank_server::rank_server][Error] falha ao incializar o rank server.", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		config_init();

		// Carrega IFF_STRUCT
		if (!sIff::getInstance().isLoad())
			sIff::getInstance().load();

		// Request Cliente
		packet_func::funcs.addPacketCall(0x00, packet_func::packet000, this);
		packet_func::funcs.addPacketCall(0x01, packet_func::packet001, this);
		packet_func::funcs.addPacketCall(0x02, packet_func::packet002, this);
		packet_func::funcs.addPacketCall(0x03, packet_func::packet003, this);
		packet_func::funcs.addPacketCall(0x04, packet_func::packet004, this);
		packet_func::funcs.addPacketCall(0x05, packet_func::packet005, this);
		

		// Resposta Server
		packet_func::funcs_sv.addPacketCall(0x1388, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x1389, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x138A, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x138B, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x138C, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x138D, packet_func::packet_svFazNada, this);

		// Auth Server
		packet_func::funcs_as.addPacketCall(0x01, packet_func::packet_as001, this);

		// Inicializado completo
		m_state = INITIALIZED;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::rank_server][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;
	}
}

rank_server::~rank_server() {
}

void rank_server::requestLogin(player& _session, packet *_packet) {
	REQUEST_BEGIN("Login");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();
		std::string id = _packet->readString();

		_packet->readBuffer(&_session.m_pi.m_sd, sizeof(search_dados_ex));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] UID: " + std::to_string(uid), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] ID: " + id, CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] SERACH: {\n" + _session.m_pi.m_sd.toString() + "\n}", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] UID: " + std::to_string(uid), CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] ID: " + id, CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] SERACH: {\n" + _session.m_pi.m_sd.toString() + "\n}", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		if (uid == 0)
			throw exception("[rank_server::requestLogin][Error] player[UID=" + std::to_string(uid) + ", ID=" 
					+ id + "] tentou logar com Server, mas o uid eh invalido. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1, 0x5200101));

		if (id.empty())
			throw exception("[rank_server::requestLogin][Error] player[UID=" + std::to_string(uid) + ", ID=" 
					+ id + "] tentou logar com Server, mas o id esta vazio. Hacker ou Bug",  STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 2, 0x5200102));

		// Verifica se o IP/MAC Address est� banido
		if (haveBanList(_session.getIP(), "", false/*N�o tem MAC Address esse pacote*/))
			throw exception("[rank_server::requestLogin][Error] Player[UID=" + std::to_string(uid) + ", ID=" + id + ", IP=" + _session.getIP()
					+ "] tentou logar com o Server, mas ele esta com IP banido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 5, 0x5200105));

		CmdPlayerInfo cmd_pi(uid, true);	// Waiter

		NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

		cmd_pi.waitEvent();

		if (cmd_pi.getException().getCodeError() != 0)
			throw cmd_pi.getException();

		*(player_info*)&_session.m_pi = cmd_pi.getInfo();

		if (id.compare(_session.m_pi.id) != 0)
			throw exception("[rank_server::requestLogin][Error] player[UID=" + std::to_string(uid) + ", ID=" 
					+ id + "] tentou logar com Server, mas o id do databse[ID_DB=" + std::string(_session.m_pi.id) + "] eh diferente do fornecido pelo cliente. Hacker ou Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 4, 0x5200104));

		// Verifica se o player est� bloqueado
		if (_session.m_pi.block_flag.m_id_state.id_state.ull_IDState != 0) {

			if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_TEMPORARY && (_session.m_pi.block_flag.m_id_state.block_time == -1 || _session.m_pi.block_flag.m_id_state.block_time > 0)) {

				throw exception("[rank_server::requestLogin][Log] Bloqueado por tempo[Time="
						+ (_session.m_pi.block_flag.m_id_state.block_time == -1 ? std::string("indeterminado") : (std::to_string(_session.m_pi.block_flag.m_id_state.block_time / 60)
						+ "min " + std::to_string(_session.m_pi.block_flag.m_id_state.block_time % 60) + "sec"))
						+ "]. player [UID=" + std::to_string(_session.m_pi.uid) + ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1029, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_FOREVER) {

				throw exception("[rank_server::requestLogin][Log] Bloqueado permanente. player [UID=" + std::to_string(_session.m_pi.uid)
						+ ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1030, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_ALL_IP) {

				// Bloquea todos os IP que o player logar e da error de que a area dele foi bloqueada

				// Add o ip do player para a lista de ip banidos
				NormalManagerDB::add(1, new CmdInsertBlockIP(_session.m_ip, "255.255.255.255"), rank_server::SQLDBResponse, this);

				// Resposta
				throw exception("[rank_server::requestLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + ", IP=" + std::string(_session.m_ip)
						+ "] Block ALL IP que o player fizer login.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1031, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_MAC_ADDRESS) {

				// Bloquea o MAC Address que o player logar e da error de que a area dele foi bloqueada

				// Add o MAC Address do player para a lista de MAC Address banidos
				//NormalManagerDB::add(2, new CmdInsertBlockMAC(mac_address), rank_server::SQLDBResponse, this);

				// Resposta
				throw exception("[rank_server::requestLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ ", IP=" + std::string(_session.m_ip) + ", MAC=UNKNON] (RANK nao recebe o MAC Address do cliente) Block MAC Address que o player fizer login.",  
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1032, 0));

			}else if (_session.m_pi.block_flag.m_flag.stBit.rank_server) {

				// Player est� bloqueado no Rank Server, ele n�o pode logar no rank server

				// Resposta
				throw exception("[rank_server::requestLogin][Log][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ ", IP=" + std::string(_session.m_ip) + "] foi bloqueado o acesso ao Rank Server pelo ADMIN no block_flag.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1033, 0));
			}
		}

		// Verifica se j� tem outro socket com o mesmo uid conectado
		auto s = HasLoggedWithOuterSocket(_session);

		if (s != nullptr) {

			_smp::message_pool::getInstance().push(new message("[rank_server::requestLogin][Log] Player[UID=" + std::to_string(uid) + ", OID="
					+ std::to_string(_session.m_oid) + ", IP=" + _session.getIP() + "] que esta logando agora, ja tem uma outra session com o mesmo UID logado, desloga o outro Player[UID="
					+ std::to_string(s->getUID()) + ", OID=" + std::to_string(s->m_oid) + ", IP=" + s->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

			if (!DisconnectSession(s))
				throw exception("[rank_server::requestLogin][Error] Nao conseguiu disconnectar o player[UID=" + std::to_string(s->getUID())
						+ "OID=" + std::to_string(s->m_oid) + ", IP=" + s->getIP() + "], ele pode esta com o bug do oid bloqueado, ou Session::UsaCtx bloqueado.",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 3, 0x5200103));
		}

		// S� verifica com o game server se ele n�o estiver autorizado(Logado)
		if (!_session.m_is_authorized) {

			// Verifica com o Auth Server se o player est� connectado no server que ele diz e se � o mesmo IP ADDRESS
			if (m_unit_connect->isLive()) {

				m_unit_connect->getInfoPlayerOnline(_session.m_pi.server_uid, _session.m_pi.uid);

			}else
				throw exception("[rank_server::requestLogin][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] tentou logar, mas nao conseguiu verificar com o Auth Server se ele estava online no Server[UID=" 
						+ std::to_string(_session.m_pi.server_uid) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 6, 0x5200106));
		
		}else {

			// Resposta para o Pedido de Login
			sendFirstPage(_session, 0);
		}

	}catch (exception& e) {

		// Resposta
		sendFirstPage(_session, 1);

		// Disconnect
#if defined(_WIN32)
		::shutdown(_session.m_sock, SD_BOTH);
#elif defined(__linux__)
		::shutdown(_session.m_sock.fd, SD_BOTH);
#endif

		_smp::message_pool::getInstance().push(new message("[message_server::requestLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::requestPlayerInfo(player& _session, packet *_packet) {
	REQUEST_BEGIN("PlayerInfo");

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		//CHECK_SESSION_IS_AUTHORIZED("PlayerInfo");
		// Request Player Info n�o usa por que ele manda junto com o de logar/pesquisar

		uint32_t uid = _packet->readUint32();
		std::string id = _packet->readString();
		unsigned char active = _packet->readUint8();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[rank_server::requestPlayerInfo][Log] UID: " + std::to_string(uid), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[rank_server::requestPlayerInfo][Log] ID: " + id, CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("[rank_server::requestPlayerInfo][Log] ACTIVE: " + std::to_string(active), CL_FILE_LOG_AND_CONSOLE));
#endif

		m_rank_registry_manager.sendPlayerFullInfo(_session, uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::requestPlayerInfo][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::sendFirstPage(player& _session, int _option) {
	CHECK_SESSION_BEGIN("sendFirstPage");

	packet p((unsigned short)0x1389);

	if (_option != 0) {
		p.addUint8((unsigned char)_option);

		p.addZeroByte(14u);
	}else {

		p.addUint8(_option);

		p.addUint8(_session.m_pi.m_sd.rank_menu);
		p.addUint8(_session.m_pi.m_sd.rank_menu_item);

		// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
		p.addUint8(_session.m_pi.m_sd.term_s5_type);

		// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
		p.addUint8(_session.m_pi.m_sd.class_type);

		m_rank_registry_manager.pageToPacket(p, _session.m_pi.m_sd);

		// Resposta da requisi��o do player
		// 0 - player est� no rank entre os player colocados. Ex (Top 100)
		// 1 - player n�o est� no rank
		// 2 - player est� no rank, por�m ele n�o est� no top. Ex (Top 100)

		if (_session.m_pi.m_sd.active)
			m_rank_registry_manager.playerPositionToPacket(p, _session, _session.m_pi.m_sd);
		else
			p.addUint8(ePLAYER_POSITION_RANK_TYPE::PPRT_NOT_TOP_RANK);
	}

	packet_func::session_send(p, &_session, 1);
}

void rank_server::updateTimeRefresh(uint32_t _ret, std::string _date) {

	try {

		if (_ret == 0/*Error*/) {

			_smp::message_pool::getInstance().push(new message("[rank_server::updateTimeRefresh][Error] Nao conseguiu atualizar os registro do Rank no banco de dados.", CL_FILE_LOG_AND_CONSOLE));
			

			// Libera o HearBeat tentar atualizar o registros de novo
			_smp::message_pool::getInstance().push(new message("[rank_server::updateTimeRefresh][Log] Libera o HearBeat tentar atualizar o registros de novo.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
			InterlockedExchange(&m_sync_update_time_refresh, 0u);
#elif defined(__linux__)
			__atomic_store_n(&m_sync_update_time_refresh, 0u, __ATOMIC_RELAXED);
#endif
		
		}else if (_ret == 1/*Sucesso*/) {

			// Atualiza tempo e recarregar o registro do Rank novamente
			m_refresh_time.setLastRefreshDate(_date);

			m_rank_registry_manager.load();

			// Cria arquivo de log, com todos os registros
			m_rank_registry_manager.makeLog();

			// Log
			_smp::message_pool::getInstance().push(new message("[rank_server::updateTimeRefresh][Log] Atualizou os registro do Rank Com sucesso. Rank Refresh[" 
					+ m_refresh_time.toString() + "]", CL_FILE_LOG_AND_CONSOLE));

			// Libera o HearBeat para verificar de novo quando tempo vai acabar
#if defined(_WIN32)
			InterlockedExchange(&m_sync_update_time_refresh, 0u);
#elif defined(__linux__)
			__atomic_store_n(&m_sync_update_time_refresh, 0u, __ATOMIC_RELAXED);
#endif
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::updateTimeRefresh][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::requestSearchPlayerInRank(player& _session, packet *_packet) {
	REQUEST_BEGIN("SearchPlayerInRank");

	enum SEARCH_OPTION : unsigned char {
		SO_NICKNAME,
		SO_POSITION,
	};

	packet p;

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("SearchPlayerInRank");

		unsigned char option = _packet->readUint8();

		if (option == SO_NICKNAME) {

			std::string nickname = _packet->readString();

			if (nickname.empty())
				throw exception("[rank_server::requestSearchPlayerInRank][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] pediu para pesquisar um player no rank, mas o nickname eh invalid(empty). Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 331, 0));

			search_dados sd{ 0u };

			_packet->readBuffer(&sd, sizeof(search_dados));

			m_rank_registry_manager.searchPlayerByNicknameAndSendPage(_session, nickname, sd);

		}else if (option == SO_POSITION) {

			uint32_t position = _packet->readUint32();

			if ((int)position < 0)
				throw exception("[rank_server::requestSearchPlayerInRank][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] pediu para pesquisar um player no rank, as position no rank value eh invalid(" + std::to_string(position) 
						+ "). Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 332, 0));

			search_dados sd{ 0u };

			_packet->readBuffer(&sd, sizeof(search_dados));

			m_rank_registry_manager.searchPlayerByRankAndSendPage(_session, position, sd);

		}else
			throw exception("[rank_server::requestSearchPlayerInRank][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] pediu para pesquisar um player no rank, mas a option(" + std::to_string((unsigned short)option)
					+ ") que ele passou eh invalid. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 330, 0));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::requestSearchPlayerInRank][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error
		p.init_plain((unsigned short)0x138C);

		p.addUint8(1u);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void rank_server::confirmLoginOnOtherServer(player& _session, uint32_t _req_server_uid, AuthServerPlayerInfo& _aspi) {
	CHECK_SESSION_BEGIN("confirmLoginOnOtherServer");

	packet p;

	try {

		if (_aspi.uid != _session.m_pi.uid)
			throw exception("[rank_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid) 
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao eh o mesmo UID que foi retornado do request com o Auth Server. Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 1, 0x5200201));

		if (_aspi.option != 1)
			throw exception("[rank_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid)
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao esta online no outro server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 2, 0x5200202));
		
		if (_aspi.id.compare(_session.m_pi.id) != 0)
			throw exception("[rank_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid)
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao eh o mesmo ID[ID=" + _session.m_pi.id + ", REQ_ID=" + _aspi.id 
					+ "] que foi retornado do request com o Auth Server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 3, 0x5200203));
		
		if (_aspi.ip.compare(_session.getIP()) != 0)
			throw exception("[rank_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid)
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao eh o mesmo IP[IP=" + _session.getIP() + ", REQ_IP=" + _aspi.ip 
					+ "] que foi retornado do request com o Auth Server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 4, 0x5200204));

		// Confirm Login com sucesso, Atualiza o cliente

		// Logado [Online] -- isso aqui � do MSN que compiei o projeto do msn para fazer esse mais r�pido
		_session.m_pi.m_state = 4;

		// Authorized a ficar online no server por tempo indeterminado
		_session.m_is_authorized = 1u;

		// Log
		_smp::message_pool::getInstance().push(new message("[rank_server::confirmLoginOnOtherServer][Log] player[UID=" + std::to_string(_session.m_pi.uid) 
				+ ", NICKNAME=" + std::string(_session.m_pi.nickname) + "] logou com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		// Resposta para o Pedido de Login
		sendFirstPage(_session, 0);

	} catch (exception& e) {

		// Resposta
		sendFirstPage(_session, 1);

		// Disconnect
#if defined(_WIN32)
		::shutdown(_session.m_sock, SD_BOTH);
#elif defined(__linux__)
		::shutdown(_session.m_sock.fd, SD_BOTH);
#endif

		_smp::message_pool::getInstance().push(new message("[rank_server::confirmLoginOnOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::authCmdShutdown(int32_t _time_sec) {

	try {

		// Shut down com tempo
		if (m_shutdown == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[rank_server::authCmdShutdown][Log] Auth Server requisitou para o server ser desligado em "
					+ std::to_string(_time_sec) + " segundos", CL_FILE_LOG_AND_CONSOLE));

			shutdown_time(_time_sec);

		}else
			_smp::message_pool::getInstance().push(new message("[rank_server::authCmdShutdown][WARNING] Auth Server requisitou para o server ser delisgado em "
					+ std::to_string(_time_sec) + " segundos, mas o server ja esta com o timer de shutdown", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[rank_server::authCmdShutdown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::authCmdBroadcastNotice(std::string _notice) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdBroadcastTicker(std::string _nickname, std::string _msg) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdBroadcastCubeWinRare(std::string _msg, uint32_t _option) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) {

	// Aqui no Rank Server Ainda n�o usa o force, ele desconecta o player do mesmo jeito
	UNREFERENCED_PARAMETER(_force);

	try {

		auto s = m_player_manager.findPlayer(_player_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[rank_server::authCmdDisconnectPlayer][log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(s->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Deconecta o Player
			DisconnectSession(s);

			// UPDATE ON Auth Server
			m_unit_connect->sendConfirmDisconnectPlayer(_req_server_uid, _player_uid);

		}else
			_smp::message_pool::getInstance().push(new message("[rank_server::authCmdDisconnectPlayer][WARNING] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(_player_uid) + "], mas nao encontrou ele no server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::authCmdDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::authCmdConfirmDisconnectPlayer(uint32_t _player_uid) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdNewMailArrivedMailBox(uint32_t _player_uid, uint32_t _mail_id) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdNewRate(uint32_t _tipo, uint32_t _qntd) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdReloadGlobalSystem(uint32_t _tipo) {
	// Rank Server n�o usa esse Comando
	return;
}

void rank_server::authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) {

	try {

		auto s = m_player_manager.findPlayer(_aspi.uid);

		if (s != nullptr) {

			// Confirma Login com outro server
			confirmLoginOnOtherServer(*s, _req_server_uid, _aspi);

		}else
			_smp::message_pool::getInstance().push(new message("[rank_server::authCmdConfirmSendInfoPlayerOnline][WARNING] Player[UID=" + std::to_string(_aspi.uid) 
					+ "] retorno do confirma login com Auth Server do Server[UID=" + std::to_string(_req_server_uid) + "], mas o palyer nao esta mais conectado.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::authCmdConfirmSendInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void rank_server::shutdown_time(int32_t _time_sec) {

	if (_time_sec <= 0)	// Desliga o Server Imediatemente
		shutdown();
	else {

		job _job(server::end_time_shutdown, this, (void*)0);

		// Se o Shutdown Timer estiver criado descria e cria um novo
		if (m_shutdown != nullptr) {

			// Para o Tempo se ele n�o estiver parado
			if (m_shutdown->getState() != timer::STOPPED)
				m_shutdown->stop();

			m_timer_mgr.deleteTimer(m_shutdown);
		}

		if ((m_shutdown = m_timer_mgr.createTimer(_time_sec * 1000, new (timer::timer_param){ _job, m_job_pool })) == nullptr)
			throw exception("[rank_server::shutdown_time][Error] nao conseguiu criar o timer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 51, 0));
	}
}

void rank_server::onAcceptCompleted(session* _session) {

	if (_session == nullptr)
		throw exception("[rank_server::onAcceptCompleted][Error] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 60, 0));

	if (!_session->getState())
		throw exception("[rank_server::onAcceptCompleted][Error] _session is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 61, 0));

	if (!_session->isConnected())
		throw exception("[rank_server::onAcceptCompleted][Error] _session is not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 62, 0));

	packet p((unsigned short)0x1388);

	// Se mandar -1 no valor da chave o cliente n�o encripta o pacote antes de enviar
	p.addInt32(_session->m_key); // Key
	p.addUint8(5u); // Type Rank Server
	p.addString(formatDateLocal(0/*Atual Date Local*/));

	p.makeRaw();
	WSABUF mb = p.getMakedBuf();

	//SHOW_DEBUG_FINAL_PACKET(mb, 1);

	try {
		_session->requestRecvBuffer();
		_session->requestSendBuffer(mb.buf, mb.len);
	}catch (exception& e) {

		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 1))
			throw;
	}
}

void rank_server::onDisconnected(session* _session) {

	if (_session == nullptr)
		throw exception("[rank_server::onDisconnect][Error] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANK_SERVER, 60, 0));

	player *p = reinterpret_cast< player* >(_session);

	_smp::message_pool::getInstance().push(new message("[rank_server::onDisconnected][Log] Player Desconectou ID: " + std::string(p->m_pi.id) + " UID: " + std::to_string(p->m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

	// Aqui n�o faz nada, no rank server por enquanto
}

void rank_server::onHeartBeat() {

	try {

#if defined(_WIN32)
		if (m_state == INITIALIZED && InterlockedCompareExchange(&m_sync_update_time_refresh, 0u, 0u) == 0u && m_refresh_time.isOutDated()) {
#elif defined(__linux__)
		uint32_t check_m = 0; // Compare
		if (m_state == INITIALIZED && __atomic_compare_exchange_n(&m_sync_update_time_refresh, &check_m, 0u, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED) && m_refresh_time.isOutDated()) {
#endif

			// Log
			_smp::message_pool::getInstance().push(new message("[rank_server::onHeartBeat][Log] Passou da hora de atualizar os registros do Rank.", CL_FILE_LOG_AND_CONSOLE));
			
			// Trava update check, para n�o ficar enviando varias requisi��o para atualizar os registro para o banco de dados

#if defined(_WIN32)
			InterlockedExchange(&m_sync_update_time_refresh, 1u);
#elif defined(__linux__)
			__atomic_store_n(&m_sync_update_time_refresh, 1u, __ATOMIC_RELAXED);
#endif

			// Envia a requisi��o para o banco de dados
			NormalManagerDB::add(1, new CmdUpdateRankRegistry(), rank_server::SQLDBResponse, this);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::onHeartBeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
	
	return;
}

void rank_server::onStart() {

	try {

		// N�o faz nada por enquanto

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[rank_server::onStart][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool rank_server::checkCommand(std::stringstream& _command) {
	
	std::string s = "";

	_command >> s;

	if (!s.empty() && s.compare("exit") == 0)
		return true;	// Sai
	else if (!s.empty() && s.compare("reload_files") == 0) {

		reload_files();
		
		_smp::message_pool::getInstance().push(new message("Rank Server files has been reloaded.", CL_FILE_LOG_AND_CONSOLE));
	
	}else if (!s.empty() && s.compare("reload_socket_config") == 0) {

		// Ler novamento o arquivo de configura��o do socket
		if (m_accept_sock != nullptr)
			m_accept_sock->reload_config_file();
		else
			_smp::message_pool::getInstance().push(new message("[rank_server::checkCommand][WARNING] m_accept_sock(socket que gerencia os socket que pode aceitar etc) is invalid.", CL_FILE_LOG_AND_CONSOLE));

	}else if (!s.empty() && s.compare("make_log") == 0) {

		// Cria log de todos os registro em uma arquivo com data
		m_rank_registry_manager.makeLog();

	}else if (!s.empty() && s.compare("snapshot") == 0) {

		try {
			int *bad_ptr_snapshot = nullptr;
			*bad_ptr_snapshot = 2;
		}catch (exception& e) {
			UNREFERENCED_PARAMETER(e);

			// Log
			_smp::message_pool::getInstance().push(new message("[rank_server::checkCommand][Log] Snapshot comando executado.", CL_FILE_LOG_AND_CONSOLE));
		}

	}else
		_smp::message_pool::getInstance().push(new message("Unknown Command: " + s, CL_ONLY_CONSOLE));

	return false;
}

bool rank_server::checkPacket(session& _session, packet* _packet) {
	
	///--------------- INICIO CHECK PACKET SESSION
	if (_session.m_check_packet.checkPacketId(_packet->getTipo())) {
		
		uint32_t limit_count = CHK_PCKT_COUNT_LIMIT;	// Padr�o

		// Rank Server Vou deixar em 7x, por que � mais dificil atacar ele
		switch (_packet->getTipo()) {
		case 0:
		default:
			limit_count += 2;	// Sobe para 7 por que o rank server � mais dif�cil de atacar ele
		}

		if (_session.m_check_packet.incrementCount() >= limit_count) {

			_smp::message_pool::getInstance().push(new message("[rank_server::checkPacket][WARNING] Tentativa de DDoS ataque com pacote ID: (0x" 
					+ hex_util::lltoaToHex(_packet->getTipo()) + ") " + std::to_string(_packet->getTipo()) + ". IP=" + std::string(_session.m_ip), CL_FILE_LOG_AND_CONSOLE));

			DisconnectSession(&_session);

			return false;
		}
	}

	///--------------- FIM CHECK PACKET SESSION

	return true;
}

void rank_server::config_init() {

	// Tipo Server
	m_si.tipo = 4;

	// Carrega a configura��o do Rank
	CmdRankConfigInfo cmd_rci(true);	// Waiter

	NormalManagerDB::add(0, &cmd_rci, nullptr, nullptr);

	cmd_rci.waitEvent();

	if (cmd_rci.getException().getCodeError() != 0)
		throw cmd_rci.getException();

	m_refresh_time = cmd_rci.getInfo();

	// Log
	_smp::message_pool::getInstance().push(new message("[rank_server::config_init][Log] Carregou a configuracao do Rank Server[" 
			+ m_refresh_time.toString() + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));
}

void rank_server::reload_files() {

	server::config_init();
	config_init();

	sIff::getInstance().reload();
}

void rank_server::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[rank_server::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[rank_server::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_rank_server = reinterpret_cast< rank_server* >(_arg);

	switch (_msg_id) {
	case 1: // Update Rank Registros
	{
		auto cmd_urr = reinterpret_cast< CmdUpdateRankRegistry* >(&_pangya_db);

		if (cmd_urr->getException().getCodeError() != 0) {

			// Exception print no console
			_smp::message_pool::getInstance().push(new message("[rank_server::SQLDBResponse][Error] " + cmd_urr->getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			// Liberar o verificador no HearBeat
			_rank_server->updateTimeRefresh(0u/*Error*/, "");

		}else
			_rank_server->updateTimeRefresh(cmd_urr->getRetState(), cmd_urr->getDate());

		break;
	}
	case 0:
	default:
		break;
	}
}
