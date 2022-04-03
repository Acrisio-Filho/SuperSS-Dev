// Arquivo game.cpp
// Criado em 12/08/2018 as 14:50 por Acrisio
// Implementação da classe Game

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "game.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PACKET/packet_func_sv.h"

#include "drop_system.hpp"

#include "item_manager.h"

#include "../PANGYA_DB/cmd_update_clubset_workshop.hpp"
#include "../PANGYA_DB/cmd_update_map_statistics.hpp"
#include "../PANGYA_DB/cmd_update_item_slot.hpp"

#include "premium_system.hpp"

#include <algorithm>

#include "../Game Server/game_server.h"

#include "../UTIL/map.hpp"

#include "../../Projeto IOCP/TIMER/timer_manager.h"
#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include "../../Projeto IOCP/Smart Calculator/Smart Calculator.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"
#include "../UTIL/club3d.hpp"

#if defined(__linux__)
#include <numbers> // std::numbers::pi
#include <cmath>
#endif

#define CHECK_SESSION(method) if (!_session.getState()) \
									throw exception("[Game::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION(std::string("request") + method); \
							  if (_packet == nullptr) \
									throw exception("[Game::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[Game::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1, 4)); \

// Artefact of EXP
#define ART_LUMINESCENT_CORAL 0x1A0001AAu			// 2%
#define ART_TROPICAL_TREE 0x1A0001ACu				// 4%
#define ART_TWIN_LUNAR_MIRROR 0x1A0001AEu			// 6%
#define ART_MACHINA_WRENCH 0x1A0001B0u				// 8%
#define ART_SILVIA_MANUAL 0x1A0001B2u				// 10%

// Artefact of Rain Rate
#define ART_SCROLL_OF_FOUR_GODS 0x1A0001C0u		// 5%
#define ART_ZEPHYR_TOTEM 0x1A0001C2u				// 10%
#define ART_DRAGON_ORB 0x1A0001F8u					// 20%

// Artefact Frozen Flame
#define ART_FROZEN_FLAME 0x1A0001FAu				// Mantém os itens Active Equipados, ou sejá não consome eles

uint32_t devil_wings[]{ 0x08016801, 0x08058801, 0x08098801, 0x080dc801, 0x08118801, 0x08160801, 0x08190801, 0x081e2801, 0x08214801, 0x08254801, 0x082d480d, 0x08314801, 0x0839c801 };
uint32_t obsidian_wings[]{ 0x0801680c, 0x0805880c, 0x0809880c, 0x080dc80c, 0x0811880c, 0x0816080c, 0x0819080c, 0x081e280c, 0x0821480c, 0x0825480c, 0x0829480c, 0x082d4806, 0x0831480a, 0x0839c80a };
uint32_t corrupt_wings[]{ 0x08016810, 0x08058810, 0x08098810, 0x080dc810, 0x08118810, 0x08160810, 0x08190810, 0x081e2810, 0x08214810, 0x08254810, 0x08294812, 0x082d4803, 0x0831480d, 0x0839c80d };
uint32_t hasegawa_chirain[]{ 0x8190809, 0x8254808 };	// Item de manter chuva
uint32_t hat_spooky_halloween[]{ 0x0801880b, 0x0805a832, 0x0809a835, 0x080d084c, 0x0811a831, 0x0815a062, 0x0818e05c, 0x081d8837, 0x08212059, 0x08252026 };
uint32_t hat_lua_sol[]{ 0x8018803, 0x805A828, 0x809A827, 0x80D083F, 0x811A823, 0x815A855, 0x818E050, 0x81D8825, 0x821204A, 0x8252015 };	// Dá 20% de Exp e Pang
uint32_t hat_birthday[]{ 0x08000885, 0x0805a81c, 0x08080832, 0x080d0836, 0x08100038, 0x0815a047, 0x0818e048, 0x081d881e, 0x0821203c, 0x08252013, 0x0829200e, 0x082d6000 };

// Motion Item da Treasure Hunter Point Também
uint32_t motion_item[]{ 0x08026800, 0x08026801, 0x08026802, 0x08064800, 0x08064801, 0x08064802, 0x08064803, 0x080A2800,
							0x080A2801, 0x080A2802, 0x080E4800, 0x080E4801, 0x080E4802, 0x08122800, 0x08122801, 0x08122802,
							0x0816E801, 0x0816E802, 0x0816E803, 0x0816E805, 0x816E806/*Kooh Dolph*/, 0x081A4800, 0x081A4801, 0x081EA800, 0x08228800,
							0x08228801, 0x08228802, 0x08228803, 0x08268800, 0x082A6800, 0x082E4800, 0x082E4801, 0x08320800,
							0x08320801, 0x08320802, 0x083A4800, 0x083A4801, 0x083A4802 };

using namespace stdA;

Game::Game(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie) 
	: m_players(_players), m_ri(_ri), m_rv(_rv), m_channel_rookie(_channel_rookie), m_start_time{0}, m_player_info(), m_course(nullptr),
		m_game_init_state(-1), m_state(false), m_player_order(), m_timer(nullptr), m_player_report_game{} {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_sync_finish_game);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

#if defined(_WIN32)
	InterlockedExchange(&m_sync_send_init_data, 0u);
#elif defined(__linux__)
	__atomic_store_n(&m_sync_send_init_data, 0u, __ATOMIC_RELAXED);
#endif

	// Tirei o make player info da classe base e coloquei para a base da classe mais alta
	/*for (auto& el : m_players)
		makePlayerInfo(*el);*/

	// Inicializar Artefact Info Of Game
	initArtefact();
	
	// Inicializar o rate chuva dos itens equipado dos players no jogo
	initPlayersItemRainRate();

	// Inicializa a flag persist rain next hole
	initPlayersItemRainPersistNextHole();
	
	// Map Dados Estáticos
	if (!sMap::getInstance().isLoad())
		sMap::getInstance().load();

	auto map = sMap::getInstance().getMap(m_ri.course & 0x7F);

	if (map == nullptr)
		_smp::message_pool::getInstance().push(new message("[Game::Game][Error][WARNING] tentou pegar o Map dados estaticos do course[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "], mas nao conseguiu encontra na classe do Server.", CL_FILE_LOG_AND_CONSOLE));

	// Cria Course
	m_course = new Course(m_ri, m_channel_rookie, ((map == nullptr) ? 1.f : map->star), m_rv.rain, m_rv.persist_rain);

}

Game::~Game() {

	if (m_course != nullptr)
		delete m_course;

	clear_player_order();

	clearAllPlayerInfo();

	clear_time();

	if (!m_player_report_game.empty())
		m_player_report_game.clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
	pthread_mutex_destroy(&m_cs_sync_finish_game);
#endif

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Game::~Game][Log] Game destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[Game::~Game][Log] Game destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_ONLY_FILE_LOG));
#endif // _DEBUG
}

void Game::sendInitialData(player& _session) {

	packet p;

	try {

		// Course
		p.init_plain((unsigned short)0x52);

		p.addUint8(m_ri.course);
		p.addUint8(m_ri.tipo_show);
		p.addUint8(m_ri.modo);
		p.addUint8(m_ri.qntd_hole);
		p.addUint32(m_ri.trofel);
		p.addUint32(m_ri.time_vs);
		p.addUint32(m_ri.time_30s);

		// Hole Info, Hole Spinning Cube, end Seed Random Course
		m_course->makePacketHoleInfo(p);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::sendInitialData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

}

void Game::sendInitialDataAfter(player& _session) {
	UNREFERENCED_PARAMETER(_session);
}

player* Game::findSessionByOID(uint32_t _oid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& el) {
		return el->m_oid == _oid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_players.end() ? *it : nullptr);
}

player* Game::findSessionByUID(uint32_t _uid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& el) {
		return el->m_pi.uid == _uid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_players.end() ? *it : nullptr);
}

player* Game::findSessionByNickname(std::string _nickname) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& el) {
		return (_nickname.compare(el->m_pi.nickname) == 0);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_players.end() ? *it : nullptr);
}

player* Game::findSessionByPlayerGameInfo(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {
		_smp::message_pool::getInstance().push(new message("[Game::findSessionByPlayerGameInfo][Error] PlayerGameInfo* _pgi is invalid(nullptr)", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
		return _el.second == _pgi;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_player_info.end() ? it->first : nullptr);
}

PlayerGameInfo* Game::getPlayerInfo(player *_session) {

	if (_session == nullptr)
		throw exception("[Game::getPlayerInfo][Error] _session is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1, 0));

	PlayerGameInfo *pgi = nullptr;
	std::map< player*, PlayerGameInfo* >::iterator i;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if ((i = m_player_info.find(_session)) != m_player_info.end())
		pgi = i->second;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return pgi;
}

std::vector< player* > Game::getSessions(player *_session) {
	
	std::vector< player* > v_sessions;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// Se _session for diferente de nullptr retorna todas as session, menos a que foi passada no _session
	for (auto& el : m_players)
		if (el != nullptr && el->getState() && el->m_pi.mi.sala_numero != -1 && (_session == nullptr || _session != el))
			v_sessions.push_back(el);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_sessions;
}

SYSTEMTIME& Game::getTimeStart() {
	return m_start_time;
}

void Game::addPlayer(player& _session) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_players.push_back(&_session);

	makePlayerInfo(_session);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

bool Game::deletePlayer(player* _session, int _option) {
	UNREFERENCED_PARAMETER(_option);

	if (_session == nullptr)
		throw exception("[Game::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 50, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find(m_players.begin(), m_players.end(), _session);

	if (it != m_players.end())
		m_players.erase(it);
	else
		_smp::message_pool::getInstance().push(new message("[Game::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return false;
}

void Game::requestActiveAutoCommand(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveAutoCommand");

	packet p;

	try {

		INIT_PLAYER_INFO("requestActiveAutoCommand", "tentou ativar Auto Command no jogo", &_session);

		if (!pgi->premium_flag) { // (não é)!PREMIUM USER

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(AUTO_COMMAND_TYPEID);

			if (pWi == nullptr)
				throw exception("[Game::requestActiveAutoCommand][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar o Auto Command Item[TYPEID="
						+ std::to_string(AUTO_COMMAND_TYPEID) + "], mas ele nao tem o item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1, 0x550001));

			if (pWi->STDA_C_ITEM_QNTD < 1)
				throw exception("[Game::requestActiveAutoCommand][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar o Auto Command Item[TYPEID="
						+ std::to_string(AUTO_COMMAND_TYPEID) + "], mas ele nao tem quantidade suficiente do item[QNTD=" + std::to_string(pWi->STDA_C_ITEM_QNTD) + ", QNTD_REQ=1]. Hacker ou Bug",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 2, 0x550002));

			auto it = pgi->used_item.v_passive.find(pWi->_typeid);

			if (it == pgi->used_item.v_passive.end())
				throw exception("[Game::requestActiveAutoCommand][Error] player[UID = " + std::to_string(_session.m_pi.uid) + "] tentou ativar Auto Command, mas ele nao tem ele no item passive usados do server. Hacker ou Bug",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 13, 0));

			if ((short)it->second.count >= pWi->STDA_C_ITEM_QNTD)
				throw exception("[Game::requestActiveAutoCommand][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Auto Command, mas ele ja usou todos os Auto Command. Hacker ou Bug",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 14, 0));

			// Add +1 ao item passive usado
			it->second.count++;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::requestActiveAutoCommand][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// !@ Não sei o que esse pacote faz, não encontrei no meu antigo pangya
		// Resposta Error
		p.init_plain((unsigned short)0x22B);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::GAME) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x550001);

		packet_func::session_send(p, &_session, 1);
	}
}

void Game::requestActiveAssistGreen(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveAssistGreen");

	packet p;

	try {

		uint32_t item_typeid = _packet->readUint32();

		if (item_typeid == 0)
			throw exception("[Game::requestActiveAssistGreen][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Assist[TYPEID=" 
					+ std::to_string(item_typeid) + "] do Green, mas o item_typeid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1, 0x5200101));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(item_typeid);

		if (pWi == nullptr)
			throw exception("[Game::requestActiveAssistGreen][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Assist[TYPEID="
					+ std::to_string(item_typeid) + "] do Green, mas o Assist Mode do player nao esta ligado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 2, 0x5200102));

		// Resposta para Active Assist Green
		p.init_plain((unsigned short)0x26B);

		p.addUint32(0);	// OK

		p.addUint32(pWi->_typeid);
		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::requestActiveAssistGreen][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x26B);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::GAME) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200100);

		packet_func::session_send(p, &_session, 1);
	}
}

void Game::requestMarkerOnCourse(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestLoadGamePercent(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestStartTurnTime(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestUnOrPause(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestExecCCGChangeWind(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestExecCCGChangeWeather(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestReplyContinue() {}

bool Game::requestUseTicketReport(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);

	return false;
}

void Game::requestChangeWindNextHoleRepeat(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestStartAfterEnter(job& _job) {
	UNREFERENCED_PARAMETER(_job);
}

void Game::requestEndAfterEnter() {}

void Game::requestUpdateTrofel() {}

void Game::requestTeamFinishHole(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

#define FIND_ELEMENT_ARRAY_OF_ARRAY(_arr1, _arr2) (std::find_if((_arr1), LAST_ELEMENT_IN_ARRAY((_arr1)), [&](auto& _element) { \
	return std::find((_arr2), LAST_ELEMENT_IN_ARRAY((_arr2)), _element) != LAST_ELEMENT_IN_ARRAY((_arr2)); \
}) != LAST_ELEMENT_IN_ARRAY((_arr1))) \

// Usa o Padrão delas
bool Game::stopTime() {

	clear_time();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Game::stopTime][Log] Parou o Timer[Tempo=" + std::to_string(m_ri.time_30s > 0 ? m_ri.time_30s : m_ri.time_vs) 
			+ (m_timer != nullptr ? ", STATE=" + std::to_string(m_timer->getState()) + "]" : "]"), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	return true;
}

bool Game::pauseTime() {

	if (m_timer != nullptr) {
		m_timer->pause();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Game::pauseTime][Log] pausou o Timer[Tempo=" + std::to_string(m_ri.time_30s > 0 ? m_ri.time_30s : m_ri.time_vs) 
				+ ", STATE=" + std::to_string(m_timer->getState()) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		return true;
	}

	return false;
}

bool Game::resumeTime() {

	if (m_timer != nullptr) {
		m_timer->start();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Game::resumerTime][Log] Retomou o Timer[Tempo=" + std::to_string(m_ri.time_30s > 0 ? m_ri.time_30s : m_ri.time_vs) 
				+ ", STATE=" + std::to_string(m_timer->getState()) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		return true;
	}

	return false;
}

void Game::requestPlayerReportChatGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("PlayerReportChatGame");

	packet p;

	try {

		// Verifica se o player já reportou o jogo
		auto it = m_player_report_game.find(_session.m_pi.uid);

		if (it != m_player_report_game.end()) {

			// Player já reportou o jogo
			p.init_plain((unsigned short)0x94);

			p.addUint8(1u);	// Player já reportou o jogo

			packet_func::session_send(p, &_session, 1);

		}else { // Primeira vez que o palyer report o jogo

			// add ao mapa de uid de player que reportaram o jogo
			m_player_report_game[_session.m_pi.uid] = _session.m_pi.uid;

			// Faz Log de quem está na sala, quando pangya, o update enviar o chat log verifica o chat
			// por que parece que o pangya não envia o chat, ele só cria um arquivo, acho que quem envia é o update
			std::string log = "";

			for (auto& el : m_players)
				if (el != nullptr)
					log = log + "UID: " + std::to_string(_session.m_pi.uid) + "\tID: " + el->m_pi.id + "\tNICKNAME: " + el->m_pi.nickname + "\n";

			// Log
			_smp::message_pool::getInstance().push(new message("[Game::requestPlayerReportChatGame][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] reportou o chat do jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + "] Log{" + log + "}", CL_FILE_LOG_AND_CONSOLE));

			// Reposta para o cliente
			p.init_plain((unsigned short)0x94);

			p.addUint8(0u); // Sucesso

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::requestPlayerReportChatGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x94);

		p.addUint8(1u);	// 1 já foi feito report do jogo por esse player

		packet_func::session_send(p, &_session, 1);
	}
}

void Game::initPlayersItemRainRate() {

	// Characters Equip
	for (auto& s : m_players) {
		if (s->getState() && 
#if defined(_WIN32)
			s->m_sock != INVALID_SOCKET 
#elif defined(__linux__)
			s->m_sock.fd != INVALID_SOCKET 
#endif
		&& s->isConnected()) {	// Check Player Connected
			
			if (s->m_pi.ei.char_info == nullptr) {	// Player não está com character equipado, kika dele do jogo
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.", 
						CL_FILE_LOG_AND_CONSOLE));
				continue;// Kika aqui "deletePlayer(s);"
			}

			// Devil Wings
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, devil_wings)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Devil Wings no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 10;
			}

			// Obsidian Wings
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, obsidian_wings)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Obsidian Wings no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 10;
			}

			// Corrupt Wings
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, corrupt_wings)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Corrupt Wings no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 15;
			}

			// Hasegawa Chirain
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, hasegawa_chirain)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Hasegawa Chirain Item Part no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 10;
			}

			// Hat Spooky Halloween -- Esse aqui "tenho que colocar a regra para funcionar só na epoca do halloween"
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, hat_spooky_halloween)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Hat Spooky Halloween no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 10;
			}

			// Card Efeito 19 rate chuva
			auto it = std::find_if(s->m_pi.v_cei.begin(), s->m_pi.v_cei.end(), [](auto& _el) {
				return sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 2/*Special*/ && _el.efeito == 19;
			});

			if (it != s->m_pi.v_cei.end()) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Card[TYPEID=" 
						+ std::to_string(it->_typeid) + ", EFEITO=" + std::to_string(it->efeito) + ", EFEITO_QNTD=" + std::to_string(it->efeito_qntd) + "] especial", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				if (it->efeito_qntd > 0)
					m_rv.rain += it->efeito_qntd;
			}

			// Mascot Poltergeist -- Esse aqui "tenho que colocar a regra para funcionar só na epoca do halloween"
			if (s->m_pi.ei.mascot_info != nullptr && s->m_pi.ei.mascot_info->_typeid == 0x40000029/*Poltergeist*/) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Mascot Poltergeist[TYPEID=" 
						+ std::to_string(s->m_pi.ei.mascot_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.mascot_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 10;
			}

			// Caddie Big Black Papel
			if (s->m_pi.ei.cad_info != nullptr && s->m_pi.ei.cad_info->_typeid == 0x1C00000E/*Big Black Papel*/) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainRate][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Caddie Big Black Papel[TYPEID="
						+ std::to_string(s->m_pi.ei.cad_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.cad_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				m_rv.rain += 10;
			}
		}
	}
}

void Game::initPlayersItemRainPersistNextHole() {
	
	// Characters Equip
	for (auto& s : m_players) {
		if (s->getState() && 
#if defined(_WIN32)
			s->m_sock != INVALID_SOCKET 
#elif defined(__linux__)
			s->m_sock.fd != INVALID_SOCKET 
#endif
		&& s->isConnected()) {	// Check Player Connected

			if (s->m_pi.ei.char_info == nullptr) {	// Player não está com character equipado, kika dele do jogo
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.",
						CL_FILE_LOG_AND_CONSOLE));
				continue;// Kika aqui "deletePlayer(s);"
			}

			// Devil Wings
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, devil_wings)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Devil Wings no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				// sai por que só precisa que 1 player tenha o item para valer para o game todo
				m_rv.persist_rain = 1;
				return;
			}

			// Obsidian Wings
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, obsidian_wings)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Obsidian Wings no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				// sai por que só precisa que 1 player tenha o item para valer para o game todo
				m_rv.persist_rain = 1;
				return;
			}

			// Corrupt Wings
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, corrupt_wings)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Corrupt Wings no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				// sai por que só precisa que 1 player tenha o item para valer para o game todo
				m_rv.persist_rain = 1;
				return;
			}

			// Hasegawa Chirain
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, hasegawa_chirain)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Hasegawa Chirain Item Part no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				// sai por que só precisa que 1 player tenha o item para valer para o game todo
				m_rv.persist_rain = 1;
				return;
			}

			// Hat Spooky Halloween -- Esse aqui "tenho que colocar a regra para funcionar só na epoca do halloween"
			if (FIND_ELEMENT_ARRAY_OF_ARRAY(s->m_pi.ei.char_info->parts_typeid, hat_spooky_halloween)) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Hat Spooky Halloween no Character[TYPEID="
						+ std::to_string(s->m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(s->m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				// sai por que só precisa que 1 player tenha o item para valer para o game todo
				m_rv.persist_rain = 1;
				return;
			}

			// Card Efeito 31 Persist chuva para o proximo hole
			auto it = std::find_if(s->m_pi.v_cei.begin(), s->m_pi.v_cei.end(), [](auto& _el) {
				return sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 2/*Special*/ && _el.efeito == 31;
			});

			if (it != s->m_pi.v_cei.end()) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[Game::initPlayersItemRainPersistNextHole][Log] player[UID=" + std::to_string(s->m_pi.uid) + "] esta equipado com Card[TYPEID="
						+ std::to_string(it->_typeid) + ", EFEITO=" + std::to_string(it->efeito) + ", EFEITO_QNTD=" + std::to_string(it->efeito_qntd) + "] especial", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
				// sai por que só precisa que 1 player tenha o item para valer para o game todo
				m_rv.persist_rain = 1;
				return;
			}
		}
	}
}

void Game::initArtefact() {

	switch (m_ri.artefato) {
	// Artefact of EXP
	case ART_LUMINESCENT_CORAL:
		m_rv.exp += 2;
		break;
	case ART_TROPICAL_TREE:
		m_rv.exp += 4;
		break;
	case ART_TWIN_LUNAR_MIRROR:
		m_rv.exp += 6;
		break;
	case ART_MACHINA_WRENCH:
		m_rv.exp += 8;
		break;
	case ART_SILVIA_MANUAL:
		m_rv.exp += 10;
		break;
	// End
	// Artefact of Rain Rate
	case ART_SCROLL_OF_FOUR_GODS:
		m_rv.rain += 5;
		break;
	case ART_ZEPHYR_TOTEM:
		m_rv.rain += 10;
		break;
	case ART_DRAGON_ORB:
		m_rv.rain += 20;
		break;
	// End
	}
}

PlayerGameInfo::eCARD_WIND_FLAG Game::getPlayerWindFlag(player& _session) {

	if (_session.m_pi.ei.char_info == nullptr) {	// Player n�o est� com character equipado, kika dele do jogo
		_smp::message_pool::getInstance().push(new message("[Game::getPlayerWindFlag][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.",
			CL_FILE_LOG_AND_CONSOLE));
		return PlayerGameInfo::eCARD_WIND_FLAG::NONE;// Kika aqui "deletePlayer(s);"
	}

	// 3 R, 17 SR, 13 SC, 12 N
	auto it = std::find_if(_session.m_pi.v_cei.begin(), _session.m_pi.v_cei.end(), [&](auto& _el) {
		return (_session.m_pi.ei.char_info->id == _el.parts_id && _session.m_pi.ei.char_info->_typeid == _el.parts_typeid) 
			&& sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 1/*Caddie*/ && (_el.efeito == 3 || _el.efeito == 17 || _el.efeito == 13 || _el.efeito == 12);
	});

	if (it != _session.m_pi.v_cei.end()) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Game::getPlayerWindFlag][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Card[TYPEID="
				+ std::to_string(it->_typeid) + ", EFEITO=" + std::to_string(it->efeito) + ", EFEITO_QNTD=" + std::to_string(it->efeito_qntd) + "] Caddie", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		switch (it->efeito) {
		case 3:
			return PlayerGameInfo::eCARD_WIND_FLAG::RARE;
		case 12:
			return PlayerGameInfo::eCARD_WIND_FLAG::NORMAL;
		case 13:
			return PlayerGameInfo::eCARD_WIND_FLAG::SECRET;
		case 17:
			return PlayerGameInfo::eCARD_WIND_FLAG::SUPER_RARE;
		}
	}

	return PlayerGameInfo::eCARD_WIND_FLAG::NONE;
}

int Game::initCardWindPlayer(PlayerGameInfo* _pgi, unsigned char _wind) {

	if (_pgi == nullptr)
		throw exception("[Game::initCardWindPlayer][Error] PlayerGameInfo* _pgi is invalid(nullptr). Ao tentar inicializar o card wind player no jogo. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1, 4));

	switch (_pgi->card_wind_flag) {
	case PlayerGameInfo::eCARD_WIND_FLAG::NORMAL:
		if (_wind == 8)	// 9m Wind
			return -1;
		break;
	case PlayerGameInfo::eCARD_WIND_FLAG::RARE:
		if (_wind > 0)	// All Wind
			return -1;
		break;
	case PlayerGameInfo::eCARD_WIND_FLAG::SUPER_RARE:
		if (_wind >= 5)	// High(strong) Wind
			return -2;
		break;
	case PlayerGameInfo::eCARD_WIND_FLAG::SECRET:
		if (_wind >= 5)		// High(strong) Wind
			return -2;
		else if (_wind > 0) // Low(weak) Wind, 1m não precisa diminuir
			return -1;
		break;
	}

	return 0;
}

PlayerGameInfo::stTreasureHunterInfo Game::getPlayerTreasureInfo(player& _session) {

	PlayerGameInfo::stTreasureHunterInfo pti{ 0 };

	if (_session.m_pi.ei.char_info == nullptr) {	// Player não está com character equipado, kika dele do jogo
		_smp::message_pool::getInstance().push(new message("[Game::getPlayerTreasureInfo][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.",
			CL_FILE_LOG_AND_CONSOLE));
		return pti;// Kika aqui "deletePlayer(s);"
	}

	std::vector< CardEquipInfoEx* > v_cei;

	// 9 N, 10 R, 14 SR por Score. 8 N, R, SR todos score
	std::for_each(_session.m_pi.v_cei.begin(), _session.m_pi.v_cei.end(), [&](auto& _el) {
		if ((_session.m_pi.ei.char_info->id == _el.parts_id && _session.m_pi.ei.char_info->_typeid == _el.parts_typeid)
			&& sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 1/*Caddie*/ && (_el.efeito == 8 || _el.efeito == 9 || _el.efeito == 10 || _el.efeito == 14))
			v_cei.push_back(&_el);
	});

	if (!v_cei.empty()) {
		for (auto& el : v_cei) {
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[Game::getPlayerTreasureInfo][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Card[TYPEID="
				+ std::to_string(el->_typeid) + ", EFEITO=" + std::to_string(el->efeito) + ", EFEITO_QNTD=" + std::to_string(el->efeito_qntd) + "] Caddie", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

			switch (el->efeito) {
			case 8:	// Todos Score
				pti.all_score = (unsigned char)el->efeito_qntd;
				break;
			case 9:		// Par
				pti.par_score = (unsigned char)el->efeito_qntd;
				break;
			case 10:	// Birdie
				pti.birdie_score = (unsigned char)el->efeito_qntd;
				break;
			case 14:	// Eagle
				pti.eagle_score = (unsigned char)el->efeito_qntd;
				break;
			}
		}
	}

	// Card Efeito 18 Aumenta o treasure point para qualquer score por 2 horas
	auto it = std::find_if(_session.m_pi.v_cei.begin(), _session.m_pi.v_cei.end(), [](auto& _el) {
		return sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 2/*Special*/ && _el.efeito == 18;
	});

	if (it != _session.m_pi.v_cei.end()) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Game::getPlayerTreasureInfo][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Card[TYPEID="
			+ std::to_string(it->_typeid) + ", EFEITO=" + std::to_string(it->efeito) + ", EFEITO_QNTD=" + std::to_string(it->efeito_qntd) + "] especial", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		
		pti.all_score += (unsigned char)it->efeito_qntd;
	}

	/// Todos que dão Drop Rate da treasue hunter point, então aonde dá o drop rate já vai dá o treasure point
	/// Angel Wings deixa que ela é uma excessão não tem os valores no IFF, é determinado pelo server e o ProjectG
	// Passarinho gordo aumenta 30 treasure hunter point para todos scores
	//if (_session.m_pi.ei.mascot_info != nullptr && _session.m_pi.ei.mascot_info->_typeid == MASCOT_FAT_BIRD) 
		//pti.all_score += 30;	// +30 all score

	// Verifica se está com asa de anjo equipada (shop ou gacha), aumenta 30 treasure hunter point para todos scores
	if (_session.m_pi.ei.char_info->AngelEquiped() && _session.m_pi.ui.getQuitRate() < GOOD_PLAYER_ICON)
		pti.all_score += 30;	// +30 all score

	return pti;
}

void Game::updatePlayerAssist(player& _session) {

	INIT_PLAYER_INFO("updatePlayerAssist", "tentou atualizar assist pang no jogo", &_session);

	if (pgi->assist_flag && pgi->level > 10/*Maior que 10 "great of Beginner A" Junior E ~ Inifinit Legend I*/)
		pgi->data.pang = (uint64_t)(pgi->data.pang * 0.7f);	// - 30% dos pangs

}

void Game::initGameTime() {
	GetLocalTime(&m_start_time);
}

uint32_t Game::getRankPlace(player& _session) {
	
	INIT_PLAYER_INFO("getRankPlace", "tentou pegar o lugar no rank do jogo", &_session);

	auto it = std::find(m_player_order.begin(), m_player_order.end(), pgi);

	return (it != m_player_order.end()) ? (uint32_t)(it - m_player_order.begin()) : ~0u;
}

DropItemRet Game::requestInitDrop(player& _session) {

	if (!sDropSystem::getInstance().isLoad())
		sDropSystem::getInstance().load();

	DropItemRet dir{ 0 };

	INIT_PLAYER_INFO("requestInitDrop", "tentou inicializar drop do hole no jogo", &_session);

	DropSystem::stCourseInfo ci{ 0 };

	auto hole = m_course->findHole(pgi->hole);

	if (hole == nullptr)
		throw exception("[Game::requestInitDrop][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializar Drop System do hole[NUMERO=" 
				+ std::to_string(pgi->hole) + "] no jogo, mas nao encontrou o hole no course do game. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 200, 0));

	// Init Course Info Drop System
	ci.artefact = m_ri.artefato;
	ci.char_motion = pgi->char_motion_item;
	ci.course = hole->getCourse() & 0x7F;		// Course do Hole, Por que no SSC, cada hole é um course
	ci.hole = pgi->hole;
	ci.seq_hole = (unsigned char)m_course->findHoleSeq(pgi->hole);
	ci.qntd_hole = m_ri.qntd_hole;
	ci.rate_drop = pgi->used_item.rate.drop;

	if (_session.m_pi.ei.char_info != nullptr && _session.m_pi.ui.getQuitRate() < GOOD_PLAYER_ICON)
		ci.angel_wings = _session.m_pi.ei.char_info->AngelEquiped();
	else
		ci.angel_wings = 0u;

	// Artefact Pang Drop
	if (m_ri.qntd_hole == ci.seq_hole && m_ri.qntd_hole == 18) {	// Ultimo Hole, de 18h Game
		auto art_pang = sDropSystem::getInstance().drawArtefactPang(ci, (uint32_t)m_players.size());

		if (art_pang._typeid != 0) {	// Dropou

			dir.v_drop.push_back(art_pang);

			if (art_pang.qntd >= 30) {	// Envia notice que o player ganhou jackpot

				packet p((unsigned short)0x40);

				p.addUint8(10);	// JackPot

				p.addString(_session.m_pi.nickname);

				p.addUint16(0);	// size Msg

				p.addUint32(art_pang.qntd * 500);

				packet_func::game_broadcast(*this, p, 1);
			}
		}
	}

	// Drop Event Course
	auto course = sDropSystem::getInstance().findCourse(ci.course & 0x7F);

	if (course != nullptr) {	// tem Drop nesse Course
		auto drop_event = sDropSystem::getInstance().drawCourse(*course, ci);

		if (!drop_event.empty())	// Dropou
			dir.v_drop.insert(dir.v_drop.end(), drop_event.begin(), drop_event.end());
	}

	// Drop Mana Artefact
	auto mana_drop = sDropSystem::getInstance().drawManaArtefact(ci);

	if (mana_drop._typeid != 0) // Dropou
		dir.v_drop.push_back(mana_drop);

	// Drop Grand Prix Ticket, não drop no Grand Prix
	if (m_ri.qntd_hole == ci.seq_hole && m_ri.tipo != RoomInfo::TIPO::GRAND_PRIX) {
		auto gp_ticket = sDropSystem::getInstance().drawGrandPrixTicket(ci, _session);

		if (gp_ticket._typeid != 0) // Dropou
			dir.v_drop.push_back(gp_ticket);
	}

	// SSC Ticket
	auto ssc = sDropSystem::getInstance().drawSSCTicket(ci);

	if (!ssc.empty()) {
		dir.v_drop.insert(dir.v_drop.end(), ssc.begin(), ssc.end());

		// SSC Ticket Achievement
		pgi->sys_achieve.incrementCounter(0x6C400053u/*SSC Ticket*/, (int)ssc.size());
	}

	// Adiciona para a lista de drop's do player
	if (!dir.v_drop.empty())
		pgi->drop_list.v_drop.insert(pgi->drop_list.v_drop.end(), dir.v_drop.begin(), dir.v_drop.end());

	return dir;
}

void Game::requestSaveDrop(player& _session) {

	INIT_PLAYER_INFO("requestSaveDrop", "tentou salvar drop item no jogo", &_session);

	if (!pgi->drop_list.v_drop.empty()) {

		std::map< uint32_t, stItem > v_item;
		std::map< uint32_t, stItem >::iterator it;
		stItem item{ 0 };

		for (auto& el : pgi->drop_list.v_drop) {
			item.clear();

			item.type = 2;
			item._typeid = el._typeid;
			item.qntd = (el.type == el.QNTD_MULTIPLE_500) ? el.qntd * 500 : el.qntd;
			item.STDA_C_ITEM_QNTD = (short)item.qntd;

			if ((it = v_item.find(item._typeid)) == v_item.end())	// Novo item
				v_item.insert(std::make_pair(item._typeid, item));
			else {	// J� tem
				it->second.qntd += item.qntd;
				it->second.STDA_C_ITEM_QNTD = (short)it->second.qntd;
			}
		}

		auto rai = item_manager::addItem(v_item, _session, 0, 0);
		
		if (rai.fails.size() > 0 && rai.type != item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
			_smp::message_pool::getInstance().push(new message("[Game:requestSaveDrop][WARNIG] nao conseguiu adicionar os drop itens. Bug", CL_FILE_LOG_AND_CONSOLE));

		packet p((unsigned short)0x216);

		p.addUint32((const uint32_t)GetSystemTimeAsUnix());
		p.addUint32((uint32_t)v_item.size());

		for (auto& el : v_item) {
			p.addUint8(el.second.type);
			p.addUint32(el.second._typeid);
			p.addInt32(el.second.id);
			p.addUint32(el.second.flag_time);
			p.addBuffer(&el.second.stat, sizeof(el.second.stat));
			p.addUint32((el.second.STDA_C_ITEM_TIME > 0) ? el.second.STDA_C_ITEM_TIME : el.second.STDA_C_ITEM_QNTD);
			p.addZeroByte(25);
		}

		packet_func::session_send(p, &_session, 1);
	}
}

DropItemRet Game::requestInitCubeCoin(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitCubeCoin");

	try {

		unsigned char opt = _packet->readUint8();
		unsigned char count = _packet->readUint8();

		// Player que tacou e tem drops (Coin ou Cube)
		if (opt == 1 && count > 0) {

			DropItemRet dir;

			INIT_PLAYER_INFO("initCubeCoin", "tentou terninar o hole no jogo", &_session);

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[Game::requestInitCubeCoin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou terminar hole[NUMERO=" 
						+ std::to_string((unsigned short)pgi->hole)  + "], mas no course nao tem esse hole. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 250, 0));

			uint32_t tipo = 0u;
			uint32_t id = 0u;
			
			CubeEx *pCube = nullptr;

			for (auto i = 0u; i < count; ++i) {

				tipo = _packet->readUint8();
				id = _packet->readUint32();

				pCube = hole->findCubeCoin(id);

				if (pCube == nullptr)
					throw exception("[Game::requestInitCubeCoin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou terminar hole[NUMERO="
							+ std::to_string((unsigned short)pgi->hole) + "], mas o cliente forneceu um cube/coin id[ID=" + std::to_string(id) + "] invalido. Hacker ou Bug", 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 251, 0));

				switch (tipo) {
				case 0: // Coin
				{
					// Tipo 3 Coin da borda do green ganha menos pangs ganha de 1 a 50, Tipo 4 Coin no chão qualquer lugar ganha mais pang de 1 a 200
					dir.v_drop.push_back({ COIN_TYPEID, (unsigned char)hole->getCourse(),
											(unsigned char)hole->getNumero(),
											(short)(sRandomGen::getInstance().rIbeMt19937_64_chrono() % (pCube->flag_location == 0 ? 50 : 200) + 1),
											(pCube->flag_location == 0) ? DropItem::eTYPE::COIN_EDGE_GREEN : DropItem::eTYPE::COIN_GROUND });

					// Achievement, coin do chão sem ser da borda do green
					if (pCube->flag_location != 0)
						pgi->sys_achieve.incrementCounter(0x6C400037u/*Coin do chão, sem ser da borda do green*/);

					break;
				}
				case 1:	// Cube
				{
					dir.v_drop.push_back({ SPINNING_CUBE_TYPEID, (unsigned char)hole->getCourse(), (unsigned char)hole->getNumero(), 1, DropItem::eTYPE::CUBE/*Cube*/ });

					// Achievement, pegou cube
					pgi->sys_achieve.incrementCounter(0x6C400036u/*Cube*/);

					break;
				}	// End Case 1 "Cube"
				}	// End Switch
			}

			// Add os Cube Coin para o player list drop
			pgi->drop_list.v_drop.insert(pgi->drop_list.v_drop.begin(), dir.v_drop.begin(), dir.v_drop.end());

			return dir;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::requestInitCubeCoin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return DropItemRet();
}

void Game::requestCalculePang(player& _session) {

	INIT_PLAYER_INFO("requestCalculePang", "tentou calcular o pang do player no jogo", &_session);

	// Course Rate of Pang
	auto course = sIff::getInstance().findCourse((m_ri.course & 0x7F) | 0x28000000u);

	// Rate do course, tem uns que é 10% a+ tem outros que é 30% a mais que o pangya JP deixou
	float course_rate = (course != nullptr && course->rate_pang >= 1.f) ? course->rate_pang : 1.f;
	float pang_rate = 0.f;

	pang_rate = TRANSF_SERVER_RATE_VALUE(pgi->used_item.rate.pang) * TRANSF_SERVER_RATE_VALUE(m_rv.pang + (m_ri.modo == RoomInfo::MODO::M_SHUFFLE ? 10/*+10% Suffle mode*/ : 0)) * course_rate;

	pgi->data.bonus_pang = (uint64_t)(((pgi->data.pang * pang_rate) - pgi->data.pang) + (pgi->data.bonus_pang * pang_rate));
}

void Game::requestSaveInfo(player& _session, int option) {

	INIT_PLAYER_INFO("requestSaveInfo", "tentou salvar o info dele no jogo", &_session);

	try {

		// Aqui dados do jogo ele passa o holein no lugar do mad_conduta <-> holein, agora quando ele passa o info user é invertido(Normal)
		// Inverte para salvar direito no banco de dados
		auto tmp_holein = pgi->ui.hole_in;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Game::requestSaveInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] UserInfo[" + pgi->ui.toString() + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		
		pgi->ui.hole_in = pgi->ui.mad_conduta;
		pgi->ui.mad_conduta = tmp_holein;

		if (option == 0) { // Terminou VS

			// Verifica se o Angel Event está ativo de tira 1 quit do player que concluí o jogo
			if (m_ri.angel_event) {
				
				pgi->ui.quitado = -1;

				_smp::message_pool::getInstance().push(new message("[Game::requestSaveInfo][Log][AngelEvent] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] vai reduzir o quit em " + std::to_string(pgi->ui.quitado * -1) + " unidade(s).", CL_FILE_LOG_AND_CONSOLE));
			}

			pgi->ui.exp = 0;
			pgi->ui.combo = 1;
			pgi->ui.jogado = 1;
			pgi->ui.media_score = pgi->data.score;

			// Os valores que eu não colocava
			pgi->ui.jogados_disconnect = 1;		// Esse aqui é o contador de jogos que o player começou é o mesmo do jogado, só que esse aqui usa para o disconnect

			auto diff = getLocalTimeDiff(m_start_time);

			if (diff > 0)
				diff /= STDA_10_MICRO_PER_SEC; // NanoSeconds To Seconds

			pgi->ui.tempo = (uint32_t)diff;

		}else if (option == 1) { // Quitou ou tomou DC
			
			// Quitou ou saiu não ganha pangs
			pgi->data.pang = 0u;
			pgi->data.bonus_pang = 0u;

			pgi->ui.exp = 0;
			pgi->ui.combo = DECREASE_COMBO_VALUE * -1;
			pgi->ui.jogado = 1;
			
			// Verifica se tomou DC ou Quitou, ai soma o membro certo
			if (!_session.m_connection_timeout)
				pgi->ui.quitado = 1;
			else
				pgi->ui.disconnect = 1;

			// Os valores que eu não colocava
			pgi->ui.jogados_disconnect = 1;		// Esse aqui é o contador de jogos que o player começou é o mesmo do jogado, só que esse aqui usa para o disconnect

			pgi->ui.media_score = pgi->data.score;

			auto diff = getLocalTimeDiff(m_start_time);

			if (diff > 0)
				diff /= STDA_10_MICRO_PER_SEC; // NanoSeconds To Seconds

			pgi->ui.tempo = (uint32_t)diff;

		}else if (option == 2) { // Não terminou o hole 1, alguem saiu ai volta para sala sem contar o combo, só conta o jogo que começou

			pgi->data.pang = 0u;
			pgi->data.bonus_pang = 0u;

			pgi->ui.exp = 0;
			pgi->ui.jogado = 1;

			// Os valores que eu não colocava
			pgi->ui.jogados_disconnect = 1;		// Esse aqui é o contador de jogos que o player começou é o mesmo do jogado, só que esse aqui usa para o disconnect

			auto diff = getLocalTimeDiff(m_start_time);

			if (diff > 0)
				diff /= STDA_10_MICRO_PER_SEC; // NanoSeconds To Seconds

			pgi->ui.tempo = (uint32_t)diff;

		}else if (option == 4) { // SSC

			pgi->ui.clear();

			// Verifica se o Angel Event está ativo de tira 1 quit do player que concluí o jogo
			if (m_ri.angel_event) {

				pgi->ui.quitado = -1;

				_smp::message_pool::getInstance().push(new message("[Game::requestSaveInfo][Log][AngelEvent] Player[UID=" + std::to_string(_session.m_pi.uid)
						+ "] vai reduzir o quit em " + std::to_string(pgi->ui.quitado * -1) + " unidade(s).", CL_FILE_LOG_AND_CONSOLE));
			}

			pgi->ui.exp = 0;
			pgi->ui.combo = 1;
			pgi->ui.jogado = 1;
			pgi->ui.media_score = 0;

			// Os valores que eu não colocava
			pgi->ui.jogados_disconnect = 1;		// Esse aqui é o contador de jogos que o player começou é o mesmo do jogado, só que esse aqui usa para o disconnect

			auto diff = getLocalTimeDiff(m_start_time);

			if (diff > 0)
				diff /= STDA_10_MICRO_PER_SEC;

			pgi->ui.tempo = (uint32_t)diff;

		}else if (option == 5/*Não conta quit*/) {

			// Quitou ou saiu não ganha pangs
			pgi->data.pang = 0u;
			pgi->data.bonus_pang = 0u;

			pgi->ui.exp = 0;
			pgi->ui.jogado = 1;
			pgi->ui.media_score = pgi->data.score;

			// Os valores que eu não colocava
			pgi->ui.jogados_disconnect = 1;		// Esse aqui é o contador de jogos que o player começou é o mesmo do jogado, só que esse aqui usa para o disconnect

			auto diff = getLocalTimeDiff(m_start_time);

			if (diff > 0)
				diff /= STDA_10_MICRO_PER_SEC; // NanoSeconds To Seconds

			pgi->ui.tempo = (uint32_t)diff;
		}

		// Achievement Records
		records_player_achievement(_session);

		// Pode tirar pangs
		int64_t total_pang = pgi->data.pang + pgi->data.bonus_pang;

		// UPDATE ON SERVER AND DB
		_session.m_pi.addUserInfo(pgi->ui, total_pang);	// add User Info

		if (total_pang > 0)
			_session.m_pi.addPang(total_pang);				// add Pang
		else if (total_pang < 0)
			_session.m_pi.consomePang(total_pang * -1);		// consome Pangs

		// Game Combo
		if (_session.m_pi.ui.combo > 0)
			pgi->sys_achieve.incrementCounter(0x6C40004Bu/*Game Combo*/, _session.m_pi.ui.combo);
	
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[Game::requestSaveInfo][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Game::requestUpdateItemUsedGame(player& _session) {

	INIT_PLAYER_INFO("requestUpdateItemUsedGame", "tentou atualizar itens usado no jogo", &_session);

	auto& ui = pgi->used_item;

	// Club Mastery // ((m_ri.course & 0x7f) == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE ? 1.5f : 1.f), SSSC sobrecarrega essa função para colocar os valores dele
	ui.club.count += (uint32_t)(1.f * 10.f * ui.club.rate * TRANSF_SERVER_RATE_VALUE(m_rv.clubset) * TRANSF_SERVER_RATE_VALUE(ui.rate.club));

	// Passive Item exceto Time Booster e Auto Command, que soma o contador por uso, o cliente passa o pacote, dizendo que usou o item
	for (auto& el : ui.v_passive) {
		
		// Verica se é o ultimo hole, terminou o jogo, ai tira soma 1 ao count do pirulito que consome por jogo
		if (CHECK_PASSIVE_ITEM(el.second._typeid) && el.second._typeid != TIME_BOOSTER_TYPEID/*Time Booster*/ && el.second._typeid != AUTO_COMMAND_TYPEID) {
			
			// Item de Exp Boost que só consome 1 Por Jogo, só soma no requestFinishItemUsedGame
			if (std::find(passive_item_exp_1perGame, LAST_ELEMENT_IN_ARRAY(passive_item_exp_1perGame), el.second._typeid) == LAST_ELEMENT_IN_ARRAY(passive_item_exp_1perGame))
				el.second.count++;
		
		}else if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::BALL	/*Ball*/
				|| sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::AUX_PART) /*AuxPart(Anel)*/
			el.second.count++;
	}
}

void Game::requestFinishItemUsedGame(player& _session) {

	std::vector< stItemEx > v_item;
	stItemEx item{ 0 };

	INIT_PLAYER_INFO("requestFinishItemUsedGame", "tentou finalizar itens usado no jogo", &_session);

	// Player já finializou os itens usados, verifica para não finalizar dua vezes os itens do player
	if (pgi->finish_item_used) {

		_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) + "] ja finalizou os itens. Bug", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	auto& ui = pgi->used_item;

	uint32_t tmp_counter_typeid = 0u;

	// Add +1 ao itens que consome 1 só por jogo
	// Item de Exp Boost que só consome 1 Por Jogo
	std::for_each(ui.v_passive.begin(), ui.v_passive.end(), [&](auto& _el) {

		if (std::find(passive_item_exp_1perGame, LAST_ELEMENT_IN_ARRAY(passive_item_exp_1perGame), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_exp_1perGame))
			_el.second.count++;
	});

	// Verifica se é premium 2 e se ele tem o auto caliper para poder somar no Achievement
	if (_session.m_pi.m_cap.stBit.premium_user && sPremiumSystem::getInstance().isPremium2(_session.m_pi.pt._typeid)) {

		auto it_ac = ui.v_passive.find(AUTO_CALIPER_TYPEID);

		if (it_ac == ui.v_passive.end()) {

			uint32_t qntd = m_course->findHoleSeq(pgi->hole);

			if (qntd == (unsigned short)~0u)
				qntd = m_ri.qntd_hole;

			// Adiciona Auto Caliper para ser contado no Achievement
			auto it_p_ac = ui.v_passive.insert({ 
				AUTO_CALIPER_TYPEID, 
				{ AUTO_CALIPER_TYPEID , qntd }
			});

			if (!it_p_ac.second && it_p_ac.first == ui.v_passive.end())
				// Log
				_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][Error][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] nao conseguiu adicionar o Auto Caliper passive item para adicionar no contador do Achievement, por que ele eh premium user 2", CL_FILE_LOG_AND_CONSOLE));
		}
	}

	// Passive Item
	for (auto& el : ui.v_passive) {
		
		if (el.second.count > 0u) {
			
			// Item Aqui tem o Achievemente de passive item
			if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::ITEM && !sIff::getInstance().IsItemEquipable(el.second._typeid)/*Nega == Passive Item*/) {

				pgi->sys_achieve.incrementCounter(0x6C400075u/*Passive Item*/, el.second.count);

				if ((tmp_counter_typeid = SysAchievement::getPassiveItemCounterTypeId(el.second._typeid)) > 0)
					pgi->sys_achieve.incrementCounter(tmp_counter_typeid, el.second.count);
			}

			// Só atualiza o Auto Caliper se não for Premium 2
			if (!_session.m_pi.m_cap.stBit.premium_user || !sPremiumSystem::getInstance().isPremium2(_session.m_pi.pt._typeid) || el.second._typeid != AUTO_CALIPER_TYPEID/*Auto Caliper*/) {

				// Tira todos itens passivo, antes estava Item e AuxPart, não ia Ball por que eu fiz errado, só preciso verifica se é item e passivo para somar o achievement
				// Para tirar os itens, tem que tirar(atualizar) todos.
				auto pWi = _session.m_pi.findWarehouseItemByTypeid(el.second._typeid);

				if (pWi != nullptr) {

						// Init Item
						item.clear();

						item.type = 2;
						item._typeid = el.second._typeid;
						item.id = pWi->id;
						item.qntd = el.second.count;
						item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

						// Add On Vector
						v_item.push_back(item);

				}else
					_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "] tentou atualizar item[TYPEID=" + std::to_string(el.second._typeid) + "] que ele nao possui. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
			}
		}
	}

	// Active Item
	for (auto& el : ui.v_active) {

		if (el.second.count > 0u) {

			// Aqui tem achievement de Item Active
			if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::ITEM && sIff::getInstance().IsItemEquipable(el.second._typeid)) {
				
				pgi->sys_achieve.incrementCounter(0x6C40004Fu/*Active Item*/, el.second.count);

				if ((tmp_counter_typeid = SysAchievement::getActiveItemCounterTypeId(el.second._typeid)) > 0)
					pgi->sys_achieve.incrementCounter(tmp_counter_typeid, el.second.count);
			}

			// Só tira os itens Active se a sala não estiver com o artefact Frozen Flame,
			// se ele estiver com artefact Frozen Flame ele mantém os Itens Active, não consome e nem desequipa do inventório do player
			if (m_ri.artefato != ART_FROZEN_FLAME) {

				// Limpa o Item Slot do player, dos itens que foram usados(Ativados) no jogo
				if (el.second.count <= el.second.v_slot.size()) {
				
					for (auto i = 0u; i < el.second.count; ++i)
						_session.m_pi.ue.item_slot[el.second.v_slot[i]] = 0;
				}

				auto pWi = _session.m_pi.findWarehouseItemByTypeid(el.second._typeid);

				if (pWi != nullptr) {
					// Init Item
					item.clear();

					item.type = 2;
					item._typeid = el.second._typeid;
					item.id = pWi->id;
					item.qntd = el.second.count;
					item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

					// Add On Vector
					v_item.push_back(item);

				}else
					_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "] tentou atualizar item[TYPEID=" + std::to_string(el.second._typeid) + "] que ele nao possui. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
			}
		}
	}

	// Update Item Equiped Slot ON DB
	snmdb::NormalManagerDB::getInstance().add(25, new CmdUpdateItemSlot(_session.m_pi.uid, (uint32_t*)_session.m_pi.ue.item_slot), Game::SQLDBResponse, this);

	// Se for o Master da sala e ele estiver com artefato tira o mana dele
	// Antes tirava assim que começava o jogo, mas aí o cliente atualizava a sala tirando o artefact aí no final não tinha como ver se o frozen flame estava equipado
	// e as outras pessoas que estão na lobby não sabe qual artefect que está na sala, por que o master mesmo mando o pacote pra tirar da sala quando o server tira o mana dele no init game
	if (m_ri.artefato != 0 && m_ri.master == _session.m_pi.uid) {

		// Tira Artefact Mana do master da sala
		auto pWi = _session.m_pi.findWarehouseItemByTypeid(m_ri.artefato + 1);

		if (pWi != nullptr) {

			item.clear();

			item.type = 2;
			item.id = pWi->id;
			item._typeid = pWi->_typeid;
			item.qntd = (pWi->STDA_C_ITEM_QNTD <= 0) ? 1 : pWi->STDA_C_ITEM_QNTD;
			item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

			// Add on Vector Update Itens
			v_item.push_back(item);

		}else
			_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][WARNING] Master[UID=" + std::to_string(_session.m_pi.uid) + "] do jogo nao tem Mana do Artefect[TYPEID="
					+ std::to_string(m_ri.artefato) + ", MANA=" + std::to_string(m_ri.artefato + 1) + "] e criou e comecou um jogo com artefact sem mana. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
	}

	// Update Item ON Server AND DB
	if (!v_item.empty()) {
		
		if (item_manager::removeItem(v_item, _session) <= 0)
			_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] nao conseguiu deletar os item do player. Bug", CL_FILE_LOG_AND_CONSOLE));
	}

	// Club Mastery
	if (ui.club.count > 0u && ui.club._typeid > 0u) {

		auto pClub = _session.m_pi.findWarehouseItemByTypeid(ui.club._typeid);

		if (pClub != nullptr) {

			pClub->clubset_workshop.mastery += ui.club.count;

			item.clear();

			item.type = 0xCC;
			item.id = pClub->id;
			item._typeid = pClub->_typeid;
#if defined(_WIN32)
			memcpy_s(item.clubset_workshop.c, sizeof(item.clubset_workshop.c), pClub->clubset_workshop.c, sizeof(item.clubset_workshop.c));
#elif defined(__linux__)
			memcpy(item.clubset_workshop.c, pClub->clubset_workshop.c, sizeof(item.clubset_workshop.c));
#endif
			item.clubset_workshop.level = (char)pClub->clubset_workshop.level;
			item.clubset_workshop.mastery = pClub->clubset_workshop.mastery;
			item.clubset_workshop.rank = pClub->clubset_workshop.rank;
			item.clubset_workshop.recovery = pClub->clubset_workshop.recovery_pts;

			snmdb::NormalManagerDB::getInstance().add(12, new CmdUpdateClubSetWorkshop(_session.m_pi.uid, *pClub, CmdUpdateClubSetWorkshop::F_TRANSFER_MASTERY_PTS/*Usa o transfere que é o mesmo que o add*/), Game::SQLDBResponse, this);

			// Add Begin Vector
			v_item.insert(v_item.begin(), item);

		}else
			_smp::message_pool::getInstance().push(new message("[Game::requestFinishItemUsedGame][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou salvar mastery do ClubSet[TYPEID=" 
					+ std::to_string(ui.club._typeid) + "] que ele nao tem. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
	}

	// Flag de que o palyer já finalizou os itens usados no jogo, para não finalizar duas vezes
	pgi->finish_item_used = 1u;

	// Atualiza ON Jogo
	if (!v_item.empty()) {
		packet p((unsigned short)0x216);

		p.addUint32((const uint32_t)GetSystemTimeAsUnix());
		p.addUint32((uint32_t)v_item.size());

		for (auto& el : v_item) {
			p.addUint8(el.type);
			p.addUint32(el._typeid);
			p.addInt32(el.id);
			p.addUint32(el.flag_time);
			p.addBuffer(&el.stat, sizeof(el.stat));
			p.addUint32((el.STDA_C_ITEM_TIME > 0) ? el.STDA_C_ITEM_TIME : el.STDA_C_ITEM_QNTD);
			p.addZeroByte(25);	// 10 PCL[C0~C4] 2 Bytes cada, 15 bytes desconhecido
			if (el.type == 0xCC)
				p.addBuffer(&el.clubset_workshop, sizeof(el.clubset_workshop));
		}

		packet_func::session_send(p, &_session, 1);
	}
}

void Game::requestFinishHole(player& _session, int option) {

	INIT_PLAYER_INFO("requestFinishHole", "tentou finalizar o dados do hole do player no jogo", &_session);

	auto hole = m_course->findHole(pgi->hole);

	if (hole == nullptr)
		throw exception("[Game::finishHole][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar hole[NUMERO="
				+ std::to_string((unsigned short)pgi->hole) + "] no jogo, mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 20, 0));

	char score_hole = 0;
	uint32_t tacada_hole = 0u;

	// Finish Hole Dados
	if (option == 0) {

		pgi->data.total_tacada_num += pgi->data.tacada_num;

		// Score do hole
		score_hole = (char)(pgi->data.tacada_num - hole->getPar().par);

		// Tacadas do hole
		tacada_hole = pgi->data.tacada_num;

		pgi->data.score += score_hole;

		// Achievement Score
		auto tmp_counter_typeid = SysAchievement::getScoreCounterTypeId(tacada_hole, hole->getPar().par);

		if (tmp_counter_typeid > 0)
			pgi->sys_achieve.incrementCounter(tmp_counter_typeid);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Game::requestFinishHole][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] terminou o hole[COURSE=" 
				+ std::to_string(hole->getCourse()) + ", NUMERO=" + std::to_string(hole->getNumero()) + ", PAR=" 
				+ std::to_string(hole->getPar().par) + ", SHOT=" + std::to_string(tacada_hole) + ", SCORE=" + std::to_string(score_hole) + ", TOTAL_SHOT=" 
				+ std::to_string(pgi->data.total_tacada_num) + ", TOTAL_SCORE=" + std::to_string(pgi->data.score) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG


		// Zera dados
		pgi->data.time_out = 0u;

		// Giveup Flag
		pgi->data.giveup = 0u;

		// Zera as penalidades do hole
		pgi->data.penalidade = 0u;

	}else if (option == 1) {	// Não acabou o hole então faz os calculos para o jogo todo

		auto pair = m_course->findRange(pgi->hole);

		for (auto it = pair.first; it != pair.second && it->first <= m_ri.qntd_hole/*9h ou 18h ele verifica*/; ++it) {
			pgi->data.total_tacada_num += it->second.getPar().total_shot;

			pgi->data.score += it->second.getPar().range_score[1];	// Max Score
		}

		// Zera dados
		pgi->data.time_out = 0u;

		pgi->data.tacada_num = 0u;

		// Giveup Flag
		pgi->data.giveup = 0u;

		// Zera as penalidades do hole do player
		pgi->data.penalidade = 0u;
	}

	// Aqui tem que atualiza o PGI direitinho com outros dados
	pgi->progress.hole = (short)m_course->findHoleSeq(pgi->hole);

	// Dados Game Progress do Player
	if (option == 0) {
		
		if (pgi->progress.hole > 0) {

			if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole)
				pgi->progress.finish_hole[pgi->progress.hole - 1] = 1;	// Terminou o hole

			pgi->progress.par_hole[pgi->progress.hole - 1] = hole->getPar().par;
			pgi->progress.score[pgi->progress.hole - 1] = score_hole;
			pgi->progress.tacada[pgi->progress.hole - 1] = tacada_hole;
		}

	}else {

		auto pair = m_course->findRange(pgi->hole);

		for (auto it = pair.first; it != pair.second && it->first <= m_ri.qntd_hole/*9h ou 18h ele verifica*/; ++it) {
			
			pgi->progress.finish_hole[it->first - 1] = 0;	// não terminou

			pgi->progress.par_hole[it->first - 1] = it->second.getPar().par;

			pgi->progress.score[it->first - 1] = it->second.getPar().range_score[1];	// Max Score

			pgi->progress.tacada[it->first - 1] = it->second.getPar().total_shot;
		}
	}
}

void Game::requestSaveRecordCourse(player& _session, int game, int option) {

	INIT_PLAYER_INFO("requestSaveRecordCourse", "tentou salvar record do course do player no jogo", &_session);

	if (_session.m_pi.ei.char_info == nullptr) {	// Player não está com character equipado, kika dele do jogo
		_smp::message_pool::getInstance().push(new message("[Game::requestSaveRecordCourse][Log] player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.", CL_FILE_LOG_AND_CONSOLE));
		return;// Kika aqui "deletePlayer(s);"
	}

	MapStatistics *pMs = nullptr;

	if (pgi->assist_flag) {	// Assist

		if (game == 52/*Grand Prix*/) {
			pMs = &_session.m_pi.a_msa_grand_prix[(m_ri.course & 0x7F)];
		}else if (m_ri.natural.stBit.natural/* & 1*/) { // Natural
			pMs = &_session.m_pi.a_msa_natural[(m_ri.course & 0x7F)];

			game = 51;	// Natural
		}else {	// Normal
			pMs = &_session.m_pi.a_msa_normal[(m_ri.course & 0x7F)];
		}

	}else {	// Sem Assist

		if (game == 52/*Grand Prix*/) {
			pMs = &_session.m_pi.a_ms_grand_prix[(m_ri.course & 0x7F)];
		}else if (m_ri.natural.stBit.natural/* & 1*/) { // Natural
			pMs = &_session.m_pi.a_ms_natural[(m_ri.course & 0x7F)];

			game = 51;	// Natural
		}else {	// Normal
			pMs = &_session.m_pi.a_ms_normal[(m_ri.course & 0x7F)];
		}
	}

	bool make_record = false;
	
	// UPDATE ON SERVER
	if (option == 1) {	// 18h pode contar record
		
		// Fez Record
		if (pMs->best_score == 127 || pgi->data.score < (int)pMs->best_score || pgi->data.pang > (uint64_t)pMs->best_pang) {
			
			// Update Best Score Record
			if (pgi->data.score < pMs->best_score)
				pMs->best_score = (char)pgi->data.score;

			// Update Best Pang Record
			if (pgi->data.pang > (uint64_t)pMs->best_pang)
				pMs->best_pang = pgi->data.pang;

			// Update Character Record
			pMs->character_typeid = _session.m_pi.ei.char_info->_typeid;

			make_record = true;
		}
	}

	// Salva os dados normais
	pMs->tacada += pgi->ui.tacada;
	pMs->putt += pgi->ui.putt;
	pMs->hole += pgi->ui.hole;
	pMs->fairway += pgi->ui.fairway;
	pMs->hole_in += pgi->ui.hole_in;
	pMs->putt_in += pgi->ui.putt_in;
	pMs->total_score += pgi->data.score;
	pMs->event_score = 0u;

	MapStatisticsEx ms{ *pMs };

	//memcpy_s(&ms, sizeof(MapStatistics), pMs, sizeof(MapStatistics));

	ms.tipo = game;
	//ms.course = pMs->course;

	// UPDATE ON DB
	snmdb::NormalManagerDB::getInstance().add(5, new CmdUpdateMapStatistics(_session.m_pi.uid, ms, pgi->assist_flag), Game::SQLDBResponse, this);

	// UPDATE ON GAME, se ele fez record, e add 1000 para ele
	if (make_record) {

		// Log
		_smp::message_pool::getInstance().push(new message("[Game::requestSaveRecordCourse][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] fez record no Map[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) +" (" + std::to_string((unsigned short)pMs->course) + "), SCORE=" + std::to_string((short)pMs->best_score) + ", PANG=" 
				+ std::to_string(pMs->best_pang) + ", CHARACTER=" + std::to_string(pMs->character_typeid) + "]", CL_FILE_LOG_AND_CONSOLE));

		// Add 1000 pang por ele ter quebrado o  record dele
		_session.m_pi.addPang(1000);

		// Resposta para make record
		packet p((unsigned short)0xB9);

		p.addInt8(m_ri.course & 0x7F);

		packet_func::session_send(p, &_session, 1);
	}
}

void Game::requestInitItemUsedGame(player& _session, PlayerGameInfo& _pgi) {

	//INIT_PLAYER_INFO("requestInitItemUsedGame", "tentou inicializar itens usado no jogo", &_session);

	// Characters Equip
	if (_session.getState() && 
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.isConnected()) {	// Check Player Connected

		if (_session.m_pi.ei.char_info == nullptr) {	// Player não está com character equipado, kika dele do jogo
			_smp::message_pool::getInstance().push(new message("[Game::requestInitItemUsedGame][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.",
					CL_FILE_LOG_AND_CONSOLE));
			return;// Kika aqui "deletePlayer(s);"
		}

		if (_session.m_pi.ei.comet == nullptr) {	// Player não está com Comet(Ball) equipado, kika dele do jogo
			_smp::message_pool::getInstance().push(new message("[Game::requestInitItemUsedGame][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao esta com Ball equipado. kika ele do jogo. pode ser Bug.",
					CL_FILE_LOG_AND_CONSOLE));
			return;// Kika aqui "deletePlayer(s);"
		}

		auto& ui = _pgi.used_item;

		// Zera os Itens usados
		ui.clear();

		/// ********** Itens Usado **********

		// Passive Item Equipado
		std::for_each(_session.m_pi.mp_wi.begin(), _session.m_pi.mp_wi.end(), [&](auto& _el) {
			if (std::find(passive_item, LAST_ELEMENT_IN_ARRAY(passive_item), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item))
				ui.v_passive.insert(std::make_pair((uint32_t)_el.second._typeid, UsedItem::Passive{ (uint32_t)_el.second._typeid, 0u }));
		});

		// Ball Equiped
		if (_session.m_pi.ei.comet->_typeid != DEFAULT_COMET_TYPEID && (!_session.m_pi.m_cap.stBit.premium_user || _session.m_pi.ei.comet->_typeid != sPremiumSystem::getInstance().getPremiumBallByTicket(_session.m_pi.pt._typeid)))
			ui.v_passive.insert(std::make_pair((uint32_t)_session.m_pi.ei.comet->_typeid, UsedItem::Passive{ (uint32_t)_session.m_pi.ei.comet->_typeid, 0u }));

		// AuxParts
		for (auto i = 0u; i < (sizeof(_session.m_pi.ei.char_info->auxparts) / sizeof(_session.m_pi.ei.char_info->auxparts[0])); ++i)
			if (_session.m_pi.ei.char_info->auxparts[i] >= 0x70000000 && _session.m_pi.ei.char_info->auxparts[i] < 0x70010000)
				ui.v_passive.insert(std::make_pair((uint32_t)_session.m_pi.ei.char_info->auxparts[i], UsedItem::Passive{ (uint32_t)_session.m_pi.ei.char_info->auxparts[i], 0u }));

		// Item Active Slot
		auto it = ui.v_active.end();

		for (auto i = 0u; i < (sizeof(_session.m_pi.ue.item_slot) / sizeof(_session.m_pi.ue.item_slot[0])); ++i) {
			
			// Diferente de 0 item está equipado
			if (_session.m_pi.ue.item_slot[i] != 0) {
				if ((it = ui.v_active.find(_session.m_pi.ue.item_slot[i])) == ui.v_active.end())	// Não tem add o novo
					ui.v_active.insert(std::make_pair((uint32_t)_session.m_pi.ue.item_slot[i], UsedItem::Active{ (uint32_t)_session.m_pi.ue.item_slot[i], 0u, std::vector< unsigned char >{(unsigned char)i} }));
				else	// Já tem add só o slot
					it->second.v_slot.push_back((unsigned char)i);	// Slot
			}
		}

		// ClubSet For ClubMastery
		ui.club._typeid = _session.m_pi.ei.csi._typeid;
		ui.club.count = 0u;
		ui.club.rate = 1.f;

		auto club = sIff::getInstance().findClubSet(ui.club._typeid);

		if (club != nullptr)
			ui.club.rate = club->work_shop.rate;
		else
			_smp::message_pool::getInstance().push(new message("[Game::requestIniItemUsedGame][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com um ClubSet[TYPEID=" 
					+ std::to_string(_session.m_pi.ei.csi._typeid) + ", ID=" + std::to_string(_session.m_pi.ei.csi.id) + "] que nao tem no IFF_STRUCT do Server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

		/// ********** Itens Usado **********

		/// ********** Itens Exp/Pang Rate **********

		// Item Buff
		auto time_limit_item = sIff::getInstance().getTimeLimitItem();

		std::for_each(_session.m_pi.v_ib.begin(), _session.m_pi.v_ib.end(), [&](auto& _el) {
			
			auto it = time_limit_item.end();

			if ((it = std::find_if(time_limit_item.begin(), time_limit_item.end(), [&](auto& _el2) {
				return _el2.second._typeid == _el._typeid;
			})) != time_limit_item.end()) {
				
				switch (it->second.type) {
				case ItemBuff::eTYPE::YAM_AND_GOLD:
					ui.rate.exp += it->second.percent;
					break;
				case ItemBuff::eTYPE::RAINBOW:
				case ItemBuff::eTYPE::RED:
					ui.rate.exp += (it->second.percent > 0) ? it->second.percent : 100;
					ui.rate.pang += (it->second.percent > 0) ? it->second.percent : 100;
					break;
				case ItemBuff::eTYPE::GREEN:
					ui.rate.exp += (it->second.percent > 0) ? it->second.percent : 100;
					break;
				case ItemBuff::eTYPE::YELLOW:
					ui.rate.pang += (it->second.percent > 0) ? it->second.percent : 100;
					break;
				}
			}
		});

		// Card Equipado, Special, NPC, e Caddie
		std::for_each(_session.m_pi.v_cei.begin(), _session.m_pi.v_cei.end(), [&](auto& _el) {
			
			if (_el.parts_id == _session.m_pi.ei.char_info->id && _el.parts_typeid == _session.m_pi.ei.char_info->_typeid
				&& sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 5/*NPC*/) {
				
				if (_el.efeito == 2/*Exp*/)
					ui.rate.exp += _el.efeito_qntd;
				else if (_el.efeito == 1/*Pang*/)
					ui.rate.pang += _el.efeito_qntd;

			}else if (_el.parts_id == 0 && _el.parts_typeid == 0 && sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 2/*Special*/) {
				
				if (_el.efeito == 3/*Exp*/)
					ui.rate.exp += _el.efeito_qntd;
				else if (_el.efeito == 2/*Pang*/)
					ui.rate.pang += _el.efeito_qntd;
				else if (_el.efeito == 34/*Club Mastery*/)
					ui.rate.club += _el.efeito_qntd;
			}
		});

		// Item Passive Boost Exp, Pang and Club Mastery

		// Pang
		std::for_each(ui.v_passive.begin(), ui.v_passive.end(), [&](auto& _el) {
			
			// Pang Boost X2
			if (std::find(passive_item_pang_x2, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x2), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x2)) {

				ui.rate.pang += 200;	// 200%

				// Flag Boost Item
				_pgi.boost_item_flag.flag.pang = 1;
			}

			// Pang Boost X4
			if (std::find(passive_item_pang_x4, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x4), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x4)) {

				ui.rate.pang += 400;	// 400%

				// Flag Boost Item
				_pgi.boost_item_flag.flag.pang_nitro = 1;
			}

			// Pang Boost X1.5
			if (std::find(passive_item_pang_x1_5, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x1_5), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x1_5)) {

				ui.rate.pang += 50;	// 150%

				// Flag Boost Item
				_pgi.boost_item_flag.flag.pang = 1;
			}

			// Pang Boost X1.4
			if (std::find(passive_item_pang_x1_4, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x1_4), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x1_4)) {

				ui.rate.pang += 40;	// 140%

				// Flag Boost Item
				_pgi.boost_item_flag.flag.pang = 1;
			}

			// Pang Boost X1.2
			if (std::find(passive_item_pang_x1_2, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x1_2), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x1_2)) {

				ui.rate.pang += 20;	// 120%

				// Flag Boost Item
				_pgi.boost_item_flag.flag.pang = 1;
			}
		});

		// Exp
		std::for_each(ui.v_passive.begin(), ui.v_passive.end(), [&](auto& _el) {
			if (std::find(passive_item_exp, LAST_ELEMENT_IN_ARRAY(passive_item_exp), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_exp))
				ui.rate.exp += 200;	// 200%
		});

		// Club Mastery Boost
		std::for_each(ui.v_passive.begin(), ui.v_passive.end(), [&](auto& _el) {
			if (std::find(passive_item_club_boost, LAST_ELEMENT_IN_ARRAY(passive_item_club_boost), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_club_boost))
				ui.rate.club += 200;	// Por Hora só tem 1 item
		});

		// Character Parts Equipado
		if (FIND_ELEMENT_ARRAY_OF_ARRAY(_session.m_pi.ei.char_info->parts_typeid, hat_birthday)) {
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[Game::requestInitItemUsedGame][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Hat Birthday no Character[TYPEID="
					+ std::to_string(_session.m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(_session.m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
			
			ui.rate.exp += 20;	// 20% Hat Birthday
		}

		// Hat Lua e sol que na epoca do evento dava +20% Exp e Pang, voud colocar para ele dá direto aqui
		if (FIND_ELEMENT_ARRAY_OF_ARRAY(_session.m_pi.ei.char_info->parts_typeid, hat_lua_sol)) {
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[Game::requestInitItemUsedGame][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Hat Lua e Sol no Character[TYPEID="
					+ std::to_string(_session.m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(_session.m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

			ui.rate.exp += 20;	// 20% Hat Lua e Sol
			ui.rate.pang += 20;	// 20% Hat Lua e Sol
		}

		// Verifica se está com o anel que da +1.1% de Club Mastery
		if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), KURAFAITO_RING_CLUBMASTERY) != LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts)) {
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[Game::requestInitItemUsedGame][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Anel (Kurafaito) que da Club Mastery +1.1% no Character[TYPEID="
					+ std::to_string(_session.m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(_session.m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

			ui.rate.club += 10;	// Kurafaito Ring da + 10% no Club Mastery
		}

		// Character AuxParts Equipado
		// Aux parts tem seus próprios valores de rate no iff
		std::for_each(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), [&](auto& _el) {

			if (_el != 0 && sIff::getInstance().getItemGroupIdentify(_el) == iff::AUX_PART) {
				
				auto auxpart = sIff::getInstance().findAuxPart(_el);

				if (auxpart != nullptr) {

					// Pang
					if (auxpart->efeito.pang_rate > 100)
						ui.rate.pang += (auxpart->efeito.pang_rate - 100);
					else if (auxpart->efeito.pang_rate > 0)
						ui.rate.pang += auxpart->efeito.pang_rate;

					// Exp
					if (auxpart->efeito.exp_rate > 100)
						ui.rate.exp += (auxpart->efeito.exp_rate - 100);
					else if (auxpart->efeito.exp_rate > 0)
						ui.rate.exp += auxpart->efeito.exp_rate;

					// Drop item, aqui ele add os 120% e no Drop System ele trata isso direito
					// Todos itens que dá drop rate da treasure hunter point
					if (auxpart->efeito.drop_rate > 100) {
						
						if (auxpart->efeito.drop_rate > 100)
							ui.rate.drop += (auxpart->efeito.drop_rate - 100);
						else if (auxpart->efeito.drop_rate > 0)
							ui.rate.drop += auxpart->efeito.drop_rate;

						// Passaro gordo que usa isso aqui, mas pode adicionar mais mascot que dé drop rate e treasure hunter point
						_pgi.thi.all_score += 15;	// Add +15 ao all score
					}
				}
			}

		});

		// Mascot Equipado Rate Exp And Pang, Drop item e Treasure Hunter rate
		if (_session.m_pi.ei.mascot_info != nullptr) {
			
			auto mascot = sIff::getInstance().findMascot(_session.m_pi.ei.mascot_info->_typeid);

			if (mascot != nullptr) {

				// Pang
				if (mascot->efeito.pang_rate > 100)
					ui.rate.pang += (mascot->efeito.pang_rate - 100);
				else if (mascot->efeito.pang_rate > 0)
					ui.rate.pang += mascot->efeito.pang_rate;

				// Exp
				if (mascot->efeito.exp_rate > 100)
					ui.rate.exp += (mascot->efeito.exp_rate - 100);
				else if (mascot->efeito.exp_rate > 0)
					ui.rate.exp += mascot->efeito.exp_rate;

				// Drop item, aqui ele add os 120% e no Drop System ele trata isso direito
				// Todos itens que dá drop rate da treasure hunter point
				if (mascot->efeito.drop_rate > 100) {

					if (mascot->efeito.drop_rate > 100)
						ui.rate.drop += (mascot->efeito.drop_rate - 100);
					else if (mascot->efeito.drop_rate > 0)
						ui.rate.drop += mascot->efeito.drop_rate;

					// Passaro gordo que usa isso aqui, mas pode adicionar mais mascot que dé drop rate e treasure hunter point
					_pgi.thi.all_score += 15;	// Add +15 ao all score
				}

			}else
				_smp::message_pool::getInstance().push(new message("[Game::requestInitItemUsedGame][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com um mascot[TYPEID=" 
						+ std::to_string(_session.m_pi.ei.mascot_info->_typeid) + ", ID=" + std::to_string(_session.m_pi.ei.mascot_info->id) + "] que nao tem no IFF_STRUCT do Server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		}

		/// ********** Premium User +10% EXP and PANG *********************

		if (_pgi.premium_flag) {
			
			auto rate_premium = sPremiumSystem::getInstance().getExpPangRateByTicket(_session.m_pi.pt._typeid);

			ui.rate.exp += rate_premium;
			ui.rate.pang += rate_premium;
		}

		/// ********** Itens Exp/Pang Rate **********
	}
}

void Game::requestSendTreasureHunterItem(player& _session) {

	INIT_PLAYER_INFO("requestSendTreasureHunterItem", "tentou enviar os itens ganho no Treasure Hunter do jogo", &_session);

	std::vector< stItem > v_item;
	stItem item{ 0 };
	BuyItem bi{ 0 };

	if (!pgi->thi.v_item.empty()) {

		for (auto& el : pgi->thi.v_item) {
			
			bi.clear();
			item.clear();

			bi.id = -1;
			bi._typeid = el._typeid;
			bi.qntd = el.qntd;

			item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

			if (item._typeid == 0) {
				_smp::message_pool::getInstance().push(new message("[Game::requestSendTreasureHunterItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializar item[TYPEID="
						+ std::to_string(bi._typeid) + "], mas nao consgeuiu. Bug", CL_FILE_LOG_AND_CONSOLE));

				continue;
			}

			v_item.push_back(item);
		}

		// Add Item, se tiver Item
		if (!v_item.empty()) {
			
			auto rai = item_manager::addItem(v_item, _session, 0, 0);
			
			if (rai.fails.size() > 0 && rai.type != item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
				_smp::message_pool::getInstance().push(new message("[Game::requestSendTreasureHunterItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] nao conseguiu adicionar os itens que ele ganhou no Treasure Hunter. Bug", CL_FILE_LOG_AND_CONSOLE));
		}
	}

	// UPDATE ON GAME
	packet p((unsigned short)0x134);

	p.addUint8((unsigned char)v_item.size());

	for (auto& el : v_item) {
		p.addUint32(_session.m_pi.uid);

		p.addUint32(el._typeid);
		p.addInt32(el.id);
		p.addUint32(el.qntd);
		p.addUint8(0);	// Opt Acho, mas nunca vi diferente de 0

		p.addUint16((unsigned short)(el.stat.qntd_dep / 0x8000));
		p.addUint16((unsigned short)(el.stat.qntd_dep % 0x8000));
	}

	packet_func::session_send(p, &_session, 1);
}

unsigned char Game::checkCharMotionItem(player& _session) {

	// Characters Equip
	if (_session.getState() && 
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.isConnected()) {	// Check Player Connected

		if (_session.m_pi.ei.char_info == nullptr) {	// Player não está com character equipado, kika dele do jogo
			_smp::message_pool::getInstance().push(new message("[Game::checkCharMotionItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao esta com Character equipado. kika ele do jogo. pode ser Bug.",
				CL_FILE_LOG_AND_CONSOLE));
			// Kika aqui "deletePlayer(s);"

			return 0;
		}

		// Motion Item
		if (FIND_ELEMENT_ARRAY_OF_ARRAY(_session.m_pi.ei.char_info->parts_typeid, motion_item)) {
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[Game::checkCharMotionItem][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] esta equipado com Motion Item no Character[TYPEID="
					+ std::to_string(_session.m_pi.ei.char_info->_typeid) + ", ID=" + std::to_string(_session.m_pi.ei.char_info->id) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
			return 1u;
		}
	}

	return 0u;
}

void Game::sendUpdateInfoAndMapStatistics(player& _session, int _option) {

	packet p((unsigned short)0x45);

	p.addBuffer(&_session.m_pi.ui, sizeof(UserInfo));

	p.addBuffer(&_session.m_pi.ti_current_season, sizeof(TrofelInfo));

	// Ainda tenho que ajeitar esses Map Statistics no Pacote Principal, No Banco de dados e no player_info class
	if (_option == -1) {

		// -1 12 Bytes, os 2 tipos de dados do Map Statistics
		p.addInt64(-1);
		p.addInt32(-1);

	}else {
		// Normal essa season
		if (_session.m_pi.a_ms_normal[m_ri.course & 0x7F].course != (m_ri.course & 0x7F))
			p.addInt8(-1);	// Não tem
		else {
			p.addInt8((char)m_ri.course & 0x7F);
			p.addBuffer(&_session.m_pi.a_ms_normal[m_ri.course & 0x7F], sizeof(MapStatistics));
		}

		// Normal rest season
		// tem que fazer o map statistics soma de todas season
		//p.addInt8((char)m_ri.course & 0x7F);
		//p.addBuffer(&_session.m_pi.aa_ms_normal_todas_season[0][m_ri.course & 0x7F], sizeof(MapStatistics));
		p.addInt8(-1);	// Não tem

		// Natural essa season
		if (_session.m_pi.a_ms_natural[m_ri.course & 0x7F].course != (m_ri.course & 0x7F))
			p.addInt8(-1);	// N�o tem
		else {
			p.addInt8((char)m_ri.course & 0x7F);
			p.addBuffer(&_session.m_pi.a_ms_natural[m_ri.course & 0x7F], sizeof(MapStatistics));
		}

		// Natural rest season
		// tem que fazer o map statistics soma de todas season
		//p.addInt8((char)m_ri.course & 0x7F);
		//p.addBuffer(&_session.m_pi.aa_ms_normal_todas_season[0][m_ri.course & 0x7F], sizeof(MapStatistics));
		p.addInt8(-1);	// Não tem

		// Normal Assist essa season
		if (_session.m_pi.a_msa_normal[m_ri.course & 0x7F].course != (m_ri.course & 0x7F))
			p.addInt8(-1);	// Não tem
		else {
			p.addInt8((char)m_ri.course & 0x7F);
			p.addBuffer(&_session.m_pi.a_msa_normal[m_ri.course & 0x7F], sizeof(MapStatistics));
		}

		// Normal Assist rest season
		// tem que fazer o map statistics soma de todas season
		//p.addInt8((char)m_ri.course & 0x7F);
		//p.addBuffer(&_session.m_pi.aa_ms_normal_todas_season[0][m_ri.course & 0x7F], sizeof(MapStatistics));
		p.addInt8(-1);	// Não tem

		// Natural Assist essa season
		if (_session.m_pi.a_msa_natural[m_ri.course & 0x7F].course != (m_ri.course & 0x7F))
			p.addInt8(-1);	// Não tem
		else {
			p.addInt8((char)m_ri.course & 0x7F);
			p.addBuffer(&_session.m_pi.a_msa_natural[m_ri.course & 0x7F], sizeof(MapStatistics));
		}

		// Natural Assist rest season
		// tem que fazer o map statistics soma de todas season
		//p.addInt8((char)m_ri.course & 0x7F);
		//p.addBuffer(&_session.m_pi.aa_ms_normal_todas_season[0][m_ri.course & 0x7F], sizeof(MapStatistics));
		p.addInt8(-1);	// Não tem

		// Grand Prix essa season
		if (_session.m_pi.a_ms_grand_prix[m_ri.course & 0x7F].course != (m_ri.course & 0x7F))
			p.addInt8(-1);	// Não tem
		else {
			p.addInt8((char)m_ri.course & 0x7F);
			p.addBuffer(&_session.m_pi.a_ms_grand_prix[m_ri.course & 0x7F], sizeof(MapStatistics));
		}

		// Grand Prix rest season
		// tem que fazer o map statistics soma de todas season
		//p.addInt8((char)m_ri.course & 0x7F);
		//p.addBuffer(&_session.m_pi.aa_ms_normal_todas_season[0][m_ri.course & 0x7F], sizeof(MapStatistics));
		p.addInt8(-1);	// Não tem

		// Grand Prix Assist essa season
		if (_session.m_pi.a_msa_grand_prix[m_ri.course & 0x7F].course != (m_ri.course & 0x7F))
			p.addInt8(-1);	// Não tem
		else {
			p.addInt8((char)m_ri.course & 0x7F);
			p.addBuffer(&_session.m_pi.a_msa_grand_prix[m_ri.course & 0x7F], sizeof(MapStatistics));
		}

		// Grand Prix Assist rest season
		// tem que fazer o map statistics soma de todas season
		//p.addInt8((char)m_ri.course & 0x7F);
		//p.addBuffer(&_session.m_pi.aa_ms_normal_todas_season[0][m_ri.course & 0x7F], sizeof(MapStatistics));
		p.addInt8(-1);	// Não tem
	}

	packet_func::session_send(p, &_session, 1);
}

void Game::sendFinishMessage(player& _session) {

	INIT_PLAYER_INFO("sendFinishMessage", "tentou enviar message no chat que o player terminou o jogo", &_session);

	packet p((unsigned short)0x40);

	p.addUint8(16);	// Msg que terminou o game

	p.addString(_session.m_pi.nickname);
	p.addUint16(0);	// Size Msg

	p.addInt32(pgi->data.score);
	p.addUint64(pgi->data.pang);
	p.addUint8(pgi->assist_flag);

	packet_func::game_broadcast(*this, p, 1);
}

void Game::requestCalculeRankPlace() {

	if (!m_player_order.empty())
		m_player_order.clear();

	for (auto& el : m_player_info)
		if (el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT)	// menos os que quitaram
			m_player_order.push_back(el.second);

	std::sort(m_player_order.begin(), m_player_order.end(), Game::sort_player_rank);
}

void Game::setGameFlag(PlayerGameInfo* _pgi, PlayerGameInfo::eFLAG_GAME _fg) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Game::setGameFlag][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_finish_game);
#endif

	_pgi->flag = _fg;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_finish_game);
#endif
}

void Game::setFinishGameFlag(PlayerGameInfo* _pgi, unsigned char _finish_game) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Game::setFinishGameFlag][Error] PlayerGameInfo* _pgi is invlaid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_finish_game);
#endif

	_pgi->finish_game = _finish_game;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_finish_game);
#endif
}

bool Game::AllCompleteGameAndClear() {
	
	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_finish_game);
#endif

	// Da error Aqui
	for (auto& el : m_players) {

		try {
			
			INIT_PLAYER_INFO("PlayersCompleteGameAndClear", "tentou verificar se o player terminou o jogo", el);

			if (pgi->flag != PlayerGameInfo::eFLAG_GAME::PLAYING)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Game::AllCompleteGameAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	ret = (count == m_players.size());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_finish_game);
#endif

	return ret;
}

bool Game::PlayersCompleteGameAndClear() {

	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_finish_game);
#endif

	// Da error Aqui
	for (auto& el : m_players) {
		
		try {
			
			INIT_PLAYER_INFO("PlayersCompleteGameAndClear", "tentou verificar se o player terminou o jogo", el);

			if (pgi->finish_game)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GamePlayersCompleteGameAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	ret = (count == m_players.size());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_finish_game);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_finish_game);
#endif

	return ret;
}

bool Game::checkEndGame(player& _session) {
	
	INIT_PLAYER_INFO("checkEndGame", "tentou verificar se eh o final do jogo", &_session);

	return (m_course->findHoleSeq(pgi->hole) == m_ri.qntd_hole);
}

uint32_t Game::getCountPlayersGame() {

	size_t count = 0u;

	count = std::count_if(m_player_info.begin(), m_player_info.end(), [](auto& _el) {
		return _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT;
	});

	return (uint32_t)count;
}

unsigned char Game::requestPlace(player& _session) {

	if (!_session.getState())
		throw exception("[Game::requestPlace][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 1, 0));

	// Valor padrão
	unsigned short hole = 0u;

	INIT_PLAYER_INFO("requestPlace", "tentou pegar o lugar[Hole] do player no jogo", &_session);

	if (pgi->hole > -1) {

		hole = m_course->findHoleSeq(pgi->hole);

		if (hole == (unsigned short)~0/*Error*/) {

			// Valor padrão
			hole = 0u;

			_smp::message_pool::getInstance().push(new message("[Game::requestPlace][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pegar a sequencia do hole[NUMERO="
					+ std::to_string(pgi->hole) + "], mas ele nao encontrou no course do game na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
		}
	
	}else if (pgi->init_first_hole)	// Só cria mensagem de log se o player já inicializou o primeiro hole do jogo e tem um valor inválido no pgi->hole (não é uma sequência de hole válida)
		_smp::message_pool::getInstance().push(new message("[Game::requesPlace][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] tentou pegar o hole[NUMERO=" + std::to_string(pgi->hole) + "] em que o player esta na sala[NUMERO=" + std::to_string(m_ri.numero) 
				+ "], mas ele esta carregando o course ou tem algum error.", CL_FILE_LOG_AND_CONSOLE));

	return (unsigned char)hole;
}

bool Game::isGamingBefore(uint32_t _uid) {

	if (_uid == 0u)
		throw exception("[Game::isGamingBefore][Error] _uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 1000, 0));

	return std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
		return _el.second->uid == _uid;
	}) != m_player_info.end();
}

void Game::requestSendTimeGame(player& _session) {
	UNREFERENCED_PARAMETER(_session);
}

void Game::requestUpdateEnterAfterStartedInfo(player& _session, EnterAfterStartInfo& _easi) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_easi);
}

void Game::requestStartFirstHoleGrandZodiac(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestReplyInitialValueGrandZodiac(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}

void Game::requestReadSyncShotData(player& _session, packet *_packet, ShotSyncData& _ssd) {
	REQUEST_BEGIN("readSyncShotData");

	try {

		// Decrypt Packet Dados, que esse o cliente encripta com a chave segura da sala
		//DECRYPT16((_packet->getBuffer() + 2), (_packet->getSize() - 2), m_ri.key);

		_packet->readBuffer(&_ssd, sizeof(_ssd));

		// Decrypt Packet Dados, que esse o cliente encripta com a chave segura da sala
		DECRYPT16((unsigned char*)&_ssd, sizeof(_ssd), m_ri.key);

		if (_ssd.pang > 40000u)
			_smp::message_pool::getInstance().push(new message("[Game::requestReadSyncShotDate][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] pode esta usando hack, PANG[" + std::to_string(_ssd.pang) + "] maior que 40k. Hacker ou Bug.", CL_FILE_LOG_AND_CONSOLE));
		
		if (_ssd.bonus_pang > 10000u)
			_smp::message_pool::getInstance().push(new message("[Game::requestReadSyncShotDate][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] pode esta usando hack, BONUS PANG[" + std::to_string(_ssd.bonus_pang) + "] maior que 10k. Hacker ou Bug.", CL_FILE_LOG_AND_CONSOLE));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("size data: " + std::to_string(sizeof(_ssd)) + "\n\r" + hex_util::BufferToHexString((unsigned char*)&_ssd, sizeof(_ssd)), CL_FILE_LOG_AND_CONSOLE));
		// Log Shot Sync Data
		_smp::message_pool::getInstance().push(new message("Log Shot Sync Data:\n\r" + _ssd.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::requestReadSyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool Game::execSmartCalculatorCmd(player& _session, std::string& _msg, eTYPE_CALCULATOR_CMD _type) {
	CHECK_SESSION("execSmartCalculatorCmd");

	bool ret = false;

	try {

		if (_type == eTYPE_CALCULATOR_CMD::SMART_CALCULATOR) {

			auto ctx = sSmartCalculator::getInstance().getPlayerCtx(_session.m_pi.uid, _type);

			if (ctx == nullptr)
				ctx = sSmartCalculator::getInstance().makePlayerCtx(_session.m_pi.uid, _type);

			auto pp = ctx->getSmartPlayer();

			auto gsv = getGameShotValueToSmartCalculator(_session, pp->m_club_index, pp->m_power_shot_index);

			pp->setGameShotValue(gsv);

			sSmartCalculator::getInstance().checkCommand(_session.m_pi.uid, _msg, _type);
		
		}else {

			// Stadium Calculator
			auto ctx = sSmartCalculator::getInstance().getPlayerCtx(_session.m_pi.uid, _type);

			if (ctx == nullptr)
				ctx = sSmartCalculator::getInstance().makePlayerCtx(_session.m_pi.uid, _type);

			sSmartCalculator::getInstance().checkCommand(_session.m_pi.uid, _msg, _type);
		}

		// OK
		ret = true;

	// Log
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Game::execSmartCalculatorCmd][Log] Player[UID="
			+ std::to_string(_session.m_pi.uid) + "] mandou o comando(" + _msg + ") para o " + std::string(_type == eTYPE_CALCULATOR_CMD::SMART_CALCULATOR ? "Smart Calculator" : "Stadium Calculator"), CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[Game::execSmartCalculatorCmd][Log] Player[UID="
			+ std::to_string(_session.m_pi.uid) + "] mandou o comando(" + _msg + ") para o " + std::string(_type == eTYPE_CALCULATOR_CMD::SMART_CALCULATOR ? "Smart Calculator" : "Stadium Calculator"), CL_ONLY_FILE_LOG));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::execSmartCalculatorCmd][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		ret = false;
	}

	return ret;
}

stGameShotValue Game::getGameShotValueToSmartCalculator(player& _session, unsigned char _club_index, unsigned char _power_shot_index) {
	CHECK_SESSION("getGameShotValueToSmartCalculator");

	stGameShotValue gsv{ 0u };

	try {

		INIT_PLAYER_INFO("getGameShotValueToSmartCalculator", "tentou executar Smart Calculator Command", &_session);

		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			throw exception("[Game::getGameShotValueToSmartCalculator][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou executar Smart Calculator command na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "], mas nao encontrou o Hole[NUMERO=" + std::to_string((short)pgi->hole)
					+ "] no Course.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 10000, 0));

		auto wind_flag = initCardWindPlayer(pgi, hole->getWind().wind);

		auto wind = hole->getWind().wind + 1 + wind_flag;
		auto distance = hole->getPinLocation().diffXZ(pgi->location) * 0.3125f;
		auto ground = 1u;
		auto power_range = 230.f;
		auto slope_break = 1.f;
		auto power = _session.m_pi.getSlotPower();
		auto angTo_rad = -std::atan2(hole->getPinLocation().x - pgi->location.x, hole->getPinLocation().z - pgi->location.z);
		auto angTo = angTo_rad * 180 / 
#if defined(_WIN32)
			std::_Pi;
#elif defined(__linux__)
			std::numbers::pi;
#endif

		auto angEarcuff = pgi->earcuff_wind_angle_shot * 180 / 
#if defined(_WIN32)
			std::_Pi;
#elif defined(__linux__)
			std::numbers::pi;
#endif

		auto ang = 0.l;

		if (pgi->effect_flag_shot.stFlag.EARCUFF_DIRECTION_WIND) {

			long double rad_earcuff_hole = pgi->earcuff_wind_angle_shot + -angTo_rad;

			if (rad_earcuff_hole < 0.f)
				rad_earcuff_hole = (2 * 
#if defined(_WIN32)
					std::_Pi
#elif defined(__linux__)
					std::numbers::pi
#endif
				) + rad_earcuff_hole;

			ang = rad_earcuff_hole * 180 / 
#if defined(_WIN32)
				std::_Pi;
#elif defined(__linux__)
				std::numbers::pi;
#endif

		}else
			ang = fmodl((pgi->degree / 255.f) * 360.f + -angTo, 360.f);

		bool pwr_by_condition_actived = pgi->effect_flag_shot.stFlag.SWITCH_TWO_EFFECT;

		if (pwr_by_condition_actived && _session.m_pi.ei.char_info != nullptr
			&& _session.m_pi.ei.char_info->isAuxPartEquiped(0x70210001u) && pgi->item_active_used_shot != 0u
			&& pgi->item_active_used_shot == POWER_MILK_TYPEID)
			pwr_by_condition_actived = false; // Usou Milk, encheu 1 ps perde a condição para ativar o efeito

		auto power_extra = _session.m_pi.getExtraPower(pwr_by_condition_actived);

		if (pgi->effect_flag_shot.stFlag.DECREASE_1M_OF_WIND && wind > 1)
			wind--;

		if (pgi->effect_flag_shot.stFlag.WIND_1M_RANDOM)
			wind = 1;

		if (pgi->effect_flag_shot.stFlag.SAFETY_CLIENT_RANDOM || pgi->effect_flag_shot.stFlag.SAFETY_RANDOM) {

			ground = 100;
			slope_break = 0.f;
		}

		if (pgi->effect_flag_shot.stFlag.GROUND_100_PERCENT_RONDOM)
			ground = 100;

		if (pgi->item_active_used_shot != 0u) {

			if (isSilentWindItem(pgi->item_active_used_shot))
				wind = 1;

			if (isSafetyItem(pgi->item_active_used_shot)) {

				ground = 100;
				slope_break = 0.f;
			}
		}

#ifdef _DEBUG
		// Log
		_smp::message_pool::getInstance().push(new message("[Game::getGameShotValueToSmartCalculator][Log] Wind=" + std::to_string(wind)
				+ ", Distance=" + std::to_string(distance) + ", Power=" + std::to_string(power) + ", Power_Extra="
				+ std::to_string(power_extra.getTotal(0)) + ", ANGLE[ANG_TO_RAD=" + std::to_string(angTo_rad)
				+ ", ANG_TO=" + std::to_string(angTo) + ", ANG=" + std::to_string(ang) + ", DEGREE=" + std::to_string((pgi->degree / 255.f) * 360.f)
				+ ", ANG_EARCUFF=" + (pgi->effect_flag_shot.stFlag.EARCUFF_DIRECTION_WIND ? std::to_string(angEarcuff) : "NONE") + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (_club_index < sAllClubInfo3D::getInstance().m_clubs.size()) {

			Club3D club(sAllClubInfo3D::getInstance().m_clubs[_club_index], calculeTypeDistance((float)distance));

			power_range = (float)club.getRange(power_extra, (float)power, ePOWER_SHOT_FACTORY(_power_shot_index));
		}

		gsv.gm = (_session.m_pi.m_cap.stBit.gm_normal || _session.m_pi.m_cap.stBit.game_master) ? true : false;

		gsv.safety = (slope_break == 0.f) ? true : false;
		gsv.ground = (ground == 100u) ? true : false;
		gsv.rain = (hole->getWeather() == 2 && !pgi->effect_flag_shot.stFlag.NO_RAIN_EFFECT) ? true : false;

		gsv.power_slot = (unsigned char)power;

		gsv.auxpart_pwr = (char)power_extra.getPowerDrive().m_auxpart;
		gsv.mascot_pwr = (char)power_extra.getPowerDrive().m_mascot;
		gsv.card_pwr = (char)power_extra.getPowerDrive().m_card;
		gsv.ps_card_pwr = (char)power_extra.getPowerShot().m_card;

		gsv.distance = (float)distance;
		gsv.wind = (float)wind;
		gsv.degree = (float)ang;

		gsv.mira_rad = angTo_rad;
		gsv.power_range = power_range;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::getGameShotValueToSmartCalculator][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return gsv;
}

void Game::clear_time() {

	// Garantir que qualquer exception derrube o server
	try {

		if (m_timer != nullptr)
			sgs::gs::getInstance().unMakeTime(m_timer);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::clear_time][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	m_timer = nullptr;
}

void Game::clear_player_order() {

	if (!m_player_order.empty()) {
		m_player_order.clear();
		m_players.shrink_to_fit();
	}
}

void Game::initAchievement(player& _session) {

	INIT_PLAYER_INFO("initAchievement", "tentou inicializar o achievemento do player no jogo", &_session);

	try {

		// Initialize Achievement Player
		pgi->sys_achieve.incrementCounter(0x6C400002u/*Normal Game*/);

		if (m_ri.natural.stBit.short_game/* & 2 /*Short Game*/)
			pgi->sys_achieve.incrementCounter(0x6C4000BBu/*Short Game*/);

		if (m_ri.master == _session.m_pi.uid) {
			pgi->sys_achieve.incrementCounter(0x6C400098u/*Master da Sala*/);

			if (m_ri.artefato > 0)
				pgi->sys_achieve.incrementCounter(0x6C400099u/*Master da Sala com Artefact*/);
		}

		if (_session.m_pi.ei.char_info != nullptr) {

			auto ctc = SysAchievement::getCharacterCounterTypeId(_session.m_pi.ei.char_info->_typeid);

			if (ctc > 0u)
				pgi->sys_achieve.incrementCounter(ctc/*Character Counter Typeid*/);
		}

		if (_session.m_pi.ei.cad_info != nullptr) {

			auto ctc = SysAchievement::getCaddieCounterTypeId(_session.m_pi.ei.cad_info->_typeid);

			if (ctc > 0u)
				pgi->sys_achieve.incrementCounter(ctc/*Caddie Counter Typeid*/);
		}

		if (_session.m_pi.ei.mascot_info != nullptr) {

			auto ctm = SysAchievement::getMascotCounterTypeId(_session.m_pi.ei.mascot_info->_typeid);

			if (ctm > 0u)
				pgi->sys_achieve.incrementCounter(ctm/*Mascot Counter Typeid*/);
		}

		auto ct = SysAchievement::getCourseCounterTypeId(m_ri.course & 0x7F);

		if (ct > 0u)
			pgi->sys_achieve.incrementCounter(ct/*Course Counter Item*/);

		ct = SysAchievement::getQntdHoleCounterTypeId(m_ri.qntd_hole);

		if (ct > 0u)
			pgi->sys_achieve.incrementCounter(ct/*Qntd Hole Counter Item*/);

		// Fim do inicializa o Achievement

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::initAchievement][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::SYS_ACHIEVEMENT)
			throw;	// relança exception
	}
}

void Game::records_player_achievement(player& _session) {
	CHECK_SESSION("records_players_achievement");

	INIT_PLAYER_INFO("records_player_achievement", "tentou atualizar os achievement de records do player no jogo", &_session);

	try {

		if (pgi->ui.ob > 0)
			pgi->sys_achieve.incrementCounter(0x6C40004Cu/*OB*/, pgi->ui.ob);

		if (pgi->ui.bunker > 0)
			pgi->sys_achieve.incrementCounter(0x6C40004Eu/*Bunker*/, pgi->ui.bunker);

		if (pgi->ui.tacada > 0 || pgi->ui.putt > 0)
			pgi->sys_achieve.incrementCounter(0x6C400055u/*Shots*/, pgi->ui.tacada + pgi->ui.putt);

		if (pgi->ui.hole > 0)
			pgi->sys_achieve.incrementCounter(0x6C400005u/*Holes*/, pgi->ui.hole);

		if (pgi->ui.total_distancia > 0)
			pgi->sys_achieve.incrementCounter(0x6C400056u/*Yards*/, pgi->ui.total_distancia);

		// Bug o valor é 0 por que (int)0.9f é 0 ele trunca não arredondo, e tem que truncar mesmo
		// Para fixa esse bug é só fazer >= 1.f sempre vai ser (int) >= 1(truncado)
		if (pgi->ui.best_drive >= 1.f)
			pgi->sys_achieve.incrementCounter(0x6C400057u/*Best Drive*/, (int)pgi->ui.best_drive);

		if (pgi->ui.best_chip_in >= 1.f)
			pgi->sys_achieve.incrementCounter(0x6C400058u/*Best Chip-in*/, (int)pgi->ui.best_chip_in);

		if (pgi->ui.best_long_putt >= 1.f)
			pgi->sys_achieve.incrementCounter(0x6C400077u/*Best Long-putt*/, (int)pgi->ui.best_long_putt);

		if (pgi->ui.acerto_pangya > 0)
			pgi->sys_achieve.incrementCounter(0x6C40000Bu/*Acerto PangYa*/, pgi->ui.acerto_pangya);

		if (pgi->data.pang > 0)
			pgi->sys_achieve.incrementCounter(0x6C40000Du/*Pangs Ganho em 1 jogo*/, (int)pgi->data.pang);

		if (pgi->data.score != 0)
			pgi->sys_achieve.incrementCounter(0x6C40000Cu/*Score*/, pgi->data.score);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::records_player_achievement][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::SYS_ACHIEVEMENT)
			throw;	// relança exception
	}
}

void Game::update_sync_shot_achievement(player& _session, Location& _last_location) {
	CHECK_SESSION("update_sync_shot_achievement");

	INIT_PLAYER_INFO("update_sync_shot_achievement", "tentou atualizar o achievement de Desafios no jogo", &_session);

	try {

		// Só conta se o player acertou o hole
		if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole) {

			// Long-putt
			if (pgi->shot_sync.state_shot.display.stDisplay.long_putt && pgi->shot_sync.state_shot.shot.stShot.club_putt) {
				auto diff = pgi->location.diffXZ(_last_location) * MEDIDA_PARA_YARDS;

				if (diff >= 30.f)
					pgi->sys_achieve.incrementCounter(0x6C400035u/*Long Putt 30y+*/);

				if (diff >= 25.f)
					pgi->sys_achieve.incrementCounter(0x6C400034u/*Long Putt 25y+*/);

				if (diff >= 20.f)
					pgi->sys_achieve.incrementCounter(0x6C400033u/*Long Putt 20y+*/);

				if (diff >= 17.f)
					pgi->sys_achieve.incrementCounter(0x6C400032u/*Long Putt 17y+*/);
			}

			// Fez o hole de Beam Impact
			if (pgi->shot_sync.state_shot.display.stDisplay.beam_impact)
				pgi->sys_achieve.incrementCounter(0x6C40006Fu/*Beam Impact*/);

			// Fez o hole com
			if (pgi->shot_sync.state_shot.shot.stShot.spin_front)
				pgi->sys_achieve.incrementCounter(0x6C400064u/*Spin Front*/);

			if (pgi->shot_sync.state_shot.shot.stShot.spin_back)
				pgi->sys_achieve.incrementCounter(0x6C400065u/*Spin Back*/);

			if (pgi->shot_sync.state_shot.shot.stShot.curve_left || pgi->shot_sync.state_shot.shot.stShot.curve_right)
				pgi->sys_achieve.incrementCounter(0x6C400066u/*Curve*/);

			if (pgi->shot_sync.state_shot.shot.stShot.tomahawk)
				pgi->sys_achieve.incrementCounter(0x6C400067u/*Tomahawk*/);

			if (pgi->shot_sync.state_shot.shot.stShot.spike)
				pgi->sys_achieve.incrementCounter(0x6C400068u/*Spike*/);

			if (pgi->shot_sync.state_shot.shot.stShot.cobra)
				pgi->sys_achieve.incrementCounter(0x6C40006Eu/*Cobra*/);

			// Fez sem usar power shot
			if (pgi->shot_sync.state_shot.display.stDisplay.chip_in_without_special_shot && !pgi->shot_sync.state_shot.display.stDisplay.special_shot/*Nega*/)
				pgi->sys_achieve.incrementCounter(0x6C40005Bu/*Fez sem usar power shot*/);

			// o pacote12 passa primeiro depois que o server response ele passa esse pacote1B, então esse valor sempre vai está certo
			// Fez Errando pangya
			if (pgi->shot_data.acerto_pangya_flag & 2/*Errou pangya*/ && !pgi->shot_sync.state_shot.shot.stShot.club_putt/*Nega*/)
				pgi->sys_achieve.incrementCounter(0x6C400059u/*Fez errando pangya*/);
		}

		// Tacada Power Shot ou Double Power Shot
		if (pgi->shot_sync.state_shot.shot.stShot.power_shot)
			pgi->sys_achieve.incrementCounter(0x6C400051u/*Power Shot*/);

		if (pgi->shot_sync.state_shot.shot.stShot.double_power_shot)
			pgi->sys_achieve.incrementCounter(0x6C400052u/*Double Power Shot*/);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::update_sync_shot_achievement][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::SYS_ACHIEVEMENT)
			throw;	// relança exception
	}
}

void Game::rain_hole_consecutivos_count(player& _session) {

	auto& chr = m_course->getConsecutivesHolesRain();

	INIT_PLAYER_INFO("rain_hole_consecutivos_count", "tentou atualizar o achievement count de chuva em holes consecutivos do player no jogo", &_session);

	try {

		uint32_t count = 0u;

		auto seq = m_course->findHoleSeq(pgi->hole);

		if (chr.isValid()) {
		
			// 2 Holes consecutivos
			if ((count = chr._2_count.getCountHolesRainBySeq(seq)) > 0u)
				pgi->sys_achieve.incrementCounter(0x6C40009Bu/*2 Holes consecutivos*/, count);

			if ((count = chr._3_count.getCountHolesRainBySeq(seq)) > 0u)
				pgi->sys_achieve.incrementCounter(0x6C40009Cu/*3 Holes consecutivos*/, count);

			if ((count = chr._4_pluss_count.getCountHolesRainBySeq(seq)) > 0u)
				pgi->sys_achieve.incrementCounter(0x6C40009Du/*4 ou mais Holes consecutivos*/, count);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::rain_hole_consecutivos_count][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::SYS_ACHIEVEMENT)
			throw;	// relança exception
	}
}

void Game::score_consecutivos_count(player& _session) {

	int32_t score = -2, last_score = -2;
	uint32_t count = 0u;

	INIT_PLAYER_INFO("rain_score_consecutivos", "tentou atualizar o achievement contador de score consecutivos do player no jogo", &_session);

	try {

		for (auto i = 0u; i < m_ri.qntd_hole; ++i) {

			score = SysAchievement::getScoreNum(pgi->progress.tacada[i], pgi->progress.par_hole[i]);

			// Change Score, Soma o Count do Score	
			if ((score != last_score || i == (m_ri.qntd_hole - 1)/*Ultimo hole*/) && last_score != -2/*Primeiro Hole*/) {

				// 1 == 2, 2 ou mais Holes com o mesmo score
				if (count >= 1u && last_score >= 0/*Scores que tem no achievement*/) {

					switch (last_score) {
					case 0: // HIO
						pgi->sys_achieve.incrementCounter(0x6C400063u/*HIO*/);
						break;
					case 1: // Alba
						pgi->sys_achieve.incrementCounter(0x6C400062u/*Alba*/);
						break;
					case 2: // Eagle
						pgi->sys_achieve.incrementCounter(0x6C400061u/*Eagle*/);
						break;
					case 3: // Birdie
						pgi->sys_achieve.incrementCounter(0x6C40005Du/*Birdie*/);
						break;
					case 4: // Par
						pgi->sys_achieve.incrementCounter(0x6C40005Eu/*Par*/);
						break;
					case 5: // Bogey
						pgi->sys_achieve.incrementCounter(0x6C40005Fu/*Bogey*/);
						break;
					case 6: // Double Bogey
						pgi->sys_achieve.incrementCounter(0x6C400060u/*Double Bogey*/);
						break;
					}
				}

				// Reseta o count
				count = 0u;

			}else if (score == last_score)
				count++;

			// Update Last Score
			last_score = score;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::score_consecutivos_count][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::SYS_ACHIEVEMENT)
			throw;	// relança exception
	}
}

void Game::rain_count(player& _session) {

	try {

		// Recovery, Chuva, Neve/*Tempo Ruim*/
		if (m_course->countHolesRain() > 0) {

			uint32_t count = 0u;

			INIT_PLAYER_INFO("rain_count_players", "tentou atualizar o achievement contador de chuva do player no jogo", &_session);

			// Pega pela quantidade de holes jogados
			auto seq = m_course->findHoleSeq(pgi->hole);

			if ((count = m_course->countHolesRainBySeq(seq)) > 0u)
				pgi->sys_achieve.incrementCounter(0x6C40009Au/*Chuva*/, count);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::rain_count][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::SYS_ACHIEVEMENT)
			throw;	// relança exception
	}
}

void Game::setEffectActiveInShot(player& _session, uint64_t _effect) {
	CHECK_SESSION("setEffectActiveInShot");

	try {

		INIT_PLAYER_INFO("setEffectActiveInShot", "tentou setar o efeito ativado na tacada", &_session);

		pgi->effect_flag_shot.ullFlag |= _effect;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::setEffectActiveInShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Game::clearDataEndShot(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr)
		throw exception("[Game::clearDataEndShot][Error] PlayerGameInfo *_pgi is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 100, 0));

	try {

		_pgi->effect_flag_shot.clear();
		_pgi->item_active_used_shot = 0u;
		_pgi->earcuff_wind_angle_shot = 0.f;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::clearDataEndShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Game::checkEffectItemAndSet(player & _session, uint32_t _typeid) {
	CHECK_SESSION("checkEffectitemAndSet");

	try {

		auto ability = sIff::getInstance().findAbility(_typeid);

		if (ability != nullptr) {

			for (auto i = 0u; i < (sizeof(ability->efeito.type) / sizeof(ability->efeito.type[0])); ++i) {

				if (ability->efeito.type[i] == 0u)
					continue;

				if (ability->efeito.type[i] == (uint32_t)IFF::Ability::eEFFECT_TYPE::COMBINE_ITEM_EFFECT) {

					// find item setEffectTable
					auto effectTable = sIff::getInstance().findSetEffectTable((uint32_t)ability->efeito.rate[i]);

					if (effectTable != nullptr) {

						for (auto j = 0u; j < (sizeof(effectTable->effect.effect) / sizeof(effectTable->effect.effect[0])); ++j) {

							if (effectTable->effect.effect[j] == 0u || effectTable->effect.effect[j] < 4u)
								continue;

							switch (effectTable->effect.effect[j]) {
							case IFF::SetEffectTable::eEFFECT::PIXEL:
								setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PIXEL_2));
								break;
							case IFF::SetEffectTable::eEFFECT::ONE_ALL_STATS:
								setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::ONE_IN_ALL_STATS));
								break;
							case IFF::SetEffectTable::eEFFECT::WIND_DECREASE:
								setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::DECREASE_1M_OF_WIND));
								break;
							case IFF::SetEffectTable::eEFFECT::PATINHA:
								setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_NOT_ACCUMULATE));
								break;
							}
						}
					}

				}else
					setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE(ability->efeito.type[i])));
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Game::checkEffectitemAndSet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Game::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[Game::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[Game::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *game = reinterpret_cast< Game* >(_arg);

	switch (_msg_id) {
	case 12:	// Update ClubSet Workshop
	{
		auto cmd_ucw = reinterpret_cast< CmdUpdateClubSetWorkshop* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[Game::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_ucw->getUID()) + "] Atualizou ClubSet[TYPEID=" + std::to_string(cmd_ucw->getInfo()._typeid) + ", ID="
				+ std::to_string(cmd_ucw->getInfo().id) + "] Workshop[C0=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[0]) + ", C1=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[1]) + ", C2="
				+ std::to_string(cmd_ucw->getInfo().clubset_workshop.c[2]) + ", C3=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[3]) + ", C4=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[4])
				+ ", Level=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.level) + ", Mastery=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.mastery) + ", Rank="
				+ std::to_string(cmd_ucw->getInfo().clubset_workshop.rank) + ", Recovery=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.recovery_pts) + "] Flag=" + std::to_string(cmd_ucw->getFlag()) + "", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 1:	// Insert Ticket Report Dados
	{
		break;
	}
	case 0:
	default:	// 25 é update item equipado slot
		break;
	}
}

bool Game::sort_player_rank(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2) {
	
	if (_pgi1->data.score == _pgi2->data.score)
		return _pgi1->data.pang > _pgi2->data.pang;

	return _pgi1->data.score < _pgi2->data.score;
}

void Game::makePlayerInfo(player& _session) {

	PlayerGameInfo *pgi = makePlayerInfoObject(_session);

	// Bloqueia o OID para ninguém pegar ele até o torneio acabar
	sgs::gs::getInstance().blockOID(_session.m_oid);

	// Update Place player
	_session.m_pi.place = 0u;	// Jogando

	pgi->uid = _session.m_pi.uid;
	pgi->oid = _session.m_oid;
	pgi->level = (unsigned char)_session.m_pi.mi.level;

	// Entrou no Jogo depois de ele ter começado
	if (m_state)
		pgi->enter_after_started = 1u;

	// Typeid do Mascot Equipado
	if (_session.m_pi.ei.mascot_info != nullptr)
		pgi->mascot_typeid = _session.m_pi.ei.mascot_info->_typeid;

	// Premium User
	if (_session.m_pi.m_cap.stBit.premium_user/* & (1 << 14)/*Premium User*/)
		pgi->premium_flag = 1u;

	// Card Wind Flag
	pgi->card_wind_flag = getPlayerWindFlag(_session);

	// Treasure Hunter Points Card Player Initialize Data
	// Não pode ser chamado depois do Init Item Used Game, por que ele vai add os pontos dos itens que dá Drop rate e treasure hunter point
	pgi->thi = getPlayerTreasureInfo(_session);

	// Flag Assist
	auto pWi = _session.m_pi.findWarehouseItemByTypeid(ASSIST_ITEM_TYPEID);

	if (pWi != nullptr)
		pgi->assist_flag = 1u;

	// Verifica se o player está com o motion item equipado
	pgi->char_motion_item = checkCharMotionItem(_session);

	// Motion Item da Treasure Hunter Point também
	if (pgi->char_motion_item)
		pgi->thi.all_score += 20;	// +20 all score

	pgi->data.clear();
	pgi->location.clear();

	auto it = m_player_info.insert(std::make_pair(&_session, pgi));

	// Check insert pair in map of game player info
	if (!it.second) {

		if (it.first != m_player_info.end() && it.first->first != nullptr && it.first->first == (&_session)) {

			if (it.first->second->uid != _session.m_pi.uid) {

				// Add novo PlayerGameInfo para a (session*), que tem um novo player conectado na session.
				// Isso pode acontecer quando um player entrou no jogo saiu com ticket ou tomou dc e desconectou do server, 
				// e outro player pegou essa session e tentou entrar no mesmo jogo que o player estava

				try {

					// pega o antigo PlayerGameInfo para usar no Log
					auto pgi_ant = m_player_info.at(&_session);

					// Novo PlayerGameInfo
					m_player_info.at(&_session) = pgi;

					// Log de que trocou o PlayerGameInfo da session
					_smp::message_pool::getInstance().push(new message("[Game::makePlayerInfo][WARNING][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] esta trocando o PlayerGameInfo[UID=" + std::to_string(pgi_ant->uid) + "] do player anterior que estava conectado com essa session, pelo o PlayerGameInfo[UID="
							+ std::to_string(pgi->uid) + "] do player atual da session.", CL_FILE_LOG_AND_CONSOLE));

					// Libera a memória do antigo player info
					delete pgi_ant;

				}catch (std::out_of_range& e) {
					UNREFERENCED_PARAMETER(e);

					_smp::message_pool::getInstance().push(new message("[Game::makePlayerInfo][Error][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "], nao conseguiu atualizar o PlayerGameInfo da session para o novo PlayerGameInfo do player atual da session. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			
			}else
				_smp::message_pool::getInstance().push(new message("[Game::makePlayerInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] nao conseguiu adicionar o PlayerGameInfo da session, por que ja tem o mesmo PlayerGameInfo no map.", CL_FILE_LOG_AND_CONSOLE));
		
		}else
			_smp::message_pool::getInstance().push(new message("[Game::makePlayerInfo][Error] nao conseguiu inserir o pair de PlayerInfo do player[UID="
					+ std::to_string(_session.m_pi.uid) + "] no map de player info do game. Bug", CL_FILE_LOG_AND_CONSOLE));
	}

	// Init Item Used Game
	requestInitItemUsedGame(_session, *it.first->second);
}

void Game::clearAllPlayerInfo() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_player_info) {

		if (el.second != nullptr) {

			sgs::gs::getInstance().unblockOID(el.second->oid);	// Desbloqueia o OID

			// Libera a memória
			delete el.second;
		}
	}

	m_player_info.clear();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void Game::initAllPlayerInfo() {

	for (auto& el : m_players)
		makePlayerInfo(*el);
}

// Make Object Player Info Polimofirsmo
PlayerGameInfo* Game::makePlayerInfoObject(player& _session) {
	UNREFERENCED_PARAMETER(_session);

	return new PlayerGameInfo{ 0 };
}

void Game::requestInitShotSended(player& _session, packet *_packet) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_packet);
}
