// Arquivo message_server.cpp
// Criado em 29/07/2018 as 13:19 por Acrisio
// Implementa��o da classe message_server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <mstcpip.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#endif

#include "message_server.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/TYPE/stdAType.h"

#include "../PACKET/packet_func_ms.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_insert_block_ip.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_update_rate_config_info.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_rate_config_info.hpp"

#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_friend_info.hpp"

#include "../../Projeto IOCP/Smart Calculator/Smart Calculator.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_verify_nick.hpp"

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[message_server::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[message_server::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0)); \

// Verifica se session est� autorizada para executar esse a��o, 
// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
#define CHECK_SESSION_IS_AUTHORIZED(method) if (!_session.m_is_authorized) \
												throw exception("[message_server::request" + std::string((method)) + "][Error] Player[UID=" + std::to_string(_session.m_pi.uid) \
														+ "] Nao esta autorizado a fazer esse request por que ele ainda nao fez o login com o Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5000501)); \

using namespace stdA;

message_server::message_server() : server(m_player_manager, 1, 12, 4), m_player_manager(*this, m_si.max_user) {

	if (m_state == FAILURE) {
		_smp::message_pool::getInstance().push(new message("[message_server::message_server][Error] falha ao incializar o message server.", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	try {

		config_init();

		// Carrega IFF_STRUCT
		if (!sIff::getInstance().isLoad())
			sIff::getInstance().load();

		// Request Cliente
		packet_func::funcs.addPacketCall(0x12, packet_func::packet012, this);
		packet_func::funcs.addPacketCall(0x13, packet_func::packet013, this);
		packet_func::funcs.addPacketCall(0x14, packet_func::packet014, this);
		packet_func::funcs.addPacketCall(0x16, packet_func::packet016, this);
		packet_func::funcs.addPacketCall(0x17, packet_func::packet017, this);
		packet_func::funcs.addPacketCall(0x18, packet_func::packet018, this);
		packet_func::funcs.addPacketCall(0x19, packet_func::packet019, this);
		packet_func::funcs.addPacketCall(0x1A, packet_func::packet01A, this);
		packet_func::funcs.addPacketCall(0x1B, packet_func::packet01B, this);
		packet_func::funcs.addPacketCall(0x1C, packet_func::packet01C, this);
		packet_func::funcs.addPacketCall(0x1D, packet_func::packet01D, this);
		packet_func::funcs.addPacketCall(0x1E, packet_func::packet01E, this);
		packet_func::funcs.addPacketCall(0x1F, packet_func::packet01F, this);
		packet_func::funcs.addPacketCall(0x23, packet_func::packet023, this);
		packet_func::funcs.addPacketCall(0x24, packet_func::packet024, this);
		packet_func::funcs.addPacketCall(0x25, packet_func::packet025, this);
		packet_func::funcs.addPacketCall(0x28, packet_func::packet028, this);
		packet_func::funcs.addPacketCall(0x29, packet_func::packet029, this);
		packet_func::funcs.addPacketCall(0x2A, packet_func::packet02A, this);
		packet_func::funcs.addPacketCall(0x2B, packet_func::packet02B, this);
		packet_func::funcs.addPacketCall(0x2C, packet_func::packet02C, this);
		packet_func::funcs.addPacketCall(0x2D, packet_func::packet02D, this);

		// Resposta Server
		packet_func::funcs_sv.addPacketCall(0x2E, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x2F, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x30, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x3B, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x3C, packet_func::packet_svFazNada, this);
		packet_func::funcs_sv.addPacketCall(0x40, packet_func::packet_svFazNada, this);	// Msg Aviso Lobby, cliente tamb�m aceita o Message Server enviar esse Pacote

		// Auth Server
		packet_func::funcs_as.addPacketCall(0x01, packet_func::packet_as001, this);
		packet_func::funcs_as.addPacketCall(0x02, packet_func::packet_as002, this);
		packet_func::funcs_as.addPacketCall(0x03, packet_func::packet_as003, this);

		// Initialized complete
		m_state = INITIALIZED;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::message_server][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;
	}
}

message_server::~message_server() {
}

void message_server::requestLogin(player& _session, packet *_packet) {
	REQUEST_BEGIN("Login");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();
		std::string nickname = _packet->readString();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("UID: " + std::to_string(uid), CL_FILE_LOG_AND_CONSOLE));
		_smp::message_pool::getInstance().push(new message("NICKNAME: " + nickname, CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("UID: " + std::to_string(uid), CL_ONLY_FILE_LOG));
		_smp::message_pool::getInstance().push(new message("NICKNAME: " + nickname, CL_ONLY_FILE_LOG));
#endif // _DEBUG

		if (uid == 0)
			throw exception("[message_server::requestLogin][Error] player[UID=" + std::to_string(uid) + ", NICKNAME=" 
					+ nickname + "] tentou logar com Server, mas o uid eh invalido. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200101));

		if (nickname.empty())
			throw exception("[message_server::requestLogin][Error] player[UID=" + std::to_string(uid) + ", NICKNAME=" 
					+ nickname + "] tentou logar com Server, mas o nickname esta vazio. Hacker ou Bug",  STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200102));

		// Verifica se o IP/MAC Address est� banido
		if (haveBanList(_session.getIP(), "", false/*N�o tem MAC Address esse pacote*/))
			throw exception("[message_server::requestLogin][Error] Player[UID=" + std::to_string(uid) + ", NICKNAME=" + nickname + ", IP=" + _session.getIP() 
					+ "] tentou logar com o Server, mas ele esta com IP banido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5200105));

		CmdPlayerInfo cmd_pi(uid, true);	// Waiter

		NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

		cmd_pi.waitEvent();

		if (cmd_pi.getException().getCodeError() != 0)
			throw cmd_pi.getException();

		*(player_info*)&_session.m_pi = cmd_pi.getInfo();

		if (nickname.compare(_session.m_pi.nickname) != 0)
			throw exception("[message_server::requestLogin][Error] player[UID=" + std::to_string(uid) + ", NICKNAME=" 
					+ nickname + "] tentou logar com Server, mas o nickname do databse[NICKNAME_DB=" + std::string(_session.m_pi.nickname) + "] eh diferente do fornecido pelo cliente. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200104));

		// Verifica se o player est� bloqueado
		if (_session.m_pi.block_flag.m_id_state.id_state.ull_IDState != 0) {

			if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_TEMPORARY && (_session.m_pi.block_flag.m_id_state.block_time == -1 || _session.m_pi.block_flag.m_id_state.block_time > 0)) {

				throw exception("[message_server::requestLogin][Log] Bloqueado por tempo[Time="
						+ (_session.m_pi.block_flag.m_id_state.block_time == -1 ? std::string("indeterminado") : (std::to_string(_session.m_pi.block_flag.m_id_state.block_time / 60)
						+ "min " + std::to_string(_session.m_pi.block_flag.m_id_state.block_time % 60) + "sec"))
						+ "]. player [UID=" + std::to_string(_session.m_pi.uid) + ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1029, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_FOREVER) {

				throw exception("[message_server::requestLogin][Log] Bloqueado permanente. player [UID=" + std::to_string(_session.m_pi.uid)
						+ ", ID=" + std::string(_session.m_pi.id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1030, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_ALL_IP) {

				// Bloquea todos os IP que o player logar e da error de que a area dele foi bloqueada

				// Add o ip do player para a lista de ip banidos
				NormalManagerDB::add(1, new CmdInsertBlockIP(_session.m_ip, "255.255.255.255"), message_server::SQLDBResponse, this);

				// Resposta
				throw exception("[message_server::requestLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + ", IP=" + std::string(_session.m_ip)
						+ "] Block ALL IP que o player fizer login.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1031, 0));

			}else if (_session.m_pi.block_flag.m_id_state.id_state.st_IDState.L_BLOCK_MAC_ADDRESS) {

				// Bloquea o MAC Address que o player logar e da error de que a area dele foi bloqueada

				// Add o MAC Address do player para a lista de MAC Address banidos
				//NormalManagerDB::add(2, new CmdInsertBlockMAC(mac_address), message_server::SQLDBResponse, this);

				// Resposta
				throw exception("[message_server::requestLogin][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ ", IP=" + std::string(_session.m_ip) + ", MAC=UNKNON] (MSG nao recebe o MAC Address do cliente) Block MAC Address que o player fizer login.",  
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1032, 0));

			}
		}

		// Verifica se j� tem outro socket com o mesmo uid conectado
		auto s = HasLoggedWithOuterSocket(_session);

		if (s != nullptr) {

			_smp::message_pool::getInstance().push(new message("[message_server::requestLogin][Log] Player[UID=" + std::to_string(uid) + ", OID="
					+ std::to_string(_session.m_oid) + ", IP=" + _session.getIP() + "] que esta logando agora, ja tem uma outra session com o mesmo UID logado, desloga o outro Player[UID="
					+ std::to_string(s->getUID()) + ", OID=" + std::to_string(s->m_oid) + ", IP=" + s->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

			if (!DisconnectSession(s))
				throw exception("[message_server::requestLogin][Error] Nao conseguiu disconnectar o player[UID=" + std::to_string(s->getUID())
						+ "OID=" + std::to_string(s->m_oid) + ", IP=" + s->getIP() + "], ele pode esta com o bug do oid bloqueado, ou Session::UsaCtx bloqueado.",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5200103));
		}

		// Verifica com o Auth Server se o player est� connectado no server que ele diz e se � o mesmo IP ADDRESS
		if (m_unit_connect->isLive()) {

			m_unit_connect->getInfoPlayerOnline(_session.m_pi.server_uid, _session.m_pi.uid);

		}else
			throw exception("[message_server::requestLogin][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou logar, mas nao conseguiu verificar com o Auth Server se ele estava online no Server[UID=" + std::to_string(_session.m_pi.server_uid) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5200106));

		/*// Loading Friend List
		_session.m_pi.m_friend_manager.init(_session.m_pi);

		// Logado [Online]
		_session.m_pi.m_state = 4;

		// Authorized a ficar online no server por tempo indeterminado
		_session.m_is_authorized = 1u;

		// Log
		_smp::message_pool::getInstance().push(new message("[Login][Log] player[UID=" + std::to_string(_session.m_pi.uid) + ", NICKNAME=" + std::string(_session.m_pi.nickname) + "] logou com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		// Resposta para o Pedido de Login
		p.init_plain((unsigned short)0x2F);

		p.addUint8(0);	// OK

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);*/

	}catch (exception& e) {

		// Resposta
		p.init_plain((unsigned short)0x2F);

		p.addUint8(1);	// Error;

		packet_func::session_send(p, &_session, 1);

		// Disconnect
#if defined(_WIN32)
		::shutdown(_session.m_sock, SD_BOTH);
#elif defined(__linux__)
		::shutdown(_session.m_sock.fd, SD_BOTH);
#endif

		_smp::message_pool::getInstance().push(new message("[message_server::requestLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::confirmLoginOnOtherServer(player& _session, uint32_t _req_server_uid, AuthServerPlayerInfo& _aspi) {
	CHECK_SESSION_BEGIN("confirmLoginOnOtherServer");

	packet p;

	try {

		if (_aspi.uid != _session.m_pi.uid)
			throw exception("[message_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid) 
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao eh o mesmo UID que foi retornado do request com o Auth Server. Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200201));

		if (_aspi.option != 1)
			throw exception("[message_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid)
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao esta online no outro server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200202));
		
		if (_aspi.id.compare(_session.m_pi.id) != 0)
			throw exception("[message_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid)
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao eh o mesmo ID[ID=" + _session.m_pi.id + ", REQ_ID=" + _aspi.id 
					+ "] que foi retornado do request com o Auth Server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5200203));
		
		if (_aspi.ip.compare(_session.getIP()) != 0)
			throw exception("[message_server::confirmLoginOnOtherServer][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + ", REQ_UID=" + std::to_string(_aspi.uid)
					+ ", REQ_SERVER=" + std::to_string(_req_server_uid) + "] request Info player, mas nao eh o mesmo IP[IP=" + _session.getIP() + ", REQ_IP=" + _aspi.ip 
					+ "] que foi retornado do request com o Auth Server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200204));

		// Confirm Login com sucesso, Atualiza o cliente

		// Loading Friend List
		_session.m_pi.m_friend_manager.init(_session.m_pi);

		// Logado [Online]
		_session.m_pi.m_state = 4;

		// Authorized a ficar online no server por tempo indeterminado
		_session.m_is_authorized = 1u;

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::confirmLoginOnOtherServer][Log] player[UID=" + std::to_string(_session.m_pi.uid) 
				+ ", NICKNAME=" + std::string(_session.m_pi.nickname) + "] logou com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		// Resposta para o Pedido de Login
		p.init_plain((unsigned short)0x2F);

		p.addUint8(0);	// OK

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	} catch (exception& e) {

		// Resposta
		p.init_plain((unsigned short)0x2F);

		p.addUint8(1);	// Error;

		packet_func::session_send(p, &_session, 1);

		// Disconnect
#if defined(_WIN32)
		::shutdown(_session.m_sock, SD_BOTH);
#elif defined(__linux__)
		::shutdown(_session.m_sock.fd, SD_BOTH);
#endif

		_smp::message_pool::getInstance().push(new message("[message_server::confirmLoginOnOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::requestFriendAndGuildMemberList(player& _session, packet *_packet) {
	REQUEST_BEGIN("FriendAndGuildMemberList");

	packet p;

	try {
		
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[FriendList][Log] envia lista de amigos para o player[UID=" + std::to_string(_session.m_pi.uid) + "].", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("FriendAndGuildMemberList");

		auto friend_list = _session.m_pi.m_friend_manager.getAllFriendAndGuildMember();

		ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);

		// UPDATE ON GAME
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x115);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);
		p.addUint32(_session.m_pi.m_state);

		p.addUint8(1);	// OK

		p.addBuffer(&_session.m_pi.m_cpi, sizeof(_session.m_pi.m_cpi));

		// Send To Player
		packet_func::session_send(p, &_session, 1);

		FriendInfoEx *pFi = nullptr;

		// Resposta para Lista de Amigos e Membros da Guild
		if (mp.paginas > 0) {

			for (auto i = 0u; i < mp.paginas; i++, ++mp) {
				p.init_plain((unsigned short)0x30);

				p.addUint16(0x102);	// Sub Packet Id

				p.addBuffer(&mp.pag, sizeof(mp.pag));
			
				auto begin = friend_list.begin() + mp.index.start;
				auto end = friend_list.begin() + mp.index.end;

				for (; begin != end; ++begin) {
					p.addBuffer((*begin), sizeof(FriendInfo));

					auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

					// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
					if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(_session.m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

						p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

						// State Icon Player
						p.addUint8(s->m_pi.m_state);

						switch (s->m_pi.m_state) {
						case 0:	// IN GAME
							(*begin)->state.stState.play = 1;
							break;
						case 1:	// AFK
							(*begin)->state.stState.AFK = 1;
							break;
						case 3:	// BUSY
							(*begin)->state.stState.busy = 1;
							break;
						case 4:	// ON
						default:
							(*begin)->state.stState.online = 1;
						}
				
						// Online
						(*begin)->state.stState.online = 1;

					}else {	// player n�o est� online
						p.addInt16(-1);		// Sala Numero
						p.addInt32(-1);		// Sala Tipo
						p.addInt32(-1);		// Server GUID
						p.addInt8(-1);		// Canal ID
						p.addZeroByte(64);	// Canal Nome

						// State Icon Player, OFFLINE not change icon
						p.addUint8(5);	// OFFLINE

						// Offline
						(*begin)->state.stState.online = 0;
					}

					p.addInt8((*begin)->cUnknown_flag);

					// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
					p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == _session.m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
					p.addUint8((*begin)->state.ucState);
					p.addUint8((*begin)->flag.ucFlag);
				}

				packet_func::session_send(p, &_session, 1);
			}
		
		}else {

			// N�o tem nenhum amigo, manda a p�gina vazia
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x102);	// Sub Packet Id

			p.addBuffer(&mp.pag, sizeof(mp.pag));

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[channel::requestFriendAndGuildMemberList][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x102);	// Sub Packet Id

		p.addUint8(1);	// pagina

		p.addUint32(0);	// 0 Members

		packet_func::session_send(p, &_session, 1);
	}

}

void message_server::requestUpdateChannelPlayerInfo(player& _session, packet *_packet) {
	REQUEST_BEGIN("UpdateChannelPlayerInfo");

	packet p;

	try {

		ChannelPlayerInfo cpi{ 0 };

		_packet->readBuffer(&cpi, sizeof(cpi));

		_session.m_pi.m_cpi = cpi;

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("UpdateChannelPlayerInfo");

#ifdef _DEBUG
		// Log
		_smp::message_pool::getInstance().push(new message("[UpdateChannelPlayerInfo][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] Atualizou Channel Info[NAME="
				+ std::string(_session.m_pi.m_cpi.name) + ", ID=" + std::to_string(_session.m_pi.m_cpi.id) + ", ROOM=" + std::to_string(_session.m_pi.m_cpi.room.number) 
				+ ", ROOM_TYPE=" + std::to_string(_session.m_pi.m_cpi.room.type) + ", SERVER_UID=" + std::to_string(_session.m_pi.m_cpi.server_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// UPDATE ON GAME
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x115);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);
		p.addUint32(_session.m_pi.m_state);
	
		p.addUint8(1);	// OK

		p.addBuffer(&_session.m_pi.m_cpi, sizeof(_session.m_pi.m_cpi));

		// Send To Player
		packet_func::session_send(p, &_session, 1);

		// Send To Player Friend(s)
		packet_func::friend_broadcast(m_player_manager.findAllFriend(_session.m_pi.m_friend_manager.getAllFriendAndGuildMember(true/*Not Send To Block Friend*/)), p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestUpdateChannelPlayerInfo][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x115);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);
		p.addUint32(_session.m_pi.m_state);

		p.addUint8(0);	// Error(ACHO)

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestUpdatePlayerState(player& _session, packet *_packet) {
	REQUEST_BEGIN("UpdatePlayerState");

	packet p;

	try {

		unsigned char state = _packet->readUint8();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("UpdatePlayerState");

		// S� Atualiza se o state for diferente
		if (_session.m_pi.m_state != state)
			_session.m_pi.m_state = state;

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[UpdateState][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] atualizou seu status[value=" + std::to_string((unsigned short)state) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Update ON GAME - To player friend(s)
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x115);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);
		p.addUint32(_session.m_pi.m_state);

		p.addUint8(1);	// OK

		p.addBuffer(&_session.m_pi.m_cpi, sizeof(_session.m_pi.m_cpi));

		// Send To Player Friend(s)
		packet_func::friend_broadcast(m_player_manager.findAllFriend(_session.m_pi.m_friend_manager.getAllFriendAndGuildMember(true/*Not Send To Block Friend*/)), p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestUpdatePlayerState][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::requestUpdatePlayerLogout(player& _session, packet *_packet) {
	REQUEST_BEGIN("UpdatePlayerLogout");

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("UpdatePlayerLogout");

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[PlayerLogout][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] deslogou-se", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Send Update Player Logout to your friends
		sendUpdatePlayerLogoutToFriends(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestUpdatePlayerLogout][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}
}

void message_server::requestChatFriend(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChatFriend");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();
		std::string msg = _packet->readString();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("ChatFriend");

		if (uid == 0)
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG=" 
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas o uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200301));

		if (msg.empty())
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas msg is empty. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200302));

		auto pFi = _session.m_pi.m_friend_manager.findFriendInAllFriend(uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas player nao eh amigo dele. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5200303));

		if (pFi->state.stState.block)
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas o amigo esta bloqueado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200304));

		auto s = (player*)m_player_manager.findSessionByUID(uid);

		if (s == nullptr)
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas o Amigo nao esta online.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5200305));

		pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(_session.m_pi.uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5200306));

		if (pFi->state.stState.block)
			throw exception("[message_server::requestChatFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Amigo[UID=" + std::to_string(uid) + "], mas amigo bloqueou ele. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE, 7, 0x5200307));

		// Log Para os GMs
		auto gm = m_player_manager.findAllGM();

		if (!gm.empty()) {

			std::string msg_gm = "\\5" + std::string(_session.m_pi.nickname) + ">" + std::string(s->m_pi.nickname) + ": '" + msg + "'";

			for (auto& el : gm) {
				
				// Nao envia o log de MSN->PM novamente para o GM que enviou ou recebeu MSN->PM
				if (el->m_pi.uid != _session.m_pi.uid && el->m_pi.uid != s->m_pi.uid) {
					// Responde no chat do player
					p.init_plain((unsigned short)0x40);

					p.addUint8(0);

					p.addString("\\1[MSN->PM]");	// Nickname

					p.addString(msg_gm);	// Message

					packet_func::session_send(p, el, 1);
				}
			}
		}

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[ChatFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] enviou Message[MSG="
				+ msg + "] para seu Amigo[UID=" + std::to_string(s->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Resposta para send chat to friend
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x113);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);			// FROM
		p.addString(_session.m_pi.nickname);	// FROM
		p.addString(msg);

		p.addUint8(0);	// Chat Friend

		packet_func::session_send(p, s, 1);		// TO

		// ------------------------------- Chat History Discord ------------------------------------
		// Envia a mensagem para o discord chat log se estiver ativado

		// Verifica se o m_chat_discod flag est� ativo para enviar o chat para o discord
		if (m_si.rate.smart_calculator && m_chat_discord)
			sendMessageToDiscordChatHistory(
				"[MSN->PM]",																										// From
				std::string(_session.m_pi.nickname) + ">" + std::string(s->m_pi.nickname) + ": '" + msg + "'"						// Msg
			);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestChatFriend][ErrorSystem] " + e.getFullMessageError() , CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x113);	// Sub Packet Id

		p.addInt32(-1);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestChatGuild(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChatGuild");

	packet p;

	try {

		std::string msg = _packet->readString();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("ChatGuild");

		if (_session.m_pi.guild_uid == 0)
			throw exception("[message_server::requestChatGuild][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG="
					+ msg + "] para o Chat da Guild[UID=" + std::to_string(_session.m_pi.guild_uid) + "], mas o player nao esta em uma guild. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200401));

		if (msg.empty())
			throw exception("[message_server::requestChatGuild][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar Message[MSG=" 
					+ msg + "] para o Chat da Guild[UID=" + std::to_string(_session.m_pi.guild_uid) + "], mas a msg is empty. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200402));

		// Log Para os GMs
		auto gm = m_player_manager.findAllGM();

		if (!gm.empty()) {

			auto guild_name = std::string(_session.m_pi.guild_name);

			size_t index = std::string::npos;
			
			while ((index = guild_name.find(' ', (index != std::string::npos ? index + 1 : 0u))) != std::string::npos)
				guild_name.replace(index, 1, " \\2");

			std::string msg_gm = "[\\2" + guild_name  + "\\0]\\5>" + std::string(_session.m_pi.nickname) + ": '" + msg + "'";

			for (auto& el : gm) {

				// Nao envia o log de Club Chat novamente para o GM que enviou ou recebeu Club Chat
				if (el->m_pi.uid != _session.m_pi.uid && el->m_pi.guild_uid != _session.m_pi.guild_uid) {
					// Responde no chat do player
					p.init_plain((unsigned short)0x40);

					p.addUint8(0);

					p.addString("\\1[CC]");	// Nickname

					p.addString(msg_gm);	// Message

					packet_func::session_send(p, el, 1);
				}
			}
		}

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[ChatGuild][Log] player[UID=" + std::to_string(_session.m_pi.uid) +"] enviu Message[MSG=" + msg + "] no Chat da Guild[UID="
				+ std::to_string(_session.m_pi.guild_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Resposta para send chat to Guild
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x113);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);			// FROM
		p.addString(_session.m_pi.nickname);	// FROM
		p.addString(msg);

		p.addUint8(1);	// Chat Guild

		packet_func::session_send(p, &_session, 1);	// SEND TO PLAYER TOO

		// Usa o m_player_manager.findAllGuildMember, que pega todos os players que est�o na mesma guild
		packet_func::friend_broadcast(m_player_manager.findAllGuildMember(_session.m_pi.guild_uid), p, &_session, 1);	// All GUILD MEMBER
		//packet_func::friend_broadcast(m_player_manager.findAllFriend(_session.m_pi.m_friend_manager.getAllGuildMember()), p, &_session, 1);	// ALL GUILD MEMBER

		// ------------------------------- Chat History Discord ------------------------------------
		// Envia a mensagem para o discord chat log se estiver ativado

		// Verifica se o m_chat_discod flag est� ativo para enviar o chat para o discord
		if (m_si.rate.smart_calculator && m_chat_discord)
			sendMessageToDiscordChatHistory(
				"[CC]",																												// From
				"[" + std::string(_session.m_pi.guild_name) + "]>" + std::string(_session.m_pi.nickname) + ": '" + msg + "'"		// Msg
			);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestChatGuild][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x113);	// Sub Packet Id

		p.addUint32(-1);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestCheckNickname(player& _session, packet *_packet) {
	REQUEST_BEGIN("CheckNickname");

	packet p;
	std::string nickname = "";

	try {
		
		nickname = _packet->readString();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("CheckNickname");

		if (nickname.empty())
			throw exception("[message_server::requestCheckNickname][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou verificar o Nickname[value=" 
					+ nickname + "], mas o nickname is empty. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200501));

		CmdVerifNick cmd_vn(nickname, true);	// Waiter

		NormalManagerDB::add(0, &cmd_vn, nullptr, nullptr);

		cmd_vn.waitEvent();

		if (cmd_vn.getException().getCodeError() != 0)
			throw cmd_vn.getException();

		if (!cmd_vn.getLastCheck())
			throw exception("[message_server::requestCheckNickname][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou verificar o Nickname[value="
				+ nickname + "], mas o nickname nao existe.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 1));

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[CheckNickname][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para verificar o Nickname[value=" + nickname + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG


		// Resposta para Check Nickname
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x117);	// Sub Packet Id

		p.addUint32(0);	// OK

		p.addString(nickname);
		p.addUint32(cmd_vn.getUID());

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestCheckNickname][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x117);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200500);

		p.addString(nickname);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestAssingApelido(player& _session, packet *_packet) {
	REQUEST_BEGIN("AssingApelido");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();
		std::string apelido = _packet->readString();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("AssingApelido");

		if (uid == 0)
			throw exception("[message_server::requestAssingApelido][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou da um apelido para o Amigo[UID=" 
					+ std::to_string(uid) + ", APELIDO=" + apelido + "], mas o uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200901));

		if (apelido.empty())
			throw exception("[message_server::requestAssingApelido][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou da um apelido para o Amigo[UID="
					+ std::to_string(uid) + ", APELIDO=" + apelido + "], mas o apelido is empty. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200902));

		if (apelido.size() >= 11)
			throw exception("[message_server::requestAssingApelido][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou da um apelido para o Amigo[UID="
					+ std::to_string(uid) + ", APELIDO=" + apelido + "], mas o comprimento do apelido[max=11, request=" + std::to_string(apelido.size()) + "] eh invalido.", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5200903));

		auto pFi = _session.m_pi.m_friend_manager.findFriend(uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestAssingApelido][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou da um apelido para o Amigo[UID="
					+ std::to_string(uid) + ", APELIDO=" + apelido + "], mas ele nao tem esse player como amigo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200903));

		// UPDATE ON SERVER
#if defined(_WIN32)
		memcpy_s(pFi->apelido, sizeof(pFi->apelido), apelido.c_str(), sizeof(pFi->apelido));
#elif defined(__linux__)
		memcpy(pFi->apelido, apelido.c_str(), sizeof(pFi->apelido));
#endif

		// UPDATE ON DB
		_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);

		// Log
		_smp::message_pool::getInstance().push(new message("[AssingApelido][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] colocou apelido[VALUE="
				+ apelido + "] no Amigo[UID="  + std::to_string(pFi->uid) + ", NICKNAME=" + std::string(pFi->nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

		// Resposta para assing apelido
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x119);	// Sub Packet Id

		p.addUint32(0);	// OK

		p.addUint32(pFi->uid);
		p.addString(pFi->apelido);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestAssingApelido][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x119);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200900);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestBlockFriend(player& _session, packet *_packet) {
	REQUEST_BEGIN("BlockFriend");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("BlockFriend");

		if (uid == 0)
			throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID=" 
					+ std::to_string(uid) + "], mas o uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5300101));

		auto pFi = _session.m_pi.m_friend_manager.findFriend(uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID="
				+ std::to_string(uid) + "], mas o player nao eh amigo dele. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5300102));

		if (pFi->state.stState.block)
			throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID="
					+ std::to_string(uid) + "], mas o amigo ja esta bloqueado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5300103));

		auto s = (player*)m_player_manager.findSessionByUID(uid);

		FriendInfoEx *pFi2 = nullptr;

		if (s != nullptr) {	// Player est� online

			if ((pFi2 = s->m_pi.m_friend_manager.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID="
						+ std::to_string(uid) + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5300104));

			// Amigo
			pFi->state.stState.block = 1;

			// UPDATE ON DB
			_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);	// REQUEST

			// Log
			_smp::message_pool::getInstance().push(new message("[BlockFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] bloqueou o Amigo[UID="
					+ std::to_string(s->m_pi.uid) + ", NICKNAME=" + std::string(s->m_pi.nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o block friend REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10C);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(s->m_pi.uid);

			packet_func::session_send(p, &_session, 1);

			// Resposta para o block friend REQUESTED, Envia que o player deslogou
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10F);	// Sub Packet Id

			p.addUint32(_session.m_pi.uid);

			packet_func::session_send(p, s, 1);

		}else {

			CmdPlayerInfo cmd_pi(uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			if (pi.uid == 0)
				throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID="
						+ std::to_string(uid) + "], mas player nao existe. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5300105));

			FriendManager fm(pi);

			fm.init(pi);

			if (!fm.isInitialized())
				throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID="
						+ std::to_string(uid) + "], nao conseguiu inicializar Friend Manager do amigo. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5300106));

			if ((pFi2 = fm.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou bloqueiar Amigo[UID="
						+ std::to_string(uid) + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5300104));

			// Amigo
			pFi->state.stState.block = 1;

			// UPDATE ON DB
			_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);	// REQUEST

			// Log
			_smp::message_pool::getInstance().push(new message("[BlockFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] bloqueou o Amigo[UID="
					+ std::to_string(pi.uid) + ", NICKNAME=" + std::string(pi.nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o block friend REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10C);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(pi.uid);

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestBlockFriend][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x10C);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5300100);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestUnblockFriend(player& _session, packet *_packet) {
	REQUEST_BEGIN("UnblockFriend");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("UnblockFriend");

		if (uid == 0)
			throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID=" 
					+ std::to_string(uid) + "], mas uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5300201));

		auto pFi = _session.m_pi.m_friend_manager.findFriend(uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID="
					+ std::to_string(uid) + "], mas o player nao eh amigo dele. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5300202));

		if (!pFi->state.stState.block)
			throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID="
					+ std::to_string(uid) + "], mas o amigo ja esta desbloqueado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5300203));

		auto s = (player*)m_player_manager.findSessionByUID(uid);

		FriendInfoEx *pFi2 = nullptr;

		if (s != nullptr) {	// Player est� online

			if ((pFi2 = s->m_pi.m_friend_manager.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID="
						+ std::to_string(uid) + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200204));

			// Amigo
			pFi->state.stState.block = 0;

			// UPDATE ON DB
			_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);	// REQUEST

			// Log
			_smp::message_pool::getInstance().push(new message("[UnBlockFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] desbloqueou o Amigo[UID="
					+ std::to_string(s->m_pi.uid) + ", NICKNAME=" + std::string(s->m_pi.nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o unblock friend REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10D);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(s->m_pi.uid);

			packet_func::session_send(p, &_session, 1);

			// Resposta para o unblock friend REQUESTED - Passa Pacote que ele esta online
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x115);	// Sub Packet Id

			p.addUint32(_session.m_pi.uid);
			p.addUint32(_session.m_pi.m_state);

			p.addUint8(1);	// OK

			p.addBuffer(&_session.m_pi.m_cpi, sizeof(_session.m_pi.m_cpi));

			packet_func::session_send(p, s, 1);

		}else {

			CmdPlayerInfo cmd_pi(uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			if (pi.uid == 0)
				throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID="
						+ std::to_string(uid) + "], mas o player nao existe. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5300205));

			FriendManager fm(pi);

			fm.init(pi);

			if (!fm.isInitialized())
				throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID="
						+ std::to_string(uid) + "], mas nao conseguiu inicializar Friend Manager do amigo. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5300206));

			if ((pFi2 = fm.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestUnBlockFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desbloquear Amigo[UID="
						+ std::to_string(uid) + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5300204));

			// Amigo
			pFi->state.stState.block = 0;

			// UPDATE ON DB
			_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);	// REQUEST

			// Log
			_smp::message_pool::getInstance().push(new message("[UnBlockFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] desbloqueou o Amigo[UID="
					+ std::to_string(pi.uid) + ", NICKNAME=" + std::string(pi.nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o unblock friend REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10D);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(pi.uid);

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestUnblockFriend][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x10D);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5300200);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestAddFriend(player& _session, packet *_packet) {
	REQUEST_BEGIN("AddFriend");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();
		std::string nickname = _packet->readString();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("AddFriend");

		if (uid == 0)
			throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID=" 
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200601));

		if (nickname.empty())
			throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o nickname is empty. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200602));

		auto pFi = _session.m_pi.m_friend_manager.findFriendInAllFriend(uid);

		if (pFi != nullptr && pFi->flag.stFlag._friend)
			throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
				+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o player ja eh amigo dele.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 2));

		if (_session.m_pi.m_friend_manager.countFriend() >= FRIEND_LIST_LIMIT)
			throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas ele esta com a lista de amigos cheia[LIMIT=" + std::to_string(FRIEND_LIST_LIMIT) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200603));

		auto s = (player*)m_player_manager.findSessionByUID(uid);

		FriendInfoEx fi{ 0 }, fi2{ 0 };

		if (s != nullptr) {	// Player est� connectado

#if defined(_WIN32)
			if (_stricmp(nickname.c_str(), s->m_pi.nickname) != 0)
#elif defined(__linux__)
			if (strcasecmp(nickname.c_str(), s->m_pi.nickname) != 0)
#endif
				throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o nickname nao bate. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE, 7, 0x5200607));

			if (s->m_pi.m_friend_manager.countFriend() >= FRIEND_LIST_LIMIT)
				throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o amigo esta com a lista full[LIMIT=" + std::to_string(FRIEND_LIST_LIMIT) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 3));

			// Friend to add
			fi.uid = s->m_pi.uid;
			fi.flag.ucFlag = (pFi == nullptr) ? 1 : pFi->flag.ucFlag | 1;	// Friend
			
#if defined(_WIN32)
			strncpy_s(fi.apelido, sizeof(fi.apelido), "Friend", sizeof(fi.apelido));
			strncpy_s(fi.nickname, sizeof(fi.nickname), s->m_pi.nickname, sizeof(fi.nickname));
#elif defined(__linux__)
			strncpy(fi.apelido, "Friend", sizeof(fi.apelido));
			strncpy(fi.nickname, s->m_pi.nickname, sizeof(fi.nickname));
#endif
			
			fi.state.stState.online = 1;
			fi.state.stState.request_friend = 1;
			fi.state.stState.sex = s->m_pi.sex;
			
			fi.level = (unsigned char)s->m_pi.level;

			// Friend that has add
			fi2.uid = _session.m_pi.uid;
			fi2.flag.ucFlag = (pFi == nullptr) ? 1 : pFi->flag.ucFlag | 1;	// Friend

#if defined(_WIN32)
			strncpy_s(fi2.apelido, sizeof(fi2.apelido), "Friend", sizeof(fi2.apelido));
			strncpy_s(fi2.nickname, sizeof(fi2.nickname), _session.m_pi.nickname, sizeof(fi2.nickname));
#elif defined(__linux__)
			strncpy(fi2.apelido, "Friend", sizeof(fi2.apelido));
			strncpy(fi2.nickname, _session.m_pi.nickname, sizeof(fi2.nickname));
#endif

			fi2.state.stState.online = 1;
			fi2.state.stState.sex = _session.m_pi.sex;

			fi2.level = (unsigned char)_session.m_pi.level;

			// UPDATE ON SERVER AND DB
			_session.m_pi.m_friend_manager.requestAddFriend(fi);	// Add On Player Request
			s->m_pi.m_friend_manager.requestAddFriend(fi2);			// Add On Player Requested

			// Log
			_smp::message_pool::getInstance().push(new message("[AddFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] add Amigo[UID=" + std::to_string(s->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o add Friend
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x104);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addBuffer(&fi, sizeof(FriendInfo));

			p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

			// State Icon Player
			p.addUint8(s->m_pi.m_state);

			p.addInt8(fi.cUnknown_flag);
			p.addUint8(fi.level);
			p.addUint8(fi.state.ucState);
			p.addUint8(fi.flag.ucFlag);

			packet_func::session_send(p, &_session, 1);

			// Resposta para o player que foi adicionado
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x106);	// Sub Packet Id

			p.addBuffer(&fi2, sizeof(FriendInfo));

			p.addBuffer(&_session.m_pi.m_cpi, sizeof(ChannelPlayerInfo));

			// State Icon Player
			p.addUint8(_session.m_pi.m_state);

			p.addInt8(fi2.cUnknown_flag);
			p.addUint8(fi2.level);
			p.addUint8(fi2.state.ucState);
			p.addUint8(fi2.flag.ucFlag);

			packet_func::session_send(p, s, 1);

		}else {

			CmdPlayerInfo cmd_pi(uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			if (pi.uid == 0)
				throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o player nao existe.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5200606));

#if defined(_WIN32)
			if (_stricmp(nickname.c_str(), pi.nickname) != 0)
#elif defined(__linux__)
			if (strcasecmp(nickname.c_str(), pi.nickname) != 0)
#endif
				throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o nickname nao bate. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE, 7, 0x5200607));

			FriendManager fm(pi);

			fm.init(pi);

			if (!fm.isInitialized())
				throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas nao conseguiu inicializar o FriendManager do Amigo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 8, 0x5200607));

			if (fm.countFriend() >= FRIEND_LIST_LIMIT)
				throw exception("[message_server::requestAddFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou add Friend[UID="
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o amigo esta com a lista full[LIMIT=" + std::to_string(FRIEND_LIST_LIMIT) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 3));

			// Friend to add
			fi.uid = pi.uid;
			fi.flag.ucFlag = (pFi == nullptr) ? 1 : pFi->flag.ucFlag | 1;	// Friend

#if defined(_WIN32)
			memcpy_s(fi.apelido, sizeof(fi.apelido), "Friend", 7);
			memcpy_s(fi.nickname, sizeof(fi.nickname), pi.nickname, sizeof(fi.nickname));
#elif defined(__linux__)
			memcpy(fi.apelido, "Friend", 7);
			memcpy(fi.nickname, pi.nickname, sizeof(fi.nickname));
#endif

			fi.state.stState.online = 1;
			fi.state.stState.request_friend = 1;
			fi.state.stState.sex = pi.sex;

			fi.level = (unsigned char)pi.level;

			// Friend that has add
			fi2.uid = _session.m_pi.uid;
			fi2.flag.ucFlag = (pFi == nullptr) ? 1 : pFi->flag.ucFlag | 1;	// Friend

#if defined(_WIN32)
			memcpy_s(fi2.apelido, sizeof(fi2.apelido), "Friend", 7);
			memcpy_s(fi2.nickname, sizeof(fi2.nickname), _session.m_pi.nickname, sizeof(fi2.nickname));
#elif defined(__linux__)
			memcpy(fi2.apelido, "Friend", 7);
			memcpy(fi2.nickname, _session.m_pi.nickname, sizeof(fi2.nickname));
#endif

			fi2.state.stState.online = 1;
			fi2.state.stState.sex = _session.m_pi.sex;

			fi2.level = (unsigned char)_session.m_pi.level;

			// UPDATE ON SERVER AND DB
			_session.m_pi.m_friend_manager.requestAddFriend(fi);	// Add On Player Request
			fm.requestAddFriend(fi2);								// Add On Player Requested

			// Log
			_smp::message_pool::getInstance().push(new message("[AddFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] add Amigo[UID=" + std::to_string(pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o add Friend
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x104);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addBuffer(&fi, sizeof(FriendInfo));

			p.addInt16(-1);		// Sala N�mero
			p.addInt32(-1);		// Sala Tipo
			p.addInt32(-1);		// Server GUID
			p.addInt8(-1);		// Canal ID
			p.addZeroByte(64);	// Canal Nome

			// State Icon Player
			p.addUint8(5);	// OFFLINE

			fi.state.stState.online = 0;	// Offline

			p.addInt8(fi.cUnknown_flag);
			p.addUint8(fi.level);
			p.addUint8(fi.state.ucState);
			p.addUint8(fi.flag.ucFlag);

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestAddFriend][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x104);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200600);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestConfirmFriend(player& _session, packet *_packet) {
	REQUEST_BEGIN("ConfirmFriend");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("ConfirmFriend");

		if (uid == 0)
			throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID=" 
					+ std::to_string(uid) + "], mas o uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200801));

		auto pFi = _session.m_pi.m_friend_manager.findFriend(uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
					+ std::to_string(uid) + "], mas o player nao eh amigo dele. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200802));

		if (pFi->state.stState.request_friend)
			throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
					+ std::to_string(uid) + "], mas ele nao pode aceitar um amigo, que ele mesmo enviou pedido de amizade. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5200803));

		if (pFi->state.stState._friend)
			throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
					+ std::to_string(uid) + "], mas o player ja eh seu amigo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 4, 0x5200804));

		auto s = (player*)m_player_manager.findSessionByUID(uid);

		FriendInfoEx *pFi2 = nullptr;

		if (s != nullptr) {	// Player est� online

			if ((pFi2 = s->m_pi.m_friend_manager.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
						+ std::to_string(uid) + "], mas o player nao esta na lista do amigo que ele vai aceitar. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5200804));

			// Amigo
			pFi->state.stState._friend = 1;

			// Amigo
			pFi2->state.stState.request_friend = 0;
			pFi2->state.stState._friend = 1;

			// UPDATE ON SERVER AND DB
			_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);	// REQUEST
			s->m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi2);		// REQUESTED

			// Log
			_smp::message_pool::getInstance().push(new message("[ConfirmFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] aceitou Amigo[UID="
					+ std::to_string(s->m_pi.uid) + ", NICKNAME=" + std::string(s->m_pi.nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o confirm friend REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16((unsigned short)0x109);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(s->m_pi.uid);

			packet_func::session_send(p, &_session, 1);

			// Resposta para o confirm friend REQUESTED
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10A);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(_session.m_pi.uid);

			packet_func::session_send(p, s, 1);

		}else {

			CmdPlayerInfo cmd_pi(uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			if (pi.uid == 0)
				throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
						+ std::to_string(uid) + "], mas o player nao existe. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5200806));

			FriendManager fm(pi);

			fm.init(pi);

			if (!fm.isInitialized())
				throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
						+ std::to_string(uid) + "], mas nao conseguiu incializar o Friend Manager do amigo. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 7, 0x5200807));

			if ((pFi2 = fm.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestConfirmFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar Amigo[UID="
						+ std::to_string(uid) + "], mas o player nao esta na lista do amigo que ele vai aceitar. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5200805));

			// Amigo
			pFi->state.stState._friend = 1;

			// Amigo
			pFi2->state.stState.request_friend = 0;
			pFi2->state.stState._friend = 1;


			// UPDATE ON SERVER AND DB
			_session.m_pi.m_friend_manager.requestUpdateFriendInfo(*pFi);	// REQUEST
			fm.requestUpdateFriendInfo(*pFi2);								// REQUESTED

			// Log
			_smp::message_pool::getInstance().push(new message("[ConfirmFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] aceitou Amigo[UID="
					+ std::to_string(pi.uid) + ", NICKNAME=" + std::string(pi.nickname) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Resposta para o confirm friend REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16((unsigned short)0x109);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(pi.uid);

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestConfirmFriend][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x109);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200800);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestDeleteFriend(player& _session, packet *_packet) {
	REQUEST_BEGIN("DeleteFriend");

	packet p;

	try {

		uint32_t uid = _packet->readUint32();
		std::string nickname = _packet->readString();

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("DeleteFriend");

		if (uid == 0)
			throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID=" 
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 1, 0x5200701));

		if (nickname.empty())
			throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas nickname is empty. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 2, 0x5200702));

		auto pFi = _session.m_pi.m_friend_manager.findFriend(uid);

		if (pFi == nullptr)
			throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
					+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o player nao eh amigo dele. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3, 0x5200703));

		auto s = (player*)m_player_manager.findSessionByUID(uid);

		FriendInfoEx *pFi2 = nullptr;

		if (s != nullptr) {	// Player est� online

#if defined(_WIN32)
			if (_stricmp(nickname.c_str(), s->m_pi.nickname) != 0)
#elif defined(__linux__)
			if (strcasecmp(nickname.c_str(), s->m_pi.nickname) != 0)
#endif
				throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o nickname nao bate. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE, 6, 0x5200705));

			if ((pFi2 = s->m_pi.m_friend_manager.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE, 4, 0x5200704));
			
			// UPDATE ON SERVER ON DB
			_session.m_pi.m_friend_manager.requestDeleteFriend(*pFi);	// REQUEST
			s->m_pi.m_friend_manager.requestDeleteFriend(*pFi2);		// REQUESTED

			// Log
			_smp::message_pool::getInstance().push(new message("[DeleteFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] deletou Amigo[UID="
					+ std::to_string(s->m_pi.uid) + ", NICKNAME=" + nickname + "]", CL_FILE_LOG_AND_CONSOLE));

			// Respsta para o delete friend TO REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10B);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(s->m_pi.uid);

			packet_func::session_send(p, &_session, 1);

			// Resposta para o delete friend TO REQUESTED
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10B);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(_session.m_pi.uid);

			packet_func::session_send(p, s, 1);

		}else {

			CmdPlayerInfo cmd_pi(uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			if (pi.uid == 0)
				throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o player nao existe. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5, 0x5200705));
			
#if defined(_WIN32)
			if (_stricmp(nickname.c_str(), pi.nickname) != 0)
#elif defined(__linux__)
			if (strcasecmp(nickname.c_str(), pi.nickname) != 0)
#endif
				throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o nickname nao bate. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 6, 0x5200706));

			FriendManager fm(pi);

			fm.init(pi);

			if (!fm.isInitialized())
				throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas nao conseguiu incializar o Friend Manager do Amigo. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 7, 0x5200707));

			if ((pFi2 = fm.findFriend(_session.m_pi.uid)) == nullptr)
				throw exception("[message_server::requestDeleteFriend][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar Amigo[UID="
						+ std::to_string(uid) + ", NICKNAME=" + nickname + "], mas o amigo nao tem ele na lista de amigos. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE, 8, 0x5200708));

			// UPDATE ON SERVER ON DB
			_session.m_pi.m_friend_manager.requestDeleteFriend(*pFi);	// REQUEST
			fm.requestDeleteFriend(*pFi2);								// REQUESTED

			// Log
			_smp::message_pool::getInstance().push(new message("[DeleteFriend][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] deletou Amigo[UID="
					+ std::to_string(pi.uid) + ", NICKNAME=" + nickname + "]", CL_FILE_LOG_AND_CONSOLE));

			// Respsta para o delete friend TO REQUEST
			p.init_plain((unsigned short)0x30);

			p.addUint16(0x10B);	// Sub Packet Id

			p.addUint32(0);	// OK

			p.addUint32(pi.uid);

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestDeleteFriend][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x30);

		p.addUint16(0x10B);	// Sub Packet Id

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::MESSAGE_SERVER) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200700);

		packet_func::session_send(p, &_session, 1);
	}
}

void message_server::requestNotityPlayerWasInvitedToRoom(player& _session, packet *_packet) {
	REQUEST_BEGIN("NotifyPlayerWasInvitedToRoom");

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("NotifyPlayerWasInvitedToRoom");

		uint32_t player_invited_uid = _packet->readUint32();

		if (player_invited_uid != _session.m_pi.uid)
			throw exception("[message_server::requestNotityPlayerWasInvitedToRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] que foi convidado passou um Player[UID=" + std::to_string(player_invited_uid) 
					+ "] com uid que nao eh o dele. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3749, 0));

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::requestNotityPlayerWasInvitedToRoom][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] foi convidado para um sala no jogo.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestNotifyPlayerWasInvitedToRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::requestInvitPlayerToGuildBattleRoom(player& _session, packet *_packet) {
	REQUEST_BEGIN("InvitPlayerToGuildBattleRoom");

	try {

		// Verifica se session est� autorizada para executar esse a��o, 
		// se ele n�o fez o login com o Server ele n�o pode fazer nada at� que ele fa�a o login
		CHECK_SESSION_IS_AUTHORIZED("InvitPlayerToGuildBattleRoom");

		uint32_t server_uid = _packet->readUint32();
		unsigned char channel_id = _packet->readUint8();
		unsigned short room_numero = _packet->readUint16();

		uint32_t player_invite_uid = _packet->readUint32();
		std::string player_invite_nickname = _packet->readString();

		uint32_t player_invited_uid = _packet->readUint32();

		if (player_invite_uid != _session.m_pi.uid)
			throw exception("[message_server::requestInvitPlayerToGuildBattleRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] nao bate com o Player[UID=" + std::to_string(player_invite_uid) + "] que fez o request para convidar o player[UID=" 
					+ std::to_string(player_invited_uid) + "] para a sala de Guild Battle. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 3750, 0));

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::requestInvitPlayerToGuildBattleRoom][Log] Player[UID=" 
				+ std::to_string(_session.m_pi.uid) + ", NICKNAME=" + player_invite_nickname + "] convidou o Player[UID=" 
				+ std::to_string(player_invited_uid) + "] no Server[UID=" + std::to_string(server_uid) + ", CHANNEL_ID=" 
				+ std::to_string((unsigned short)channel_id) + ", ROOM=" + std::to_string(room_numero) + "] para Guild Battle.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestInvitPlayerToGuildBattleRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

#define REQUEST_AUTH_COMMAND_BEGIN(_method) if (_packet == nullptr) \
	throw exception("[message_server::request" + std::string((_method)) + "][Error] _packet is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5400, 0));

void message_server::requestAcceptGuildMember(packet *_packet) {
	REQUEST_AUTH_COMMAND_BEGIN("AcceptGuildMember");

	packet p;

	try {

		uint32_t club_id = _packet->readUint32();
		uint32_t member_uid = _packet->readUint32();

		if (club_id == 0u)
			throw exception("[message_server::requestAcceptGuildMember][Error] club_id is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5401, 0));

		if (member_uid == 0u)
			throw exception("[message_server::requestAcceptGuildMember][Error] member_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5401, 0));

		// Find all Club Members
		auto v_cm = m_player_manager.findAllGuildMember(club_id);

		if (v_cm.empty())
			_smp::message_pool::getInstance().push(new message("[message_server::requestAcceptGuildMember][WARNING] Club[ID=" + std::to_string(club_id) 
					+ "] nao tem nenhum membro online para atualizar.", CL_FILE_LOG_AND_CONSOLE));

		// Update All Friend/Guild Member from All Club Members
		for (auto& el : v_cm)
			if (el.second != nullptr)
				el.second->m_pi.m_friend_manager.init(el.second->m_pi);

		// Verifica se o player est� online
		auto s = m_player_manager.findPlayer(member_uid);

		// Player n�o est� online
		if (s == nullptr || 
#if defined(_WIN32)
			s->m_sock == INVALID_SOCKET
#elif defined(__linux__)
			s->m_sock.fd == INVALID_SOCKET
#endif
		) {

			CmdPlayerInfo cmd_pi(member_uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			// Envia para cada player sua lista de amigos
			{
				for (auto& el : v_cm) {

					if (el.second != nullptr) {

						auto friend_list = el.second->m_pi.m_friend_manager.getAllFriendAndGuildMember();

						ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);
		
						FriendInfoEx *pFi = nullptr;

						// Resposta para Lista de Amigos e Membros da Guild
						if (mp.paginas > 0) {

							for (auto i = 0u; i < mp.paginas; i++, ++mp) {
								p.init_plain((unsigned short)0x30);

								p.addUint16(0x102);	// Sub Packet Id

								p.addBuffer(&mp.pag, sizeof(mp.pag));
			
								auto begin = friend_list.begin() + mp.index.start;
								auto end = friend_list.begin() + mp.index.end;

								for (; begin != end; ++begin) {
									p.addBuffer((*begin), sizeof(FriendInfo));

									auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

									// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
									if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(el.second->m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

										p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

										// State Icon Player
										p.addUint8(s->m_pi.m_state);

										switch (s->m_pi.m_state) {
										case 0:	// IN GAME
											(*begin)->state.stState.play = 1;
											break;
										case 1:	// AFK
											(*begin)->state.stState.AFK = 1;
											break;
										case 3:	// BUSY
											(*begin)->state.stState.busy = 1;
											break;
										case 4:	// ON
										default:
											(*begin)->state.stState.online = 1;
										}
				
										// Online
										(*begin)->state.stState.online = 1;

									}else {	// player n�o est� online
										p.addInt16(-1);		// Sala Numero
										p.addInt32(-1);		// Sala Tipo
										p.addInt32(-1);		// Server GUID
										p.addInt8(-1);		// Canal ID
										p.addZeroByte(64);	// Canal Nome

										// State Icon Player, OFFLINE not change icon
										p.addUint8(5);	// OFFLINE

										// Offline
										(*begin)->state.stState.online = 0;
									}

									p.addInt8((*begin)->cUnknown_flag);

									// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
									p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == el.second->m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
									p.addUint8((*begin)->state.ucState);
									p.addUint8((*begin)->flag.ucFlag);
								}

								packet_func::session_send(p, el.second, 1);
							}
		
						}else {

							// N�o tem nenhum amigo, manda a p�gina vazia
							p.init_plain((unsigned short)0x30);

							p.addUint16(0x102);	// Sub Packet Id

							p.addBuffer(&mp.pag, sizeof(mp.pag));

							packet_func::session_send(p, el.second, 1);
						}
					}
				}
			}

			// Notifica os player d� Guild que o player foi aceito na Guild
			p.init_plain((unsigned short)0x3B);

			p.addUint32(pi.uid);
			p.addUint32(club_id);
			p.addUint8(pi.sex);
			p.addString(pi.id);
			p.addString(pi.nickname);
			p.addUint16(0x1F);				// No Server Antigo estava 0x1F e 0x125 nas op��es que peguei nos pacotes do pangya USA

			packet_func::friend_broadcast(v_cm, p, (session*)1/*S� para enviar, esse ele n�o usa s� verifica se � o mesmo para n�o enviar para o mesmo player*/, 1);

		}else {	// Player est� online	

			// Add o player a Guild
			s->m_pi.guild_uid = club_id;

			// Update All Friend/Guild Member
			s->m_pi.m_friend_manager.init(s->m_pi);

			// Envia para cada player sua lista de amigos
			{
				for (auto& el : v_cm) {

					if (el.second != nullptr) {

						auto friend_list = el.second->m_pi.m_friend_manager.getAllFriendAndGuildMember();

						ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);
		
						FriendInfoEx *pFi = nullptr;

						// Resposta para Lista de Amigos e Membros da Guild
						if (mp.paginas > 0) {

							for (auto i = 0u; i < mp.paginas; i++, ++mp) {
								p.init_plain((unsigned short)0x30);

								p.addUint16(0x102);	// Sub Packet Id

								p.addBuffer(&mp.pag, sizeof(mp.pag));
			
								auto begin = friend_list.begin() + mp.index.start;
								auto end = friend_list.begin() + mp.index.end;

								for (; begin != end; ++begin) {
									p.addBuffer((*begin), sizeof(FriendInfo));

									auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

									// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
									if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(el.second->m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

										p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

										// State Icon Player
										p.addUint8(s->m_pi.m_state);

										switch (s->m_pi.m_state) {
										case 0:	// IN GAME
											(*begin)->state.stState.play = 1;
											break;
										case 1:	// AFK
											(*begin)->state.stState.AFK = 1;
											break;
										case 3:	// BUSY
											(*begin)->state.stState.busy = 1;
											break;
										case 4:	// ON
										default:
											(*begin)->state.stState.online = 1;
										}
				
										// Online
										(*begin)->state.stState.online = 1;

									}else {	// player n�o est� online
										p.addInt16(-1);		// Sala Numero
										p.addInt32(-1);		// Sala Tipo
										p.addInt32(-1);		// Server GUID
										p.addInt8(-1);		// Canal ID
										p.addZeroByte(64);	// Canal Nome

										// State Icon Player, OFFLINE not change icon
										p.addUint8(5);	// OFFLINE

										// Offline
										(*begin)->state.stState.online = 0;
									}

									p.addInt8((*begin)->cUnknown_flag);

									// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
									p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == el.second->m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
									p.addUint8((*begin)->state.ucState);
									p.addUint8((*begin)->flag.ucFlag);
								}

								packet_func::session_send(p, el.second, 1);
							}
		
						}else {

							// N�o tem nenhum amigo, manda a p�gina vazia
							p.init_plain((unsigned short)0x30);

							p.addUint16(0x102);	// Sub Packet Id

							p.addBuffer(&mp.pag, sizeof(mp.pag));

							packet_func::session_send(p, el.second, 1);
						}
					}
				}
			}

			// Notifica os player d� Guild que o player foi aceito na Guild
			p.init_plain((unsigned short)0x3B);

			p.addUint32(s->m_pi.uid);
			p.addUint32(club_id);
			p.addUint8(s->m_pi.sex);
			p.addString(s->m_pi.id);
			p.addString(s->m_pi.nickname);
			p.addUint16(0x1F);				// No Server Antigo estava 0x1F e 0x125 nas op��es que peguei nos pacotes do pangya USA

			packet_func::friend_broadcast(v_cm, p, s, 1);
		}

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::requestAcceptGuildMember][Log] Player[UID=" + std::to_string(member_uid) 
				+ "] foi aceito no Club[UID=" + std::to_string(club_id) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestAcceptGuildMember][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::requestMemberExitedFromGuild(packet *_packet) {
	REQUEST_AUTH_COMMAND_BEGIN("MemberExitedFromGuild");

	packet p;

	try {

		uint32_t club_id = _packet->readUint32();
		uint32_t member_uid = _packet->readUint32();

		if (club_id == 0u)
			throw exception("[message_server::requestMemberExitedFromGuild][Error] club_id is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5401, 0));

		if (member_uid == 0u)
			throw exception("[message_server::requestMemberExitedFromGuild][Error] member_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5401, 0));

		// Find all Club Members
		auto v_cm = m_player_manager.findAllGuildMember(club_id);

		if (v_cm.empty())
			_smp::message_pool::getInstance().push(new message("[message_server::requestMemberExitedFromGuild][WARNING] Club[ID=" + std::to_string(club_id)
					+ "] nao tem nenhum membro online para atualizar.", CL_FILE_LOG_AND_CONSOLE));

		// Update All Friend/Guild Member from All Club Members
		for (auto& el : v_cm)
			if (el.second != nullptr)
				el.second->m_pi.m_friend_manager.init(el.second->m_pi);

		// Verifica se o player est� online
		auto s = m_player_manager.findPlayer(member_uid);

		// Player n�o est� online
		if (s == nullptr || 
#if defined(_WIN32)
			s->m_sock == INVALID_SOCKET
#elif defined(__linux__)
			s->m_sock.fd == INVALID_SOCKET
#endif
		) {

			CmdPlayerInfo cmd_pi(member_uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			// Envia para cada player sua lista de amigos
			{
				for (auto& el : v_cm) {

					if (el.second != nullptr) {

						auto friend_list = el.second->m_pi.m_friend_manager.getAllFriendAndGuildMember();

						ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);
		
						FriendInfoEx *pFi = nullptr;

						// Resposta para Lista de Amigos e Membros da Guild
						if (mp.paginas > 0) {

							for (auto i = 0u; i < mp.paginas; i++, ++mp) {
								p.init_plain((unsigned short)0x30);

								p.addUint16(0x102);	// Sub Packet Id

								p.addBuffer(&mp.pag, sizeof(mp.pag));
			
								auto begin = friend_list.begin() + mp.index.start;
								auto end = friend_list.begin() + mp.index.end;

								for (; begin != end; ++begin) {
									p.addBuffer((*begin), sizeof(FriendInfo));

									auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

									// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
									if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(el.second->m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

										p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

										// State Icon Player
										p.addUint8(s->m_pi.m_state);

										switch (s->m_pi.m_state) {
										case 0:	// IN GAME
											(*begin)->state.stState.play = 1;
											break;
										case 1:	// AFK
											(*begin)->state.stState.AFK = 1;
											break;
										case 3:	// BUSY
											(*begin)->state.stState.busy = 1;
											break;
										case 4:	// ON
										default:
											(*begin)->state.stState.online = 1;
										}
				
										// Online
										(*begin)->state.stState.online = 1;

									}else {	// player n�o est� online
										p.addInt16(-1);		// Sala Numero
										p.addInt32(-1);		// Sala Tipo
										p.addInt32(-1);		// Server GUID
										p.addInt8(-1);		// Canal ID
										p.addZeroByte(64);	// Canal Nome

										// State Icon Player, OFFLINE not change icon
										p.addUint8(5);	// OFFLINE

										// Offline
										(*begin)->state.stState.online = 0;
									}

									p.addInt8((*begin)->cUnknown_flag);

									// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
									p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == el.second->m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
									p.addUint8((*begin)->state.ucState);
									p.addUint8((*begin)->flag.ucFlag);
								}

								packet_func::session_send(p, el.second, 1);
							}
		
						}else {

							// N�o tem nenhum amigo, manda a p�gina vazia
							p.init_plain((unsigned short)0x30);

							p.addUint16(0x102);	// Sub Packet Id

							p.addBuffer(&mp.pag, sizeof(mp.pag));

							packet_func::session_send(p, el.second, 1);
						}
					}
				}
			}

			// Notifica os player d� Guild que o player saiu na Guild
			p.init_plain((unsigned short)0x3C);

			p.addUint32(pi.uid);

			packet_func::friend_broadcast(v_cm, p, (session*)1/*S� para enviar, esse ele n�o usa s� verifica se � o mesmo para n�o enviar para o mesmo player*/, 1);

		}else {	// Player est� online	

			// player saiu da Guild
			s->m_pi.guild_uid = 0;

			// Update All Friend/Guild Member
			s->m_pi.m_friend_manager.init(s->m_pi);

			// Envia para cada player sua lista de amigos
			{
				for (auto& el : v_cm) {

					if (el.second != nullptr) {

						auto friend_list = el.second->m_pi.m_friend_manager.getAllFriendAndGuildMember();

						ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);
		
						FriendInfoEx *pFi = nullptr;

						// Resposta para Lista de Amigos e Membros da Guild
						if (mp.paginas > 0) {

							for (auto i = 0u; i < mp.paginas; i++, ++mp) {
								p.init_plain((unsigned short)0x30);

								p.addUint16(0x102);	// Sub Packet Id

								p.addBuffer(&mp.pag, sizeof(mp.pag));
			
								auto begin = friend_list.begin() + mp.index.start;
								auto end = friend_list.begin() + mp.index.end;

								for (; begin != end; ++begin) {
									p.addBuffer((*begin), sizeof(FriendInfo));

									auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

									// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
									if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(el.second->m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

										p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

										// State Icon Player
										p.addUint8(s->m_pi.m_state);

										switch (s->m_pi.m_state) {
										case 0:	// IN GAME
											(*begin)->state.stState.play = 1;
											break;
										case 1:	// AFK
											(*begin)->state.stState.AFK = 1;
											break;
										case 3:	// BUSY
											(*begin)->state.stState.busy = 1;
											break;
										case 4:	// ON
										default:
											(*begin)->state.stState.online = 1;
										}
				
										// Online
										(*begin)->state.stState.online = 1;

									}else {	// player n�o est� online
										p.addInt16(-1);		// Sala Numero
										p.addInt32(-1);		// Sala Tipo
										p.addInt32(-1);		// Server GUID
										p.addInt8(-1);		// Canal ID
										p.addZeroByte(64);	// Canal Nome

										// State Icon Player, OFFLINE not change icon
										p.addUint8(5);	// OFFLINE

										// Offline
										(*begin)->state.stState.online = 0;
									}

									p.addInt8((*begin)->cUnknown_flag);

									// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
									p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == el.second->m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
									p.addUint8((*begin)->state.ucState);
									p.addUint8((*begin)->flag.ucFlag);
								}

								packet_func::session_send(p, el.second, 1);
							}
		
						}else {

							// N�o tem nenhum amigo, manda a p�gina vazia
							p.init_plain((unsigned short)0x30);

							p.addUint16(0x102);	// Sub Packet Id

							p.addBuffer(&mp.pag, sizeof(mp.pag));

							packet_func::session_send(p, el.second, 1);
						}
					}
				}
			}

			// Notifica os player d� Guild que o player saiu na Guild
			p.init_plain((unsigned short)0x3C);

			p.addUint32(s->m_pi.uid);

			packet_func::friend_broadcast(v_cm, p, s, 1);
		}

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::requestMemberExitedFromGuild][Log] Player[UID=" + std::to_string(member_uid)
				+ "] saiu do Club[UID=" + std::to_string(club_id) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestMemberExitedFromGuild][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::requestKickGuildMember(packet *_packet) {
	REQUEST_AUTH_COMMAND_BEGIN("KickGuildMember");

	packet p;

	try {

		uint32_t club_id = _packet->readUint32();
		uint32_t member_uid = _packet->readUint32();

		if (club_id == 0u)
			throw exception("[message_server::requestKickGuildMember][Error] club_id is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5401, 0));

		if (member_uid == 0u)
			throw exception("[message_server::requestKickGuildMember][Error] member_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 5401, 0));

		// Find all Club Members
		auto v_cm = m_player_manager.findAllGuildMember(club_id);

		if (v_cm.empty())
			_smp::message_pool::getInstance().push(new message("[message_server::requestKickGuildMember][WARNING] Club[ID=" + std::to_string(club_id)
					+ "] nao tem nenhum membro online para atualizar.", CL_FILE_LOG_AND_CONSOLE));

		// Update All Friend/Guild Member from All Club Members
		for (auto& el : v_cm)
			if (el.second != nullptr)
				el.second->m_pi.m_friend_manager.init(el.second->m_pi);

		// Verifica se o player est� online
		auto s = m_player_manager.findPlayer(member_uid);

		// Player n�o est� online
		if (s == nullptr || 
#if defined(_WIN32)
			s->m_sock == INVALID_SOCKET
#elif defined(__linux__)
			s->m_sock.fd == INVALID_SOCKET
#endif
		) {

			CmdPlayerInfo cmd_pi(member_uid, true);	// Waiter

			NormalManagerDB::add(0, &cmd_pi, nullptr, nullptr);

			cmd_pi.waitEvent();

			if (cmd_pi.getException().getCodeError() != 0)
				throw cmd_pi.getException();

			auto pi = cmd_pi.getInfo();

			// Envia para cada player sua lista de amigos
			{
				for (auto& el : v_cm) {

					if (el.second != nullptr) {

						auto friend_list = el.second->m_pi.m_friend_manager.getAllFriendAndGuildMember();

						ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);
		
						FriendInfoEx *pFi = nullptr;

						// Resposta para Lista de Amigos e Membros da Guild
						if (mp.paginas > 0) {

							for (auto i = 0u; i < mp.paginas; i++, ++mp) {
								p.init_plain((unsigned short)0x30);

								p.addUint16(0x102);	// Sub Packet Id

								p.addBuffer(&mp.pag, sizeof(mp.pag));
			
								auto begin = friend_list.begin() + mp.index.start;
								auto end = friend_list.begin() + mp.index.end;

								for (; begin != end; ++begin) {
									p.addBuffer((*begin), sizeof(FriendInfo));

									auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

									// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
									if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(el.second->m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

										p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

										// State Icon Player
										p.addUint8(s->m_pi.m_state);

										switch (s->m_pi.m_state) {
										case 0:	// IN GAME
											(*begin)->state.stState.play = 1;
											break;
										case 1:	// AFK
											(*begin)->state.stState.AFK = 1;
											break;
										case 3:	// BUSY
											(*begin)->state.stState.busy = 1;
											break;
										case 4:	// ON
										default:
											(*begin)->state.stState.online = 1;
										}
				
										// Online
										(*begin)->state.stState.online = 1;

									}else {	// player n�o est� online
										p.addInt16(-1);		// Sala Numero
										p.addInt32(-1);		// Sala Tipo
										p.addInt32(-1);		// Server GUID
										p.addInt8(-1);		// Canal ID
										p.addZeroByte(64);	// Canal Nome

										// State Icon Player, OFFLINE not change icon
										p.addUint8(5);	// OFFLINE

										// Offline
										(*begin)->state.stState.online = 0;
									}

									p.addInt8((*begin)->cUnknown_flag);

									// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
									p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == el.second->m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
									p.addUint8((*begin)->state.ucState);
									p.addUint8((*begin)->flag.ucFlag);
								}

								packet_func::session_send(p, el.second, 1);
							}
		
						}else {

							// N�o tem nenhum amigo, manda a p�gina vazia
							p.init_plain((unsigned short)0x30);

							p.addUint16(0x102);	// Sub Packet Id

							p.addBuffer(&mp.pag, sizeof(mp.pag));

							packet_func::session_send(p, el.second, 1);
						}
					}
				}
			}

			// Notifica os player d� Guild que o player saiu na Guild
			p.init_plain((unsigned short)0x3C);

			p.addUint32(pi.uid);

			packet_func::friend_broadcast(v_cm, p, (session*)1/*S� para enviar, esse ele n�o usa s� verifica se � o mesmo para n�o enviar para o mesmo player*/, 1);

		}else {	// Player est� online	

			// player saiu da Guild
			s->m_pi.guild_uid = 0;

			// Update All Friend/Guild Member
			s->m_pi.m_friend_manager.init(s->m_pi);

			// Envia para cada player sua lista de amigos
			{
				for (auto& el : v_cm) {

					if (el.second != nullptr) {

						auto friend_list = el.second->m_pi.m_friend_manager.getAllFriendAndGuildMember();

						ManyPacket mp((const unsigned short)friend_list.size(), FRIEND_PAG_LIMIT);
		
						FriendInfoEx *pFi = nullptr;

						// Resposta para Lista de Amigos e Membros da Guild
						if (mp.paginas > 0) {

							for (auto i = 0u; i < mp.paginas; i++, ++mp) {
								p.init_plain((unsigned short)0x30);

								p.addUint16(0x102);	// Sub Packet Id

								p.addBuffer(&mp.pag, sizeof(mp.pag));
			
								auto begin = friend_list.begin() + mp.index.start;
								auto end = friend_list.begin() + mp.index.end;

								for (; begin != end; ++begin) {
									p.addBuffer((*begin), sizeof(FriendInfo));

									auto s = (player*)m_player_manager.findSessionByUID((*begin)->uid);

									// Se o Player tem ele na lista de amigos, e ele n�o estiver bloqueado na lista do amigo
									if (s != nullptr && (pFi = s->m_pi.m_friend_manager.findFriendInAllFriend(el.second->m_pi.uid)) != nullptr && !pFi->state.stState.block) {	// Player est� online

										p.addBuffer(&s->m_pi.m_cpi, sizeof(ChannelPlayerInfo));

										// State Icon Player
										p.addUint8(s->m_pi.m_state);

										switch (s->m_pi.m_state) {
										case 0:	// IN GAME
											(*begin)->state.stState.play = 1;
											break;
										case 1:	// AFK
											(*begin)->state.stState.AFK = 1;
											break;
										case 3:	// BUSY
											(*begin)->state.stState.busy = 1;
											break;
										case 4:	// ON
										default:
											(*begin)->state.stState.online = 1;
										}
				
										// Online
										(*begin)->state.stState.online = 1;

									}else {	// player n�o est� online
										p.addInt16(-1);		// Sala Numero
										p.addInt32(-1);		// Sala Tipo
										p.addInt32(-1);		// Server GUID
										p.addInt8(-1);		// Canal ID
										p.addZeroByte(64);	// Canal Nome

										// State Icon Player, OFFLINE not change icon
										p.addUint8(5);	// OFFLINE

										// Offline
										(*begin)->state.stState.online = 0;
									}

									p.addInt8((*begin)->cUnknown_flag);

									// Aqui quando � o player e ele est� guild � 1/*Master*/, 2 sub, e outros membro guild � 0, e quando � friend � o level
									p.addUint8((*begin)->flag.ucFlag == 2/*S� Guild Member*/ ? ((*begin)->uid == el.second->m_pi.uid ? 1/*Master*/ : 0) : (*begin)->level);
					
									p.addUint8((*begin)->state.ucState);
									p.addUint8((*begin)->flag.ucFlag);
								}

								packet_func::session_send(p, el.second, 1);
							}
		
						}else {

							// N�o tem nenhum amigo, manda a p�gina vazia
							p.init_plain((unsigned short)0x30);

							p.addUint16(0x102);	// Sub Packet Id

							p.addBuffer(&mp.pag, sizeof(mp.pag));

							packet_func::session_send(p, el.second, 1);
						}
					}
				}
			}

			// Notifica os player d� Guild que o player saiu na Guild
			p.init_plain((unsigned short)0x3C);

			p.addUint32(s->m_pi.uid);

			packet_func::friend_broadcast(v_cm, p, s, 1);
		}

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::requestKickGuildMember][Log] Player[UID=" + std::to_string(member_uid)
				+ "] foi chutado do Club[UID=" + std::to_string(club_id) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::requestKickGuildMember][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::authCmdShutdown(int32_t _time_sec) {

	try {

		// Shut down com tempo
		if (m_shutdown == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[message_server::authCmdShutdown][Log] Auth Server requisitou para o server ser desligado em "
					+ std::to_string(_time_sec) + " segundos", CL_FILE_LOG_AND_CONSOLE));

			shutdown_time(_time_sec);

		}else
			_smp::message_pool::getInstance().push(new message("[message_server::authCmdShutdown][WARNING] Auth Server requisitou para o server ser delisgado em "
					+ std::to_string(_time_sec) + " segundos, mas o server ja esta com o timer de shutdown", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[message_server::authCmdShutdown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::authCmdBroadcastNotice(std::string /*_notice*/) {
	// Message Server n�o usa esse Comando
	return;
}

void message_server::authCmdBroadcastTicker(std::string /*_nickname*/, std::string /*_msg*/) {
	// Message Server n�o usa esse Comando
	return;
}

void message_server::authCmdBroadcastCubeWinRare(std::string /*_msg*/, uint32_t /*_option*/) {
	// Message Server n�o usa esse Comando
	return;
}

void message_server::authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) {

	// Aqui no Message Server Ainda n�o usa o force, ele desconecta o player do mesmo jeito
	UNREFERENCED_PARAMETER(_force);

	try {

		auto s = m_player_manager.findPlayer(_player_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[message_server::authCmdDisconnectPlayer][log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(s->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

			// Deconecta o Player
			DisconnectSession(s);

			// UPDATE ON Auth Server
			m_unit_connect->sendConfirmDisconnectPlayer(_req_server_uid, _player_uid);

		}else
			_smp::message_pool::getInstance().push(new message("[message_server::authCmdDisconnectPlayer][WARNING] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu para desconectar o Player[UID=" + std::to_string(_player_uid) + "], mas nao encontrou ele no server.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::authCmdDisconnectPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::authCmdConfirmDisconnectPlayer(uint32_t _player_uid) {
	// Message Server n�o usa esse Comando
	return;
}

void message_server::authCmdNewMailArrivedMailBox(uint32_t /*_player_uid*/, uint32_t /*_mail_id*/) {
	// Message Server n�o usa esse Comando
	return;
}

void message_server::authCmdNewRate(uint32_t _tipo, uint32_t _qntd) {
	
	try {

		updateRateAndEvent(_tipo, _qntd);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::authCmdNewRate][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::authCmdReloadGlobalSystem(uint32_t _tipo) {
	
	try {

		reloadGlobalSystem(_tipo);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::authCmdReloadGlobalSystem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) {

	try {

		auto s = m_player_manager.findPlayer(_aspi.uid);

		if (s != nullptr) {

			// Confirma Login com outro server
			confirmLoginOnOtherServer(*s, _req_server_uid, _aspi);

		}else
			_smp::message_pool::getInstance().push(new message("[message_server::authCmdConfirmSendInfoPlayerOnline][WARNING] Player[UID=" + std::to_string(_aspi.uid) 
					+ "] retorno do confirma login com Auth Server do Server[UID=" + std::to_string(_req_server_uid) + "], mas o palyer nao esta mais conectado.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::authCmdConfirmSendInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[message_server::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[message_server::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_message_server = reinterpret_cast< message_server*>(_arg);

	switch (_msg_id) {
	case 1:	// Insert Block IP
	{
		auto cmd_ibi = reinterpret_cast<CmdInsertBlockIP*>(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[message_server::SQLDBResponse][Log] Inseriu Block IP[IP=" + cmd_ibi->getIP()
				+ ", MASK=" + cmd_ibi->getMask() + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[message_server::SQLDBResponse][Log] Inseriu Block IP[IP=" + cmd_ibi->getIP()
				+ ", MASK=" + cmd_ibi->getMask() + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 2:	// Update Server Rate Config Info
	{

		auto cmd_urci = reinterpret_cast<CmdUpdateRateConfigInfo*>(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[message_server::SQLDBResponse][Log] Atualizou Rate Config Info[SERVER_UID=" + std::to_string(cmd_urci->getServerUID())
				+ ", " + cmd_urci->getInfo().toString() + "]", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
};

void message_server::shutdown_time(int32_t _time_sec) {

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
			throw exception("[message_server::shutdown_time][Error] nao conseguiu criar o timer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 51, 0));
	}
}

bool message_server::sendUpdatePlayerLogoutToFriends(player& _session) {
	CHECK_SESSION_BEGIN("sendUpdatePlayerLogoutToFriends");

	bool ret = true;

	packet p;

	try {

		// J� enviou os pacote de update de logou do player
		// Se estiver 0 troca para 1, e retorno o que estava e compara, s� sai se for 1
#if defined(_WIN32)
		if (InterlockedCompareExchange(&_session.m_pi.m_logout, 1, 0) == 1)
			return false;
#elif defined(__linux__)
		uint32_t check_m = 0; // Compare
		if (!__atomic_compare_exchange_n(&_session.m_pi.m_logout, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED) && check_m == 1u)
			return false;
#endif

		// Resposta para os amigos do player, que ele deslogou
		p.init_plain((unsigned short)0x30);

		p.addUint16(0x10F);	// Sub Packet Id

		p.addUint32(_session.m_pi.uid);

		packet_func::friend_broadcast(m_player_manager.findAllFriend(_session.m_pi.m_friend_manager.getAllFriendAndGuildMember(true/*Not Send To Block Friend*/)), p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::sendUpdatePlayerLogoutToFriends][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error
		ret = false;
	}

	return ret;
}

void message_server::onAcceptCompleted(session *_session) {

	if (_session == nullptr)
		throw exception("[message_server::onAcceptCompleted][Error] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 60, 0));

	if (!_session->getState())
		throw exception("[message_server::onAcceptCompleted][Error] _session is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 61, 0));

	if (!_session->isConnected())
		throw exception("[message_server::onAcceptCompleted][Error] _session is not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 62, 0));

	packet p((unsigned short)0x2E);

	p.addUint8(1);
	p.addUint8(1);
	p.addUint32(_session->m_key);

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

void message_server::onDisconnected(session *_session) {

	if (_session == nullptr)
		throw exception("[message_server::onDisconnect][Error] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 60, 0));

	player *p = reinterpret_cast< player* >(_session);

	bool ret = true;

	try {
		
#if defined(_WIN32)
		if (p->isConnected() && p->getState() && InterlockedCompareExchange(&p->m_pi.m_logout, p->m_pi.m_logout, 0) == 0) {
#elif defined(__linux__)
		uint32_t check_m = 0; // Compare

		if (p->isConnected() && p->getState() && __atomic_compare_exchange_n(&p->m_pi.m_logout, &check_m, 0, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
			
			// S� envia o UpdatePlayerLogout se o player estiver autorizado(Fez o login)
			if (p->m_is_authorized)
				ret = sendUpdatePlayerLogoutToFriends(*p);

		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::onDisconnected][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	// Log, Para n�o mostrar essa mensagem 2x
	if (ret)
		_smp::message_pool::getInstance().push(new message("[message_server::onDisconnected][Log] Player Desconectou ID: " + std::string(p->m_pi.id) + " UID: " + std::to_string(p->m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

}

void message_server::onHeartBeat() {
	// Aqui depois tenho que colocar uma verifica��o que eu queira fazer no server
	// Esse fun��o � chamada na thread monitor

	try {

		// Server ainda n�o est� totalmente iniciado
		if (m_state != INITIALIZED)
			return;

		// Begin Check System Singleton Static

		// Carrega Smart Calculator Lib, S� inicializa se ele estiver ativado
		if (m_si.rate.smart_calculator && !sSmartCalculator::getInstance().hasStopped() && !sSmartCalculator::getInstance().isLoad())
			sSmartCalculator::getInstance().load();

		// End Check System Singleton Static

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::onHeartBeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return;
}

void message_server::onStart() {

	try {

		// N�o faz nada por enquanto

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::onStart][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool message_server::checkCommand(std::stringstream& _command) {

	std::string s = "";

	_command >> s;

	if (!s.empty() && s.compare("exit") == 0)
		return true;	// Sai
	else if (!s.empty() && s.compare("reload_files") == 0) {

		reload_files();
		
		_smp::message_pool::getInstance().push(new message("Message Server files has been reloaded.", CL_FILE_LOG_AND_CONSOLE));
	
	}else if (!s.empty() && s.compare("reload_socket_config") == 0) {

		// Ler novamento o arquivo de configura��o do socket
		if (m_accept_sock != nullptr)
			m_accept_sock->reload_config_file();
		else
			_smp::message_pool::getInstance().push(new message("[message_server::checkCommand][WARNING] m_accept_sock(socket que gerencia os socket que pode aceitar etc) is invalid.", CL_FILE_LOG_AND_CONSOLE));

	}else if (!s.empty() && s.compare("smart_calc") == 0) {

		std::string sTipo = "";

		_command >> sTipo;

		if (!sTipo.empty()) {

			if (m_si.rate.smart_calculator) {

				if (sTipo.compare("reload") == 0)
					sSmartCalculator::getInstance().load(); // Recarrega
				else if (sTipo.compare("close") == 0) {

					// Set Flag stopped para n�o reiniciar sozinho no onHearBeat
					sSmartCalculator::getInstance().setStop(true);

					sSmartCalculator::getInstance().close(); // Fecha

				}else if (sTipo.compare("start") == 0)
					sSmartCalculator::getInstance().load(); // Inicia(Load)
				else if (sTipo.compare("chat_discord") == 0) {

					m_chat_discord = !m_chat_discord;

					// Log
					_smp::message_pool::getInstance().push(new message("[message_server::checkCommand][Log] Chat Discord Flag agora esta " + std::string(m_chat_discord ? "Ativado" : "Desativado"), CL_ONLY_CONSOLE));
				
				}else
					_smp::message_pool::getInstance().push(new message("[message_server::checkCommand][Error] Unknown Command: \"smart_calc " + sTipo + "\"", CL_ONLY_CONSOLE));
			
			}else
				_smp::message_pool::getInstance().push(new message("[message_server::checkCommand][Error] Smart Calculator not active, exec Command Event smart_calc to active it.", CL_ONLY_CONSOLE));

		}else
			_smp::message_pool::getInstance().push(new message("[message_server::checkCommand][Error] Unknown Command: \"smart_calc " + sTipo + "\"", CL_ONLY_CONSOLE));

	}else if (!s.empty() && s.compare("snapshot") == 0) {

		try {
			int *bad_ptr_snapshot = nullptr;
			*bad_ptr_snapshot = 2;
		}catch (exception& e) {
			UNREFERENCED_PARAMETER(e);

			// Log
			_smp::message_pool::getInstance().push(new message("[message_server::checkCommand][Log] Snapshot comando executado.", CL_FILE_LOG_AND_CONSOLE));
		}

	}else
		_smp::message_pool::getInstance().push(new message("Unknown Command: " + s, CL_ONLY_CONSOLE));

	return false;
}

bool message_server::checkPacket(session& _session, packet *_packet) {

	///--------------- INICIO CHECK PACKET SESSION
	if (/*(std::clock() - _session.m_check_packet.tick) <= CHK_PCKT_INTERVAL_LIMIT /* Dentro do Interval */
		/*&& _session.m_check_packet.packet_id == _packet->getTipo() /* Mesmo pacote */
		_session.m_check_packet.checkPacketId(_packet->getTipo())) {
		
		uint32_t limit_count = CHK_PCKT_COUNT_LIMIT;	// Padr�o

		// Game Server Vou deixar em 10x, por que � mais dificil atacar ele
		switch (_packet->getTipo()) {
		case 0x1D:	// Pacote que Atualiza Status, ele envia um a cada pagina de lista de amigo
			limit_count += 7;	// Add + 7 para ficar 10, 10 p�gina no m�ximo de amigos
			break;
		case 0:
		default:
			limit_count += 2;	// Sobe para 5 por que o message server � mais dif�cil de atacar ele
		}

		if (_session.m_check_packet.incrementCount() >= limit_count/*_session.m_check_packet.count++ >= limit_count /* deu o limit de count DERRUBA a session */) {

			_smp::message_pool::getInstance().push(new message("[message_server::checkPacket][WARNING] Tentativa de DDoS ataque com pacote ID: (0x" 
					+ hex_util::lltoaToHex(_packet->getTipo()) + ") " + std::to_string(_packet->getTipo()) + ". IP=" + std::string(_session.m_ip), CL_FILE_LOG_AND_CONSOLE));

			DisconnectSession(&_session);

			return false;
		}
	}//else // Zera o contador, n�o entrou no intervalo ou n�o � o mesmo pacote
		//_session.m_check_packet.count = 0;

	// Att packet ID
	//_session.m_check_packet.packet_id = _packet->getTipo();

	// Att tick
	//_session.m_check_packet.tick = std::clock();

	///--------------- FIM CHECK PACKET SESSION

	return true;
}

void message_server::init_option_accepted_socket(SOCKET _accepted) {

	BOOL tcp_nodelay = 1u;

	// set KEEPALIVE
	//if (setsockopt(_accepted, SOL_SOCKET, SO_KEEPALIVE, (char*)&keep_alive, sizeof(keep_alive)) == SOCKET_ERROR)
		//throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu setar o socket option KEEPALIVE para: " + std::to_string(keep_alive), 
				//STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, WSAGetLastError()));

#if defined(_WIN32)
	tcp_keepalive keep;
	DWORD retk = 0;

	keep.onoff = 1;
	keep.keepalivetime = 20000; // Estava 10000(10 segundos)
	keep.keepaliveinterval = 2000; // Estava 1000(1 segundo)

	if (WSAIoctl(_accepted, SIO_KEEPALIVE_VALS, &keep, sizeof(keep), nullptr, 0, &retk, nullptr, nullptr) == SOCKET_ERROR)
		throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu setar o socket option KEEPALIVE[ONOFF=" + std::to_string(keep.onoff) 
				+ ", TIME=" + std::to_string(keep.keepalivetime) + ", INTERVAL=" + std::to_string(keep.keepaliveinterval) 
				+ "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, WSAGetLastError()));

	// KEEPALIVE Log
	_smp::message_pool::getInstance().push(new message("[message_server::init_option_accepted_socket][Log] socket[ID=" + std::to_string(_accepted) 
			+ "] KEEPALIVE[ONOFF=" + std::to_string(keep.onoff) + ", TIME=" + std::to_string(keep.keepalivetime) 
			+ ", INTERVAL=" + std::to_string(keep.keepaliveinterval) + "] foi ativado para esse ", CL_FILE_LOG_AND_CONSOLE));

	// Deixa o TCP_NODELAY o nagle algorithm desligado para teste se o message mesmo com o keep alive ainda fica caindo e reconectando
	if (setsockopt(_accepted, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu desabilitar tcp delay(nagle algorithm).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, WSAGetLastError()));
#elif defined(__linux__)
	int flag = O_NONBLOCK;
	if (fcntl(_accepted.fd, F_SETFL, flag) != 0)
		throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu habilitar o NONBLOCK(fcntl).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, errno));

	// keepalive do linux
	int keepalive = 1; // Enable
	if (setsockopt(_accepted.fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) == -1)
		throw exception("[message_server::init_option_accepted_socket][error] nao conseguiu habilitaro keepalive(setsockopt).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, errno));

	keepalive = 20; // Keepalive 20 segundos
	if (setsockopt(_accepted.fd, SOL_TCP, TCP_KEEPIDLE, &keepalive, sizeof(keepalive)) == -1)
		throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu setar o keepalive idl time(setsockopt).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, errno));

	keepalive = 2;	// Keepalive interval probs, 2 segundos
	if (setsockopt(_accepted.fd, SOL_TCP, TCP_KEEPINTVL, &keepalive, sizeof(keepalive)) == -1)
		throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu setar o keepalive interval pobs(setsockopt).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, errno));

	// KEEPALIVE Log
	_smp::message_pool::getInstance().push(new message("[message_server::init_option_accepted_socket][Log] socket[ID=" + std::to_string(_accepted.fd)
			+ "] KEEPALIVE[ONOFF=1, TIME=20000, INTERVAL=2000] foi ativado para esse ", CL_FILE_LOG_AND_CONSOLE));

	if (setsockopt(_accepted.fd, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		throw exception("[message_server::init_option_accepted_socket][Error] nao conseguiu desabilitar tcp delay(nagle algorithm).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, errno));
#endif
}

void message_server::config_init() {

	// Tipo Server
	m_si.tipo = 3;

	// Recupera Valores de rate do server do banco de dados
	CmdRateConfigInfo cmd_rci(m_si.uid, true);	// Waiter

	NormalManagerDB::add(0, &cmd_rci, nullptr, nullptr);

	cmd_rci.waitEvent();

	if (cmd_rci.getException().getCodeError() != 0 || cmd_rci.isError()/*Deu erro na consulta n�o tinha o rate config info para esse server, pode ser novo*/) {

		if (cmd_rci.getException().getCodeError() != 0)
			_smp::message_pool::getInstance().push(new message("[message_server::config_init][ErrorSystem] " + cmd_rci.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("[message_server::config_init][Error] nao conseguiu recuperar os valores de rate do server[UID=" 
				+ std::to_string(m_si.uid) + "] no banco de dados. Utilizando valores padroes de rates.", CL_FILE_LOG_AND_CONSOLE));
		
		m_si.rate.scratchy = 100;
		m_si.rate.papel_shop_rare_item = 100;
		m_si.rate.papel_shop_cookie_item = 100;
		m_si.rate.treasure = 100;
		m_si.rate.memorial_shop = 100;
		m_si.rate.chuva = 100;
		m_si.rate.grand_zodiac_event_time = 1u;	// Ativo por padr�o
		m_si.rate.grand_prix_event = 1u;		// Ativo por padr�o
		m_si.rate.golden_time_event = 1u;		// Ativo por padr�o
		m_si.rate.login_reward_event = 1u;		// Ativo por padr�o
		m_si.rate.bot_gm_event = 1u;			// Ativo por padr�o
		m_si.rate.smart_calculator = 1u;		// Atibo por padr�o
		
		m_si.rate.angel_event = 0u;				// Desativado por padr�o
		m_si.rate.pang = 0u;
		m_si.rate.exp = 0u;
		m_si.rate.club_mastery = 0u;

		// Atualiza no banco de dados
		NormalManagerDB::add(2, new CmdUpdateRateConfigInfo(m_si.uid, m_si.rate), message_server::SQLDBResponse, this);
	
	}else {	// Conseguiu recuperar com sucesso os valores do server

		m_si.rate.scratchy = cmd_rci.getInfo().scratchy;
		m_si.rate.papel_shop_rare_item = cmd_rci.getInfo().papel_shop_rare_item;
		m_si.rate.papel_shop_cookie_item = cmd_rci.getInfo().papel_shop_cookie_item;
		m_si.rate.treasure = cmd_rci.getInfo().treasure;
		m_si.rate.memorial_shop = cmd_rci.getInfo().memorial_shop;
		m_si.rate.chuva = cmd_rci.getInfo().chuva;
		m_si.rate.grand_zodiac_event_time = cmd_rci.getInfo().grand_zodiac_event_time;
		m_si.rate.grand_prix_event = cmd_rci.getInfo().grand_prix_event;
		m_si.rate.golden_time_event = cmd_rci.getInfo().golden_time_event;
		m_si.rate.login_reward_event = cmd_rci.getInfo().login_reward_event;
		m_si.rate.bot_gm_event = cmd_rci.getInfo().bot_gm_event;
		m_si.rate.smart_calculator = cmd_rci.getInfo().smart_calculator;

		m_si.rate.angel_event = cmd_rci.getInfo().angel_event;
		m_si.rate.pang = cmd_rci.getInfo().pang;
		m_si.rate.exp = cmd_rci.getInfo().exp;
		m_si.rate.club_mastery = cmd_rci.getInfo().club_mastery;
	}
}

void message_server::reload_files() {

	server::config_init();
	config_init();

	sIff::getInstance().reload();
}

void message_server::reload_systems() {

	// Recarrega IFF_STRUCT
	sIff::getInstance().load();

	// Recarrega Smart Calculator Lib, s� recarrega se ele estiver ativado
	if (m_si.rate.smart_calculator)
		sSmartCalculator::getInstance().load();
}

void message_server::reloadGlobalSystem(uint32_t _tipo) {

	try {

		switch (_tipo) {
		case 0:	// Reload All Globals Systems
			reload_systems();
			break;
		case 1:		// IFF
			// Recarrega IFF_STRUCT
			sIff::getInstance().load();
			break;
		case 2:		// Card
		case 3:		// Comet Refill
		case 4:		// Papel Shop
		case 5:		// Box
		case 6:		// Memorial Shop
		case 7:		// Cube e Coin
		case 8:		// Treasure Hunter
		case 9:		// Drop
		case 10:	// Attendance Reward
		case 11:	// Map Course Dados
		case 12:	// Approach Mission
		case 13:	// Grand Zodiac Event
		case 14:	// Coin Cube Location Update System
		case 15:	// Golden Time System
		case 16:	// Login Reward System
		case 17:	// Bot GM Event
			// N�o tem esses Systemas aqui
			break;
		case 18:	// Smart Calculator Lib
			// Recarrega Smart Calculator Lib
			sSmartCalculator::getInstance().load();
			break;
		default:
			throw exception("[message_server::reloadGlobalSystem][Error] Tipo[VALUE=" + std::to_string(_tipo) + "] desconhecido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 400, 0));
		}

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::reloadGlobalSystem][Log] Recarregou o Sistema[Tipo=" + std::to_string(_tipo) + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[message_server::reloadGlobalSystem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void message_server::updateRateAndEvent(uint32_t _tipo, uint32_t _qntd) {

	try {

		if (_qntd == 0u && _tipo != 9/*Grand Zodiac Event Time*/ && _tipo != 10/*Angel Event*/ 
				&& _tipo != 11/*Grand Prix Event*/ && _tipo != 12/*Golden Time Event*/ && _tipo != 13/*Login Reward Event*/
				&& _tipo != 14/*Bot GM Event*/ && _tipo != 15/*Smart Calculator*/)
			throw exception("[message_server::updateRateAndEvent][Error] Rate[TIPO=" + std::to_string(_tipo) + ", QNTD="
					+ std::to_string(_qntd) + "], qntd is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 120, 0));

		switch (_tipo) {
		case 0: // Pang
		case 1:	// Exp
		case 2:	// Mastery
		case 3:	// Chuva
		case 4:	// Treasure Hunter
		case 5:	// Scratchy
		case 6:	// Papel Shop Rare Item
		case 7:	// Papel Shop Cookie Item
		case 8: // Memorial shop
		case 9: // Event Grand Zodiac Time Event [Active/Desactive]
		case 10: // Event Angel (Reduce 1 quit per game done)
		case 11: // Grand Prix Event
		case 12: // Golden Time Event
		case 13: // Login Reward System Event
		case 14: // Bot GM Event
		case 15: // Smart Calculator
		{
			m_si.rate.smart_calculator = (short)_qntd;

			// Recarrega o Smart Calculator System se ele foi ativado
			if (m_si.rate.smart_calculator)
				reloadGlobalSystem(18/*Smart Calculator*/);

			break;
		}
		default:
			throw exception("[message_server::updateRateAndEvent][Error] troca Rate[TIPO=" + std::to_string(_tipo) + ", QNTD="
					+ std::to_string(_qntd) + "], tipo desconhecido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 120, 0));
		}

		// Update no DB os server do server que foram alterados
		NormalManagerDB::add(2, new CmdUpdateRateConfigInfo(m_si.uid, m_si.rate), message_server::SQLDBResponse, this);

		// Log
		_smp::message_pool::getInstance().push(new message("[message_server::updateRateAndEvent][Log] New Rate[Tipo=" + std::to_string(_tipo) + ", QNTD="
				+ std::to_string(_qntd) + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));

	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[game_server::updateRateAndEvent][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}
