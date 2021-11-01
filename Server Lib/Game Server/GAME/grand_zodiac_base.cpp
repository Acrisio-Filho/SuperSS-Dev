// Arquivo grand_zodiac_base.cpp
// Criado em 23/06/2020 as 16:09 por Acrisio
// Implementa��o da classe GrandZodiacBase

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "grand_zodiac_base.hpp"

#include "../PACKET/packet_func_sv.h"

#include "../Game Server/game_server.h"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[GrandZodiacBase" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[GrandZodiacBase::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) PlayerGrandZodiacInfo *pgi = reinterpret_cast< PlayerGrandZodiacInfo* >(getPlayerInfo((__session))); \
	if (pgi == nullptr) \
		throw exception("[GrandZodiacBase::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 1, 4)); \

using namespace stdA;

bool GrandZodiacBase::sort_grand_zodiac_rank_place(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2) {

	// All nullptr
	if (_pgi1 == nullptr && _pgi2 == nullptr)
		return false;
	
	if (_pgi1 != nullptr && _pgi2 == nullptr)
		return true;
	else if (_pgi1 == nullptr && _pgi2 != nullptr)
		return false;
		
	return reinterpret_cast< PlayerGrandZodiacInfo* >(_pgi1)->m_gz.total_score > reinterpret_cast< PlayerGrandZodiacInfo* >(_pgi2)->m_gz.total_score;
}

GrandZodiacBase::GrandZodiacBase(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: TourneyBase(_players, _ri, _rv, _channel_rookie), m_golden_beam_state(0u), m_mp_golden_beam_player(), m_state_gz(),
	  m_thread_sync_first_hole(nullptr), 
#if defined(_WIN32)
	  m_hEvent_sync_hole(INVALID_HANDLE_VALUE), m_hEvent_sync_hole_pulse(INVALID_HANDLE_VALUE) 
#elif defined(__linux__)
	  m_hEvent_sync_hole(nullptr), m_hEvent_sync_hole_pulse(nullptr) 
#endif
{

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_sync_shot);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Aqui tem que inicializar os players info
	initAllPlayerInfo();

	init_values_seed();

	// Cria evento que vai para a thread sync hole
#if defined(_WIN32)
	if ((m_hEvent_sync_hole = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[GrandZodiacBase::GrandZodiacBase][Error] ao criar evento sync hole.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 1050, GetLastError()));
#elif defined(__linux__)
	m_hEvent_sync_hole = new Event(true, 0u);

	if (!m_hEvent_sync_hole->is_good()) {

		delete m_hEvent_sync_hole;

		m_hEvent_sync_hole = nullptr;

		throw exception("[GrandZodiacBase::GrandZodiacBase][Error] ao criar evento sync hole.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 1050, errno));
	}
#endif

	// Cria evento que vai pulsar a thread sync hole para ir mais r�pido quando um player tacar
#if defined(_WIN32)
	if ((m_hEvent_sync_hole_pulse = CreateEvent(NULL, FALSE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[GrandZodiacBase::GrandZodiacBase][Error] ao criar evento sync hole pulse.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 1050, GetLastError()));
#elif defined(__linux__)
	m_hEvent_sync_hole_pulse = new Event(false, 0u);

	if (!m_hEvent_sync_hole_pulse->is_good()) {

		delete m_hEvent_sync_hole_pulse;

		m_hEvent_sync_hole_pulse = nullptr;

		throw exception("[GrandZodiacBase::GrandZodiacBase][Error] ao criar evento sync hole pulse.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 1050, errno));
	}
#endif

	// Cria a thread que vai sincronizar os player no hole
	m_thread_sync_first_hole = new thread(1061/*sync first hole*/, GrandZodiacBase::_syncFirstHole, (LPVOID)this);
}

GrandZodiacBase::~GrandZodiacBase() {

#if defined(_WIN32)
	InterlockedExchange(&m_golden_beam_state, 0u);
#elif defined(__linux__)
	__atomic_store_n(&m_golden_beam_state, 0u, __ATOMIC_RELAXED);
#endif

	if (!m_mp_golden_beam_player.empty())
		m_mp_golden_beam_player.clear();

	if (!m_initial_values_seed.empty()) {
		m_initial_values_seed.clear();
		m_initial_values_seed.shrink_to_fit();
	}

	// Termina a thread sync first hole
	finish_thread_sync_first_hole();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs_sync_shot);
#endif
}

bool GrandZodiacBase::deletePlayer(player* _session, int _option) {

	if (_session == nullptr)
		throw exception("[GrandZodiacBase::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 50, 0));
	
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

			if (m_game_init_state == 1/*Come�ou*/) {

				packet p;

				INIT_PLAYER_INFO("deletePlayer", "tentou sair do jogo", _session);

				auto sessions = getSessions(*it);

				requestSaveInfo(*(*it), 1/*Saiu*/);

				requestUpdateItemUsedGame(*(*it)); // Atualiza primeiro, por que o Grand Zodiac n�o atualiza, a cada hole, s� no final

				requestFinishItemUsedGame(*(*it));	// Salva itens usados no Grand Zodiac

				setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::QUIT);

				// Resposta Player saiu do jogo MSG
				p.init_plain((unsigned short)0x40);

				p.addUint8(2);	// Player Saiu Msg

				p.addString((*it)->m_pi.nickname);

				p.addUint16(0);	// size Msg, n�o precisa de msg o pangya j� manda na opt 2

				packet_func::vector_send(p, sessions, 1);

				if (AllCompleteGameAndClear())
					ret = true;	// Termina o Grand Zodiac
			}

			// Delete Player
			m_players.erase(it);

		}else
			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::deletePlayer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Critical Section
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
	}

	return ret;
}

void GrandZodiacBase::deleteAllPlayer() {

	while (!m_players.empty())
		deletePlayer(*m_players.begin(), 0);
}

void GrandZodiacBase::sendInitialData(player& _session) {
	CHECK_SESSION_BEGIN("sendInitialData");

	try {

		// Envia aqui os valores do hole, size_cup
		packet p((unsigned short)0x1F9);

		auto size_cup = _session.m_pi.getSizeCupGrandZodiac();

		p.addUint32(size_cup);											// Start Hole Size Cup
		p.addUint32(size_cup);											// Finish size cup (OU O tee acho que seja)
		p.addUint32((uint32_t)_session.m_pi.grand_zodiac_pontos);	// Aqui acho que � os pangs que ele faz por cada hio, com rela��o ao size do cup (OU OS PONTOS DO GRAND ZODIAC)

		packet_func::session_send(p, &_session, 1);

		// Send Initial Data, por�m ele tamb�m � sincronizado, s� envia os dados quando todos fizer o mesmo pedido
		TourneyBase::sendInitialData(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::sendInitialData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestInitHole(player& _session, packet *_packet) {
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
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestInitHole][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] Hole[NUMERO=" + std::to_string((unsigned short)ctx_hole.numero) + ", PAR=" + std::to_string((unsigned short)ctx_hole.par) 
				+ " OPT=" + std::to_string(ctx_hole.option) + ", UNKNOWN=" + std::to_string(ctx_hole.ulUnknown) 
				+ "] Tee[X=" + std::to_string(ctx_hole.tee.x) + ", Z=" + std::to_string(ctx_hole.tee.z)
				+ "] Pin[X=" + std::to_string(ctx_hole.pin.x) + ", Z=" + std::to_string(ctx_hole.pin.z) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG


		auto hole = m_course->findHole(ctx_hole.numero);

		if (hole == nullptr)
			throw exception("[GrandZodiacBase::requestInitHole][Error] course->findHole nao encontrou o hole retonou nullptr, o server esta com erro no init course do GrandZodiacBase.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 2555, 0));

		hole->init(ctx_hole.tee, ctx_hole.pin);

		INIT_PLAYER_INFO("requestInitHole", "tentou inicializar o hole[NUMERO = " + std::to_string(hole->getNumero()) + "] no jogo", &_session);

		// Update Location Player in Hole
		pgi->location.x = ctx_hole.tee.x;
		pgi->location.z = ctx_hole.tee.z;
		
		// N�mero do hole atual, que o player est� jogandp
		pgi->hole = ctx_hole.numero;

		// Flag que marca se o player j� inicializou o primeiro hole do jogo
		if (!pgi->init_first_hole)
			pgi->init_first_hole = 1u;

		// Gera degree para o player ou pega o degree sem gerar que � do modo do hole repeat
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
		p.addUint8((wind_flag < 0) ? 1 : 0);	// Flag de card de vento, aqui � a qnd diminui o vento, 1 Vento azul
		p.addUint16(pgi->degree);
		p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestInitHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}
}

bool GrandZodiacBase::requestFinishLoadHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishLoadHole");

	packet p;

	bool ret = false;

	try {

		auto size_cup = _packet->readUint32();

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestFinishLoadHole][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ ", SIZE_CUP=" + std::to_string(size_cup) + "] na sala[NUMERO=" + std::to_string(m_ri.numero) + "].", CL_FILE_LOG_AND_CONSOLE));

		INIT_PLAYER_INFO("requestFinishLoadHole", "tentou finalizar carregamento do hole no jogo", &_session);

		pgi->finish_load_hole = 1;

		// Valores de double, aleat�rio que passar, pode ser s� da rota��o da camera
		// Mas vou deixar o mesmo valor para todos, para fazer um teste
		for (auto& el : m_initial_values_seed) {

			p.init_plain((unsigned short)0x1EC);

			p.addDouble(el);

			packet_func::session_send(p, &_session, 1);
		}

		// Primeiro pacote dizendo que terminou de carregar o course GZ
		p.init_plain((unsigned short)0x201);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void GrandZodiacBase::requestFinishCharIntro(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishCharIntro");

	packet p;

	try {

		INIT_PLAYER_INFO("requestFinishCharIntro", "tentou finalizar intro do char no jogo", &_session);

		pgi->finish_char_intro = 1;

		pgi->data.tacada_num = 0u;
		
		// Giveup Flag
		pgi->data.giveup = 0u;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestInitShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShot");

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestInitShot][Log] Packet Hex: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		ShotDataEx sd{ 0 };

		sd.option = _packet->readUint16();

		// Power Shot
		if (sd.option == 1)
			_packet->readBuffer(&sd.power_shot, sizeof(sd.power_shot));

		_packet->readBuffer(&sd, sizeof(ShotDataBase));

		INIT_PLAYER_INFO("requestInitShot", "tentou iniciar tacada no jogo", &_session);

		pgi->shot_data = sd;

		// Aqui seta o state e verifica se � para mandar a resposta
		if (pgi->m_sync_shot_gz.setStateAndCheckAllAndClear(SyncShotGrandZodiac::eSYNC_SHOT_GRAND_ZODIAC_STATE::SSGZS_FIRST_SHOT_INIT))
			sendReplyInitShotAndSyncShot(_session);

#ifdef _DEBUG
		// Log Shot Data Ex
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestInitShot][Log] Log Shot Data Ex:\n\r" + sd.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestInitShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestActiveBooster(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveBooster");

	packet p;

	try {

		// Grand Zodiac o Booster � gr�tis
		float velocidade = _packet->readFloat();

		// Resposta para Active Booster
		p.init_plain((unsigned short)0xC7);

		p.addFloat(velocidade);
		p.addUint32(_session.m_oid);

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestActiveBooster][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestActiveCutin(player& _session, packet *_packet) {
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
		uint32_t char_typeid;	// Aqui pode ter o typeid do cutin tamb�m
		unsigned char active;		// Active acho, sempre com valor 1 que peguei, 1 quando � id do character, 0 quando � o typeid do Cutin
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stActiveCutin ac{ 0 };

		// Consome o buffer, mas grand zodiac responde com op��o 3, cuttin desativado
		_packet->readBuffer(&ac, sizeof(ac));

		// Resposta para Active Cutin
		p.init_plain((unsigned short)0x18D);

		p.addUint8(0u);	// OK

		p.addUint16(3);	// Cuttin do Grand Zodiac, Cuttin desativado

		packet_func::session_send(p, &_session, 1);

		// No Modo GrandZodic, n�o envia Cutin, ent�o envia o pacote18D com option 0(Uint8), e valor 3(Uint16)

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestActiveCutin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x18D);

		p.addUint8(0);	// OPT

		p.addUint16(1);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void GrandZodiacBase::requestStartFirstHoleGrandZodiac(player& _session, packet *_packet) {
	REQUEST_BEGIN("StartFirstHoleGrandZodiac");

	packet p;

	try {

		INIT_PLAYER_INFO("requestStartFirstHoleGrandZodiac", "tentou inicializar o primeiro hole do grand zodiac", &_session);
		
		// Aqui tem que sincronizar
		setInitFirstHole(pgi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestStartFirstHoleGrandZodiac][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestReplyInitialValueGrandZodiac(player& _session, packet *_packet) {
	REQUEST_BEGIN("ReplyInitialValueGrandZodiac");

	try {

		double value = _packet->readDouble();

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestReplyInitialValueGrandZodiac][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] Valor Inicial (" + std::to_string(value) + ")", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestReplyInitialValueGrandZodiac][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool GrandZodiacBase::requestFinishGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishGame");
	
	bool ret = false;

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestFinishGame][Log] Packet Hex: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Packet0CB
		ret = finish_game(_session, 0x12C);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestFinishGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void GrandZodiacBase::sendRemainTime(player& _session) {

	try {

		auto remain_time = 0ull;

		if (m_timer != nullptr)
			remain_time = m_timer->getElapsed(); // Milli segundos

		// Resposta tempo percorrido do Tourney
		packet p((unsigned short)0x8D);

		p.addUint32((uint32_t)0u); // Grand Zodiac, passa o tempo decorrido no pacote200

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::sendRemainTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestFinishHole(player& _session, int option) {

	INIT_PLAYER_INFO("requestFinishHole", "tentou finalizar o dados do hole do player no jogo", &_session);

	auto hole = m_course->findHole(pgi->hole);

	if (hole == nullptr)
		throw exception("[GrandZodiacBase::finishHole][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar hole[NUMERO="
				+ std::to_string((unsigned short)pgi->hole) + "] no jogo, mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 20, 0));

	char score_hole = 0;
	uint32_t tacada_hole = 0u;

	// Finish Hole Dados
	if (option == 0) {

		// Score Player
		packet p((unsigned short)0x1EF);

		p.addUint32(_session.m_oid);

		p.addUint32(pgi->m_gz.total_score);

		p.addUint32(pgi->data.score);

		p.addUint32((pgi->m_gz.total_score + pgi->data.score));

		packet_func::game_broadcast(*this, p, 1);

		// Update Values
		pgi->data.total_tacada_num += pgi->data.tacada_num;

		// Score do hole
		score_hole = (char)pgi->data.score;

		// Tacadas do hole
		tacada_hole = pgi->data.tacada_num;

		pgi->m_gz.total_score += score_hole;

		pgi->m_gz.hole_in_one++;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestFinishHole][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] terminou o hole[COURSE=" 
				+ std::to_string(hole->getCourse()) + ", NUMERO=" + std::to_string(hole->getNumero()) + ", PAR=" 
				+ std::to_string(hole->getPar().par) + ", SHOT=" + std::to_string(tacada_hole) + ", SCORE=" + std::to_string(score_hole) + ", TOTAL_SHOT=" 
				+ std::to_string(pgi->data.total_tacada_num) + ", TOTAL_SCORE=" + std::to_string(pgi->data.score) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Zera dados
		pgi->data.time_out = 0u;

		pgi->data.tacada_num = 0u;

		// Zera o score, que o Grand Zodiac usa o total_score
		pgi->data.score = 0l;

		// Giveup Flag
		pgi->data.giveup = 0u;

		// Zera as penalidades do hole
		pgi->data.penalidade = 0u;

	}else if (option == 1) {	// N�o acabou o hole ent�o faz os calculos para o jogo todo

		// Zera dados
		pgi->data.time_out = 0u;

		pgi->data.tacada_num = 0u;

		// Zera o score, que o Grand Zodiac usa o total_score
		pgi->data.score = 0l;

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
			
			pgi->progress.finish_hole[it->first - 1] = 0;	// n�o terminou

			pgi->progress.par_hole[it->first - 1] = it->second.getPar().par;

			pgi->progress.score[it->first - 1] = it->second.getPar().range_score[1];	// Max Score

			pgi->progress.tacada[it->first - 1] = it->second.getPar().total_shot;
		}
	}
}

void GrandZodiacBase::requestUpdateItemUsedGame(player& _session) {

	INIT_PLAYER_INFO("requestUpdateItemUsedGame", "tentou atualizar itens usado no jogo", &_session);

	auto& ui = pgi->used_item;

	// Passive Item exceto Time Booster e Auto Command, que soma o contador por uso, o cliente passa o pacote, dizendo que usou o item
	for (auto& el : ui.v_passive) {
		
		// Verica se � o ultimo hole, terminou o jogo, ai tira soma 1 ao count do pirulito que consome por jogo
		if (CHECK_PASSIVE_ITEM(el.second._typeid) && el.second._typeid != TIME_BOOSTER_TYPEID/*Time Booster*/ && el.second._typeid != AUTO_COMMAND_TYPEID) {
			
			// Item de Exp Boost que s� consome 1 Por Jogo, s� soma no requestFinishItemUsedGame
			if (std::find(passive_item_exp_1perGame, LAST_ELEMENT_IN_ARRAY(passive_item_exp_1perGame), el.second._typeid) == LAST_ELEMENT_IN_ARRAY(passive_item_exp_1perGame))
				el.second.count = (pgi->data.total_tacada_num / 4); // Gasta 1 a cada 4 tacadas
		
		}else if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::BALL	/*Ball*/
				|| sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::AUX_PART) /*AuxPart(Anel)*/
			el.second.count = (pgi->data.total_tacada_num / 4); // uma comet e um anel por 4 tacadas
	}
}

void GrandZodiacBase::requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) {
	CHECK_SESSION_BEGIN("requestTranslateSyncShotData");

	try {

		auto s = findSessionByOID(_ssd.oid);

		if (s == nullptr)
			throw exception("[GrandZodiacBase::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada do player[OID="
					+ std::to_string(_ssd.oid) + "], mas o player nao existe nessa jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC_BASE, 200, 0));

		// Update Sync Shot Player
		if (_session.m_pi.uid == s->m_pi.uid) {

			INIT_PLAYER_INFO("requestTranslateSyncShotData", "tentou sincronizar a tacada no jogo", &_session);

			pgi->shot_sync = _ssd;

			// Last Location Player
			auto last_location = pgi->location;

			// Update Location Player, aqui no outro server, eu n�o setava a location do player
			//pgi->location.x = _ssd.location.x;
			//pgi->location.z = _ssd.location.z;

			// Update Pang and Bonus Pang
			pgi->data.pang = _ssd.pang;
			pgi->data.bonus_pang = _ssd.bonus_pang;

			// J� s� na fun��o que come�a o tempo do player do turno
			pgi->data.tacada_num++;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestTranslateSyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestSaveInfo(player& _session, int option) {

	INIT_PLAYER_INFO("requestSaveInfo", "tentou salvar o info dele no jogo", &_session);

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestSaveInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] UserInfo[" + pgi->ui.toString() + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (option == 1) { // Saiu

			// Zera os pangs ele saiu
			pgi->data.pang = 0ull;
			pgi->data.bonus_pang = 0ull;
		}

		// Limpa o User Info por que n�o add nada, s� o tempo e os pangs ganhos
		pgi->ui.clear();

		auto diff = getLocalTimeDiff(m_start_time);

		if (diff > 0)
			diff /= STDA_10_MICRO_PER_SEC; // NanoSeconds To Seconds

		pgi->ui.tempo = (uint32_t)diff;

		// Pode tirar pangs
		int64_t total_pang = pgi->data.pang + pgi->data.bonus_pang;

		// Adiciona o Jackpot, se ele ganhou
		if (option != 1 && pgi->m_gz.jackpot > 0ull)
			total_pang += pgi->m_gz.jackpot;

		// UPDATE ON SERVER AND DB
		_session.m_pi.addUserInfo(pgi->ui, total_pang);	// add User Info

		if (total_pang > 0)
			_session.m_pi.addPang(total_pang);				// add Pang
		else if (total_pang < 0)
			_session.m_pi.consomePang(total_pang * -1);		// consome Pangs

		// Log
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestSaveInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] " + (option == 0 ? "Terminou o Grand Zodiac " : "Saiu do Grand Zodiac ") + " com [TOTAL_SCORE=" + std::to_string(pgi->m_gz.total_score) 
				+ ", TOTAL_HIO=" + std::to_string(pgi->m_gz.hole_in_one) + ", TOTAL_PONTOS=" + std::to_string(pgi->m_gz.pontos) + ", TOTAL_TACADAS=" + std::to_string(pgi->data.total_tacada_num)
				+ ", TOTAL_PANG=" + std::to_string(total_pang) + "]", CL_FILE_LOG_AND_CONSOLE));
	
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestSaveInfo][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::requestCalculeRankPlace() {

	if (!m_player_order.empty())
		m_player_order.clear();

	for (auto& el : m_player_info)
		if (el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT)	// menos os que quitaram
			m_player_order.push_back(el.second);

	std::sort(m_player_order.begin(), m_player_order.end(), GrandZodiacBase::sort_grand_zodiac_rank_place);

	uint32_t position = 1u;
	uint32_t score = (uint32_t)~0u;

	PlayerGrandZodiacInfo *pgzi = nullptr;

	// Calcula posi��es, o Grand Zodiac tem player com a mesma posi��o se eles terminarem com o mesmo score
	for (auto& el : m_player_order) {

		if (el->flag != PlayerGameInfo::eFLAG_GAME::QUIT && (pgzi = reinterpret_cast< PlayerGrandZodiacInfo* >(el)) != nullptr) {

			if (score == ~0u) {

				pgzi->m_gz.position = position;

				score = pgzi->m_gz.total_score;
			
			}else if (score == pgzi->m_gz.total_score)
				pgzi->m_gz.position = position;
			else
				pgzi->m_gz.position = ++position;
		}
	}
}

void GrandZodiacBase::requestReplySyncShotData(player& _session) {
	CHECK_SESSION_BEGIN("requestReplySyncShotData");

	try {

		INIT_PLAYER_INFO("requestReplySyncShotData", "tentou enviar a resposta do Sync Shot do jogo", &_session);

		// Resposta Sync Shot
		sendSyncShot(_session);

		// Deixai assim por que o Original manda a msg depois, pelo que estava no outro server
		drawDropItem(_session);

		// Aqui seta o state e verifica se � para mandar a resposta
		if (pgi->m_sync_shot_gz.setStateAndCheckAllAndClear(SyncShotGrandZodiac::eSYNC_SHOT_GRAND_ZODIAC_STATE::SSGZS_FIRST_SHOT_SYNC))
			sendReplyInitShotAndSyncShot(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::requestReplySyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::sendPlacar(player& _session) {

	try {

		INIT_PLAYER_INFO("sendPlacar", "tentou enviar o placar do jogo", &_session);

		packet p((unsigned short)0x1F3);

		p.addUint32((pgi->flag == PlayerGameInfo::eFLAG_GAME::FINISH) ? 1 : 2); // 1 Terminou, 2 Saiu

		p.addUint32(getCountPlayersGame());

		PlayerGrandZodiacInfo *pgzi = nullptr;

		for (auto& el : m_player_info) {

			if (el.second != nullptr && el.second->flag != PlayerGameInfo::eFLAG_GAME::QUIT 
					&& (pgzi = reinterpret_cast< PlayerGrandZodiacInfo* >(el.second)) != nullptr) {

				p.addUint32(pgzi->oid);
				p.addUint32(pgzi->m_gz.position);
				p.addUint32(pgzi->m_gz.total_score);
				p.addUint32(pgzi->m_gz.hole_in_one);
				p.addUint32(pgzi->data.total_tacada_num);
				p.addUint32(pgzi->m_gz.pontos);
				p.addUint32(pgzi->data.exp);
				p.addUint64(pgzi->data.pang);
				p.addUint64(pgzi->data.bonus_pang);
				p.addUint64(pgzi->m_gz.jackpot);
				p.addUint32(pgzi->m_gz.trofeu);

				if (!pgzi->drop_list.v_drop.empty()) {

					p.addUint32((uint32_t)pgzi->drop_list.v_drop.size());	// Count

					for (auto& el2 : pgzi->drop_list.v_drop) {
						
						p.addUint32(el2._typeid);
						p.addUint32(el2.qntd);
					}

				}else
					p.addUint32(0u); // N�o ganhou drop item
			}
		}

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::sendPlacar][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::sendTimeIsOver(player& _session) {

	packet p((unsigned short)0x8C);

	packet_func::session_send(p, &_session, 1);
}

int GrandZodiacBase::checkEndShotOfHole(player& _session) {

	try {

		// Agora verifica o se ele acabou o hole e essas coisas
		INIT_PLAYER_INFO("checkEndShotOfHole", "tentou verificar a ultima tacada do hole no jogo", &_session);

		if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || pgi->data.giveup) {

			// Finish Hole and change
			finishHole(_session);

			changeHole(_session);
	
		}else // Update Shot
			updateFinishHole(_session, 0/*N�o fez hio*/);

		// Limpa, terminou a tacada
		pgi->m_gz.m_score_shot.clear();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::checkEndShotOfHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
}

void GrandZodiacBase::init_values_seed() {

	if (!m_initial_values_seed.empty())
		m_initial_values_seed.clear();

	std::srand((uint32_t)std::clock() * 777 * std::clock() * (uint32_t)(size_t)this);

	double var_d = ((std::rand() % 1000) / 100.f) + 1.f;

	for (auto i = 0u; i < 10u; ++i) {
		
		m_initial_values_seed.push_back((var_d + i * (((std::rand() % 10) / 10.f) + 1.f)));

		// Log
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::init_values_seed][Log] Seed da camera de quando comeca o Grand Zodiac. Value(" 
				+ std::to_string(m_initial_values_seed.back()) + ")", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
	}
}

void GrandZodiacBase::nextHole(player& _session) {

	try {

		INIT_PLAYER_INFO("nextHole", "tentou trocar o hole no jogo", &_session);

		packet p((unsigned short)0x1F4);

		p.addUint32(1u); // Inicializa o pr�ximo hole

		packet_func::session_send(p, &_session, 1);

		// Wind
		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			throw exception("[GrandZodiacBase::requestInitHole][Error] course->findHole nao encontrou o hole retonou nullptr, o server esta com erro no init course do Chip-in Practice.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHIP_IN_PRACTICE, 2555, 0));

		auto wind = m_course->shuffleWind(std::clock());

		hole->setWind(wind);

		// Gera degree para o player ou pega o degree sem gerar que � do modo do hole repeat
		pgi->degree = hole->getWind().degree.getShuffleDegree();

		auto wind_flag = initCardWindPlayer(pgi, hole->getWind().wind);

		// Resposta do vento do hole
		p.init_plain((unsigned short)0x5B);

		p.addUint8(hole->getWind().wind + wind_flag);
		p.addUint8((wind_flag < 0) ? 1 : 0);	// Flag de card de vento, aqui � a qnd diminui o vento, 1 Vento azul
		p.addUint16(pgi->degree);
		p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original

		packet_func::session_send(p, &_session, 1);

		// Remain Time em segundos
		auto remain_time = 0ull;

		if (m_timer != nullptr)
			remain_time = m_timer->getElapsed();

		if (remain_time > 0)
			remain_time /= 1000/*Milli por segundos*/;

		p.init_plain((unsigned short)0x200);

		p.addUint32((uint32_t)remain_time);

		packet_func::session_send(p, &_session, 1);

		// Update, finaliza o hole
		updateFinishHole(_session, 1/*Fez HIO*/);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::nextHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiacBase::setInitFirstHole(PlayerGrandZodiacInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setInitFirstHole][Error] PlayerGrandZodiacInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->init_first_hole_gz = 1u;

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

bool GrandZodiacBase::checkAllInitFirstHole() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("checkAllInitFirstHole", "tentou verificar se todos os player terminaram de inicializar o primeiro hole do Grand Zodiac no jogo", _el);

			if (pgi->init_first_hole_gz)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::checkAllInitFirstHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void GrandZodiacBase::clearInitFirstHole() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_init_first_hole();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool GrandZodiacBase::setInitFirstHoleAndCheckAllInitFirstHoleAndClear(PlayerGrandZodiacInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setInitFirstHoleAndCheckAllInitFirstHoleAndClear][Error] PlayerGrandZodiacInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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
	_pgi->init_first_hole_gz = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("setInitFirstHoleAndCheckAllInitFirstHoleAndClear", "tentou verificar se todos os player terminaram de inicializar o primeiro hole do Grand Zodiac no jogo", _el);

			if (pgi->init_first_hole_gz)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setInitFirstHoleAndCheckAllInitFirstHoleAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_init_first_hole();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void GrandZodiacBase::setEndGame(PlayerGrandZodiacInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setEndGame][Error] PlayerGrandZodiacInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->end_game = 1u;

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

bool GrandZodiacBase::checkAllEndGame() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("checkAllEndGame", "tentou verificar se todos os player terminaram o jogo no Grand Zodiac", _el);

			if (pgi->end_game)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::checkAllEndGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void GrandZodiacBase::clearEndGame() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_end_game();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool GrandZodiacBase::setEndGameAndCheckAllEndGameAndClear(PlayerGrandZodiacInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setEndGameAndCheckAllEndGameAndClear][Error] PlayerGrandZodiacInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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
	_pgi->end_game = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("setEndGameAndCheckAllEndGameAndClear", "tentou verificar se todos os player terminaram o jogo no Grand Zodiac", _el);

			if (pgi->end_game)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setInitFirstHoleAndCheckAllInitFirstHoleAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_end_game();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void GrandZodiacBase::clear_all_init_first_hole() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_init_first_hole", " tentou limpar all init first hole do Grand Zodiac no jogo", _el);

			pgi->init_first_hole_gz = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::clear_all_init_first_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void GrandZodiacBase::clear_all_end_game() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_end_game", " tentou limpar all end game do Grand Zodiac", _el);

			pgi->end_game = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::clear_all_end_game][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void GrandZodiacBase::sendReplyInitShotAndSyncShot(player& _session) {

	try {

		INIT_PLAYER_INFO("sendReplyInitShotAndSyncShot", "tentou enviar a resposta da tacada do player no Grand Zodiac", &_session);

		if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || pgi->data.giveup) {

			// Set Player no golden beam map se j� estiver no tempo do golden beam e se ele n�o estiver no map
#if defined(_WIN32)
			if (InterlockedCompareExchange(&m_golden_beam_state, 1u, 1u) == 1u)
#elif defined(__linux__)
			uint32_t check_m = 1u; // Compare
			if (__atomic_compare_exchange_n(&m_golden_beam_state, &check_m, 1u, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
#endif
				setPlayerGoldenBeam(_session);

			if (pgi->shot_sync.state_shot.shot.stShot.cobra || pgi->shot_sync.state_shot.shot.stShot.tomahawk || pgi->shot_sync.state_shot.shot.stShot.spike) {

				pgi->m_gz.m_score_shot.push_back(eGRAND_ZODIAC_TYPE_SHOT::GZTS_SPECIAL_SHOT);

				pgi->data.score++;
			}

			if (pgi->data.tacada_num == 1) {

				pgi->m_gz.m_score_shot.push_back(eGRAND_ZODIAC_TYPE_SHOT::GZTS_FIRST_SHOT);

				pgi->data.score++;
			}

			// Sem setas, se ele n�o mandou nenhum special shot ou spin ou curva ele n�o apertou setas, s� as apagadas essas n�o conta
			if (pgi->shot_data.special_shot.ulSpecialShot == 0u /*N�o mandou, spin, curva, toma, cobra e spike*/) {

				pgi->m_gz.m_score_shot.push_back(eGRAND_ZODIAC_TYPE_SHOT::GZTS_WITHOUT_COMMANDS);

				pgi->data.score++;
			}

			if (pgi->shot_data.acerto_pangya_flag == 3/*Errou PangYa*/) {

				pgi->m_gz.m_score_shot.push_back(eGRAND_ZODIAC_TYPE_SHOT::GZTS_MISS_PANGYA);

				pgi->data.score += 3;
			}

			// O score que o player fez hio
			pgi->m_gz.m_score_shot.insert(pgi->m_gz.m_score_shot.begin(), eGRAND_ZODIAC_TYPE_SHOT::GZTS_HIO_SCORE);

			pgi->data.score++; // Add +1 que � da tacada que ele deu, que ele fez hio

			// S� envia se tiver mais que 1
			if (pgi->m_gz.m_score_shot.size() > 1) {

				// Send Scores para o player
				packet p((unsigned short)0x1F5);

				p.addUint32((uint32_t)pgi->m_gz.m_score_shot.size());

				for (auto& el : pgi->m_gz.m_score_shot)
					p.addUint32(el);

				packet_func::session_send(p, &_session, 1);
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::sendReplyInitShotAndSyncShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

PlayerGameInfo* GrandZodiacBase::makePlayerInfoObject(player& _session) {
	
	auto pgzi = new PlayerGrandZodiacInfo{ 0 };

	try {

		CHECK_SESSION_BEGIN("makePlayerInfoObject");

		// Aqui se eu precisar inicializar algum valor

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::makePlayerInfoObject][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return pgzi;
}

void GrandZodiacBase::setPlayerGoldenBeam(player& _session) {

	try {

		// N�o tem o player no map add
		if (m_mp_golden_beam_player.find(&_session) == m_mp_golden_beam_player.end())
			m_mp_golden_beam_player.insert(std::make_pair(&_session, true));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::setPlayerGoldenBeam][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

#if defined(_WIN32)
DWORD WINAPI GrandZodiacBase::_syncFirstHole(LPVOID lpParameter) {
#elif defined(__linux__)
void* GrandZodiacBase::_syncFirstHole(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(GrandZodiacBase);

	result = pTP->syncFirstHole();

	END_THREAD_SETUP("syncFirstHole");
}

#if defined(_WIN32)
DWORD GrandZodiacBase::syncFirstHole() {
#elif defined(__linux__)
void* GrandZodiacBase::syncFirstHole() {
#endif

	try {

		// Log
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][Log] syncFirstHole iniciado com sucesso!", CL_FILE_LOG_AND_CONSOLE));

		packet p;

		DWORD retWait = WAIT_TIMEOUT;

#if defined(_WIN32)
		HANDLE wait_events[2] = { m_hEvent_sync_hole, m_hEvent_sync_hole_pulse };

		while ((retWait = WaitForMultipleObjects((sizeof(wait_events) / sizeof(wait_events[0])), wait_events, false, 1000/*1 segundo*/)) == WAIT_TIMEOUT || retWait == (WAIT_OBJECT_0 + 1)) {
#elif defined(__linux__)
		std::vector< Event* > wait_events = { m_hEvent_sync_hole, m_hEvent_sync_hole_pulse };

		while ((retWait = Event::waitMultipleEvent(wait_events.size(), wait_events, false, 1000/*1 segundo*/)) == WAIT_TIMEOUT || retWait == (WAIT_OBJECT_0 + 1)) {
#endif

			try {

				m_state_gz.lock();

				switch (m_state_gz.getState()) {
					case eSTATE_GRAND_ZODIAC_SYNC::FIRST_HOLE:
					{

						if (checkAllInitFirstHole()) {

							clearInitFirstHole();

							// Come�a o tempo do jogo
							startTime();

							for (auto& el : m_players) {

								if (el != nullptr) {

									sendRemainTime(*el);

									// Resposta passa o oid do player que vai come�a o Hole
									p.init_plain((unsigned short)0x53);

									p.addUint32(el->m_oid);

									packet_func::session_send(p, el, 1);

									// Passa a localiza��o do player, esse � a primeira, vez ent�o passa os valores zerados
									updateFinishHole(*el, 1/*Come�ou o hole*/);

									// Come�a o Grand Zodiac
									p.init_plain((unsigned short)0x1F4);

									p.addUint32(1u); // Start

									packet_func::session_send(p, el, 1);
								}
							}

							// Verifica o tempo do start golden beam
							m_state_gz.setState(eSTATE_GRAND_ZODIAC_SYNC::START_GOLDEN_BEAM);
						}

						break;
					}
					case eSTATE_GRAND_ZODIAC_SYNC::START_GOLDEN_BEAM:
					{

						if (m_timer != nullptr) {

							auto elapsed = m_timer->getElapsed();

							if (elapsed >= (m_ri.time_30s - 60000)) {

								// Come�a o tempo do golden beam time
								startGoldenBeam();

								// Verifica o tempo do end golden beam
								m_state_gz.setState(eSTATE_GRAND_ZODIAC_SYNC::END_GOLDEN_BEAM);
							}
						}

						break;
					}
					case eSTATE_GRAND_ZODIAC_SYNC::END_GOLDEN_BEAM:
					{
						if (m_timer != nullptr) {

							auto elapsed = m_timer->getElapsed();

							if (elapsed >= (m_ri.time_30s - 30000)) {

								// Terminar o golden beam
								endGoldenBeam();

								m_state_gz.setState(eSTATE_GRAND_ZODIAC_SYNC::WAIT_END_GAME);
							}
						}

						break;
					}
					case eSTATE_GRAND_ZODIAC_SYNC::LOAD_HOLE:
					{
						break; // Faz nada por enquanto
					}
					case eSTATE_GRAND_ZODIAC_SYNC::LOAD_CHAR_INTRO:
					{
						break; // Faz nada por enquanto
					}
					case eSTATE_GRAND_ZODIAC_SYNC::END_SHOT:
					{
						break; // Faz nada por enquanto
					}
					case eSTATE_GRAND_ZODIAC_SYNC::WAIT_END_GAME:
					{
						
						if (checkAllEndGame()) {

							clearEndGame();

							// Log
							_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][Log] Grand Zodiac Game End.", CL_FILE_LOG_AND_CONSOLE));
						}

						break;
					}
				}

				// Libera
				m_state_gz.unlock();

			}catch (exception& e) {

				m_state_gz.unlock();

				_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}
		
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[GrandZodiacBase::syncFirstHole][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][ErrorSystem] syncFirstHole() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE));
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][Log] Saindo de syncHoleTime()...", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::syncFirstHole][Log] Saindo de syncHoleTime()...", CL_ONLY_FILE_LOG));
#endif // _DEBUG

#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return (void*)0;
#endif
}

void GrandZodiacBase::finish_thread_sync_first_hole() {

	try {
		
		if (m_thread_sync_first_hole != nullptr) {

#if defined(_WIN32)
			if (m_hEvent_sync_hole != INVALID_HANDLE_VALUE)
				SetEvent(m_hEvent_sync_hole);
#elif defined(__linux__)
			if (m_hEvent_sync_hole != nullptr)
				m_hEvent_sync_hole->set();
#endif

			m_thread_sync_first_hole->waitThreadFinish(INFINITE);

			delete m_thread_sync_first_hole;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacBase::finish_thread_sync_first_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (m_thread_sync_first_hole != nullptr) {

			m_thread_sync_first_hole->exit_thread();

			delete m_thread_sync_first_hole;
		}
	}

	m_thread_sync_first_hole = nullptr;

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
