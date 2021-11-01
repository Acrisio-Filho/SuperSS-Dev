// Arquivo approach.cpp
// Criado em 12/06/2020 as 12:03 por Acrisio
// Implementa��o da classe Approach

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "approach.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "treasure_hunter_system.hpp"

#include "../PACKET/packet_func_sv.h"

#include "../Game Server/game_server.h"

#include "../../Projeto IOCP/UTIL/hex_util.h"

#include "../GAME/approach_mission_system.hpp"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[Approach" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[Approach::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) PlayerApproachInfo *pgi = reinterpret_cast< PlayerApproachInfo* >(getPlayerInfo((__session))); \
	if (pgi == nullptr) \
		throw exception("[Approach::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 1, 4)); \

using namespace stdA;

bool Approach::sort_approach_rank_place(approach_dados_ex& _ad1, approach_dados_ex& _ad2) {

	// Verifica os state primeiro
	if (_ad1.state.ucState != 0u)
		return false;
	else if (_ad2.state.ucState != 0u)
		return true;

	if (_ad1.distance == _ad2.distance)
		return _ad1.time > _ad2.time;

	return _ad1.distance < _ad2.distance;
}

Approach::Approach(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: TourneyBase(_players, _ri, _rv, _channel_rookie), m_approach_state(false), m_thread_sync_hole(nullptr), 
#if defined(_WIN32)
	m_hEvent_sync_hole(INVALID_HANDLE_VALUE), m_hEvent_sync_hole_pulse(INVALID_HANDLE_VALUE), 
#elif defined(__linux__)
	m_hEvent_sync_hole(nullptr), m_hEvent_sync_hole_pulse(nullptr), 
#endif
	m_state_app(), m_timeout(0u), m_mission(0u) {

	// Inicializa o critical section do sync shot
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_sync_shot);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	if (!sApproachMissionSystem::getInstance().isLoad())
		sApproachMissionSystem::getInstance().load();

	// Inicializa o Treasure Hunter System
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	auto course = sTreasureHunterSystem::getInstance().findCourse(m_ri.course & 0x7F);

	if (course == nullptr)
		_smp::message_pool::getInstance().push(new message("[Approach::Approach][Error] tentou pegar o course do Treasure Hunter System, mas o course[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "] nao existe no sistema", CL_FILE_LOG_AND_CONSOLE));
	else
		sTreasureHunterSystem::getInstance().updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado

	// Aqui tem que inicializar os players info
	initAllPlayerInfo();

	// Cria evento que vai para a thread sync hole
#if defined(_WIN32)
	if ((m_hEvent_sync_hole = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[Approach::Approach][Error] ao criar evento sync hole.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 1050, GetLastError()));
#elif defined(__linux__)

	m_hEvent_sync_hole = new Event(true, 0u);

	if (!m_hEvent_sync_hole->is_good()) {

		delete m_hEvent_sync_hole;

		m_hEvent_sync_hole = nullptr;

		throw exception("[Approach::Approach][Error] ao criar evento sync hole.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 1050, errno));
	}
#endif

	// Cria evento que vai pulsar a thread sync hole para ir mais r�pido quando um player tacar
#if defined(_WIN32)
	if ((m_hEvent_sync_hole_pulse = CreateEvent(NULL, FALSE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[Approach::Approach][Error] ao criar evento sync hole pulse.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 1050, GetLastError()));
#elif defined(__linux__)

	m_hEvent_sync_hole_pulse = new Event(false, 0u);

	if (!m_hEvent_sync_hole_pulse->is_good()) {

		delete m_hEvent_sync_hole_pulse;

		m_hEvent_sync_hole_pulse = nullptr;

		throw exception("[Approach::Approach][Error] ao criar evento sync hole pulse.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 1050, errno));
	}
#endif

	// Cria a thread que vai sincronizar os player no hole
	m_thread_sync_hole = new thread(1060/*sync hole*/, Approach::_syncHoleTime, (LPVOID)this);

	m_state = init_game();
}

Approach::~Approach() {

	m_approach_state = false;

	if (m_game_init_state != 2)
		finish();

	while (!PlayersCompleteGameAndClear())
#if defined(_WIN32)
		Sleep(500);
#elif defined(__linux__)
		usleep(500000);
#endif

	deleteAllPlayer();

	// Finish Thread Sync hole
	finish_thread_sync_hole();

	// Libera mem�ria do critical section
#if defined(_WIN32)
	DeleteCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs_sync_shot);
#endif

	// Log
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Approach::~Approach][Log] Approach destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

bool Approach::deletePlayer(player* _session, int _option) {
	
	if (_session == nullptr)
		throw exception("[Approach::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 50, 0));
	
	bool ret = false;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		auto it = std::find(m_players.begin(), m_players.end(), _session);

		if (it != m_players.end()) {

			unsigned char opt = 3;	// Saiu Quitou

			packet p;

			auto sessions = getSessions(*it);

			if (m_game_init_state == 1/*Come�ou*/) {

				INIT_PLAYER_INFO("deletePlayer", "tentou sair do jogo", _session);

				requestFinishItemUsedGame(*(*it));	// Salva itens usados no Approach

				requestSaveInfo(*(*it), (_option == 0x800) ? 5/*N�o conta quit*/ : 1); // Quitou ou tomou DC

				setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::QUIT);

				// Saiu do approach
				pgi->m_app_dados.setLeftGame();

				// Resposta Player saiu do Jogo, tira ele do list de score
				p.init_plain((unsigned short)0x61);

				p.addUint32((*it)->m_oid);

				packet_func::vector_send(p, sessions, 1);

				// Resposta Player saiu do jogo MSG
				p.init_plain((unsigned short)0x40);

				p.addUint8(2);	// Player Saiu Msg

				p.addString((*it)->m_pi.nickname);

				p.addUint16(0);	// size Msg, n�o precisa de msg o pangya j� manda na opt 2

				packet_func::vector_send(p, sessions, 1);

				// Resposta Player saiu do jogo
				sendUpdateState(*_session, opt);

				if (AllCompleteGameAndClear())
					ret = true;	// Termina o Approach
				
				sendUpdateInfoAndMapStatistics(*_session, -1);
			
			}

			// Delete Player
			m_players.erase(it);
		}else
			_smp::message_pool::getInstance().push(new message("[Approach::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::deletePlayer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Critical Section
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
	}

	return ret;
}

void Approach::deleteAllPlayer() {

	while (!m_players.empty())
		deletePlayer(*m_players.begin(), 0);
}

#if defined(_WIN32)
DWORD WINAPI Approach::_syncHoleTime(LPVOID lpParameter) {
#elif defined(__linux__)
void* Approach::_syncHoleTime(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(Approach);

	result = pTP->syncHoleTime();

	END_THREAD_SETUP("syncHoleTIme");
}

#if defined(_WIN32)
DWORD Approach::syncHoleTime() {
#elif defined(__linux__)
void* Approach::syncHoleTime() {
#endif

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[Approach::syncHoleTime][Log] syncHoleTime iniciado com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		packet p;

		DWORD retWait = WAIT_TIMEOUT;

#if defined(_WIN32)
		HANDLE wait_events[2] = { m_hEvent_sync_hole, m_hEvent_sync_hole_pulse };

		while ((retWait = WaitForMultipleObjects((sizeof(wait_events) / sizeof(wait_events[0])), wait_events, false, 1000/*1 segundo*/)) == WAIT_TIMEOUT || retWait == (WAIT_OBJECT_0 + 1)) {
#elif defined(__linux__)
		std::vector< Event* > wait_events = { m_hEvent_sync_hole, m_hEvent_sync_hole_pulse };

		while ((retWait = Event::waitMultipleEvent(wait_events.size(), wait_events, false, 1000/*1 segundos*/)) == WAIT_TIMEOUT || retWait == (WAIT_OBJECT_0 + 1)) {
#endif

			try {

				m_state_app.lock();

				switch (m_state_app.getState()) {
					case eSTATE_APPROACH_SYNC::LOAD_HOLE:
					{

						if (checkAllLoadHole()) {

							clearLoadHole();

							init_mission();

							for (auto& el : m_players) {

								p.init_plain((unsigned short)0x53);
								p.addUint32(el->m_oid);

								packet_func::session_send(p, el, 1);
							}

							sendRatesOfApproach();

							// Mission
							p.init_plain((unsigned short)0x14F);

							m_mission.toPacket(p);

							packet_func::game_broadcast(*this, p, 1);

							m_state_app.setState(eSTATE_APPROACH_SYNC::LOAD_CHAR_INTRO);
						}

						break;
					}
					case eSTATE_APPROACH_SYNC::LOAD_CHAR_INTRO:
					{

						if (checkAllFinishCharIntro()) {

							clearFinishCharIntro();

#if defined(_WIN32)
							InterlockedExchange(&m_timeout, 0u);
#elif defined(__linux__)
							__atomic_store_n(&m_timeout, 0u, __ATOMIC_RELAXED);
#endif

							startTime();

							p.init_plain((unsigned short)0x90);

							packet_func::game_broadcast(*this, p, 1);

							m_state_app.setState(eSTATE_APPROACH_SYNC::END_SHOT);
						}

						break;
					}
					case eSTATE_APPROACH_SYNC::END_SHOT:
					{
						
#if defined(_WIN32)
						if (checkAllSyncShot() && checkAllFinishShot() && InterlockedCompareExchange(&m_timeout, 1u, 1u) == 1u) {
#elif defined(__linux__)
						uint32_t check_m = 1u;
						if (checkAllSyncShot() && checkAllFinishShot() && __atomic_compare_exchange_n(&m_timeout, &check_m, 1u, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif

							clearSyncShot();
							clearFinishShot();

							requestCalculeRankPlaceHole();

							mission_win();

							top_rank_win();

							// Score board Approach
							sendScoreBoard();

							for (auto& el : m_players) {
								
								finishHole(*el);

								changeHole(*el);
							}
						}

						break;
					}
					case eSTATE_APPROACH_SYNC::WAIT_END_GAME:
					{
						// Faz nada por enquanto
						break;
					}
				}

				// Libera
				m_state_app.unlock();

			}catch (exception& e) {

				m_state_app.unlock();

				_smp::message_pool::getInstance().push(new message("[Approach::syncHoleTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}
		
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[Approach::syncHoleTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[Approach::syncHoleTime][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		_smp::message_pool::getInstance().push(new message("[Approach::syncHoleTime][ErrorSystem] syncHoleTime() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE));
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Approach::syncHoleTime][Log] Saindo de syncHoleTime()...", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[Approach::syncHoleTime][Log] Saindo de syncHoleTime()...", CL_ONLY_FILE_LOG));
#endif // _DEBUG

#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return (void*)0;
#endif
}

bool Approach::requestFinishLoadHole(player& _session, packet* _packet) {
	REQUEST_BEGIN("FinishLoadHole");
	
	bool ret = false;

	try {

		m_state_app.setStateWithLock(eSTATE_APPROACH_SYNC::LOAD_HOLE);

		INIT_PLAYER_INFO("requestFinishLoadHole", "tentou finalizar carregamento do hole no jogo", &_session);

		setLoadHole(pgi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::requestFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void Approach::requestFinishCharIntro(player& _session, packet* _packet) {
	REQUEST_BEGIN("FinishCharIntro");

	try {

		INIT_PLAYER_INFO("requestFinishCharIntro", "tentou finalizar intro do char no jogo", &_session);

		// Zera todas as tacada num dos players
		pgi->data.tacada_num = 0u;
		
		// Giveup Flag
		pgi->data.giveup = 0u;

		setFinishCharIntro(pgi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::requestFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}


void Approach::changeHole(player& _session) {

	if (checkEndGame(_session))
		finish_approach(_session, 0);
	else
		// Resposta terminou o hole
		updateFinishHole(_session, 1);	// Terminou
}

void Approach::finishHole(player& _session) {

	requestFinishHole(_session, 0);

	requestUpdateItemUsedGame(_session);

	delete_all_quiter();
}

void Approach::requestInitShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShot");

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Approach::requestInitShot][Log] Packet Hex: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		ShotDataEx sd{ 0 };

		sd.option = _packet->readUint16();

		// Power Shot
		if (sd.option == 1)
			_packet->readBuffer(&sd.power_shot, sizeof(sd.power_shot));

		_packet->readBuffer(&sd, sizeof(ShotDataBase));

		INIT_PLAYER_INFO("requestInitShot", "tentou iniciar tacada no jogo", &_session);

		pgi->shot_data = sd;

#ifdef _DEBUG
		// Log Shot Data Ex
		_smp::message_pool::getInstance().push(new message("[Approach::requestInitShot][Log] Log Shot Data Ex:\n\r" + sd.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::requestInitShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Approach::finish_approach(player& _session, int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		INIT_PLAYER_INFO("finish_approach", "tentou terminar o tourney no jogo", &_session);

		if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

			// Calcula os pangs que o player ganhou
			requestCalculePang(_session);

			// Atualizar os pang do player se ele estiver com assist ligado, e for maior que beginner E
			updatePlayerAssist(_session);

			if (m_game_init_state == 1 && _option == 0) {

				// Achievement Counter
				pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

			}else if (m_game_init_state == 1 && _option == 1) {	// Acabou o Tempo

				requestFinishHole(_session, 1);		// Acabou o Tempo
			}
		}

		setGameFlag(pgi, (_option == 0) ? PlayerGameInfo::eFLAG_GAME::FINISH : PlayerGameInfo::eFLAG_GAME::END_GAME);
		
		GetLocalTime(&pgi->time_finish);

		if (AllCompleteGameAndClear() && m_game_init_state == 1)
			finish();	// Envia os pacotes que termina o jogo Ex: 0xCE, 0x79 e etc
	}
}

bool Approach::requestFinishGame(player& _session, packet* _packet) {
	REQUEST_BEGIN("FinishGame");
	
	bool ret = false;

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Approach::requestFinishGame][Log] Packet Hex: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// OID do player que enviou o pacote para terminar o Jogo(Approach)
		uint32_t oid = _packet->readUint32();

		if (oid != _session.m_oid)
			throw exception("[Approach::requestFinishGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ ", OID=" + std::to_string(_session.m_oid) + "] OID(" + std::to_string(oid) + ") is wrong", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 410, 0));

		// Packet0CB
		ret = finish_game(_session, 0xCB);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::requestFinishGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void Approach::timeIsOver() {

#if defined(_WIN32)
	InterlockedExchange(&m_timeout, 1);
#elif defined(__linux__)
	__atomic_store_n(&m_timeout, 1, __ATOMIC_RELAXED);
#endif

#if defined(_WIN32)
	if (m_hEvent_sync_hole_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_sync_hole_pulse);
#elif defined(__linux__)
	if (m_hEvent_sync_hole_pulse != nullptr)
		m_hEvent_sync_hole_pulse->set();
#endif

	short hole = -1;

	if (m_players.size() > 0) {

		INIT_PLAYER_INFO("timeIsOver", "acabou o tempo do hole, tentou pegar o info do primeiro player do jogo", m_players[0]);

		hole = pgi->hole;
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Approach::timeIsOver][Log] Tempo do Hole[" + std::to_string(hole) + "] acabou no Approach. na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

}

bool Approach::init_game() {
	
	if (m_players.size() > 0) {

		// variavel que salva a data local do sistema
		initGameTime();

		m_game_init_state = 1;	// Come�ou

		m_approach_state = true;
	}

	return true;
}

uint32_t Approach::getCountPlayersGame() {
	
	size_t count = 0u;

	count = std::count_if(m_player_info.begin(), m_player_info.end(), [](auto& _el) {
		return _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT || ((PlayerApproachInfo*)_el.second)->m_app_dados.state_quit == approach_dados_ex::eSTATE_QUIT::SQ_QUIT_START;
	});

	return (uint32_t)count;
}

void Approach::requestUpdateItemUsedGame(player& _session) {

	INIT_PLAYER_INFO("requestUpdateItemUsedGame", "tentou atualizar itens usado no jogo", &_session);

	auto& ui = pgi->used_item;

	ui.club.count += (uint32_t)(1.f * 10.f * ui.club.rate * TRANSF_SERVER_RATE_VALUE(m_rv.clubset) * TRANSF_SERVER_RATE_VALUE(ui.rate.club));

	// Passive Item exceto Time Booster e Auto Command, que soma o contador por uso, o cliente passa o pacote, dizendo que usou o item
	for (auto& el : ui.v_passive) {

		// Passive Item no Approach s� consome os item boost de pang e o Club Mastery Boost,
		// Consome todos os outros menos os de Experi�ncia
		if (std::find(passive_item_exp, LAST_ELEMENT_IN_ARRAY(passive_item_exp), el.second._typeid) == LAST_ELEMENT_IN_ARRAY(passive_item_exp)) {

			if (CHECK_PASSIVE_ITEM(el.second._typeid) && el.second._typeid != TIME_BOOSTER_TYPEID/*Time Booster*/ && el.second._typeid != AUTO_COMMAND_TYPEID)
				el.second.count++;
			else if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::BALL	/*Ball*/
					|| sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::AUX_PART) /*AuxPart(Anel)*/
				el.second.count++;
		}
	}
}

void Approach::finish() {

	m_game_init_state = 2;	// Acabou

	requestCalculeRankPlace();

	top_rank_win();

	finishAllDadosApproach();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el);
	}
}

void Approach::requestFinishData(player& _session) {

	// Finish Artefact Frozen Flame agora � direto no Finish Item Used Game
	requestFinishItemUsedGame(_session);

	requestDrawTreasureHunterItem(_session);

	// Resposta terminou game - Placar
	sendPlacar(_session);

	// Resposta Treasure Hunter Item Draw
	sendTreasureHunterItemDrawGUI(_session);

	requestSaveInfo(_session, 0);

	// Passa o finaliza o Approach
	packet p((unsigned short)0x151);

	packet_func::game_broadcast(*this, p, 1);

	// Resposta Treasure Hunter Item
	requestSendTreasureHunterItem(_session);
}

void Approach::requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) {

	CHECK_SESSION_BEGIN("requestTranslateSyncShotData");

	try {

		auto s = findSessionByOID(_ssd.oid);

		if (s == nullptr)
			throw exception("[Approach::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada do player[OID="
					+ std::to_string(_ssd.oid) + "], mas o player nao existe nessa jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 200, 0));

		// Update Sync Shot Player
		if (_session.m_pi.uid == s->m_pi.uid) {

			INIT_PLAYER_INFO("requestTranslateSyncShotData", "tentou sincronizar a tacada no jogo", &_session);

			pgi->shot_sync = _ssd;

			// Last Location Player
			auto last_location = pgi->location;

			// Update Location Player
			pgi->location.x = _ssd.location.x;
			pgi->location.z = _ssd.location.z;

			// Update Pang and Bonus Pang
			pgi->data.pang = _ssd.pang;
			pgi->data.bonus_pang = _ssd.bonus_pang;

			// J� s� na fun��o que come�a o tempo do player do turno
			pgi->data.tacada_num++;

			if (_ssd.state == ShotSyncData::OUT_OF_BOUNDS || _ssd.state == ShotSyncData::UNPLAYABLE_AREA) {
				
				pgi->data.tacada_num++;

				// Approach
				pgi->m_app_dados.state.stState.ob_or_water_hazard = 1u;
			}

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[Approach::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada no hole[NUMERO="
						+ std::to_string((unsigned short)pgi->hole) + "], mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::APPROACH, 12, 0));

			// Conta j� a pr�xima tacada, no give up
			if (!_ssd.state_shot.display.stDisplay.acerto_hole && hole->getPar().total_shot <= (pgi->data.tacada_num + 1)) {

				// +1 que � giveup, s� add se n�o passou o n�mero de tacadas
				if (pgi->data.tacada_num < hole->getPar().total_shot)
					pgi->data.tacada_num++;

				pgi->data.giveup = 1;

				// Approach
				pgi->m_app_dados.state.stState.giveup = 1u;

				// Soma +1 no Bad Condute
  				pgi->data.bad_condute++;
			}

			// Approach
			if (_ssd.state == ShotSyncData::SHOT_STATE::INTO_HOLE || std::abs(hole->getPinLocation().diffXZ(pgi->location) * MEDIDA_PARA_YARDS) <= 0.08/*Est� dentro do hole � chip-in*/)
				pgi->m_app_dados.state.stState.chip_in = 1u;

			// Pega a distancia e o tempo
			int32_t elapsed_time = (int)(m_ri.time_30s - m_timer->getElapsed());

			if (elapsed_time <= 0l)
				pgi->m_app_dados.state.stState.timeout = 1u;

			if (pgi->m_app_dados.state.ucState == 0u) {
				
				pgi->m_app_dados.distance = (uint32_t)(std::round(hole->getPinLocation().diffXZ(pgi->location) * MEDIDA_PARA_YARDS * 10));
				pgi->m_app_dados.time = elapsed_time;
			
			}else {

				pgi->m_app_dados.distance = (uint32_t)(std::round(hole->getPinLocation().diffXZ(last_location) * MEDIDA_PARA_YARDS * 10));
				pgi->m_app_dados.time = (elapsed_time <= 0l ? 0l : elapsed_time);
			}

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[Approach::requestTranslateSyncShotData][Log] SyncShot(time_shot: " 
					+ std::to_string(pgi->shot_sync.tempo_shot) + "), Timer(Elapsed: " + std::to_string(elapsed_time) + ")", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestTranslateSyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Approach::requestReplySyncShotData(player& _session) {
	CHECK_SESSION_BEGIN("requestReplySyncShotData");

	try {

		// Resposta Sync Shot
		sendSyncShot(_session);

		INIT_PLAYER_INFO("requestReplySyncShotData", "tentou responder o Sync Shot Data", &_session);

		// Set Sync Shot Flag
		setSyncShot(pgi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::requestReplySyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

int Approach::checkEndShotOfHole(player& _session) {

	INIT_PLAYER_INFO("checkEndShotOfHole", "tentou verificar o fim do shot no hole", &_session);

	setFinishShot(pgi);

	return 0;
}

void Approach::updateFinishHole(player& _session, int option) {

	INIT_PLAYER_INFO("updateFinishHole", "tentou terminar o hole no jogo", &_session);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Approach::updateFinishHole][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] Terminou o hole[NUMERO=" + std::to_string(pgi->hole) + "].", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	packet p((unsigned short)0x65);

	packet_func::game_broadcast(*this, p, 1);
}

void Approach::sendRatesOfApproach() {

	try {
		// Table Rate Voice And Effect
		TableRateVoiceAndEffect table = { "W_BIGBONGDARI", TableRateVoiceAndEffect::eTYPE::W_BIGBONGDARI };

		// Rate Table Voice
		packet p((unsigned short)0x115);

		p.addString(table.name);

		p.addBuffer(table.table, sizeof(table.table));

		packet_func::game_broadcast(*this, p, 1);

		// Table Rate Voice And Effect
		table = { "R_BIGBONGDARI", TableRateVoiceAndEffect::eTYPE::R_BIGBONGDARI };

		p.init_plain((unsigned short)0x115);

		p.addString(table.name);

		p.addBuffer(table.table, sizeof(table.table));

		packet_func::game_broadcast(*this, p, 1);

		// Table Rate Voice And Effect
		table = { "VOICE_CLUB", TableRateVoiceAndEffect::eTYPE::VOICE_CLUB };

		p.init_plain((unsigned short)0x115);

		p.addString(table.name);

		p.addBuffer(table.table, sizeof(table.table));

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::sendRatesOfVersusBase][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Approach::finish_thread_sync_hole() {

	try {
		
		if (m_thread_sync_hole != nullptr) {

#if defined(_WIN32)
			if (m_hEvent_sync_hole != INVALID_HANDLE_VALUE)
				SetEvent(m_hEvent_sync_hole);
#elif defined(__linux__)
			if (m_hEvent_sync_hole != nullptr)
				m_hEvent_sync_hole->set();
#endif

			m_thread_sync_hole->waitThreadFinish(INFINITE);

			delete m_thread_sync_hole;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::finish_thread_sync_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (m_thread_sync_hole != nullptr) {

			m_thread_sync_hole->exit_thread();

			delete m_thread_sync_hole;
		}
	}

	m_thread_sync_hole = nullptr;

#if defined(_WIN32)
	if (m_hEvent_sync_hole != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent_sync_hole);

	if (m_hEvent_sync_hole_pulse != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent_sync_hole_pulse);

	m_hEvent_sync_hole = INVALID_HANDLE_VALUE;
	m_hEvent_sync_hole_pulse = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	if (m_hEvent_sync_hole != nullptr)
		delete m_hEvent_sync_hole;

	if (m_hEvent_sync_hole_pulse != nullptr)
		delete m_hEvent_sync_hole_pulse;

	m_hEvent_sync_hole = nullptr;
	m_hEvent_sync_hole_pulse = nullptr;
#endif
}

void Approach::requestFinishHole(player& _session, int option) {

	INIT_PLAYER_INFO("requestFinishHole", "tentou finalizar o dados do hole do player no jogo", &_session);

	auto hole = m_course->findHole(pgi->hole);

	if (hole == nullptr)
		throw exception("[Approach::finishHole][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar hole[NUMERO="
				+ std::to_string((unsigned short)pgi->hole) + "] no jogo, mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 20, 0));

	uint32_t time_hole = 0u;
	uint32_t distance_hole = 0u;

	// Finish Hole Dados
	if (option == 0) {

		pgi->m_app_dados.total_distance += pgi->m_app_dados.distance;
		pgi->m_app_dados.total_time += pgi->m_app_dados.time;
		pgi->m_app_dados.total_box += pgi->m_app_dados.box;
		pgi->m_app_dados.total_box += pgi->m_app_dados.rank_box;

		time_hole = pgi->m_app_dados.time;
		distance_hole = pgi->m_app_dados.distance;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Approach::requestFinishHole][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] terminou o hole[COURSE=" 
				+ std::to_string(hole->getCourse()) + ", NUMERO=" + std::to_string(hole->getNumero()) + ", PAR=" 
				+ std::to_string(hole->getPar().par) + ", DISTANCE=" + std::to_string(distance_hole) + ", TIME=" + std::to_string(time_hole) + ", BOX=" 
				+ std::to_string(pgi->m_app_dados.box) + ", RANK_BOX=" + std::to_string(pgi->m_app_dados.rank_box) + ", TOTAL_DISTANCE=" 
				+ std::to_string(pgi->m_app_dados.total_distance) + ", TOTAL_TIME=" + std::to_string(pgi->m_app_dados.total_time) 
				+ ", TOTAL_BOX=" + std::to_string(pgi->m_app_dados.total_box) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		pgi->m_app_dados.distance = 0u;
		pgi->m_app_dados.time = 0u;
		pgi->m_app_dados.box = 0u;
		pgi->m_app_dados.rank_box = 0u;
		pgi->m_app_dados.state.ucState = 0u;

		// Zera dados
		pgi->data.time_out = 0u;

		// Giveup Flag
		pgi->data.giveup = 0u;

		// Zera as penalidades do hole
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
			pgi->progress.score[pgi->progress.hole - 1] = (unsigned char)time_hole;
			pgi->progress.tacada[pgi->progress.hole - 1] = distance_hole;
		}

	}
}

void Approach::requestSaveInfo(player& _session, int option) {

	INIT_PLAYER_INFO("requestSaveInfo", "tentou salvar o info dele no jogo", &_session);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Approach::requestSaveInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] UserInfo[" + pgi->ui.toString() + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Limpa o User Info por que n�o add nada, s� o tempo e os pangs ganhos
		pgi->ui.clear();

		auto diff = getLocalTimeDiff(m_start_time);

		if (diff > 0)
			diff /= STDA_10_MICRO_PER_SEC; // NanoSeconds To Seconds

		pgi->ui.tempo = (uint32_t)diff;

		// Pode tirar pangs
		int64_t total_pang = pgi->data.pang + pgi->data.bonus_pang;

		// UPDATE ON SERVER AND DB
		_session.m_pi.addUserInfo(pgi->ui, total_pang);	// add User Info

		if (total_pang > 0)
			_session.m_pi.addPang(total_pang);				// add Pang
		else if (total_pang < 0)
			_session.m_pi.consomePang(total_pang * -1);		// consome Pangs

		// Log
		_smp::message_pool::getInstance().push(new message("[Approach::requestSaveInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] " + (option == 0 ? "Terminou o Approach " : "Saiu do Approach ") + " com [TOTAL_DISTANCE=" + std::to_string(pgi->m_app_dados.distance) 
				+ ", TOTAL_TIME=" + std::to_string(pgi->m_app_dados.time) + ", TOTAL_BOX=" + std::to_string(pgi->m_app_dados.box) 
				+ ", TOTAL_PANG=" + std::to_string(total_pang) + "]", CL_FILE_LOG_AND_CONSOLE));
	
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[Approach::requestSaveInfo][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Approach::requestDrawTreasureHunterItem(player& _session) {

	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	INIT_PLAYER_INFO("requestDrawTreasureHunterItem", "tentou sortear os item(ns) do Treasure Hunter do jogo", &_session);

	pgi->thi.v_item = sTreasureHunterSystem::getInstance().drawApproachBox(pgi->m_app_dados.box, m_ri.course & 0x7F);
}

void Approach::sendPlacar(player& _session) {

	packet p((unsigned short)0x14E);

	uint32_t count = getCountPlayersGame();

	p.addUint8((unsigned char)count);

	for (auto& el : m_player_info)
		if (el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT || ((PlayerApproachInfo*)el.second)->m_app_dados.state_quit == approach_dados_ex::eSTATE_QUIT::SQ_QUIT_START)
			((PlayerApproachInfo*)el.second)->m_app_dados.toPacket(p);

	packet_func::game_broadcast(*this, p, 1);
}

void Approach::sendSyncShot(player& _session) {

	INIT_PLAYER_INFO("sendSyncShot", "tentou sincronizar a tacada do jogador no jogo", &_session);

	packet p((unsigned short)0x6E);

	p.addUint32(pgi->shot_sync.oid);

	p.addInt8(pgi->hole);

	p.addFloat(pgi->location.x);
	p.addFloat(pgi->location.z);

	p.addUint32(pgi->shot_sync.state_shot.shot.ulState);

	if (pgi->m_app_dados.state.ucState != 0u) {

		p.addUint32((uint32_t)~0u);
		p.addUint32(0u);

	}else {

		p.addUint32(pgi->m_app_dados.distance);
		p.addUint32(pgi->m_app_dados.time);
	}

	p.addUint16(pgi->shot_sync.tempo_shot);

	packet_func::game_broadcast(*this, p, 1);
}

PlayerGameInfo* Approach::makePlayerInfoObject(player& _session) {

	auto pai = new PlayerApproachInfo{ 0 };

	try {

		CHECK_SESSION_BEGIN("makePlayerInfoObject");

		pai->m_app_dados.uid = _session.m_pi.uid;
		pai->m_app_dados.oid = _session.m_oid;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Approach::makePlayerInfoObject][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return pai;
}

void Approach::setFinishShot(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setFinishShot][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_shot = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

#if defined(_WIN32)
	if (m_hEvent_sync_hole_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_sync_hole_pulse);
#elif defined(__linux__)
	if (m_hEvent_sync_hole_pulse != nullptr)
		m_hEvent_sync_hole_pulse->set();
#endif
}

bool Approach::checkAllFinishShot() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("CheckAllFinishShot", "tentou verificar se todos os player terminaram a Tacada no jogo", _el);

			if (pgi->finish_shot)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::CheckAllFinishShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void Approach::clearFinishShot() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_finish_shot();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool Approach::setFinishShotAndCheckAllFinishShotAndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {
		
		_smp::message_pool::getInstance().push(new message("[Approach::setFinishShotAndCheckAllFinishShotAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));
		
		return false;
	}

	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_shot = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("setFinishShotAndCheckAllFinishShotAndClear", "tentou verificar se todos os player terminaram a Tacada no jogo", _el);

			if (pgi->finish_shot)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::setFinishShotAndCheckAllFinishShotAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_finish_shot();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void Approach::setLoadHole(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setLoadHole][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_load_hole = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

#if defined(_WIN32)
	if (m_hEvent_sync_hole_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_sync_hole_pulse);
#elif defined(__linux__)
	if (m_hEvent_sync_hole_pulse != nullptr)
		m_hEvent_sync_hole_pulse->set();
#endif
}

bool Approach::checkAllLoadHole() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("CheckAllLoadHole", "tentou verificar se todos os player terminaram de carregar o hole no jogo", _el);

			if (pgi->finish_load_hole)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::CheckAllLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void Approach::clearLoadHole() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_load_hole();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool Approach::setLoadHoleAndCheckAllLoadHoleAndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setLoadHoleAndCheckAllLoadHoleAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_load_hole = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("setLoadHoleAndCheckAllLoadHoleAndClear", "tentou verificar se todos os player terminaram de carregar o hole no jogo", _el);

			if (pgi->finish_load_hole)
				count++;
		
		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::setLoadHoleAndCheckAllLoadHoleAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_load_hole();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void Approach::setFinishCharIntro(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setFinishCharIntro][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_char_intro = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

#if defined(_WIN32)
	if (m_hEvent_sync_hole_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_sync_hole_pulse);
#elif defined(__linux__)
	if (m_hEvent_sync_hole_pulse != nullptr)
		m_hEvent_sync_hole_pulse->set();
#endif
}

bool Approach::checkAllFinishCharIntro() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("CheckAllFinishCharIntro", "tentou verificar se todos os player terminaram a Intro do Character no jogo", _el);

			if (pgi->finish_char_intro)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::CheckAllFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void Approach::clearFinishCharIntro() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_finish_char_intro();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool Approach::setFinishCharIntroAndCheckAllFinishCharIntroAndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setFinishCharIntroAndCheckAllFinishCharIntroAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_char_intro = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("setFinishCharIntroAndCheckAllFinishCharIntroAndClear", "tentou verificar se todos os player terminaram a Intro do Character no jogo", _el);

			if (pgi->finish_char_intro)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::setFinishCharIntroAndCheckAllFinishCharIntroAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_finish_char_intro();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void Approach::setSyncShot(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setSyncShot[Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->sync_shot_flag = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

#if defined(_WIN32)
	if (m_hEvent_sync_hole_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_sync_hole_pulse);
#elif defined(__linux__)
	if (m_hEvent_sync_hole_pulse != nullptr)
		m_hEvent_sync_hole_pulse->set();
#endif
}

bool Approach::checkAllSyncShot() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("CheckAllSyncShot", "tentou verificar se todos os player sincronizaram a Tacada no jogo", _el);

			if (pgi->sync_shot_flag)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::CheckAllSyncShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void Approach::clearSyncShot() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_sync_shot();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool Approach::setSyncShotAndCheckAllSyncShotAndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Approach::setSyncShotAndCheckAllSyncShotAndClear][Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));
		
		return false;
	}

	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->sync_shot_flag = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("setSyncShotAndCheckAllSyncShotAndClear", "tentou verificar se todos os player sincronizaram a Tacada no jogo", _el);

			if (pgi->sync_shot_flag)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::setSyncShotAndCheckAllSyncShotAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_sync_shot();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void Approach::clear_all_load_hole() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("clear_all_load_hole", " tentou limpar all load hole no jogo", _el);

			pgi->finish_load_hole = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::clear_all_load_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void Approach::clear_all_finish_shot() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_finish_shot", " tentou limpar all finish tacada no jogo", _el);

			pgi->finish_shot = 0u;

			// Limpa o tick_sync_end_shot para a pr�xima tacada(shot)
			pgi->tick_sync_end_shot.clear();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::clear_all_finish_shot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void Approach::clear_all_finish_char_intro() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_finish_char_intro", " tentou limpar all finish char intro no jogo", _el);

			pgi->finish_char_intro = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::clear_all_finish_char_intro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void Approach::clear_all_sync_shot() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_sync_shot", " tentou limpar all sync shot do jogo", _el);

			pgi->sync_shot_flag = 0u;

			// Limpa o tick_sync_shot para a pr�xima tacada(shot)
			pgi->tick_sync_shot.clear();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::clear_all_sync_shot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void Approach::requestCalculeRankPlace() {

	std::vector< approach_dados_ex > v_ad{};
	approach_dados_ex tmp{0u};
	PlayerApproachInfo *pai = nullptr;

	for (auto& el : m_player_info) {

		if (el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {	// menos os que quitaram

			pai = (PlayerApproachInfo*)el.second;

			tmp.clear();

			tmp.uid = pai->m_app_dados.uid;
			tmp.oid = pai->m_app_dados.oid;

			tmp.distance = pai->m_app_dados.total_distance;
			tmp.time = pai->m_app_dados.total_time;
			
			v_ad.push_back(tmp);
		}
	}

	std::sort(v_ad.begin(), v_ad.end(), Approach::sort_approach_rank_place);

	// Set positions
	unsigned char position = 1u;

	for (auto& el : v_ad) {
		
		try {

			INIT_PLAYER_INFO("requestCalculeRankPlace", "Tentou inicializar a position dos player", findSessionByUID(el.uid));
			
			pgi->m_app_dados.position = position++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::requestCalculeRankPlace][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	if (!v_ad.empty()) {
		v_ad.clear();
		v_ad.shrink_to_fit();
	}
}

void Approach::requestCalculeRankPlaceHole() {

	std::vector< approach_dados_ex > v_ad{};

	for (auto& el : m_player_info)
		if (el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT)	// menos os que quitaram
			v_ad.push_back(((PlayerApproachInfo*)el.second)->m_app_dados);

	std::sort(v_ad.begin(), v_ad.end(), Approach::sort_approach_rank_place);

	// Set positions
	unsigned char position = 1u;

	for (auto& el : v_ad) {
		
		try {

			INIT_PLAYER_INFO("requestCalculeRankPlaceHole", "Tentou inicializar a position dos player", findSessionByUID(el.uid));

			pgi->m_app_dados.position = position++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[Approach::requestCalculeRankPlaceHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	if (!v_ad.empty()) {
		v_ad.clear();
		v_ad.shrink_to_fit();
	}
}

void Approach::top_rank_win() {

	auto count = getCountPlayersGame();

	if (count >= 5 && count < 11) {

		auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
			return (_el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT && ((PlayerApproachInfo*)_el.second)->m_app_dados.position == 1);
		});

		if (it != m_player_info.end())
			((PlayerApproachInfo*)it->second)->m_app_dados.rank_box = 1u;

	}else if (count >= 11 && count < 18) {

		std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

			if (_el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

				auto pai = (PlayerApproachInfo*)_el.second;

				switch (pai->m_app_dados.position) {
				case 1:
					pai->m_app_dados.rank_box = 2;
					break;
				case 2:
					pai->m_app_dados.rank_box = 1;
					break;
				}
			}
		});

	}else if (count >= 18 && count < 26) {

		std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

			if (_el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

				auto pai = (PlayerApproachInfo*)_el.second;

				switch (pai->m_app_dados.position) {
				case 1:
					pai->m_app_dados.rank_box = 3;
					break;
				case 2:
					pai->m_app_dados.rank_box = 2;
					break;
				case 3:
					pai->m_app_dados.rank_box = 1;
					break;
				}
			}
		});

	}else if (count >= 26) {

		std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

			if (_el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

				auto pai = (PlayerApproachInfo*)_el.second;

				switch (pai->m_app_dados.position) {
				case 1:
					pai->m_app_dados.rank_box = 4;
					break;
				case 2:
					pai->m_app_dados.rank_box = 3;
					break;
				case 3:
					pai->m_app_dados.rank_box = 2;
					break;
				case 4:
					pai->m_app_dados.rank_box = 1;
					break;
				}
			}
		});
	}
}

void Approach::finishAllDadosApproach() {

	PlayerApproachInfo* pai = nullptr;

	for (auto& el : m_player_info) {

		pai = (PlayerApproachInfo*)el.second;

		if (pai != nullptr && pai->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

			pai->m_app_dados.box = pai->m_app_dados.total_box + pai->m_app_dados.rank_box;
			pai->m_app_dados.distance = pai->m_app_dados.total_distance;
			pai->m_app_dados.time = pai->m_app_dados.total_time;
			pai->m_app_dados.rank_box = 0u;
		}
	}
}

void Approach::sendScoreBoard() {

	packet p((unsigned short)0x150);

	uint32_t count = getCountPlayersGame();

	p.addUint8((unsigned char)count);

	for (auto& el : m_player_info)
		if (el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT || ((PlayerApproachInfo*)el.second)->m_app_dados.state_quit == approach_dados_ex::eSTATE_QUIT::SQ_QUIT_START)
			((PlayerApproachInfo*)el.second)->m_app_dados.toPacket(p);

	packet_func::game_broadcast(*this, p, 1);
}

void Approach::init_mission() {

	m_mission = sApproachMissionSystem::getInstance().drawMission((uint32_t)m_players.size());

	if (m_mission.is_player_uid) {
		
		auto index_p = m_mission.condition[1];

		m_mission.condition[1] = 0u;

		if (m_players[index_p] != nullptr && m_players[index_p]->getState()) {

			m_mission.condition[0] = m_players[index_p]->m_pi.uid;
			m_mission.nick = m_players[index_p]->m_pi.nickname;
		}
	}
}

void Approach::mission_win() {

	if (m_mission.numero > 0) {

		switch (m_mission.numero) {
		case 1: // Par ou �mpar, Primeiro lugar parar com dist�ncia Par ou �mpar(mission), todos ganham box
		{
			bool win = false;

			PlayerApproachInfo* pai = nullptr;

			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT 
					&& ((PlayerApproachInfo*)_el.second)->m_app_dados.state.ucState == 0u && ((PlayerApproachInfo*)_el.second)->m_app_dados.position == 1);
			});

			if (it != m_player_info.end()) {

				pai = (PlayerApproachInfo*)it->second;

				if (m_mission.condition[0] == 0) { // Par

					win = (pai->m_app_dados.distance % 2) == 0;

				}else { // Impar

					win = (pai->m_app_dados.distance % 2) != 0;
				}
			}

			if (win) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 3: // Quem fazer chip-in ganha box
			for (auto& el : m_player_info)
				if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
						&& ((PlayerApproachInfo*)el.second)->m_app_dados.state.stState.chip_in)
					((PlayerApproachInfo*)el.second)->m_app_dados.box = m_mission.box_qntd;
			break;
		case 6: // Mascot equiped, Primeiro lugar estiver com o mascot da mission, todos ganham box
		{
			bool win = false;

			PlayerApproachInfo* pai = nullptr;

			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT 
						&& ((PlayerApproachInfo*)_el.second)->m_app_dados.state.ucState == 0u && ((PlayerApproachInfo*)_el.second)->m_app_dados.position == 1);
			});

			if (it != m_player_info.end()) {

				pai = (PlayerApproachInfo*)it->second;

				switch (m_mission.condition[0]) {
				case 1:
					win = it->second->mascot_typeid == 0x40000002u; // Cocoa
					break;
				case 2:
					win = it->second->mascot_typeid == 0x40000001u; // Puff
					break;
				case 3:
					win = it->second->mascot_typeid == 0x40000003u; // Bily
					break;
				case 4:
					win = it->second->mascot_typeid == 0x40000000u; // Lemmy
					break;
				}
			}

			if (win) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 7: // Rank, O player que ficar no rank da mission ele ganha box, independente se ele fez OUT
		{
			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT 
					&& ((PlayerApproachInfo*)_el.second)->m_app_dados.position == m_mission.condition[0]);
			});

			if (it != m_player_info.end())
				((PlayerApproachInfo*)it->second)->m_app_dados.box = m_mission.box_qntd;

			break;
		}
		case 10: // Character Equiped, Todos que estiverem com o character da mission e pararem coma dist�ncia menor que 10y eles ganham box
		{

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					auto pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.ucState == 0u && pai->m_app_dados.distance < 100/*10y*/
							&& _el.first->m_pi.ei.char_info != nullptr && m_mission.condition[0] == ((player*)_el.first)->m_pi.ei.char_info->_typeid)
						pai->m_app_dados.box = m_mission.box_qntd;
				}
			});

			break;
		}
		case 11: // Mais (Par ou �mpar), Se tiver dist�ncias dos players mais (Par ou �mpar)(mission), todos ganham box
		{
			bool win = false;

			uint32_t count_par = 0u, count_impar = 0u;

			PlayerApproachInfo* pai = nullptr;

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.ucState == 0u) {

						if ((pai->m_app_dados.distance % 2) == 0)
							count_par++;
						else
							count_impar++;
					}
				}

			});

			if (m_mission.condition[0] == 0) { // Mais par

				win = count_par > count_impar;

			}else { // Mais Impar

				win = count_impar > count_par;
			}

			if (win) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 12: // Chip-in, se a maioria dos players fizer chip-in, todos que chiparam e n�o fizeram OUT(timeout, ob) ganham box
		{

			PlayerApproachInfo* pai = nullptr;

			uint32_t count_chip = 0u, count_p = getCountPlayersGame();

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.stState.chip_in)
						count_chip++;
				}
			});

			if (count_chip > (count_p / 2)) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& ((pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u || pai->m_app_dados.state.stState.chip_in))
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 14: // Menor ou igual a dist�ncia(mission) o player ganha box
		{
			PlayerApproachInfo* pai = nullptr;

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.ucState == 0u && pai->m_app_dados.distance <= (m_mission.condition[0] * 10))
						pai->m_app_dados.box = m_mission.box_qntd;
				}
			});

			break;
		}
		case 15: // Rank dist�ncia Par ou �mpar, se o player no rank(mission) a dist�ncia for (Par ou �mpar)(mission), todos que n�o deram OUT ganham box
		{
			bool win = false;

			PlayerApproachInfo* pai = nullptr;

			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT 
						&& ((PlayerApproachInfo*)_el.second)->m_app_dados.state.ucState == 0u && ((PlayerApproachInfo*)_el.second)->m_app_dados.position == m_mission.condition[0]);
			});

			if (it != m_player_info.end()) {

				pai = (PlayerApproachInfo*)it->second;

				if (m_mission.condition[1] == 0) { // Par

					win = (pai->m_app_dados.distance % 2) == 0;

				}else { // Impar

					win = (pai->m_app_dados.distance % 2) != 0;
				}
			}

			if (win) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 16: // Chip-in, Se 4 ou mais player fazer chip-in, todos que fizeram chip-in e n�o deram OUT(timeout, ob) ganham box
		{
			PlayerApproachInfo* pai = nullptr;

			uint32_t count_chip = 0u;

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.stState.chip_in)
						count_chip++;
				}
			});

			if (count_chip >= 4) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& ((pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u || pai->m_app_dados.state.stState.chip_in))
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 17: // Total dist�ncia de todos players n�o pode passar da dist�ncia da mission, que todos que n�o fizeram OUT ganham box
		{
			uint32_t total_dist = 0u;

			PlayerApproachInfo* pai = nullptr;

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					pai = (PlayerApproachInfo*)_el.second;

					// Quem fez OUT conta a dist�ncia tamb�m
					total_dist += pai->m_app_dados.distance;
				}
			});

			if (total_dist < (m_mission.condition[1] * 10)) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 18: // Se um player ficar na dist�ncia exata da mission, todos que n�o fizeram OUT ganham box
		{
			PlayerApproachInfo* pai = nullptr;

			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
					&& ((PlayerApproachInfo*)_el.second)->m_app_dados.state.ucState == 0u && ((PlayerApproachInfo*)_el.second)->m_app_dados.distance == ((m_mission.condition[0] * 10) + m_mission.condition[1]));
			});

			if (it != m_player_info.end()) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 20: // Player com a maior dist�ncia que n�o fez OUT ganha box
		{

			auto it_win = m_player_info.end();

			for (auto it = m_player_info.begin(); it != m_player_info.end(); ++it) {

				if ((it->second->flag != PlayerGameInfo::eFLAG_GAME::QUIT && ((PlayerApproachInfo*)it->second)->m_app_dados.state.ucState == 0u) 
						&& (it_win == m_player_info.end() || ((PlayerApproachInfo*)it->second)->m_app_dados.distance > ((PlayerApproachInfo*)it_win->second)->m_app_dados.distance))
					it_win = it;
			}

			if (it_win != m_player_info.end())
				((PlayerApproachInfo*)it_win->second)->m_app_dados.box = m_mission.box_qntd;

			break;
		}
		case 22: // Se o tempo do player em primeiro lugar for menor ou igual a 10s(10.000 milliseconds), todos que n�o fizeram OUT ganham box
		{
			PlayerApproachInfo* pai = nullptr;

			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
					&& ((PlayerApproachInfo*)_el.second)->m_app_dados.state.ucState == 0u && ((PlayerApproachInfo*)_el.second)->m_app_dados.position == 1 && ((PlayerApproachInfo*)_el.second)->m_app_dados.time <= 10000);
			});

			if (it != m_player_info.end()) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& (pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u)
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}
		case 23: // Caddie equiped, o Player que tiver o caddie da mission equipado e ficar com a dist�ncia menor que 20y ganha box
		{
			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					auto pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.ucState == 0u && pai->m_app_dados.distance < 200/*20y*/ && _el.first->m_pi.ei.cad_info != nullptr) {

						switch (m_mission.condition[0]) {
						case 0: // Papel
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000000u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 1: // Pippin
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000001u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000010u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 2: // Titan Boo
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000002u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000011u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 3: // Dolphini
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000003u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000012u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 4: // Lolo
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000004u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000013u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 5: // Kuma
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000005u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000014u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 6: // Tiki
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000006u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000015u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						case 7: // Cadie
							if (_el.first->m_pi.ei.cad_info->_typeid == 0x1C000007u || _el.first->m_pi.ei.cad_info->_typeid == 0x1C000016u)
								pai->m_app_dados.box = m_mission.box_qntd;
							break;
						}
					}
				}
			});

			break;
		}
		case 24: // Primeiro player a tacar e que n�o deu OUT ganha box
		{
			auto it_win = m_player_info.end();

			for (auto it = m_player_info.begin(); it != m_player_info.end(); ++it) {

				if ((it->second->flag != PlayerGameInfo::eFLAG_GAME::QUIT && ((PlayerApproachInfo*)it->second)->m_app_dados.state.ucState == 0u)
						&& (it_win == m_player_info.end() || ((PlayerApproachInfo*)it->second)->m_app_dados.time > ((PlayerApproachInfo*)it_win->second)->m_app_dados.time))
					it_win = it;
			}

			if (it_win != m_player_info.end())
				((PlayerApproachInfo*)it_win->second)->m_app_dados.box = m_mission.box_qntd;

			break;
		}
		case 25: // Par ou �mpar, quem ficar com a dist�ncia (Par ou �mpar)(mission) e n�o fizer OUT ganha box
		{
			PlayerApproachInfo* pai = nullptr;

			std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {

				if (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					pai = (PlayerApproachInfo*)_el.second;

					if (pai->m_app_dados.state.ucState == 0u) {

						if (m_mission.condition[0] == 0) { // Par

							if ((pai->m_app_dados.distance % 2) == 0)
								pai->m_app_dados.box = m_mission.box_qntd;

						}else { // Impar

							if ((pai->m_app_dados.distance % 2) != 0)
								pai->m_app_dados.box = m_mission.box_qntd;
						}
					}
				}
			});

			break;
		}
		case 26: // Ultimo player a tacar e que n�o deu OUT ganha box
		{
			auto it_win = m_player_info.end();

			for (auto it = m_player_info.begin(); it != m_player_info.end(); ++it) {

				if ((it->second->flag != PlayerGameInfo::eFLAG_GAME::QUIT && ((PlayerApproachInfo*)it->second)->m_app_dados.state.ucState == 0u)
						&& (it_win == m_player_info.end() || ((PlayerApproachInfo*)it->second)->m_app_dados.time < ((PlayerApproachInfo*)it_win->second)->m_app_dados.time))
					it_win = it;
			}

			if (it_win != m_player_info.end())
				((PlayerApproachInfo*)it_win->second)->m_app_dados.box = m_mission.box_qntd;

			break;
		}
		case 29: // Player(mission) que fazer chip-in, todos que n�o fizeram OUT ganha box e o player que fez chip-in ganha box
		{

			PlayerApproachInfo* pai = nullptr;

			auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
				return (_el.second != nullptr && _el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
					&& ((PlayerApproachInfo*)_el.second)->m_app_dados.uid == m_mission.condition[0] && ((PlayerApproachInfo*)_el.second)->m_app_dados.state.stState.chip_in);
			});

			if (it != m_player_info.end()) {

				for (auto& el : m_player_info)
					if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT
							&& ((pai = (PlayerApproachInfo*)el.second)->m_app_dados.state.ucState == 0u || pai->m_app_dados.uid == m_mission.condition[0]))
						pai->m_app_dados.box = m_mission.box_qntd;
			}

			break;
		}	// End Case 29

		}	// End Switch
	}
}

void Approach::delete_all_quiter() {

	PlayerApproachInfo* pai = nullptr;

	for (auto& el : m_player_info) {

		pai = (PlayerApproachInfo*)el.second;

		if (pai != nullptr && pai->m_app_dados.state_quit == approach_dados_ex::eSTATE_QUIT::SQ_QUIT_START)
			pai->m_app_dados.state_quit = approach_dados_ex::eSTATE_QUIT::SQ_QUIT_ENDED;
	}
}

bool Approach::finish_game(player& _session, int option) {
	
	if (
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.getState() && _session.isConnected() && m_players.size() > 0) {

		packet p;

		if (option == 0xCB/*packetCB pacote que termina o Approach*/) {

			if (m_approach_state)
				finish_approach(_session, 1);	// Termina sem ter acabado de jogar

			INIT_PLAYER_INFO("finish_game", "tentou terminar o jogo", &_session);

			// Update Info Map Statistics
			sendUpdateInfoAndMapStatistics(_session, 0);

			// Update Mascot Info ON GAME, se o player estiver com um mascot equipado
			if (_session.m_pi.ei.mascot_info != nullptr) {
				packet_func::pacote06B(p, &_session, &_session.m_pi, 8);

				packet_func::session_send(p, &_session, 1);
			}

			// Resposta que tem sempre que acaba um jogo, n�o sei o que � ainda, esse s� n�o tem no HIO Event
			p.init_plain((unsigned short)0x244);

			p.addUint32(0);	// OK

			packet_func::session_send(p, &_session, 1);

			// Esse � novo do JP, tem Tourney, VS, Grand Prix, HIO Event, n�o vi talvez tenha nos outros tamb�m
			p.init_plain((unsigned short)0x24F);

			p.addUint32(0);	// OK

			packet_func::session_send(p, &_session, 1);

			// Resposta Update Pang
			p.init_plain((unsigned short)0xC8);

			p.addUint64(_session.m_pi.ui.pang);

			p.addUint64(0ull);

			packet_func::session_send(p, &_session, 1);

			// Colocar o finish_game Para 1 quer dizer que ele acabou o camp
			pgi->finish_game = 1;

			// Flag do game que terminou
			m_game_init_state = 2;	// ACABOU

		}
	}

	return (PlayersCompleteGameAndClear() && m_approach_state);
}
