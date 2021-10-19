// Arquivo packet_func_client.cpp
// Crido em 21/10/2017 por Acrisio
// Implementação da classe packet_func para o cliente(BOT)

// Tem que ter o pack aqui se não da erro na hora da allocação do HEAP
#pragma pack(1)

#include <WinSock2.h>
#include "../../Projeto IOCP/THREAD POOL/threadpl_client.h"
#include "packet_func_client.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/PACKET/packet.h"
#include "../../Projeto IOCP/UTIL/hex_util.h"
#include "../../Projeto IOCP/UTIL/util_time.h"
#include "../SESSION/player.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../../Projeto IOCP/UTIL/iphelp.h"
#include <ctime>

#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../Multi Client/multi_client.h"

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).make((_session)->m_key); \
						 WSABUF mb = (_packet).getMakedBuf(); \
						 try { \
							(_session)->requestSendBuffer(mb.buf, mb.len); \
						 }catch (exception& e) { \
							if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 1)) \
								throw; \
						 } \

//#define MAKE_BEGIN_PACKET ParamPacket pp; \
//						  pp.m_pw = reinterpret_cast<ParamWorkerC *>(arg); \
//						  pp.m_session = pd._packet->getSession(); \
//						  _smp::message_pool.push(new message("Trata pacote " + std::to_string(pd._packet->getTipo()) + "(0x" + hex_util::ltoaToHex(pd._packet->getTipo()) + ")", 3)); \
//
//#define MAKE_BEGIN_PACKET_SUB ParamPacket_sub *pp_sub = reinterpret_cast<ParamPacket_sub *>(arg); \
//							  ParamWorkerC pw = pp_sub->m_pw; \
//							  ParamPacket pp = { &pw, pp_sub->m_session }; \
//							  delete pp_sub; \
//
//#define MAKE_BEGIN_PACKET_WITHOUT_MSG ParamPacket pp; \
//						  pp.m_pw = reinterpret_cast<ParamWorkerC *>(arg); \
//						  pp.m_session = _session; \

//#define CLIENT_VERSION "855.00"
#define CLIENT_VERSION "SS.R7.989.00"

//#define packet_ver_key "{782AE110-2EEF-4c61-B030-A53F17634F7D}"
#define packet_ver_key "{a65ec0d3-7bde-4ec1-8e73-4b3e0eac6abb}"

#define de_encrypt_packet_ver(packet_ver) unsigned char *_tmp_p_ver = (unsigned char*)&(packet_ver); \
					size_t i, index; \
					for (i = 0, index = 0; i < sizeof(packet_ver_key); i++) { \
						_tmp_p_ver[index] ^= packet_ver_key[i]; \
						index = (index == 3) ? 0 : ++index; \
					} \

#define encrypt_packet_ver(packet_ver) unsigned char *_tmp_p_ver = (unsigned char*)&(packet_ver); \
					size_t i, index; \
					for (i = sizeof(packet_ver_key) - 1, index = 0; i > 0; --i) { \
						_tmp_p_ver[index] ^= packet_ver_key[i]; \
						index = (index == 0) ? 3 : --index; \
					} \

using namespace stdA;

int packet_func::packet000(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	_smp::message_pool::getInstance().push(new message("Time: " + std::to_string((std::clock() - pd._session.m_time_start) / (double)CLOCKS_PER_SEC), CL_ONLY_FILE_TIME_LOG));

	_smp::message_pool::getInstance().push(new message("Recebi Primeiro packet com a key; Player ID: "  + std::string(pd._session.m_ci.m_id), CL_ONLY_FILE_LOG));

	pd._session.m_key = (unsigned char)pd._packet->readInt32();
	pd._session.m_ci.m_uid = pd._packet->readInt32();	// ServerGuid

	pd._session.m_ci.m_luid = pd._session.m_ci.m_uid;

	pd._session.m_time_start = std::clock();	// Time send packet 1 response to login server

	packet p;
	pacote001(p, &pd._session);
	session_send(p, &pd._session, 1);

	return 0;
};

int packet_func::packet001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	ClientInfo *ci = &pd._session.m_ci;

	int err = (unsigned char)pd._packet->readInt8();
	
	if (err == 0) {
		pd._packet->readString();
		ci->m_uid = pd._packet->readInt32();
		ci->m_cap = pd._packet->readInt32();
		ci->m_level = pd._packet->readInt8();
		pd._packet->readInt32();
		pd._packet->readInt8();

		int opt = pd._packet->readInt32();

		if (opt == 0) {
			strcpy_s(ci->m_nickname, pd._packet->readString().c_str());
			pd._packet->readInt16();
		}else if (opt == 5) {
			pd._packet->readString();	// Data de compilacao do server acho
			pd._packet->readString();	// senha do pass de dentro do jogo(so o KR tem) (ACHO QUE � ISSO)
			pd._packet->readInt64();
			strcpy_s(ci->m_nickname, pd._packet->readString().c_str());
		}
	}else if (err == 4) {
		_smp::message_pool::getInstance().push(new message("Erro ja estava logado desloga.", CL_ONLY_CONSOLE));

		packet p;
		pacote004(p, &pd._session, 0);
		session_send(p, &pd._session, 1);
	}else if (err == 0xE2)
		_smp::message_pool::getInstance().push(new message("Erro ao logar. Erro code: " + std::to_string(pd._packet->readInt32()), CL_ONLY_CONSOLE));
	else
		_smp::message_pool::getInstance().push(new message("Erro ao logar: " + std::to_string(err), CL_ONLY_CONSOLE));

	return 0;
};

int packet_func::packet002(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	ClientInfo *ci = &pd._session.m_ci;
	
	ci->m_list_servers.num_servers = pd._packet->readInt8();

	if (ci->m_list_servers.num_servers > 0) {
		
		ci->m_list_servers.a_servers = new ServerInfo[ci->m_list_servers.num_servers];

		pd._packet->readBuffer(ci->m_list_servers.a_servers, sizeof(ServerInfo) * ci->m_list_servers.num_servers);

		ci->m_guid = ci->m_list_servers.a_servers[0].uid;

		std::srand(777 * std::clock());

		Sleep(1000 | (std::rand() & 3000));

		packet p;
		pacote003(p, &pd._session, ci->m_guid);
		session_send(p, &pd._session, 1);
	}else {
		_smp::message_pool::getInstance().push(new message("Nao tem nenhum server: ON; Player UID: " + std::to_string(pd._session.m_ci.m_uid) + "\tNickname: " + std::string(pd._session.m_ci.m_nickname)
			+ "\tDesconectar e tenta novamente", CL_FILE_LOG_AND_CONSOLE));

		//mc->DisconnectSession(&pd._session);

		// Coloca no monitor para reconectar
		pd._session.m_ci.m_uid = ~0;
	}
	return 0;
};

int packet_func::packet003(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	int opt = pd._packet->readInt32();

	if (opt == 0) {
		ClientInfo *ci = &pd._session.m_ci;

		strcpy_s(ci->m_keys[1], pd._packet->readString().c_str());

		if (ci->m_list_servers.num_servers > 0) {
			ClientInfo ci_tmp{};

			ci_tmp.m_uid = pd._session.m_ci.m_uid;
			ci_tmp.m_cap = pd._session.m_ci.m_cap;
			ci_tmp.m_level = pd._session.m_ci.m_level;
			strcpy_s(ci_tmp.m_id, pd._session.m_ci.m_id);
			strcpy_s(ci_tmp.m_pass, pd._session.m_ci.m_pass);
			strcpy_s(ci_tmp.m_nickname, pd._session.m_ci.m_nickname);
			strcpy_s(ci_tmp.m_client_version, pd._session.m_ci.m_client_version);
			memcpy_s(ci_tmp.m_keys, sizeof(ci_tmp.m_keys), pd._session.m_ci.m_keys, sizeof(ci_tmp.m_keys));
			ci_tmp.m_packet_version = pd._session.m_ci.m_packet_version;

			mc->ConnectAndAssoc(ci->m_list_servers.a_servers[0].ip, ci->m_list_servers.a_servers[0].port, ci_tmp);
			
			// Desconecta do Login Server
			//::shutdown(pd._session.m_sock, SD_BOTH);
			mc->DisconnectSession(&pd._session);
		}else {
			_smp::message_pool::getInstance().push(new message("Nao tem nenhum server: ON; Player UID: " + std::to_string(pd._session.m_ci.m_uid) + "\tNickname: " + std::string(pd._session.m_ci.m_nickname)
				+ "\tDesconectar e tenta novamente", CL_FILE_LOG_AND_CONSOLE));

			mc->DisconnectSession(&pd._session);

			// Coloca no monitor para reconectar
			pd._session.m_ci.m_uid = ~0;
		}
	}

	return 0;
};

int packet_func::packet006(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	pd._packet->readBuffer(&pd._session.m_ci.m_chat_macro, sizeof(chat_macro_user));

	return 0;
};

int packet_func::packet009(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	size_t num_msn_servers = pd._packet->readInt8();

	if (num_msn_servers > 0) {
		ServerInfo *a_msns = new ServerInfo[num_msn_servers];

		pd._packet->readBuffer(a_msns, sizeof(ServerInfo));

		delete[] a_msns;
	}

	return 0;
};

int packet_func::packet010(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	strcpy_s(pd._session.m_ci.m_keys[0], pd._packet->readString().c_str());

	return 0;
};

int packet_func::packet03F(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	//ClientInfo *ci = &pd._session.m_ci;

	pd._packet->readInt8(); // opt 1
	pd._packet->readInt8(); // opt 2

	pd._session.m_key = pd._packet->readInt8();
	//pd._session.m_ci.m_uid = pd._session.m_ci.m_guid;

	packet p;
	pacote002(p, &pd._session);
	session_send(p, &pd._session, 1);

	return 0;
};

int packet_func::packet044(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	int err = (unsigned char)pd._packet->readInt8();

	if (err == 0) {
		// Pacote Principal
		_smp::message_pool::getInstance().push(new message("Pacote Principal.", CL_ONLY_CONSOLE));
	}else if (err == 1)
		_smp::message_pool::getInstance().push(new message("Erro ao logar com o game server. erro code: " + std::to_string((int)pd._packet->readInt8()), CL_ONLY_CONSOLE));
	else if (err == 0xD3)
		_smp::message_pool::getInstance().push(new message("Logou no game server com sucesso.", CL_ONLY_CONSOLE));
	else if (err == 0xD2)
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Porcentagem de carregagem dos dados do player no server: " + std::to_string(pd._packet->readInt32() / 36.f * 100.f), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("Porcentagem de carregagem dos dados do player no server: " + std::to_string(pd._packet->readInt32() / 36.f * 100.f), CL_ONLY_FILE_LOG));
#endif
	return 0;
};

int packet_func::packet04D(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	ClientInfo *ci = &pd._session.m_ci;

	ci->m_list_canais.num_canais = pd._packet->readInt8();

	if (ci->m_list_canais.num_canais > 0) {

		ci->m_list_canais.a_canais = new ChannelInfo[ci->m_list_canais.num_canais];

		pd._packet->readBuffer(ci->m_list_canais.a_canais, sizeof(ChannelInfo) * ci->m_list_canais.num_canais);

		ci->m_cuid = ci->m_list_canais.a_canais[0].id;	// Index 0
		
		InterlockedExchange(&ci->m_RC, 1);	// Recebi canais

		_smp::message_pool::getInstance().push(new message("Recebi Canais: " + std::to_string(ci->m_list_canais.num_canais), CL_ONLY_CONSOLE));

		entraCanal(&pd._session, (unsigned char)ci->m_cuid);
	}

	return 0;
};

int packet_func::packet04E(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	int err = pd._packet->readInt8();

	packet p;

	if (err == 1 || err == 0) {
		_smp::message_pool::getInstance().push(new message("Player ID: " + std::string(pd._session.m_ci.m_id) + ". Entrou no canal: " + std::to_string(pd._session.m_ci.m_cuid & 0xFF) + " com sucesso.", CL_ONLY_CONSOLE));

		pd._session.m_ci.m_channel = (unsigned char)pd._session.m_ci.m_cuid;

		// Entra na lobby
		pacote081(p, &pd._session);
		session_send(p, &pd._session, 1);
	}else if (err != 0 && err != 1) {
		_smp::message_pool::getInstance().push(new message("Player ID: " + std::string(pd._session.m_ci.m_id) + ". Nao conseguiu entrar no canal: " + std::to_string(pd._session.m_ci.m_cuid & 0xFF) + ". Tenta entrar em outro se tiver.", CL_FILE_LOG_AND_CONSOLE));

		unsigned short channel_index = (unsigned short)(pd._session.m_ci.m_cuid >> 8) + 1;

		if (pd._session.m_ci.m_list_canais.num_canais > channel_index) {
			pd._session.m_ci.m_cuid = pd._session.m_ci.m_list_canais.a_canais[channel_index].id | (channel_index << 8);

			_smp::message_pool::getInstance().push(new message("Player ID: " + std::string(pd._session.m_ci.m_id) + ". Entrando no canal: " + std::to_string(pd._session.m_ci.m_cuid & 0xFF), CL_FILE_LOG_AND_CONSOLE));

			pacote004(p, &pd._session, 1, (unsigned char)pd._session.m_ci.m_cuid);
			session_send(p, &pd._session, 1);
		}else
			_smp::message_pool::getInstance().push(new message("Player ID: " + std::string(pd._session.m_ci.m_id) + ". Todos canais estao cheios.", CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet0F5(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	_smp::message_pool::getInstance().push(new message("Player ID: " + std::string(pd._session.m_ci.m_id) + ". Entrou na lobby.", CL_ONLY_CONSOLE));

	pd._session.m_ci.m_lobby = 1;

	// Tenta entrar na sala 0
	packet p((unsigned short)0x09);

	p.addUint16(0u);	// Sala Número 0
	p.addUint16(0u);	// Nenhum senha

	session_send(p, &pd._session, 1);

	// Cria o timer de mandar msg na lobby para esse player
	std::vector< DWORD > interval = { 100 };

	std::srand(std::clock() * 7 * (DWORD_PTR)&pd._session * (DWORD_PTR)&pd._session);

	if ((pd._session.m_ci.m_timer_msg_lobby = mc->postTimer(15000 + (std::rand() % 5000),
		new (timer::timer_param)
		{
			job(packet_func::sendMessage, &pd._session, nullptr),
			mc->getJobPool()
		}, interval, timer::PERIODIC_INFINITE)) == nullptr)
		_smp::message_pool::getInstance().push(new message("Erro ao criar o Timer em packet_func::packet0F5()", CL_FILE_LOG_AND_CONSOLE));

	return 0;
};

int packet_func::packet0D7(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	_smp::message_pool::getInstance().push(new message("Pacote de verificacao de Game Guard. Sai do jogo antes de ele derrubar", CL_FILE_LOG_AND_CONSOLE));

	mc->DisconnectSession(&pd._session);

	return 0;
};

int packet_func::packet1A9(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	if (pd._packet->readInt8() == 1) {
		pd._session.m_ci.m_TTL = pd._packet->readInt32();

		// Cria o timer de mandar TTL Time To Live (HeartBeat)
		std::vector< DWORD > interval = { pd._session.m_ci.m_TTL };

		if ((pd._session.m_ci.m_timer_ttl = mc->postTimer(pd._session.m_ci.m_TTL,
				new (timer::timer_param)
				{
					job(packet_func::HeartBeat, _arg1, &pd._session),
					mc->getJobPool()
				}, interval, timer::PERIODIC_INFINITE)) == nullptr)
			_smp::message_pool::getInstance().push(new message("Erro ao criar o Timer em packet_func::packet1A9()", CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

int packet_func::packet1AD(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	packet p;

	if (pd._packet->readInt32() == 1) {
		strcpy_s(pd._session.m_ci.m_web_key, pd._packet->readString().c_str());

		switch (pd._session.m_ci.m_TRWK) {
			case 0x02:	// GACHA
				_smp::message_pool::getInstance().push(new message(std::string(pd._session.m_ci.link_gacha) + "?ID=" + std::string(pd._session.m_ci.m_id) + "&KID=" + std::string(pd._session.m_ci.m_web_key), CL_FILE_LOG_AND_CONSOLE));
				pacote0A1(p, &pd._session, pd._session.m_ci.m_TRWK);
				session_send(p, &pd._session, 1);
				break;
			case 0x05:	// GUILD
				_smp::message_pool::getInstance().push(new message(std::string(pd._session.m_ci.link_guild) + "?ID=" + std::string(pd._session.m_ci.m_id) + "&KID=" + std::string(pd._session.m_ci.m_web_key), CL_FILE_LOG_AND_CONSOLE));
				pacote0A1(p, &pd._session, pd._session.m_ci.m_TRWK);
				session_send(p, &pd._session, 1);
				break;
			case 0x0A:	// ENTRANCE
				_smp::message_pool::getInstance().push(new message(std::string(pd._session.m_ci.link_entrance) + "?ID=" + std::string(pd._session.m_ci.m_id) + "&KID=" + std::string(pd._session.m_ci.m_web_key), CL_FILE_LOG_AND_CONSOLE));
				pacote0A1(p, &pd._session, pd._session.m_ci.m_TRWK);
				session_send(p, &pd._session, 1);
				break;
			case 0x0B:	// LINK ON LOBBY 1
				_smp::message_pool::getInstance().push(new message(std::string(pd._session.m_ci.link_weblink[0]) + "?ID=" + std::string(pd._session.m_ci.m_id) + "&KID=" + std::string(pd._session.m_ci.m_web_key), CL_FILE_LOG_AND_CONSOLE));
				pacote0A1(p, &pd._session, pd._session.m_ci.m_TRWK);
				session_send(p, &pd._session, 1);
				break;
			case 0x0C:	// ENTRY POINT(cookies) mas n�o pede senha
				break;
			case 0x0D: // LINK ON LOBBY 2
				_smp::message_pool::getInstance().push(new message(std::string(pd._session.m_ci.link_weblink[1]) + "?ID=" + std::string(pd._session.m_ci.m_id) + "&KID=" + std::string(pd._session.m_ci.m_web_key), CL_FILE_LOG_AND_CONSOLE));
				pacote0A1(p, &pd._session, 0x0B);
				session_send(p, &pd._session, 1);
				break;
			case 0x0E: // LINK ON LOBBY 2
				_smp::message_pool::getInstance().push(new message(std::string(pd._session.m_ci.link_weblink[2]) + "?ID=" + std::string(pd._session.m_ci.m_id) + "&KID=" + std::string(pd._session.m_ci.m_web_key), CL_FILE_LOG_AND_CONSOLE));
				pacote0A1(p, &pd._session, 0x0B);
				session_send(p, &pd._session, 1);
				break;
		}
	}else
		_smp::message_pool::getInstance().push(new message("Erro ao pegar WebKey.", CL_FILE_LOG_AND_CONSOLE));

	return 0;
};

int packet_func::packet_svFazNada(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	// Esse pacote é para os pacotes que o server envia para o cliente
	// e não precisa de tratamento depois que foi enviado para o cliente

	return 0;
};

int packet_func::packet_sv001(void* _arg1, void* _arg2) {
	MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2);

	_smp::message_pool::getInstance().push(new message("Send Packet001 Time: " + std::to_string((std::clock() - pd._session.m_time_start) / (double)CLOCKS_PER_SEC), CL_ONLY_FILE_TIME_LOG));

	return 0;
};

int packet_func::pacote001(packet& p, player *_session, int option) {

	p.init_plain((unsigned short)0x001);

	std::vector< IP_ADAPTER_INFO > v_adapters = ip_help::getVectorAdaptersInfo();

	p.addString(_session->m_ci.m_id);
	p.addString(_session->m_ci.m_pass);
	p.addInt8(2);
	p.addInt64(0);
	p.addInt64(0x7FFFFFFFFFFFFFFF);

	if (!v_adapters.empty())
		p.addString(ip_help::makeMACString(v_adapters[0].Address, v_adapters[0].AddressLength));

	return 0;
};

int packet_func::pacote002(packet& p, player *_session, int option) {

	p.init_plain((unsigned short)0x002);

	int packet_ver = _session->m_ci.m_packet_version;

	de_encrypt_packet_ver(packet_ver);

	std::vector< IP_ADAPTER_INFO > v_adapters = ip_help::getVectorAdaptersInfo();

	p.addString(_session->m_ci.m_id);
	p.addInt32(_session->m_ci.m_uid);
	p.addInt32(0);
	p.addInt16(0x6696);
	p.addString(_session->m_ci.m_keys[0]);
	p.addString(_session->m_ci.m_client_version);
	p.addInt32(packet_ver);

	if (!v_adapters.empty())
		p.addString(ip_help::makeMACString(v_adapters[0].Address, v_adapters[0].AddressLength));

	p.addString(_session->m_ci.m_keys[1]);

	return 0;
};

int packet_func::pacote003(packet& p, player *_session, unsigned long guid) {

	p.init_plain((unsigned short)0x03);

	p.addInt32(guid);

	return 0;
};

int packet_func::pacote003g(packet& p, player *_session, std::string _nickname, std::string _msg) {

	p.init_plain((unsigned short)0x03);

	p.addString(_nickname);
	p.addString(_msg);

	return 0;
};

int packet_func::pacote004(packet& p, player *_session, int option, unsigned char canal_id) {

	p.init_plain((unsigned short)0x04);

	// OPT 0 é Login Server Confirma desloga player já logado
	// Tem mais coisas, que é premium, pcbang dailyquest mailbox não lido e etc
	// que ver isso depois que o cliente entra no server e não antes como está agora
	if (option == 1/*Entra Canal Game Server*/)
		p.addInt8(canal_id);

	return 0;
};

int packet_func::pacote03D(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0x3D);
	
	return 0;
};

int packet_func::pacote081(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0x81);

	return 0;
};

int packet_func::packet09E(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0x9E);

	return 0;
};

int packet_func::pacote0A1(packet& p, player *_session, unsigned char opt) {

	p.init_plain((unsigned short)0xA1);

	p.addInt8(opt);

	return 0;
};

int packet_func::pacote0A2(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0xA2);

	return 0;
};

int packet_func::pacote0C1(packet& p, player *_session, unsigned char opt) {

	p.init_plain((unsigned short)0xC1);

	p.addInt8(opt);

	return 0;
};

int packet_func::pacote0F4(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0xF4);

	return 0;
};

int packet_func::pacote0FB(packet& p, player *_session) {
	
	p.init_plain((unsigned short)0xFB);

	return 0;
};

inline void packet_func::session_send(packet& p, player *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_CLIENT, 1, 2));

	MAKE_SEND_BUFFER(p, s);

	SHOW_DEBUG_PACKET(p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);

	//delete p;
};

inline void packet_func::session_send(std::vector< packet* > v_p, player *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_CLIENT, 1, 2));

	for (auto i = 0ul; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			MAKE_SEND_BUFFER(*v_p[i], s);

			SHOW_DEBUG_PACKET(*v_p[i], _debug);

			SHOW_DEBUG_FINAL_PACKET(mb, _debug);

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::session_send()", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};

void packet_func::abrirWeblink(player *_session, unsigned char who) {

	packet p;

	if (who == 12/*Entry Point - n�o precisa de chave*/) {
		_session->m_ci.m_TRWK = who; // Quem (Qual weblink abrir)

		_smp::message_pool::getInstance().push(new message(std::string(_session->m_ci.link_point) + "?ID=" + std::string(_session->m_ci.m_id) + "&KID=", CL_FILE_LOG_AND_CONSOLE));
		
		pacote0A1(p, _session, _session->m_ci.m_TRWK);
		session_send(p, _session, 1);
		pacote0C1(p, _session, 2);
		session_send(p, _session, 1);
	}else {
		_smp::message_pool::getInstance().push(new message("Request WebKey.", CL_FILE_LOG_AND_CONSOLE));

		_session->m_ci.m_TRWK = who; // Quem (Qual weblink abrir)

		pacote0FB(p, _session);
		session_send(p, _session, 1);
	}
};

void packet_func::entraCanal(player *_session, unsigned char canal_id) {
	
	packet p;
	pacote004(p, _session, 1, canal_id);
	session_send(p, _session, 1);
};

int packet_func::sendMessage(void *_arg1, void *_arg2) {
	player *_session = reinterpret_cast< player* >(_arg1);
	UNREFERENCED_PARAMETER(_arg2);

	if (_session == nullptr)
		return 1;

	// _session não está mais conectada para o timer
	if (!_session->getState()) {
		_session->m_ci.m_timer_msg_lobby->stop();
		
		return 0;
	}
	
	sendMessage(_session, _session->m_ci.m_nickname, "[Canal ID: " + std::to_string(_session->m_ci.m_cuid & 0xFF) + "] [Timer->Ola Lobby]");

	return 0;
};

int packet_func::HeartBeat(void *_arg1, void *_arg2) {

	if (_arg1 == nullptr || _arg2 == nullptr)
		return 1;

	_MAKE_BEGIN_CLIENT(_arg1);
	player *_session = reinterpret_cast< player* >(_arg2);

	// _session não está mais conectada para o timer
	if (!_session->getState()) {
		_session->m_ci.m_timer_ttl->stop();

		return 0;
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("Player UID: " + std::to_string(_session->m_ci.m_uid) + "\tTTL tick.", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("Player UID: " + std::to_string(_session->m_ci.m_uid) + "\tTTL tick.", CL_ONLY_FILE_LOG));
#endif

	packet p;
	pacote0F4(p, _session);
	session_send(p, _session);

	return 0;
};

void packet_func::sendMessage(player *_session, std::string _nickname, std::string _msg) {

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("Player UID: " + std::to_string(_session->m_ci.m_uid)
		+ "\tNickname: " + _nickname + "\r\nMessage to Lobby: " + _msg, CL_FILE_LOG_AND_CONSOLE));
#endif

	packet p;
	pacote003g(p, _session, _nickname, _msg);
	session_send(p, _session, 1);
};
