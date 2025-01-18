// Arquivo tourney_base.cpp
// Criado em 18/08/2018 as 15:14 por Acrisio
// Implementa��o da classe TourneyBase

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "tourney_base.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PACKET/packet_func_sv.h"

#include "item_manager.h"

#include "../Game Server/game_server.h"

#include "treasure_hunter_system.hpp"
#include "../UTIL/map.hpp"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include "coin_cube_location_update_system.hpp"

#include "../TYPE/_3d_type.hpp"

#if defined(__linux__)
#include <numbers> // pi
#endif

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[TourneyBase" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[TourneyBase::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[TourneyBase::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 1, 4)); \

using namespace stdA;

TourneyBase::TourneyBase(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: Game(_players, _ri, _rv, _channel_rookie), m_tri{0}, m_max_player(255u), m_entra_depois_flag(-1) {

	for (auto i = 0u; i < (sizeof(m_medal) / sizeof(m_medal[0])); ++i)
		m_medal[i].clear();
}

TourneyBase::~TourneyBase() {
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[TourneyBase::~TourneyBase][Log] TourneyBase destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

void TourneyBase::sendInitialData(player& _session) {

	packet p;

	try {

#if defined(_WIN32)
		if (InterlockedIncrement(&m_sync_send_init_data) == m_players.size()) {
#elif defined(__linux__)
		if (__atomic_add_fetch(&m_sync_send_init_data, 1u, __ATOMIC_RELAXED) == m_players.size()) {
#endif

			// Zera Variavel Volatile(atomic)
#if defined(_WIN32)
			InterlockedExchange(&m_sync_send_init_data, 0u);
#elif defined(__linux__)
			__atomic_store_n(&m_sync_send_init_data, 0u, __ATOMIC_RELAXED);
#endif

			// Game Data Init
			p.init_plain((unsigned short)0x76);

			p.addUint8(m_ri.tipo_show);
			p.addUint32(1);

			p.addBuffer(&m_start_time, sizeof(m_start_time));

			//packet_func::session_send(p, &_session, 1);
			packet_func::game_broadcast(*this, p, 1);

			// Course
			// Send Individual Packet to all players in game
			for (auto& el : m_players)
				Game::sendInitialData(*el);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::sendInitialData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

}

void TourneyBase::sendInitialDataAfter(player& _session) {
	
	packet p;

	try {

		// Send Initial Data of Game
		p.init_plain((unsigned short)0x113);

		p.addUint8(4);
		p.addUint32(3);

		p.addBuffer(&m_start_time, sizeof(m_start_time));

		packet_func::session_send(p, &_session, 1);

		// Course
		p.init_plain((unsigned short)0x113);

		p.addUint8(4);
		p.addUint8(4);

		p.addUint8(m_ri.course);
		p.addUint8(m_ri.tipo_show);
		p.addUint8(m_ri.modo);
		p.addUint8(m_ri.qntd_hole);
		p.addUint32(m_ri.trofel);
		p.addUint32(m_ri.time_vs);
		p.addUint32(m_ri.time_30s);

		// Hole Info, Hole Spinning Cube, end Seed Random Course
		m_course->makePacketHoleInfo(p, 1);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::sendInitialDataBefore][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestInitHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitHole");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stInitHole {
		void clear() { memset(this, 0, sizeof(stInitHole)); };
		unsigned char numero;
		uint32_t option;
		uint32_t ulUnknown;
		unsigned char par;
		stXZLocation tee;
		stXZLocation pin;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stInitHole ctx_hole{ 0 };

		_packet->readBuffer(&ctx_hole, sizeof(ctx_hole));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestInitHole][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] Hole[NUMERO=" + std::to_string((unsigned short)ctx_hole.numero) + ", PAR=" + std::to_string((unsigned short)ctx_hole.par) 
				+ " OPT=" + std::to_string(ctx_hole.option) + ", UNKNOWN=" + std::to_string(ctx_hole.ulUnknown) 
				+ "] Tee[X=" + std::to_string(ctx_hole.tee.x) + ", Z=" + std::to_string(ctx_hole.tee.z)
				+ "] Pin[X=" + std::to_string(ctx_hole.pin.x) + ", Z=" + std::to_string(ctx_hole.pin.z) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG


		auto hole = m_course->findHole(ctx_hole.numero);

		if (hole == nullptr)
			throw exception("[TourneyBase::requestInitHole][Error] course->findHole nao encontrou o hole retonou nullptr, o server esta com erro no init course do tourney_base.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 2555, 0));

		hole->init(ctx_hole.tee, ctx_hole.pin);

		INIT_PLAYER_INFO("requestInitHole", "tentou inicializar o hole[NUMERO = " + std::to_string(hole->getNumero()) + "] no jogo", &_session);

		// Update Location Player in Hole
		pgi->location.x = ctx_hole.tee.x;
		pgi->location.z = ctx_hole.tee.z;
		
		// Número do hole atual, que o player está jogandp
		pgi->hole = ctx_hole.numero;

		// Flag que marca se o player já inicializou o primeiro hole do jogo
		if (!pgi->init_first_hole)
			pgi->init_first_hole = 1u;

		// Gera degree para o player ou pega o degree sem gerar que é do modo do hole repeat
		pgi->degree = (m_ri.modo == Hole::M_REPEAT) ? hole->getWind().degree.getDegree() : hole->getWind().degree.getShuffleDegree();

		// Resposta de tempo do hole
		p.init_plain((unsigned short)0x9E);

		p.addUint16(hole->getWeather());
		p.addUint8(0);	// Option do tempo, sempre peguei zero aqui dos pacotes que vi

		packet_func::session_send(p, &_session, 1);

		auto wind_flag = initCardWindPlayer(pgi, hole->getWind().wind);

		// Resposta do vento do hole
		p.init_plain((unsigned short)0x5B);

		p.addUint8(hole->getWind().wind + wind_flag);
		p.addUint8((wind_flag < 0) ? 1 : 0);	// Flag de card de vento, aqui é a qnd diminui o vento, 1 Vento azul
		p.addUint16(pgi->degree);
		p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original

		packet_func::session_send(p, &_session, 1);

		// envia tempo decorrido do Tourney
		// Resposta tempo percorrido do Tourney
		sendRemainTime(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestInitHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		
	}
}

bool TourneyBase::requestFinishLoadHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishLoadHole");

	packet p;

	// Esse aqui é para Trocar Info da Sala
	// para colocar a sala no modo que pode entrar depois de ter começado
	bool ret = false;

	try {

		INIT_PLAYER_INFO("requestFinishLoadHole", "tentou finalizar carregamento do hole no jogo", &_session);

		pgi->finish_load_hole = 1;

		if (pgi->enter_after_started) {
			// Add Player Score
			p.init_plain((unsigned short)0x113);

			p.addUint8(9);
			p.addUint8(0);

			p.addUint32(_session.m_oid);
			p.addUint32((uint32_t)m_players.size());

			packet_func::game_broadcast(*this, p, 1);
		}

		// Resposta passa o oid do player que vai começa o Hole
		p.init_plain((unsigned short)0x53);

		p.addUint32(_session.m_oid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void TourneyBase::requestFinishCharIntro(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishCharIntro");

	packet p;

	try {

		INIT_PLAYER_INFO("requestFinishCharIntro", "tentou finalizar intro do char no jogo", &_session);

		pgi->finish_char_intro = 1;

		// Zera todas as tacada num dos players se for camp normal se for short game coloca o n�mero de tacadas inicial
		if (m_ri.natural.stBit.short_game/* & 2*/) {	// Short Game
			
			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[TourneyBase::requestFinishCharIntro][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar intro do char, mas nao conseguiu encontrar o hole[NUMERO=" 
						+ std::to_string(pgi->hole) + "] no course do jogo. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 30, 0));
			
			switch (hole->getPar().par) {
			case 5:
				pgi->data.tacada_num = 2u;
				break;
			case 4:
				pgi->data.tacada_num = 1u;
				break;
			case 3:
			default:
				pgi->data.tacada_num = 0u;
				break;
			}

		}else
			pgi->data.tacada_num = 0u;
		
		// Giveup Flag
		pgi->data.giveup = 0u;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestFinishHoleData(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishHoleData");

	try {

		UserInfoEx ui{ 0 };

		_packet->readBuffer(&ui, sizeof(UserInfo));

		// aqui o cliente passa mad_conduta com o hole_in, trocados, mad_conduto <-> hole_in

		INIT_PLAYER_INFO("requestFinishHoleData", "tentou finalizar hole dados no jogo", &_session);

		pgi->ui = ui;

		if (!pgi->shot_sync.state_shot.display.stDisplay.acerto_hole) {	// Terminou o Hole sem acerta ele, Give Up

			// Ainda não colocara o give up, o outro pacote, coloca nesse(muito difícil, n�o colocar só se estiver com bug)
			if (!pgi->data.giveup) {
				pgi->data.giveup = 1;

				// Incrementa o Bad Condute
				pgi->data.bad_condute++;
			}
		}

		// Aqui Salva os dados do Pgi, os best Chipin, Long putt e best drive(max distância)
		// Não sei se precisa de salvar aqui, já que estou salvando no pgi User Info
		pgi->progress.best_chipin = ui.best_chip_in;
		pgi->progress.best_long_puttin = ui.best_long_putt;
		pgi->progress.best_drive = ui.best_drive;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestFinishHoleData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestInitShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShot");

	//packet p;

	try {

		ShotDataEx sd{ 0 };

		sd.option = _packet->readUint16();

		// Power Shot
		if (sd.option == 1)
			_packet->readBuffer(&sd.power_shot, sizeof(sd.power_shot));

		_packet->readBuffer(&sd, sizeof(ShotData));

		INIT_PLAYER_INFO("requestInitShot", "tentou iniciar tacada no jogo", &_session);

		pgi->shot_data = sd;

#ifdef _DEBUG
		// Log Shot Data Ex
		_smp::message_pool::getInstance().push(new message("Log Shot Data Ex:\n\r" + sd.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Aqui não manda resposta no TourneyBase ou Practice, mas outro modos(VS, MATCH) manda e outros também não(TOURNEY)

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestInitShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestSyncShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("SyncShot");

	packet p;

	try {

		ShotSyncData ssd{ 0 };

		// game read request sync shot
		Game::requestReadSyncShotData(_session, _packet, ssd);

		// Request Calcule Shot Spinning Cube
		requestCalculeShotSpinningCube(_session, ssd); // esse não precisa verificar o usuário, por que em tourney só o próprio player que envia

		// Request Calcule Shot Coin
		requestCalculeShotCoin(_session, ssd); // esse não precisa verificar o usuário, por que em tourney só o próprio player que envia

		requestTranslateSyncShotData(_session, ssd);

		requestReplySyncShotData(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestSyncShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestInitShotArrowSeq(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShotArrowSeq");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	union uArrow {
		void clear() { memset(this, 0, sizeof(uArrow)); };
		uint32_t ulArrow;
		struct _stArrow {
			unsigned char cima : 1;
			unsigned char baixo : 1;
			unsigned char esquerda : 1;
			unsigned char direita : 1;
			unsigned char azul_claro : 1;		// Seta Ativada, o Power Spin ou Power Curve
			unsigned char _bit6_a_13 : 8;
			unsigned char _bit14_a_21 : 8;
			unsigned char _bit22_a_29 : 8, : 0;
			//unsigned char _bit30_a_32 : 3;
		}stArrow;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		unsigned char count_seta = _packet->readUint8();

		if (count_seta == 0)
			throw exception("[TourneyBase::requestInitShotArrowSeq][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializar as sequencia de setas, mas nao enviou nenhuma seta. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 5, 0));

		std::vector< uArrow > setas;

		for (auto i = 0u; i < count_seta; ++i)
			setas.push_back({ _packet->readUint32() });

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestInitShotArrowSeq][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestShotEndData(player& _session, packet *_packet) {
	REQUEST_BEGIN("requestShotEndData");

	packet p;

	try {

		// ----------------- LEMBRETE --------------
		// Aqui vou usar para as tacadas do spinning cube que gera no course
		// --- Já estou usando o pacote no sync, por que preciso verificar uns valores lá ---

		ShotEndLocationData seld{ 0 };

		_packet->readBuffer(&seld, sizeof(seld));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Log Shot End Location Data:\n\r" + seld.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		INIT_PLAYER_INFO("requestShotEndData", "tentou finalizar local da tacada no jogo", &_session);

		pgi->shot_data_for_cube = seld;

		// Resposta para Shot End Data
		p.init_plain((unsigned short)0x1F7);

		p.addUint32(pgi->oid);
		p.addUint8(pgi->hole);
		
		p.addBuffer(&seld, sizeof(seld));

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestShotEndData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

RetFinishShot TourneyBase::requestFinishShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishShot");

	RetFinishShot ret{ 0 };

	try {

#if defined(_WIN32)
		LARGE_INTEGER frequency, tick, end_tick;

		QueryPerformanceFrequency(&frequency);

		QueryPerformanceCounter(&tick);
#elif defined(__linux__)
		timespec frequency, tick, end_tick;

		clock_getres(CLOCK_MONOTONIC_RAW, &frequency);

		clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
#endif

		// Request Init Cube Coin
		auto cube = requestInitCubeCoin(_session, _packet);

		// Resposta para Finish Shot
		sendEndShot(_session, cube);

		ret.ret = checkEndShotOfHole(_session);

		if (ret.ret == 2)
			ret.p = &_session;

		INIT_PLAYER_INFO("requestFinishShot", "tentou finalizar a tacada", &_session);

		// Limpa dados que usa para cada tacada
		clearDataEndShot(pgi);

#if defined(_WIN32)
		QueryPerformanceCounter(&end_tick);
#elif defined(__linux__)
		clock_gettime(CLOCK_MONOTONIC_RAW, &end_tick);
#endif

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestFinishShot][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] finalizar tacada, o Server demorou "
				+ std::to_string(
#if defined(_WIN32)
					(end_tick.QuadPart - tick.QuadPart) * 1000000 / frequency.QuadPart
#elif defined(__linux__)
					DIFF_TICK_MICRO(end_tick, tick, frequency)
#endif
				) + " micro-segundos", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestFinishShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void TourneyBase::requestChangeMira(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeMira");

	packet p;

	try {

		float mira = _packet->readFloat();

		INIT_PLAYER_INFO("requestChangeMira", "tentou mudar a mira[MIRA=" + std::to_string(mira) + "] no jogo", &_session);

		pgi->location.r = mira;

		// Resposta para o Change mira
		p.init_plain((unsigned short)0x56);

		p.addUint32(pgi->oid);
		p.addFloat(pgi->location.r);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestChangeMira][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestChangeStateBarSpace(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateBarSpace");

	packet p;

	try {

		unsigned char state = _packet->readUint8();
		float point = _packet->readFloat();

		INIT_PLAYER_INFO("requestChangeStateBarSpace", "tentou mudar o estado[STATE=" + std::to_string((unsigned short)state) + ", POINT=" + std::to_string(point) + "] da barra de espaco no jogo", &_session);

		if (!pgi->bar_space.setStateAndPoint(state, point))
			throw exception("[TourneyBase::requestChangeStateBarSpace][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mudar o estado da barra de espaco[STATE=" 
					+ std::to_string((unsigned short)state) + " POINT=" + std::to_string(point) + "] no jogo, mas o estado eh desconhecido, Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 5, 0));

		if (state == 3) {
			pgi->bar_space.setState(0);	// Volta para 1 depois que taca, era esse meu comentário no antigo

			pgi->tempo = 0;	// Reseta o tempo
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestChangeStateBarSpace][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActivePowerShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActivePowerShot");

	packet p;

	try {
		
		unsigned char ps = _packet->readUint8();

		INIT_PLAYER_INFO("requestActivePowerShot", "tentou ativar power shot, no jogo", &_session);

		pgi->power_shot = ps;

		// Resposta para Active Power Shot
		p.init_plain((unsigned short)0x58);

		p.addUint32(_session.m_oid);
		p.addUint8(pgi->power_shot);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActivePowerShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestChangeClub(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeClub");

	packet p;

	try {

		unsigned char club = _packet->readUint8();

		INIT_PLAYER_INFO("requestChangeClub", "tentou trocar taco no jogo", &_session);

		pgi->club = club;

		// Resposta para Change Club
		p.init_plain((unsigned short)0x59);

		p.addUint32(_session.m_oid);
		p.addUint8(pgi->club);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestChangeClub][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestUseActiveItem(player& _session, packet *_packet) {
	REQUEST_BEGIN("UseActiveItem");

	packet p;

	try {

		uint32_t item_typeid = _packet->readUint32();

		INIT_PLAYER_INFO("requestUseActiveItem", "tentou usar item ativo no jogo", &_session);

		if (item_typeid == 0)
			throw exception("[TourneyBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID=" 
					+ std::to_string(item_typeid) + "] no jogo, mas o item_typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 7, 0));

		auto iffItem = sIff::getInstance().findCommomItem(item_typeid);
		
		if (iffItem == nullptr)
			throw exception("[TourneyBase::requestActiveItem][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + " tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas o item nao tem no IFF_STRUCT. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 77, 0));
		
		if (sIff::getInstance().getItemGroupIdentify(item_typeid) != iff::ITEM || !sIff::getInstance().IsItemEquipable(item_typeid))
			throw exception("[TourneyBase::requestActiveItem][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas o item nao eh equipavel(usar). Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 78, 0));
		
		// Verifica se tem algum card de tempo equipado com efeito de mulligan rose
		if (item_typeid == MULLIGAN_ROSE_TYPEID) {
			
			// Card Special - Efeito mulligan rose == 32
			if (std::count_if(_session.m_pi.v_cei.begin(), _session.m_pi.v_cei.end(), [](auto& _el) {
				return (_el.parts_id == 0 && _el.parts_typeid == 0 && sIff::getInstance().getItemSubGroupIdentify22(_el._typeid) == 2/*Special*/ && _el.efeito == 32/*Mulligan Rose*/);
			}) > 0) {
				
				// Resposta para o Use Active Item
				p.init_plain((unsigned short)0x5A);
				
				p.addUint32(item_typeid);
				p.addUint32((uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chrono());				// Seed Rand Failure Active Item
				p.addUint32(_session.m_oid);
				
				packet_func::game_broadcast(*this, p, 1);
				
				// Sai
				return;
				
			}
			
		}
		
		// Verifica se o player tem o item para usar
		auto pWi = _session.m_pi.findWarehouseItemByTypeid(item_typeid);

		if (pWi == nullptr)
			throw exception("[TourneyBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas ele nao tem esse item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 8, 0));

		auto it = pgi->used_item.v_active.find(pWi->_typeid);

		if (it == pgi->used_item.v_active.end())
			throw exception("[TourneyBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas ele nao equipou esse item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 9, 0));

		if (it->second.count >= it->second.v_slot.size())
			throw exception("[TourneyBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas ele ja usou todos os item desse que ele equipou. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 10, 0));

		// Add +1 ou countador
		it->second.count++;

		// item que foi usado na tacada
		pgi->item_active_used_shot = pWi->_typeid;

		// Resposta para o Use Active Item
		p.init_plain((unsigned short)0x5A);

		p.addUint32(pWi->_typeid);
		p.addUint32((uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chrono());				// Seed Rand Failure Active Item
		p.addUint32(_session.m_oid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestUseActiveItem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestChangeStateTypeing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateTypeing");

	packet p;

	try {

		short typeing = _packet->readInt16();

		INIT_PLAYER_INFO("requestChangeStateTypeing", "tentou mudar o estado de escrevendo no jogo", &_session);

		pgi->typeing = typeing;

		// Resposta para Change State Typeing /*Escrevendo*/
		p.init_plain((unsigned short)0x5D);

		p.addUint32(_session.m_oid);
		p.addInt16(pgi->typeing);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestChangeStateTypeing][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestMoveBall(player& _session, packet *_packet) {
	REQUEST_BEGIN("MoveBall");

	packet p;

	try {

		float x = _packet->readFloat();
		float y = _packet->readFloat();
		float z = _packet->readFloat();

		INIT_PLAYER_INFO("requestMoveBall", "tentou recolocar a bola no jogo", &_session);

		pgi->location.x = x;
		pgi->location.y = y;
		pgi->location.z = z;

		// Add + 1 a tacada, já que ele recolocou em vez de tacar
		pgi->data.tacada_num++;

		// Resposta para Move Ball
		p.init_plain((unsigned short)0x60);

		p.addFloat(pgi->location.x);
		p.addFloat(pgi->location.y);
		p.addFloat(pgi->location.z);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestMoveBall][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestChangeStateChatBlock(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateChatBlock");

	packet p;

	try {

		unsigned char chat_block = _packet->readUint8();

		INIT_PLAYER_INFO("requestChangeStateChatBlock", "tentou mudar estado do chat block no jogo", &_session);

		pgi->chat_block = chat_block;

		// Resposta para Chat Block
		p.init_plain((unsigned short)0xAC);

		p.addUint32(_session.m_oid);
		p.addUint8(pgi->chat_block);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestChangeStateChatBlock][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveBooster(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveBooster");

	packet p;

	try {

		float velocidade = _packet->readFloat();

		INIT_PLAYER_INFO("requestActiveBooster", "tentou ativar Time Booster no jogo", &_session);

		if ((_session.m_pi.m_cap.stBit.premium_user/* & (1 << 14)/*0x00004000/*PREMIUM USER*/) == 0) { // (não é)!PREMIUM USER

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(TIME_BOOSTER_TYPEID);

			if (pWi == nullptr)
				throw exception("[TourneyBase::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele nao tem o item passive. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 11, 0));

			if (pWi->STDA_C_ITEM_QNTD <= 0)
				throw exception("[TourneyBase::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele nao tem quantidade suficiente[VALUE=" 
						+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", REQUEST=1] do item de time booster.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 12, 0));

			auto it = pgi->used_item.v_passive.find(pWi->_typeid);

			if (it == pgi->used_item.v_passive.end())
				throw exception("[TourneyBase::requestActiveBooster][Error] player[UID = " + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele nao tem ele no item passive usados do server. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 13, 0));

			if ((short)it->second.count >= pWi->STDA_C_ITEM_QNTD)
				throw exception("[TourneyBase::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele ja usou todos os time booster. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 14, 0));

			// Add +1 ao item passive usado
			it->second.count++;

		}else { // Soma +1 no contador de counter item do booster do player e passive item

			pgi->sys_achieve.incrementCounter(0x6C400075u/*Passive Item*/);

			pgi->sys_achieve.incrementCounter(0x6C400050u);
		}

		// Resposta para Active Booster
		p.init_plain((unsigned short)0xC7);

		p.addFloat(velocidade);
		p.addUint32(_session.m_oid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveBooster][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveReplay(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveReplay");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[TourneyBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID=" 
					+ std::to_string(_typeid) + "], mas o typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 200, 0));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[TourneyBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem o item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 201, 0));

		if (pWi->STDA_C_ITEM_QNTD <= 0)
			throw exception("[TourneyBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem quantidade suficiente[VALUE=" + std::to_string(pWi->STDA_C_ITEM_QNTD) + ", REQUEST=1] do item. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 202, 0));

		// UPDATE ON SERVER AND DB
		stItem item{ 0 };

		item.type = 2;
		item._typeid = pWi->_typeid;
		item.id = pWi->id;
		item.qntd = 1;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		if (item_manager::removeItem(item, _session) <= 0)
			throw exception("[TourneyBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID="
					+ std::to_string(_typeid) + "], nao conseguiu deletar ou atualizar qntd do item[TYPEID=" + std::to_string(item._typeid) + ", ID=" + std::to_string(item.id) + "]", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 203, 0));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveReplay][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] ativou replay e ficou com " 
				+ std::to_string(item.stat.qntd_dep) + " + fita(s)", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// UPDATE ON GAME
		// Resposta para o Active Replay
		p.init_plain((unsigned short)0xA4);

		p.addUint16((unsigned short)item.stat.qntd_dep);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveReplay][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveCutin(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveCutin");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stActiveCutin {
		void clear() { memset(this, 0, sizeof(stActiveCutin)); };
		uint32_t uid;
		uint32_t tipo;
		unsigned short opt;
		uint32_t char_typeid;	// Aqui pode ter o typeid do cutin também
		unsigned char active;		// Active acho, sempre com valor 1 que peguei, 1 quando é id do character, 0 quando é o typeid do Cutin
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stActiveCutin ac{ 0 };

		_packet->readBuffer(&ac, sizeof(ac));

		player* s = nullptr;

		if (ac.uid != _session.m_pi.uid || (s = findSessionByUID(ac.uid)) == nullptr)
			throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID=" 
					+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
					+ std::to_string(ac.uid) + "], mas o jogador nao esta no jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 1, 0x5200101));

		if (s->m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
					+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
					+ std::to_string(ac.uid) + "], mas o jogador nao tem um character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 2, 0x5200102));

		IFF::CutinInfomation *pCutin = nullptr;

		// Cutin Padrão que o player equipa, quando o cliente envia o cutin type é que é efeito por roupas equipadas
		if (sIff::getInstance().getItemGroupIdentify(ac.char_typeid) == iff::CHARACTER && ac.active) {

			if (s->m_pi.ei.char_info->_typeid != ac.char_typeid)
				throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
						+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
						+ std::to_string(ac.uid) + "], mas o character typeid passado nao eh igual ao equipado do player. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 4, 0x5200104));

			WarehouseItemEx *pWi = nullptr;

			auto end = (sizeof(s->m_pi.ei.char_info->cut_in) / sizeof(s->m_pi.ei.char_info->cut_in[0]));

			for (auto i = 0u; i < end; ++i) {

				if (s->m_pi.ei.char_info->cut_in[i] > 0) {

					if ((pWi = _session.m_pi.findWarehouseItemById(s->m_pi.ei.char_info->cut_in[i])) != nullptr) {

						if ((pCutin = sIff::getInstance().findCutinInfomation(pWi->_typeid)) == nullptr)
							throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
									+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ", ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
									+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
									+ std::to_string(pWi->_typeid) + ", ID=" + std::to_string(pWi->id) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 3, 0x5200103));

						if (pCutin->tipo.ulCondition == ac.tipo)
							break;
						else if ((i + 1) == end)
							throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
									+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
									+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
									+ std::to_string(pWi->_typeid) + ", ID=" + std::to_string(pWi->id) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 3, 0x5200103));
					}
				}
			}

		}else if (sIff::getInstance().getItemGroupIdentify(ac.char_typeid) == iff::SKIN/*Cutin é uma Skin*/ && ac.active == 0) {

			// Verificar se ele tem os itens para ativar esse Cutin

			if ((pCutin = sIff::getInstance().findCutinInfomation(ac.char_typeid)) == nullptr)
				throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
						+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
						+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
						+ std::to_string(ac.char_typeid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 3, 0x5200103));

			// Esses que passa o cutin typeid, pode ativar com tipo 1 e 2, 1 PS e 2 PS
			/*if (pCutin->tipo != ac.tipo)
				throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
						+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
						+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
						+ std::to_string(ac.char_typeid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 3, 0x5200103));*/
		}

		if (pCutin == nullptr)
			throw exception("[TourneyBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
					+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
					+ std::to_string(ac.uid) + "], mas o cution nao foi encontrado[TYPEID="
					+ std::to_string(ac.char_typeid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 4, 0x5200104));

		// Resposta para Active Cutin
		p.init_plain((unsigned short)0x18D);

		p.addUint8(1u);	// OK

		p.addUint32(pCutin->_typeid);
		p.addUint32(pCutin->sector);
		p.addUint32(pCutin->tipo.ulCondition);
		
		p.addUint32(pCutin->img[0].tipo);
		p.addUint32(pCutin->img[1].tipo);
		p.addUint32(pCutin->img[2].tipo);
		p.addUint32(pCutin->img[3].tipo);
		
		p.addUint32(pCutin->tempo);

		p.addBuffer(pCutin->img[0].sprite, sizeof(IFF::CutinInfomation::Img::sprite));
		p.addBuffer(pCutin->img[1].sprite, sizeof(IFF::CutinInfomation::Img::sprite));
		p.addBuffer(pCutin->img[2].sprite, sizeof(IFF::CutinInfomation::Img::sprite));
		p.addBuffer(pCutin->img[3].sprite, sizeof(IFF::CutinInfomation::Img::sprite));

		packet_func::session_send(p, &_session, 1);

		// No Modo GrandZodic, não envia Cutin, então envia o pacote18D com option 0(Uint8), e valor 3(Uint16)

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveCutin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x18D);

		p.addUint8(0);	// OPT

		p.addUint16(1);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestActiveRing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRing");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stRing {
		void clear() { memset(this, 0, sizeof(stRing)); };
		uint32_t _typeid;
		uint32_t effect_value;		// valor do efeito
		unsigned char efeito;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stRing r{ 0 };

		_packet->readBuffer(&r, sizeof(r));

		if (r._typeid == 0)
			throw exception("[TourneyBase::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID=" 
					+ std::to_string(r._typeid) + "], mas o typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 30, 0x330001));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(r._typeid);

		if (pWi == nullptr)
			throw exception("[TourneyBase::requestActiveRing][Error] player[UID = " + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID = " 
					+ std::to_string(r._typeid) + "], mas ele nao tem o anel. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 31, 0x330002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID="
					+ std::to_string(r._typeid) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 32, 0x330003));

		if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), r._typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
			throw exception("[TourneyBase::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID="
					+ std::to_string(r._typeid) + "], mas ele nao esta equipado com o anel. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 33, 0x330004));

		// Adiciona o efeito que foi ativado
		checkEffectItemAndSet(_session, r._typeid);

		// Resposta para o cliente
		p.init_plain((unsigned short)0x237);

		p.addUint32(0);	// OK

		p.addUint32(_session.m_pi.uid);

		p.addUint32(r._typeid);
		p.addUint8(r.efeito);

		packet_func::session_send(p, &_session, 1);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRing][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x237);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x330000);

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestActiveRingGround(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingGround");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stRingGround {
		void clear() { memset(this, 0, sizeof(stRingGround)); };
		bool isValid() {
			return (ring[0] != 0 && ring[1] != 0);
		};
		uint32_t efeito;
		uint32_t ring[2];	// 2 Anel, "Set"(Conjunto)
		uint32_t option;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stRingGround rg{ 0 };

		_packet->readBuffer(&rg, sizeof(rg));

		// Log para saber qual é o efeito 31(0x1F)
		if (IFF::Ability::eEFFECT_TYPE(rg.efeito) == IFF::Ability::eEFFECT_TYPE::UNKNOWN_31)
			_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRingGround][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] ativou o efeito 0x1F(31) com os itens[TYPEID_1=" + std::to_string(rg.ring[0])
					+ ", TYPEID_2=" + std::to_string(rg.ring[1]) + "] e OPTION=" + std::to_string(rg.option), CL_FILE_LOG_AND_CONSOLE));

		if (!rg.isValid())
			throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE=" 
					+ std::to_string(rg.efeito) +", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
					+ std::to_string(rg.option) + "], mas os typeid's nao sao validos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 50, 0x340001));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
					+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
					+ std::to_string(rg.option) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 51, 0x340002));

		if (sIff::getInstance().getItemGroupIdentify(rg.ring[0]) == iff::AUX_PART) {	// Anel

			auto pRing = _session.m_pi.findWarehouseItemByTypeid(rg.ring[0]);

			if (pRing == nullptr)
				throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao tem o Anel[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 52, 0x340002));

			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rg.ring[0]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao esta com o Anel[0] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 53, 0x340003));

			if (rg.ring[0] != rg.ring[1]) {	// Ativou Habilidade em conjunto 2 aneis

				auto pRing2 = _session.m_pi.findWarehouseItemByTypeid(rg.ring[1]);

				if (pRing2 == nullptr)
					throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao tem o Anel[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 52, 0x340002));

				if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
					throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao esta com o Anel[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 53, 0x340003));
			}

		}else if (sIff::getInstance().getItemGroupIdentify(rg.ring[0]) == iff::PART) {	// Part

			auto pRing = _session.m_pi.findWarehouseItemByTypeid(rg.ring[0]);

			if (pRing == nullptr)
				throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao tem o Part[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 52, 0x340002));

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), rg.ring[0]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao esta com o Part[0] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 53, 0x340003));

			if (rg.ring[0] != rg.ring[1]) {	// Ativou Habilidade em conjunto 2 aneis

				auto pRing2 = _session.m_pi.findWarehouseItemByTypeid(rg.ring[1]);

				if (pRing2 == nullptr)
					throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao tem o Part[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 52, 0x340002));

				if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), rg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
					throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao esta com o Part[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 53, 0x340003));
			}
		
		}else if (sIff::getInstance().getItemGroupIdentify(rg.ring[0]) == iff::MASCOT) {

			auto pMascot = _session.m_pi.findMascotByTypeid(rg.ring[0]);

			if (pMascot == nullptr)
				throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao tem o Mascot[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 52, 0x340002));

			if (rg.ring[0] != rg.ring[1]) { // Ativou Habilidade em conjunto 2 aneis

				auto pPart2 = _session.m_pi.findWarehouseItemByTypeid(rg.ring[1]);

				if (pPart2 == nullptr)
					throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao tem o Part[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 52, 0x340002));

				if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), rg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
					throw exception("[TourneyBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao esta com o Part[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 53, 0x340003));
			}
		}

		// Adiciona o efeito que foi ativado
		//checkEffectItemAndSet(_session, rg.ring[0]);
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE(rg.efeito)));

		// Resposta para o Active Ring Terreno
		p.init_plain((unsigned short)0x266);

		p.addUint32(0);	// OK

		p.addBuffer(&rg, sizeof(rg));

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRingGround][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x266);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x340000);

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestActiveRingPawsRainbowJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPawsRainbowJP");

	packet p;

	try {

		// Efeito patinha não passa o TYPEID do item que ativou
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_ACCUMULATE));

		// Resposta para o Active Ring Paws Rainbow JP
		p.init_plain((unsigned short)0x27E);

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRingPawsRainbowJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveRingPawsRingSetJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPawsRingSetJP");

	packet p;

	try {

		// Efeito patinha não passa o TYPEID do item que ativou
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_NOT_ACCUMULATE));

		// Resposta para o Active Ring Paws Ring Set JP
 		p.init_plain((unsigned short)0x281);

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRingPawsRingSetJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveRingPowerGagueJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPowerGagueJP");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stRingPowerGagueJP {
		void clear() { memset(this, 0, sizeof(stRingPowerGagueJP)); };
		bool isValid() {
			return (ring[0] != 0 && ring[1] != 0);
		};
		uint32_t efeito;
		uint32_t ring[2];	// 2 Ring é Conjuntos de Aneis
		uint32_t option;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stRingPowerGagueJP rpg{ 0 };

		_packet->readBuffer(&rpg, sizeof(rpg));

		if (!rpg.isValid())
			throw exception("[TourneyBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas os typeid's nao sao validos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 150, 0x390001));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 151, 0x390002));

		auto pRing = _session.m_pi.findWarehouseItemByTypeid(rpg.ring[0]);

		if (pRing == nullptr)
			throw exception("[TourneyBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas ele nao tem o Anel[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 152, 0x390002));

		if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rpg.ring[0]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
			throw exception("[TourneyBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas ele nao esta com o Anel[0] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 153, 0x390003));

		if (rpg.ring[0] != rpg.ring[1]) {	// Ativou Habilidade em conjunto 2 aneis

			auto pRing2 = _session.m_pi.findWarehouseItemByTypeid(rpg.ring[1]);

			if (pRing2 == nullptr)
				throw exception("[TourneyBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
						+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
						+ std::to_string(rpg.option) + "], mas ele nao tem o Anel[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 152, 0x390002));

			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rpg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[TourneyBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
						+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
						+ std::to_string(rpg.option) + "], mas ele nao esta com o Anel[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 153, 0x390003));
		}

		// Effect
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::POWER_GAUGE_FREE));

		// Resposta para o Active Ring Power Gague JP
		p.init_plain((unsigned short)0x27F);

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRingPowerGagueJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveRingMiracleSignJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingMiracleSign");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[TourneyBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID=" 
					+ std::to_string(_typeid) + "] Olho Magico JP, mas o typeid eh invalido(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 70, 0x350001));

		WarehouseItemEx *pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[TourneyBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
					+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao tem o 'Anel'. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 71, 0x350002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
					+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 72, 0x350003));

		if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::AUX_PART) {	// Anel
			
			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[TourneyBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
						+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao esta com o Anel equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 0x73, 0x350004));

		}else if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::PART) {	// Part

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[TourneyBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
						+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao esta com a Part equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 74, 0x350005));

		}	// else Item Passive, o item do assist, mas acho que ele n�o chame esse, ele chama o proprio pacote dele

		// Effect
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::MIRACLE_SIGN_RANDOM));

		// Resposta para o Active Ring Miracle Sign JP
		p.init_plain((unsigned short)0x280);

		p.addUint32(0);	// OK;

		p.addUint32(_typeid);
		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveRingMiracleSign][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x280);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x350000);

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestActiveWing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveWing");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[TourneyBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID=" 
					+ std::to_string(_typeid) + "], mas o typeid eh invalido(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 90, 0x360001));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[TourneyBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem esse item 'Asa', Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 91, 0x360002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 92, 0x360003));

		if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
			throw exception("[TourneyBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao esta com o item 'Asa' equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 93, 0x360004));

		// Adiciona o efeito que foi ativado
		checkEffectItemAndSet(_session, _typeid);

		// Resposta para o Active Wing
		p.init_plain((unsigned short)0x203);

		p.addUint32(_session.m_pi.uid);

		p.addUint32(_typeid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::ActiveWing][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActivePaws(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActivePaws");

	packet p;

	try {

		// Efeito patinha não passa o TYPEID do item que ativou, Animal Ring(Anel) ou Patinha
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_NOT_ACCUMULATE));

		// Resposta para o Active Paws
		p.init_plain((unsigned short)0x236);

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActivePaws][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestActiveGlove(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveGlove");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[TourneyBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID=" 
					+ std::to_string(_typeid) + "], mas o typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 110, 0x370001));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[TourneyBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem esse item 'Luva'. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 111, 0x370002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[TourneyBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 112, 0x370003));

		if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::PART) {	// Luva

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[TourneyBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
						+ std::to_string(_typeid) + "], mas ele nao tem a Luva equipada. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 113, 0x370004));

		}else if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::AUX_PART) {	// Anel

			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[TourneyBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
						+ std::to_string(_typeid) + "], mas ele nao tem o Anel equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 114, 0x370005));
		}

		// Adiciona o efeito que foi ativado
		checkEffectItemAndSet(_session, _typeid);

		// Resposta para o Active Glove
		p.init_plain((unsigned short)0x265);

		p.addUint32(0);	// OK

		p.addUint32(_typeid);

		p.addUint32(_session.m_pi.uid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveGlove][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x265);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x370000);

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestActiveEarcuff(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveEarcuff");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stEarcuff {
		void clear() { memset(this, 0, sizeof(stEarcuff)); };
		uint32_t _typeid;
		unsigned char angle;	// Sentido do angulo, back(Angel) ou front(Devil)
		float x_point_angle;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stEarcuff ec{ 0 };

		_packet->readBuffer(&ec, sizeof(ec));

		if (ec._typeid == 0)
			throw exception("[TourneyBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff'Mascot'[TYPEID=" 
					+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE=" 
					+ std::to_string(ec.x_point_angle) + "], mas o typeid eh invalido. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 130, 0x380001));

		if (sIff::getInstance().getItemGroupIdentify(ec._typeid) == iff::PART) {	// Earcuff

			if (_session.m_pi.ei.char_info == nullptr)
				throw exception("[TourneyBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 131, 0x380002));

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(ec._typeid);

			if (pWi == nullptr)
				throw exception("[TourneyBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao tem o Part. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 132, 0x380003));

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), ec._typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[TourneyBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao esta com o Part equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 133, 0x380004));

		}else if (sIff::getInstance().getItemGroupIdentify(ec._typeid) == iff::MASCOT)	{	// Mascot Dragon

			auto pMi = _session.m_pi.findMascotByTypeid(ec._typeid);

			if (pMi == nullptr)
				throw exception("[TourneyBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao tem esse Mascot. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 134, 0x380005));

			if (_session.m_pi.ei.mascot_info == nullptr)
				throw exception("[TourneyBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff'Mascot'[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao esta com o Mascot equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 135, 0x380006));
		}

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveEarcuff][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] Typeid=" + std::to_string(ec._typeid) + ", ANG_DIRECTION=" + std::to_string((unsigned short)ec.angle) 
				+ ", ANG=" + std::to_string(ec.x_point_angle), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		INIT_PLAYER_INFO("requestActiveEarcuff", "tentou ativar o efeito earcuff de direcao de vento", &_session);

		// Effect
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::EARCUFF_DIRECTION_WIND));

		// Radianos do angulo que foi trocado a direção
		pgi->earcuff_wind_angle_shot = (float)(ec.x_point_angle < 0.f
			? (2 * 
#if defined(_WIN32)
				std::_Pi
#elif defined(__linux__)
				std::numbers::pi
#endif
			) + ec.x_point_angle
			: ec.x_point_angle
		);

		// Resposta para o Active Earcuff
		p.init_plain((unsigned short)0x24C);

		p.addUint32(0);	// OK

		p.addUint32(ec._typeid);

		p.addUint32(_session.m_pi.uid);

		p.addUint8(ec.angle);

		p.addFloat(ec.x_point_angle);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestActiveEarcuff][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x24C);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x380000);

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestUpdateTrofel() {

	uint32_t soma = 0u;

	std::for_each(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
		if (_el.first != nullptr)
			soma += (_el.second->level > 60) ? 60 : (_el.second->level > 0 ? _el.second->level - 1 : 0);
	});

	uint32_t new_trofel = STDA_MAKE_TROFEL(soma, (uint32_t)m_player_info.size());

	// Check se o trofeu anterior era o GM e se o novo não é mais, aí tira a flag de GM da sala
	if (m_ri.trofel == TROFEL_GM_EVENT_TYPEID && new_trofel != TROFEL_GM_EVENT_TYPEID)
		m_ri.flag_gm = 0;

	if (new_trofel > 0 && new_trofel != m_ri.trofel)
		m_ri.trofel = new_trofel;
}

void TourneyBase::requestSendTimeGame(player& _session) {
	CHECK_SESSION_BEGIN("requestSendTimeGame");

	packet p;

	try {

		if (isGamingBefore(_session.m_pi.uid))
			throw exception("[TourneyBase::requestSendTimeGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas o player ja tinha jogado nessa sala e saiu, e nao pode mais entrar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 2703, 6));

		packet p((unsigned short)0x113);

		p.addUint8(3);	// Remain Time of Game
		p.addUint8(0);

		p.addInt16(m_ri.numero);

		auto remain_time = getLocalTimeDiff(m_start_time);

		if (remain_time > 0)
			remain_time /= STDA_10_MICRO_PER_MILLI;	// Miliseconds

		p.addUint32((const uint32_t)remain_time);
		p.addUint32(m_ri.time_30s);

		p.addBuffer(&m_ri, sizeof(RoomInfo));

		packet_func::session_send(p, &_session, 0);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestSendTimeGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta erro
		p.init_plain((unsigned short)0x113);

		p.addUint8(6);		// Option Error

		// Error Code
		p.addUint8((unsigned char)((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error System*/));

		packet_func::session_send(p, &_session, 1);
	}
}

void TourneyBase::requestUpdateEnterAfterStartedInfo(player& _session, EnterAfterStartInfo& _easi) {
	CHECK_SESSION_BEGIN("requestUpdateEnterAfterStartedInfo");

	packet p;

	try {

		if (_session.m_oid != _easi.owner_oid)
			throw exception("[TourneyBase::requestUpdateEnterAfterStartedInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou atualizar info depois de entrar na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "] que ja tinha comecado, mas os oid[owner=" + std::to_string(_session.m_oid) + ", owner=" 
					+ std::to_string(_easi.owner_oid) + "] nao bate. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 2708, 1));

		auto s = findSessionByOID(_easi.request_oid);

		if (s == nullptr)
			throw exception("[TourneyBase::requestUpdateEnterAfterStartedInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou atualizar info depois de entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] que ja tinha comecado, mas o player[OID=" + std::to_string(_easi.request_oid) + "] nao esta no jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 2709, 1));

		INIT_PLAYER_INFO("requestUpdateEnterAfterStartedInfo", "tentou atualizar info depois de entar na sala[NUMERO=" + std::to_string(m_ri.numero) + "] no jogo", &_session);

		packet p((unsigned short)0x113);

		p.addUint8(10);	// Update Info Scores
		p.addUint8(0);

		p.addUint32(_session.m_oid);
		p.addUint8((unsigned char)pgi->data.total_tacada_num);
		p.addUint8(pgi->hole);
		p.addInt32(pgi->data.score);
		p.addUint64(pgi->data.pang);

		p.addBuffer(&_easi, sizeof(EnterAfterStartInfo));

		packet_func::session_send(p, &_session, 1);
		packet_func::session_send(p, s, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestUpdateEnterAfterStartedInfo][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta erro
		p.init_plain((unsigned short)0x113);

		p.addUint8(6);		// Option Error

		// Error Code
		p.addUint8((unsigned char)((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::TOURNEY_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error System*/));

		packet_func::session_send(p, &_session, 1);
	}
}

bool TourneyBase::requestFinishGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishGame");

	packet p;
	
	bool ret = false;

	try {

		UserInfoEx ui{ 0 };

		_packet->readBuffer(&ui, sizeof(UserInfo));

		// aqui o cliente passa mad_conduta com o hole_in, trocados, mad_conduto <-> hole_in

		INIT_PLAYER_INFO("requestFinishGame", "tentou terminar o jogo", &_session);

		pgi->ui = ui;

		// Packet06
		ret = finish_game(_session, 6);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestFinishGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

// Usa o Padrão delas
void TourneyBase::startTime() {
	
	// Para Tempo se já estiver 1 timer
	if (m_timer != nullptr)
		stopTime();

	job j(TourneyBase::end_time, this, nullptr);

	//if (sgs::gs != nullptr)
		m_timer = sgs::gs::getInstance().makeTime(m_ri.time_30s/*milliseconds*/, j);	// já está em minutos milliseconds
	/*else
		_smp::message_pool::getInstance().push(new message("[TourneyBase::startTime][Error] tentou inicializar um timer, mas a variavel global estatica do Server eh invalida.", CL_FILE_LOG_AND_CONSOLE));*/

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[TourneyBase::startTime][Log] Criou o Timer[Tempo=" + std::to_string((m_ri.time_30s > 0) ? m_ri.time_30s / 60000 : 0) + "min, STATE=" + std::to_string(m_timer->getState()) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

void TourneyBase::requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) {
	CHECK_SESSION_BEGIN("requestTranslateSyncShotData");

	try {

		auto s = findSessionByOID(_ssd.oid);

		if (s == nullptr)
			throw exception("[TourneyBase::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada do player[OID="
					+ std::to_string(_ssd.oid) + "], mas o player nao existe nessa jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 200, 0));

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

			// Já só na função que come�a o tempo do player do turno
			pgi->data.tacada_num++;

			if (_ssd.state == ShotSyncData::OUT_OF_BOUNDS || _ssd.state == ShotSyncData::UNPLAYABLE_AREA)
				pgi->data.tacada_num++;

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[TourneyBase::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada no hole[NUMERO="
						+ std::to_string((unsigned short)pgi->hole) + "], mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 12, 0));

			// Conta já a próxima tacada, no give up
			if (!_ssd.state_shot.display.stDisplay.acerto_hole && hole->getPar().total_shot <= (pgi->data.tacada_num + 1)) {

				// +1 que é giveup, só add se n�o passou o número de tacadas
				if (pgi->data.tacada_num < hole->getPar().total_shot)
					pgi->data.tacada_num++;

				pgi->data.giveup = 1;

				// Soma +1 no Bad Condute
  				pgi->data.bad_condute++;
			}

			// aqui os achievement de power shot int32_t putt beam impact e etc
			update_sync_shot_achievement(_session, last_location);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestTranslateSyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestReplySyncShotData(player& _session) {
	CHECK_SESSION_BEGIN("requestReplySyncShotData");

	try {

		drawDropItem(_session);

		// Resposta Sync Shot
		sendSyncShot(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestReplySyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::sendRemainTime(player& _session) {
	
	auto remain_time = getLocalTimeDiff(m_start_time);

	if (remain_time > 0)
		remain_time /= STDA_10_MICRO_PER_MILLI;

	// Resposta tempo percorrido do Tourney
	packet p((unsigned short)0x8D);

	p.addUint32((const uint32_t)remain_time);

	packet_func::session_send(p, &_session, 1);
}

void TourneyBase::updateFinishHole(player& _session, int _option) {

	INIT_PLAYER_INFO("updateFinishHole", "tentou terminar o hole no jogo", &_session);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[TourneyBase::updateFinishHole][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] Terminou o hole[NUMERO=" + std::to_string(pgi->hole) + "].", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	packet p((unsigned short)0x6D);

	p.addUint32(_session.m_oid);
	p.addUint8(pgi->hole);
	p.addUint8((unsigned char)pgi->data.total_tacada_num);
	p.addInt32(pgi->data.score);
	p.addUint64(pgi->data.pang);
	p.addUint64(pgi->data.bonus_pang);
	p.addUint8((unsigned char)_option);	// 1 - Terminou o Hole, 0 - N�o terminou o Hole

	packet_func::game_broadcast(*this, p, 1);
}

void TourneyBase::updateTreasureHunterPoint(player& _session) {

	INIT_PLAYER_INFO("updateTreasureHunterPoint", "tentou atualizar os pontos do Treasure Hunter no jogo", &_session);

	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	auto hole = m_course->findHole(pgi->hole);

	if (hole == nullptr)
		throw exception("[TourneyBase::updateTreasureHunterPoint][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou atualizar os pontos do Treasure Hunter no hole[NUMERO="
				+ std::to_string((unsigned short)pgi->hole) + "], mas o hole nao existe. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 30, 0));

	// Calcule Treasure Pontos
	if (m_ri.tipo == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE)
		/*pgi->thi.treasure_point += TreasureHunterSystem::calcPointSSC(pgi->data.tacada_num, hole->getPar().par) + pgi->thi.getPoint(pgi->data.tacada_num, hole->getPar().par);*/
		pgi->thi.treasure_point += sTreasureHunterSystem::getInstance().calcPointSSC(pgi->data.tacada_num, hole->getPar().par) + pgi->thi.getPoint(pgi->data.tacada_num, hole->getPar().par);
	else
		/*pgi->thi.treasure_point += TreasureHunterSystem::calcPointNormal(pgi->data.tacada_num, hole->getPar().par) + pgi->thi.getPoint(pgi->data.tacada_num, hole->getPar().par);*/
		pgi->thi.treasure_point += sTreasureHunterSystem::getInstance().calcPointNormal(pgi->data.tacada_num, hole->getPar().par) + pgi->thi.getPoint(pgi->data.tacada_num, hole->getPar().par);

	// Mostra score board
	packet p((unsigned short)0x132);

	p.addUint32(pgi->thi.treasure_point);

	// No Modo Match passa outro valor tbm

	packet_func::session_send(p, &_session, 1);
}

void TourneyBase::requestDrawTreasureHunterItem(player& _session) {

	// Sorteia os itens ganho do Treasure ponto do player
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	INIT_PLAYER_INFO("requestDrawTreasureHunterItem", "tentou sortear os item(ns) do Treasure Hunter do jogo", &_session);

	// Guarda os item(ns) ganho no treasure hunter system, no Player Game Info, para poder consultar ele depois
	/*pgi->thi.v_item = TreasureHunterSystem::drawItem(pgi->thi.treasure_point, m_ri.course & 0x7F);*/
	pgi->thi.v_item = sTreasureHunterSystem::getInstance().drawItem(pgi->thi.treasure_point, m_ri.course & 0x7F);

	if (pgi->thi.v_item.empty())
		_smp::message_pool::getInstance().push(new message("[TourneyBase::requestDrawTreasureHunterItem][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sortear os item(ns) do Treasure Hunter do jogo," +
				"mas o Treasure Hunter Item nao conseguiu sortear nenhum item", CL_FILE_LOG_AND_CONSOLE));
}

void TourneyBase::sendSyncShot(player& _session) {

	INIT_PLAYER_INFO("sendSyncShot", "tentou sincronizar a tacada do jogador no jogo", &_session);

	packet p((unsigned short)0x6E);

	p.addUint32(pgi->shot_sync.oid);

	p.addInt8(pgi->hole);

	p.addFloat(pgi->location.x);
	p.addFloat(pgi->location.z);

	p.addUint32(pgi->shot_sync.state_shot.shot.ulState);

	// No Modo de jogo Approach aqui tem mais 2 uint32_t, com tempo da tacada e a dist�ncia do hole

	p.addUint16(pgi->shot_sync.tempo_shot);

	packet_func::game_broadcast(*this, p, 1);
}

void TourneyBase::sendEndShot(player& _session, DropItemRet& _cube) {
	
	packet p((unsigned short)0xCC);

	p.addUint32(_session.m_oid);

	// Count, Coin/Cube "Drop"
	p.addUint8((unsigned char)_cube.v_drop.size());

	if (!_cube.v_drop.empty()) {
		for (auto& el : _cube.v_drop)
			p.addBuffer(&el, sizeof(el));

		// Aqui o server passa 128 itens de drop, os que dropou e o resto vazio
		if (_cube.v_drop.size() < 128)
			p.addZeroByte((128 - _cube.v_drop.size()) * 16);
	}

	packet_func::session_send(p, &_session, 1);
}

void TourneyBase::sendUpdateState(player& _session, int _option) {

	packet p((unsigned short)0x6C);

	p.addUint32(_session.m_oid);

	p.addUint8((unsigned char)_option);	// 2 Terminou, 3 Saiu

	packet_func::game_broadcast(*this, p, 1);
}

void TourneyBase::sendDropItem(player& _session) {

	INIT_PLAYER_INFO("sendDropItem", "tentou enviar os itens dropado do player no jogo", &_session);

	packet p((unsigned short)0xCE);

	p.addUint8(0);	// OK

	p.addUint16((unsigned short)pgi->drop_list.v_drop.size());

	for (auto& el : pgi->drop_list.v_drop)
		p.addUint32(el._typeid);

	packet_func::session_send(p, &_session, 1);
}

void TourneyBase::sendPlacar(player& _session) {
	
	INIT_PLAYER_INFO("sendPlacar", "tentou enviar o placar do jogo", &_session);

	packet p((unsigned short)0x79);

	p.addUint32(pgi->data.exp);

	p.addUint32(m_ri.trofel);

	p.addUint8(pgi->trofel);	// Trofel Que o Player Ganhou
	p.addUint8(pgi->team);	// Team Win, 0 - vermelho, 1 - Azul, 2 nenhum

	// Medalhas
	for (auto i = 0u; i < (sizeof(m_medal) / sizeof(m_medal[0])); ++i)
		p.addBuffer(&m_medal[i], sizeof(Medal));

	// N�o sei se � a geral ou se � s� a do Tourney, (DEIXEI A GERAL) todas as medalhas que ele tem
	p.addBuffer(&_session.m_pi.ui.medal, sizeof(stMedal));

	packet_func::session_send(p, &_session, 1);
}

void TourneyBase::sendTreasureHunterItemDrawGUI(player& _session) {
	
	INIT_PLAYER_INFO("sendTreasureHunterItemDrawGUI", "tentou enviar os itens ganho no Treasure Hunter(so o Visual) do jogo", &_session);

	packet p((unsigned short)0x133);

	p.addUint8((unsigned char)pgi->thi.v_item.size());

	// No VS aqui os itens s�o dividido entres os players do versus
	for (auto& el : pgi->thi.v_item) {
		p.addUint32(pgi->uid);	// UID do player que ganhou o item
		p.addUint32(el._typeid);
		p.addUint16((unsigned short)el.qntd);
		p.addUint8(0);	// Acho que sej� op��o ou dizendo que acabou o struct de Treasure Hunter Item Draw
	}

	packet_func::session_send(p, &_session, 1);
}

void TourneyBase::sendTimeIsOver(player& _session) {

	packet p((unsigned short)0x8C);

	packet_func::session_send(p, &_session, 1);
}

int TourneyBase::checkEndShotOfHole(player& _session) {

	// Agora verifica o se ele acabou o hole e essas coisas
	INIT_PLAYER_INFO("checkEndShotOfHole", "tentou verificar a ultima tacada do hole no jogo", &_session);

	if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || pgi->data.giveup) {

		if (pgi->data.bad_condute >= 3) {	// Kika player deu 3 give up

			// !!@@@
			// Tira o player da sala
			return 2;
		}

		// Verifica se o player terminou jogo, fez o ultimo hole
		if (m_course->findHoleSeq(pgi->hole) == m_ri.qntd_hole) {

			// Resposta para o player que terminou o ultimo hole do Game
			packet p((unsigned short)0x199);

			packet_func::session_send(p, &_session, 1);

			// Fez o Ultimo Hole, Calcula Clear Bonus para o player
			if (pgi->shot_sync.state_shot.display.stDisplay.clear_bonus) {

				if (!sMap::getInstance().isLoad())
					sMap::getInstance().load();

				auto map = sMap::getInstance().getMap(m_ri.course & 0x7F);

				if (map == nullptr)
					_smp::message_pool::getInstance().push(new message("[TourneyBase::checkEndShotOfHole][Error][WARNING] tentou pegar o Map dados estaticos do course[COURSE="
							+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "], mas nao conseguiu encontra na classe do Server.", CL_FILE_LOG_AND_CONSOLE));
				else
					pgi->data.bonus_pang += sMap::getInstance().calculeClear30s(*map, m_ri.qntd_hole);
			}
		}

		finishHole(_session);

		changeHole(_session);
	
	}

	return 0;
}

void TourneyBase::drawDropItem(player& _session) {

	INIT_PLAYER_INFO("drawDropItem", "tentou sortear item drop para o jogador no jogo", &_session);

	if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole) {
		auto drop = requestInitDrop(_session);

		if (!drop.v_drop.empty()) {
			packet p((unsigned short)0xCC);

			p.addUint32(_session.m_oid);

			// Count, Coin/Cube "Drop"
			p.addUint8((unsigned char)drop.v_drop.size());

			if (!drop.v_drop.empty()) {
				for (auto& el : drop.v_drop)
					p.addBuffer(&el, sizeof(el));

				// Aqui o server passa 128 itens de drop, os que dropou e o resto vazio
				if (drop.v_drop.size() < 128)
					p.addZeroByte((128 - drop.v_drop.size()) * 16);
			}

			packet_func::session_send(p, &_session, 1);
		}
	}
}

void TourneyBase::achievement_top_3_1st(player& _session) {
	CHECK_SESSION_BEGIN("achievement_top_3_1st");

	auto rank = getRankPlace(_session);

	if (rank != ~0u) {

		if (rank < 3) {

			INIT_PLAYER_INFO("achievement_top_3_1st", "tentou atualizar achievement contador de top 3 rank do player no jogo", &_session);

			pgi->sys_achieve.incrementCounter(0x6C4000B6u/*Top 3*/);

			if (rank == 0u)
				pgi->sys_achieve.incrementCounter(0x6C4000AFu/*1st(Primeiro)*/);
		}
	}
}

void TourneyBase::calcule_shot_to_spinning_cube(player& _session, ShotSyncData& _ssd) {
	CHECK_SESSION_BEGIN("calcule_shot_to_spinning_cube");

	try {

		INIT_PLAYER_INFO("calcule_shot_to_spinning_cube", "tentou calcular a tacada para o spinning cube", &_session);

		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			return;

		if (_ssd.state != ShotSyncData::SHOT_STATE::PLAYABLE_AREA && _ssd.state != ShotSyncData::SHOT_STATE::INTO_HOLE)
			return; // Sai

		// Bogey+ ou errou pangya ou bunker não calcula
		if (pgi->data.tacada_num > hole->getPar().par || pgi->shot_data.acerto_pangya_flag != 4/*Acertou PangYa*/ || _ssd.bunker_flag != 0)
			return; // Sai, tacada bogey não calcula spinning cube

		// Calcule Shot Cube
		sCoinCubeLocationUpdateSystem::getInstance().pushOrderToCalcule(CalculeCoinCubeUpdateOrder{
			CalculeCoinCubeUpdateOrder::CUBE,
			_session.m_pi.uid,
			pgi->location,
			hole->getPinLocation(),
			pgi->shot_data_for_cube,
			(unsigned char)(m_ri.course & 0x7Fu),
			(unsigned char)(m_ri.modo == RoomInfo::MODO::M_REPEAT ? hole->getHoleRepeat() : hole->getNumero())
		});
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::calcule_shot_to_spinning_cube][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::calcule_shot_to_coin(player& _session, ShotSyncData& _ssd) {
	CHECK_SESSION_BEGIN("calcule_shot_to_coin");

	try {

		constexpr float MIN_DISTANCE_TO_HOLE_TO_SPAWN_COIN = 70.f * SCALE_PANGYA; // 70y

		INIT_PLAYER_INFO("calcule_shot_to_coin", "tentou verificar a tacada para a coin", &_session);

		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			return;

		if (_ssd.state != ShotSyncData::SHOT_STATE::PLAYABLE_AREA && _ssd.state != ShotSyncData::SHOT_STATE::INTO_HOLE)
			return; // Sai

		// Bogey+ ou errou pangya ou bunker não calcula
		if (pgi->data.tacada_num > hole->getPar().par || pgi->shot_data.acerto_pangya_flag != 4/*Acertou PangYa*/ || _ssd.bunker_flag != 0)
			return; // Sai, tacada bogey não calcula coin

		if (std::abs(hole->getPinLocation().diffXZ(*(Location*)&_ssd.location)) <= MIN_DISTANCE_TO_HOLE_TO_SPAWN_COIN)
			return; // Sai, muito perto do hole para spawnar uma coin

		// Calcule Shot Coin
		sCoinCubeLocationUpdateSystem::getInstance().pushOrderToCalcule(CalculeCoinCubeUpdateOrder{
			CalculeCoinCubeUpdateOrder::eTYPE::COIN,
			_session.m_pi.uid,
			*(Location*)&_ssd.location, // Aqui é onde o player caiu
			hole->getPinLocation(),
			pgi->shot_data_for_cube,
			(unsigned char)(m_ri.course & 0x7Fu),
			(unsigned char)(m_ri.modo == RoomInfo::MODO::M_REPEAT ? hole->getHoleRepeat() : hole->getNumero())
		});

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::calcule_shot_to_coin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void TourneyBase::requestCalculeShotSpinningCube(player& _session, ShotSyncData& _ssd) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_ssd);

	// o Calculo da tacada do spinning cube só é calculado no Tourney(normal) e no Grand Prix(Normal)
}

void TourneyBase::requestCalculeShotCoin(player& _session, ShotSyncData& _ssd) {
	UNREFERENCED_PARAMETER(_session);
	UNREFERENCED_PARAMETER(_ssd);

	// o Calculo da tacada da coin só é calculado no Tourney(normal) e no Grand Prix(Normal)
}

int TourneyBase::end_time(void* _arg1, void* _arg2) {
	UNREFERENCED_PARAMETER(_arg2);

	auto game = reinterpret_cast< TourneyBase* >(_arg1);

	try {
		
		// Tempo Acabou
		game->timeIsOver();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[TourneyBase::end_time][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
}
