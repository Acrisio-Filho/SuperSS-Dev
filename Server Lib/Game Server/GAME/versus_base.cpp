// Arquivo versus_base.cpp
// Criado em 20/10/2018 as 15:31 por Acrisio
// Implementação da classe VersusBase

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "versus_base.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PACKET/packet_func_sv.h"

#include "item_manager.h"

#include <algorithm>

#include "../Game Server/game_server.h"

#include "../UTIL/map.hpp"

#include "treasure_hunter_system.hpp"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

#if defined(__linux__)
#include <numbers> // pi
#endif

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[VersusBase::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[VersusBase::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[VersusBase::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 4)); \

using namespace stdA;

VersusBase::VersusBase(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie) 
	: Game(_players, _ri, _rv, _channel_rookie), m_player_turn(0u), m_flag_next_step_game(0u), m_thi{0}, m_seed_mascot_effect(0u), m_count_pause(0u)
	  ,m_state_vs(), m_thread_chk_turn(nullptr), 
#if defined(_WIN32)
	  m_hEvent_chk_turn(INVALID_HANDLE_VALUE), m_hEvent_chk_turn_pulse(INVALID_HANDLE_VALUE) 
#elif defined(__linux__)
	  m_hEvent_chk_turn(nullptr), m_hEvent_chk_turn_pulse(nullptr) 
#endif
{

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_sync_shot);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	m_seed_mascot_effect = (uint32_t)(sRandomGen::getInstance().rIbeMt19937_64_chrono()) & 0xFFFF;/*Teste pode ser que seja __int16*/

#if defined(_WIN32)
	if ((m_hEvent_chk_turn = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[VersusBase::VersusBase][Error] ao criar o evento check versus turn.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1050, GetLastError()));
#elif defined(__linux__)

	m_hEvent_chk_turn = new Event(true, 0u);

	if (!m_hEvent_chk_turn->is_good()) {

		delete m_hEvent_chk_turn;

		m_hEvent_chk_turn = nullptr;

		throw exception("[VersusBase::VersusBase][Error] ao criar o evento check versus turn.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1050, errno));
	}
#endif

#if defined(_WIN32)
	if ((m_hEvent_chk_turn_pulse = CreateEvent(NULL, false, false, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[VersusBase::VersusBase][Error] ao criar o evento check versus turn pulse.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1050, GetLastError()));
#elif defined(__linux__)

	m_hEvent_chk_turn_pulse = new Event(false, 0u);

	if (!m_hEvent_chk_turn_pulse->is_good()) {

		delete m_hEvent_chk_turn_pulse;

		m_hEvent_chk_turn_pulse = nullptr;

		throw exception("[VersusBase::VersusBase][Error] ao criar o evento check versus turn pulse.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1050, errno));
	}
#endif

	m_thread_chk_turn = new thread(1050/*Check Versus*/, VersusBase::_checkVersusTurn, (LPVOID)this);
}

VersusBase::~VersusBase() {
	clear_treasure_hunter();

	try {
		
		if (m_thread_chk_turn != nullptr) {

#if defined(_WIN32)
			if (m_hEvent_chk_turn != INVALID_HANDLE_VALUE)
				SetEvent(m_hEvent_chk_turn);
#elif defined(__linux__)
			if (m_hEvent_chk_turn != nullptr)
				m_hEvent_chk_turn->set();
#endif

			m_thread_chk_turn->waitThreadFinish(INFINITE);

			delete m_thread_chk_turn;
		}

	} catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::~VersusBase][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (m_thread_chk_turn != nullptr) {

			m_thread_chk_turn->exit_thread();

			delete m_thread_chk_turn;
		}
	}

	m_thread_chk_turn = nullptr;

#if defined(_WIN32)
	if (m_hEvent_chk_turn != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent_chk_turn);

	if (m_hEvent_chk_turn_pulse != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent_chk_turn_pulse);

	m_hEvent_chk_turn = INVALID_HANDLE_VALUE;
	m_hEvent_chk_turn_pulse = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	if (m_hEvent_chk_turn != nullptr)
		delete m_hEvent_chk_turn;

	if (m_hEvent_chk_turn_pulse != nullptr)
		delete m_hEvent_chk_turn_pulse;

	m_hEvent_chk_turn = nullptr;
	m_hEvent_chk_turn_pulse = nullptr;
#endif

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs_sync_shot);
#endif
}

void VersusBase::sendRatesOfVersusBase() {

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

void VersusBase::sendReplyFinishCharIntro() {

	// Resposta para Finish Char Intro
	packet p((unsigned short)0x90);

	packet_func::game_broadcast(*this, p, 1);
}

void VersusBase::sendPlayerTurn() {

	if (m_player_turn == nullptr)
		throw exception("[VersusBase::sendPlayerTurn][Error] PlayerGameInfo *m_player_turn is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 100, 1));

	auto hole = m_course->findHole(m_player_turn->hole);

	if (hole == nullptr)
		throw exception("[VersusBase::sendPlayerTurn][Error] player[UID=" + std::to_string(m_player_turn->uid) + "] tentou encontrar o hole[NUMERO="
				+ std::to_string(m_player_turn->hole) + "] do course no jogo, mas nao foi encontrado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 101, 0));

	auto wind_flag = initCardWindPlayer(m_player_turn, hole->getWind().wind);

	// Resposta do vento do hole
	packet p((unsigned short)0x5B);

	p.addUint8(hole->getWind().wind + wind_flag);
	p.addUint8((wind_flag < 0) ? 1u : 0u);	// Flag de card de vento, aqui é a qnd diminui o vento, 1 Vento azul
	p.addUint16(m_player_turn->degree);
	p.addUint8(1u/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original, , Também é flag para trocar o vento no Pang Battle se mandar o valor 0

	packet_func::game_broadcast(*this, p, 1);

	// Resposta passa o oid do player que vai começa o Hole
	p.init_plain((unsigned short)0x63);

	if (m_player_turn == nullptr) {
		_smp::message_pool::getInstance().push(new message("[VersusBase::sendPlayerTurn][Error] player_turn is invalid(nullptr)", CL_FILE_LOG_AND_CONSOLE));

		p.addUint32(0);
	}else
		p.addUint32(m_player_turn->oid);

	// Aqui tem 2 bytes a+, int16 de um valor, que acho que acontece de ver em quando, ou só no pang battle isso estava escrito no meu outro
	// acho que possa ser do pang battle, certaza, aqui acabei de ver na classe pang battle do antigo

	packet_func::game_broadcast(*this, p, 1);
}

void VersusBase::changeTurn() {

	if (m_player_turn == nullptr)
		throw exception("[VersusBase::changeTurn][Error] PlayerGameInfo *m_player_turn is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 100, 0));

	// Para o tempo do player do turno
	stopTime();

	// Check Player Turn finish last hole
	if (m_player_turn->shot_sync.state_shot.display.stDisplay.acerto_hole || m_player_turn->data.giveup) {

		// Verifica se o player terminou jogo, fez o ultimo hole
		if (m_course->findHoleSeq(m_player_turn->hole) == m_ri.qntd_hole) {

			// Resposta para o player que terminou o ultimo hole do Game
			packet p((unsigned short)0x199);

			packet_func::game_broadcast(*this, p, 1);

			// Fez o Ultimo Hole, Calcula Clear Bonus para o player
			if (m_player_turn->shot_sync.state_shot.display.stDisplay.clear_bonus) {

				if (!sMap::getInstance().isLoad())
					sMap::getInstance().load();

				auto map = sMap::getInstance().getMap(m_ri.course & 0x7F);

				if (map == nullptr)
					_smp::message_pool::getInstance().push(new message("[VersusBase::changeTurn][Error][WARNING] tentou pegar o Map dados estaticos do course[COURSE="
							+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "], mas nao conseguiu encontra na classe do Server.", CL_FILE_LOG_AND_CONSOLE));
				else
					m_player_turn->data.bonus_pang += sMap::getInstance().calculeClearVS(*map, (uint32_t)m_players.size(), m_ri.qntd_hole);
			}
		}
	}

	// Limpa dados que usa para cada tacada
	clearDataEndShot(m_player_turn);

	// Verifica se todos fizeram o hole
	//if (checkAllClearHoleAndClear()) {
	if (checkAllClearHole()) {

		clear_all_flag_sync();
		
		finishHole();

		// Utilizo ele antes no finish hole, limpo ele aqui depois
		clearAllClearHole();

		changeHole();
	
	}else if (m_players.size() == 1 && m_course->findHoleSeq(m_player_turn->hole) < 4) {	// Finaliza o game

		clear_all_flag_sync();

		finishHole();

		changeHole();

	}else {	// Troca o Turno

		clear_all_flag_sync();

		// Recalcula Turno
		requestCalculePlayerTurn();

		// Cnvia para todos o vento e oid do player turn, o player que vai tacar nesse momento
		sendPlayerTurn();
	}
}

void VersusBase::CCGChangeWind(player& _gm, unsigned char _wind, unsigned short _degree) {

	try {

		if (m_player_turn == nullptr)
			throw exception("[VersusBase::CCGChangeWind][Error] player[UID=" + std::to_string(_gm.m_pi.uid) + "] tentou executar o comando de troca de vento no versus na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o player_turn do versus eh invalido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 0x5700100));

		auto hole = m_course->findHole(m_player_turn->hole);

		if (hole == nullptr)
			throw exception("[VersusBase::CCGChangeWind][Error] player[UID=" + std::to_string(_gm.m_pi.uid) + "] tentou executar o comando de troca de vento no versus na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o nao encontrou o hole[VALUE=" + std::to_string((short)m_player_turn->hole) + "] no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 2, 0x5700100));

		auto wind = hole->getWind();

		// Change Wind of Hole
		wind.wind = _wind;

		hole->setWind(wind);

		// Change Degree of player
		m_player_turn->degree = _degree % LIMIT_DEGREE;

		// Log
		_smp::message_pool::getInstance().push(new message("[VersusBase::CCGChangeWind][Log] [GM] player[UID=" + std::to_string(_gm.m_pi.uid) + "] trocou o vento e graus da sala[NUMERO="
				+ std::to_string(m_ri.numero) + ", VENTO=" + std::to_string((unsigned short)_wind + 1) + ", GRAUS=" + std::to_string(_degree) + "]", CL_FILE_LOG_AND_CONSOLE));

		auto wind_flag = initCardWindPlayer(m_player_turn, hole->getWind().wind);

		// UPDATE ON GAME
		packet p((unsigned short)0x5B);

		p.addUint8(hole->getWind().wind + wind_flag);	// Wind
		p.addUint8((wind_flag < 0) ? 1 : 0);			// Card Wind Flag, minus wind flag
		p.addUint16(m_player_turn->degree);				// Degree
		p.addUint8(1);									// Flag 1 = Reset Degree, 0 = Plus Degree, , Também é flag para trocar o vento no Pang Battle se mandar o valor 0

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::CCGChangeWind][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}
}

int VersusBase::checkEndShotOfHole(player& _session) {

	// Agora verifica o se ele acabou o hole e essas coisas
	INIT_PLAYER_INFO("checkEndShotOfHole", "tentou verificar a ultima tacada do hole no jogo", &_session);
	
	//pgi->finish_shot = 1u;

	if (pgi->data.bad_condute >= 3)
		return 2;	// Tira da sala
	else
		setFinishShot(pgi);

	// @@!!
	//if (setFinishShotAndCheckAllFinishShotAndClear(pgi)) {
	//	//clear_all_finish_shot();

	//	if (m_flag_next_step_game != 0) {

	//		if (m_flag_next_step_game == 1) {

	//			packet p((unsigned short)0x92);

	//			packet_func::session_send(p, &_session, 1);

	//			m_flag_next_step_game = 0u;

	//		}else if (m_flag_next_step_game == 2) {

	//			// Finaliza o Game
	//			return 1;
	//		}

	//	}else if (m_player_turn->data.bad_condute >= 3) {	// Kika player deu 3 give up

	//		// Tira o player da sala
	//		return 2;

	//	}else // Troca o Turno ou troca de hole ou termina o game
	//		changeTurn();
	//}

	return 0;
}

void VersusBase::drawDropItem(player& _session) {

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

			packet_func::game_broadcast(*this, p, 1);
		}
	}
}

void VersusBase::init_turn_hole_start() {

	if (!m_player_order.empty())
		m_player_order.clear();

	for (auto& el : m_players) {
		
		if (el != nullptr) {

			INIT_PLAYER_INFO("init_turn_hole_start", " tentou calcular o player do turno do comeco do hole no jogo", el);

			if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
				m_player_order.push_back(pgi);
		}
	}

	std::sort(m_player_order.begin(), m_player_order.end(), VersusBase::sort_player_turn_hole_start);
}

PlayerGameInfo* VersusBase::getNextPlayerTurnHole() {

	PlayerGameInfo *pgi = nullptr;

	if (!m_player_order.empty()) {

		pgi = *m_player_order.begin();

		m_player_order.erase(m_player_order.begin());

		if (pgi == nullptr || pgi->flag == PlayerGameInfo::eFLAG_GAME::QUIT)
			return getNextPlayerTurnHole();
	}

	return pgi;
}

PlayerGameInfo* VersusBase::requestCalculePlayerTurn() {

	if ((m_player_turn = getNextPlayerTurnHole()) != nullptr)
		return m_player_turn;

	if (!m_player_info.empty()) {

		auto hole = m_course->findHole(m_player_info.begin()->second->hole);

		if (hole == nullptr) {
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestCalculePlayerTurn][Error] player[UID=" + std::to_string(m_player_info.begin()->second->uid) + "] o hole[NUMERO="
					+ std::to_string(m_player_info.begin()->second->hole) + "] nao foi encontrado no course. Bug", CL_FILE_LOG_AND_CONSOLE));
			
			m_player_turn = nullptr;

			return nullptr;
		}

		std::vector< PlayerOrderTurnCtx > v_player_order_turn;

		for (auto& el : m_players) {
			
			if (el != nullptr) {

				INIT_PLAYER_INFO("requestCalculePlayerTurn", " tentou calcular o player do turno no jogo", el);

				if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
					v_player_order_turn.push_back({ pgi, hole });
			}
		}
		
		if (v_player_order_turn.empty()) {
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestCalculePlayerTurn][Error] nao tem players, para calcular o turno. Bug", CL_FILE_LOG_AND_CONSOLE));

			m_player_turn = nullptr;

			return nullptr;
		}

		std::sort(v_player_order_turn.begin(), v_player_order_turn.end(), VersusBase::sort_player_turn);

		m_player_turn = v_player_order_turn.begin()->pgi;
	}

	return m_player_turn;
}

void VersusBase::clear_treasure_hunter() {

	m_thi.clear();
}

bool VersusBase::sort_player_turn_hole_start(PlayerGameInfo* _pgi1, PlayerGameInfo* _pgi2) {

	if ((_pgi1->progress.hole - 1) < 0)
		return false;

	auto index = _pgi1->progress.hole - 1;

	if (_pgi1->progress.score[index] < _pgi2->progress.score[index])
		return true;
	else if (_pgi1->progress.score[index] == _pgi2->progress.score[index] && _pgi1->data.pang > _pgi2->data.pang)
		return true;
	else if (_pgi1->progress.score[index] == _pgi2->progress.score[index] && _pgi1->data.pang == _pgi2->data.pang && _pgi1->data.score < _pgi2->data.score)
		return true;

	return false;
}

bool VersusBase::sort_player_turn(PlayerOrderTurnCtx& _potc1, PlayerOrderTurnCtx& _potc2) {

	auto diff = _potc1.hole->getPinLocation().diffXZ(_potc1.pgi->location);
	auto diff2 = _potc1.hole->getPinLocation().diffXZ(_potc2.pgi->location);

	if (!_potc1.pgi->shot_sync.state_shot.display.stDisplay.acerto_hole && (_potc2.pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || _potc2.pgi->data.giveup))
		return true;
	else if (_potc1.pgi->data.tacada_num == 0 && _potc2.pgi->data.tacada_num > 0)
		return true;
	else if (diff > diff2 && !_potc1.pgi->shot_sync.state_shot.display.stDisplay.acerto_hole && !_potc1.pgi->data.giveup)
		return true;
	else if (diff == diff2 && _potc1.pgi->data.tacada_num < _potc2.pgi->data.tacada_num && !_potc1.pgi->shot_sync.state_shot.display.stDisplay.acerto_hole && !_potc1.pgi->data.giveup)
		return true;
	else if (diff == diff2 && _potc1.pgi->data.tacada_num == _potc2.pgi->data.tacada_num && _potc1.pgi->data.pang > _potc2.pgi->data.pang 
			&& !_potc1.pgi->shot_sync.state_shot.display.stDisplay.acerto_hole && !_potc1.pgi->data.giveup)
		return true;

	return false;
}

int VersusBase::end_time(void* _arg1, void* _arg2) {

	auto game = reinterpret_cast< VersusBase* >(_arg1);

	try {
		
		// Tempo Acabou
		game->timeIsOver(_arg2);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::end_time][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
}

#if defined(_WIN32)
DWORD VersusBase::_checkVersusTurn(LPVOID lpParameter) {
#elif defined(__linux__)
void* VersusBase::_checkVersusTurn(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(VersusBase);

	result = pTP->checkVersusTurn();
	
	END_THREAD_SETUP("checkVersusTurn");
}

#if defined(_WIN32)
DWORD VersusBase::checkVersusTurn() {
#elif defined(__linux__)
void* VersusBase::checkVersusTurn() {
#endif
	
	try {

		_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] checkVersusTurn iniciado com sucesso!"));
		
		// Begin QueryPerformanceQuery Initialization
#if defined(_WIN32)
		LARGE_INTEGER frequency;
#elif defined(__linux__)
		timespec frequency;
#endif

		constexpr uint64_t frequency_to_microsecond = 1000000ull;
		constexpr int64_t five_second_in_microsecond = 5000000ll;
		constexpr int64_t ten_second_in_microsecond = 10000000ll;

#if defined(_WIN32)
		if (!QueryPerformanceFrequency(&frequency))
			_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] nao conseguiu pegar QueryPerformanceFrequency. Bug", CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
		if (clock_getres(CLOCK_MONOTONIC_RAW, &frequency) == -1)
			_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] nao conseguiu pegar clock_getres. Bug", CL_FILE_LOG_AND_CONSOLE));
#endif

		// End QueryPerformanceQuery Initialization

		DWORD retWait = WAIT_TIMEOUT;

#if defined(_WIN32)
		HANDLE wait_events[2] = { m_hEvent_chk_turn, m_hEvent_chk_turn_pulse };
		
		while ((retWait = WaitForMultipleObjects((sizeof(wait_events) / sizeof(wait_events[0])), wait_events, false, 1000/*1 segundo*/)) == WAIT_TIMEOUT || retWait == (WAIT_OBJECT_0 + 1)) {
#elif defined(__linux__)
		std::vector< Event* > wait_events = { m_hEvent_chk_turn, m_hEvent_chk_turn_pulse };
		
		while ((retWait = Event::waitMultipleEvent(wait_events.size(), wait_events, false, 1000/*1 segundo*/)) == WAIT_TIMEOUT || retWait == (WAIT_OBJECT_0 + 1)) {
#endif
			
			try {

				m_state_vs.lock();

				switch (m_state_vs.getState()) {
					case STATE_VERSUS::WAIT_HIT_SHOT:
					{
						if (m_flag_next_step_game != 0) {

							if (m_player_turn == nullptr)
								throw exception("[VersusBase::checkVersus][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 1201, 0));

							if (m_flag_next_step_game == 1 && m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT) {

								packet p((unsigned short)0x92);

								packet_func::game_broadcast(*this, p, 1);

							}else if (m_flag_next_step_game == 2) {

								// Finaliza o Game
								//return 1;
								_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Finaliza game.", CL_FILE_LOG_AND_CONSOLE));
							
							}else if (m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT)
								changeTurn();

							if (m_flag_next_step_game != 1)
								m_flag_next_step_game = 0u;

						}

						break;
					}
					case STATE_VERSUS::SHOTING:
					{

						if (checkAllSyncShot()) {

							if (m_player_turn == nullptr)
								throw exception("[VersusBase::checkVersus][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 1201, 0));

							auto _session_turn = findSessionByPlayerGameInfo(m_player_turn);

							if (_session_turn != nullptr)
								drawDropItem(*_session_turn);
							else
								_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersus][Error] PlayerGameInfo *m_player_turn[UID=" + std::to_string(m_player_turn->uid) +
										"], nao conseguiu encontrar o player dele no map de player_info. Bug", CL_FILE_LOG_AND_CONSOLE));

							// Resposta Sync Shot
							sendSyncShot();

							// Limpa a flag de init sync shot aqui, para evitar excessões lá em cima e não conseguir manda a resposta de sync shot
							clearSyncShot();

							// Espera o final da tacada
							m_state_vs.setState(STATE_VERSUS::END_SHOT);

						}else if (m_flag_next_step_game != 0) {

							if (m_player_turn == nullptr)
								throw exception("[VersusBase::checkVersus][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 1201, 0));

							if (m_flag_next_step_game == 1 && m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT) {

								packet p((unsigned short)0x92);

								packet_func::game_broadcast(*this, p, 1);

							}else if (m_flag_next_step_game == 2) {

								// Finaliza o Game
								//return 1;
								_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Finaliza game.", CL_FILE_LOG_AND_CONSOLE));
							
							}else if (m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT)
								changeTurn();

							if (m_flag_next_step_game != 1)
								m_flag_next_step_game = 0u;

						}else {	// Verifica o tempo dos players para ver se precisa pedir para o cliente enviar o pacote novamente

#if defined(_WIN32)
							LARGE_INTEGER current;
#elif defined(__linux__)
							timespec current;
#endif

							int64_t diff_time = 0ll;

							for (auto s : m_players) {

								INIT_PLAYER_INFO("checkVersusTurn", "Verifica se o tempo do player para responder com o pacote1B sync Shot ja deu 5 segundos, e pede para ele enviar novamente que nao recebeu", s);

								try {

#if defined(_WIN32)
									EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
									pthread_mutex_lock(&m_cs_sync_shot);
#endif

									// Só entra se o cliente recebeu o pacote55 e ainda não respondeu com o pacote1B
									if (pgi->sync_shot_flag2 == 0u && pgi->tick_sync_shot.active) {

#if defined(_WIN32)
										if (!QueryPerformanceCounter(&current))
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][checkTimeShot][Error] Player[UID=" + std::to_string(s->m_pi.uid) 
													+ "] nao conseguiu pegar QueryPerformanceCounter. Bug", CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
										if (clock_gettime(CLOCK_MONOTONIC_RAW, &current) == -1)
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][checkTimeShot][Error] Player[UID=" + std::to_string(s->m_pi.uid) 
													+ "] nao conseguiu pegar QueryPerformanceCounter. Bug", CL_FILE_LOG_AND_CONSOLE));
#endif

#if defined(_WIN32)
										diff_time = ((current.QuadPart - pgi->tick_sync_shot.tick.QuadPart) * frequency_to_microsecond) / frequency.QuadPart;
#elif defined(__linux__)
										diff_time = DIFF_TICK_MICRO(current, pgi->tick_sync_shot.tick, frequency);
#endif

										if (diff_time > five_second_in_microsecond/*5 segundos*/) {
#ifdef _DEBUG
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
													+ "] nao enviou o pacote1B sync shot em 5 segundos. tempo decorrido " + std::to_string(diff_time) + " microsegundos.", CL_FILE_LOG_AND_CONSOLE));
#else
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
													+ "] nao enviou o pacote1B sync shot em 5 segundos. tempo decorrido " + std::to_string(diff_time) + " microsegundos.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

											// Terceira vez desconectar o player, 15 segundos e nenhum resposta dele
											if (++pgi->tick_sync_shot.count >= 3) {

												// Limpa o tick timer do player para não entrar mais aqui que ele foi derrubado
												pgi->tick_sync_shot.clear();

												// Derruba o socket do player com o shutdown
#if defined(_WIN32)
												shutdown(s->m_sock, SD_BOTH);
#elif defined(__linux__)
												shutdown(s->m_sock.fd, SD_BOTH);
#endif

												// Envia esse pacote para desconectar o player imadiatemente, por que o server demorar alguns segundos para reconhecer o shutdown
												packet p((unsigned short)0x8A);
												packet_func::session_send(p, s, 1);

												_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid) 
														+ "] passou 15 (3x de 5) segundos, e o player nao enviou o pacote1B, desconecta ele para nao derrubar os outros jogadores do jogo.", CL_FILE_LOG_AND_CONSOLE));
											
											}else {

												// Limpa o time dele para contar mais 5 segundos para reinviar o pacote8A novamente
												pgi->tick_sync_shot.active = 0u;
#if defined(_WIN32)
												pgi->tick_sync_shot.tick.QuadPart = 0ll;
#elif defined(__linux__)
												pgi->tick_sync_shot.tick = { 0u, 0u };
#endif

												// Pedindo para ele enviar o pacote1B sync shot novamente
												packet p((unsigned short)0x8A);

												packet_func::session_send(p, s, 1);
											}
										}

#ifdef _DEBUG
										_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
												+ "] tempo decorrido " + std::to_string(diff_time) + " microsegundos.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
									}

#if defined(_WIN32)
									LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
									pthread_mutex_unlock(&m_cs_sync_shot);
#endif

								}catch (exception& e) {

#if defined(_WIN32)
									LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
									pthread_mutex_unlock(&m_cs_sync_shot);
#endif

									_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

									// relança
									throw;
								}
							}
						}

						break;
					}
					case STATE_VERSUS::END_SHOT:
					{

						if (checkAllFinishShot()) {

							clearFinishShot();

							if (m_player_turn == nullptr)
								throw exception("[VersusBase::checkVersus][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 1201, 0));

							if (m_flag_next_step_game != 0) {

								if (m_flag_next_step_game == 1) {

									packet p((unsigned short)0x92);

									packet_func::game_broadcast(*this, p, 1);

								}else if (m_flag_next_step_game == 2) {

									// Finaliza o Game
									//return 1;
									_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Finaliza game.", CL_FILE_LOG_AND_CONSOLE));

									changeTurn();
								
								}else //if (m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT)
									changeTurn();

								m_flag_next_step_game = 0u;

							}else // Troca o Turno ou troca de hole ou termina o game
								changeTurn();

						}else if (m_flag_next_step_game != 0) {

							if (m_player_turn == nullptr)
								throw exception("[VersusBase::checkVersus][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 1201, 0));

							if (m_flag_next_step_game == 1 && m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT) {

								packet p((unsigned short)0x92);

								packet_func::game_broadcast(*this, p, 1);

							}else if (m_flag_next_step_game == 2) {

								// Finaliza o Game
								//return 1;
								_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Finaliza game.", CL_FILE_LOG_AND_CONSOLE));
							
							}else if (m_player_turn->flag == PlayerGameInfo::eFLAG_GAME::QUIT)
								changeTurn();

							if (m_flag_next_step_game != 1)
								m_flag_next_step_game = 0u;

						}else {	// Verifica o tempo dos players para ver se precisa pedir desconectar para não derrubar os outros jogadores

#if defined(_WIN32)
							LARGE_INTEGER current;
#elif defined(__linux__)
							timespec current;
#endif

							int64_t diff_time = 0ll;

							for (auto s : m_players) {

								INIT_PLAYER_INFO("checkVersusTurn", "Verifica se o tempo do player para responder com o pacote1C sync Finish Shot ja deu 10 segundos, se deu desconecta ele", s);

								try {

#if defined(_WIN32)
									EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
									pthread_mutex_lock(&m_cs_sync_shot);
#endif

									// Só entra se o server recebeu o pacote1C de qualquer jogador do jogo
									if (pgi->finish_shot2 == 0u && pgi->tick_sync_end_shot.active) {

#if defined(_WIN32)
										if (!QueryPerformanceCounter(&current))
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][checkTimeShot][Error] Player[UID=" + std::to_string(s->m_pi.uid) 
													+ "] nao conseguiu pegar QueryPerformanceCounter. Bug", CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
										if (clock_gettime(CLOCK_MONOTONIC_RAW, &current) == -1)
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][checkTimeShot][Error] Player[UID=" + std::to_string(s->m_pi.uid) 
													+ "] nao conseguiu pegar QueryPerformanceCounter. Bug", CL_FILE_LOG_AND_CONSOLE));
#endif

#if defined(_WIN32)
										diff_time = ((current.QuadPart - pgi->tick_sync_end_shot.tick.QuadPart) * frequency_to_microsecond) / frequency.QuadPart;
#elif defined(__linux__)
										diff_time = DIFF_TICK_MICRO(current, pgi->tick_sync_end_shot.tick, frequency);
#endif

										if (diff_time > ten_second_in_microsecond/*10 segundos*/) {
#ifdef _DEBUG
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
													+ "] nao enviou o pacote1C sync Finish shot em 10 segundos. tempo decorrido " + std::to_string(diff_time) + " microsegundos.", CL_FILE_LOG_AND_CONSOLE));
#else
											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
													+ "] nao enviou o pacote1C sync Finish shot em 10 segundos. tempo decorrido " + std::to_string(diff_time) + " microsegundos.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

											// Limpa para não entra mais nesse jogador que foi desconectado
											pgi->tick_sync_end_shot.clear();

											// Derruba o socket do player com o shutdown
#if defined(_WIN32)
											shutdown(s->m_sock, SD_BOTH);
#elif defined(__linux__)
											shutdown(s->m_sock.fd, SD_BOTH);
#endif

											// Envia esse pacote para desconectar o player imadiatemente, por que o server demorar alguns segundos para reconhecer o shutdown
											packet p((unsigned short)0x8A);
											packet_func::session_send(p, s, 1);

											_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
													+ "] passou 10 segundos, e o player nao enviou o pacote1C, desconecta ele para nao derrubar os outros jogadores do jogo.", CL_FILE_LOG_AND_CONSOLE));

										}

#ifdef _DEBUG
										_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][Log] Player[UID=" + std::to_string(s->m_pi.uid)
												+ "] tempo decorrido " + std::to_string(diff_time) + " microsegundos.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
									}

#if defined(_WIN32)
									LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
									pthread_mutex_unlock(&m_cs_sync_shot);
#endif

								}catch (exception& e) {

#if defined(_WIN32)
									LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
									pthread_mutex_unlock(&m_cs_sync_shot);
#endif

									_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

									// relança
									throw;
								}
							}
						}

						break;
					}
					case STATE_VERSUS::LOAD_HOLE:
					{

						if (checkAllLoadHole()) {

							clearLoadHole();

							// Resposta para o Finish Load Hole
							sendReplyFinishLoadHole();

							// Send Rates of VersusBase, Mascot, voice Club e etc
							sendRatesOfVersusBase();

							m_state_vs.setState(STATE_VERSUS::WAIT_HIT_SHOT);
						}

						break;
					}
					case STATE_VERSUS::WAIT_END_GAME:
					{

						// Faz nada só espera o versus terminar

						break;
					}
				}

				m_state_vs.unlock();

			}catch (exception& e) {

				m_state_vs.unlock();

				_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[VersusBase::checkVersusTurn][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		_smp::message_pool::getInstance().push(new message("[VersusBase::checkVersusTurn][ErrorSystem] checkVersusTurn() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE));
	}
		
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("Saindo de checkVersusTurn()...", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("Saindo de checkVersusTurn()...", CL_ONLY_FILE_LOG));
#endif // _DEBUG
	
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
}

void VersusBase::sendInitialData(player& _session) {

	packet p;

	try {

		// No Versus tem o Update Last 5 players play
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

			uint32_t st_i = 0u;

			std::vector< CardEquipInfoEx > v_card_equip_char_and_special;

			// Game Data Init
			p.init_plain((unsigned short)0x76);

			p.addUint8(m_ri.tipo_show);

			p.addUint8((unsigned char)m_players.size());

			for (auto& el : m_players) {

				// Member Info
				p.addInt16(m_ri.numero);
				p.addBuffer(&el->m_pi.mi, sizeof(MemberInfo));

				// User Info
				p.addUint32(el->m_pi.uid);
				p.addBuffer(&el->m_pi.ui, sizeof(UserInfo));

				// Trofel Info Current Season
				p.addBuffer(&el->m_pi.ti_current_season, sizeof(TrofelInfo));

				// User Equipped Item
				p.addBuffer(&el->m_pi.ue, sizeof(UserEquip));

				// Map Statistics Normal
				for (st_i = 0u; st_i < MS_NUM_MAPS; st_i++)
					p.addBuffer(&el->m_pi.a_ms_normal[st_i], sizeof(MapStatistics));

				// Map Statistics Natural
				for (st_i = 0u; st_i < MS_NUM_MAPS; st_i++)
					p.addBuffer(&el->m_pi.a_ms_natural[st_i], sizeof(MapStatistics));

				// Map Statistics Grand Prix
				for (st_i = 0u; st_i < MS_NUM_MAPS; st_i++)
					p.addBuffer(&el->m_pi.a_ms_grand_prix[st_i], sizeof(MapStatistics));

				for (size_t j = 0; j < 9/*season's*/; j++)
					// Map Statistics Normal
					for (st_i = 0u; st_i < MS_NUM_MAPS; st_i++)
						p.addBuffer(&el->m_pi.aa_ms_normal_todas_season[j][st_i], sizeof(MapStatistics));

				// Character Info(CharEquip)
				if (el->m_pi.ei.char_info != nullptr) {

					auto tmp_char_info = *el->m_pi.ei.char_info;

					int32_t value = -1;

					for (auto stats = (unsigned)CharacterInfo::Stats::S_POWER; stats < CharacterInfo::Stats::S_CURVE; stats++) {

						value = el->m_pi.getCharacterMaxSlot(CharacterInfo::Stats(stats));

						// Não deixa passar do Slot em jogo
						if (value != -1 && tmp_char_info.pcl[stats] > value)
							tmp_char_info.pcl[stats] = (unsigned char)value;
					}

					p.addBuffer(&tmp_char_info, sizeof(CharacterInfo));

				}else
					p.addZeroByte(sizeof(CharacterInfo));

				// Caddie Info
				if (el->m_pi.ei.cad_info != nullptr)
					p.addBuffer(el->m_pi.ei.cad_info->getInfo(), sizeof(CaddieInfo));
				else
					p.addZeroByte(sizeof(CaddieInfo));

				// Club Set Info
				if (el->m_pi.ei.clubset != nullptr) {

					auto tmp_csi = el->m_pi.ei.csi;

					int32_t value = -1;
					
					for (auto stats = (unsigned)CharacterInfo::Stats::S_POWER; stats < CharacterInfo::Stats::S_CURVE; stats++) {

						value = el->m_pi.getClubSetMaxSlot(CharacterInfo::Stats(stats));

						// Não deixa passar do Slot em jogo
						if (value != -1 && tmp_csi.slot_c[stats] > value)
							tmp_csi.slot_c[stats] = (short)value;
					}

					p.addBuffer(&tmp_csi, sizeof(ClubSetInfo));

				}else
					p.addZeroByte(sizeof(ClubSetInfo));

				// Mascot Info
				if (el->m_pi.ei.mascot_info != nullptr)
					p.addBuffer((MascotInfo*)el->m_pi.ei.mascot_info, sizeof(MascotInfo));
				else
					p.addZeroByte(sizeof(MascotInfo));

				// Time Start
				p.addBuffer(&m_start_time, sizeof(m_start_time));

				// Card(s) Equipped, acho que aqui não vai os itens buff, por que ele só da buff de exp e pang, o outro player nao precisa saber
				v_card_equip_char_and_special.clear();

				for (auto& el2 : el->m_pi.v_cei)
					if ((el2.parts_id == 0 && el2.parts_typeid == 0) || (el->m_pi.ei.char_info != nullptr && el2.parts_id == el->m_pi.ei.char_info->id && el2.parts_typeid == el->m_pi.ei.char_info->_typeid))
						v_card_equip_char_and_special.push_back(el2);

				p.addUint8((unsigned char)v_card_equip_char_and_special.size());

				for (auto& el2 : v_card_equip_char_and_special)
					p.addBuffer(&el2, sizeof(CardEquipInfo));
			}

			//packet_func::session_send(p, &_session, 1);
			packet_func::game_broadcast(*this, p, 1);

			// Send Individual Packet to all players in game
			for (auto& el : m_players) {
				// Send MapStatistics Info
				sendUpdateInfoAndMapStatistics(*el, -1);

				// Course
				Game::sendInitialData(*el);

				// Send seed Mascot Effect
				p.init_plain((unsigned short)0x16A);

				p.addUint32(m_seed_mascot_effect);

				packet_func::session_send(p, el, 1);

				// No meu tem que atualizar o info do player na sala, sendUpdatePlayerRoomInfo
				//p.init_plain((unsigned short)0x48)
				//p.addUint8(3);	// Update 0x103
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::sendInitialData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

}

void VersusBase::requestInitHole(player& _session, packet *_packet) {
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

		auto hole = m_course->findHole(ctx_hole.numero);

		hole->init(ctx_hole.tee, ctx_hole.pin);

		INIT_PLAYER_INFO("requestInitHole", "tentou inicializar o hole[NUMERO = " + std::to_string(hole->getNumero()) + "] no jogo", &_session);

		// Update Location Player in Hole
		pgi->location.x = ctx_hole.tee.x;
		pgi->location.z = ctx_hole.tee.z;
		
		// Número do hole atual, que o player está jogando
		pgi->hole = ctx_hole.numero;

		// Flag que marca se o player já inicializou o primeiro hole do jogo
		if (!pgi->init_first_hole)
			pgi->init_first_hole = 1u;

		// Gera degree para o player ou pega o degree sem gerar que é do modo do hole repeat
		pgi->degree = (m_ri.modo == Hole::M_REPEAT) ? hole->getWind().degree.getDegree() : hole->getWind().degree.getShuffleDegree();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitHole][Log] player[UID=" + std::to_string(pgi->uid) + "] Vento[Graus=" + std::to_string(pgi->degree) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}
}

bool VersusBase::requestFinishLoadHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishLoadHole");

	packet p;

	// Esse aqui é para Trocar Info da Sala
	// para colocar a sala no modo que pode entrar depois de ter começado
	bool ret = false;

	try {

		m_state_vs.setStateWithLock(STATE_VERSUS::LOAD_HOLE);

		INIT_PLAYER_INFO("requestFinishLoadHole", "tentou finalizar carregamento do hole no jogo", &_session);

		//pgi->finish_load_hole = 1;

		setLoadHole(pgi);

		//if (setLoadHoleAndCheckAllLoadHoleAndClear(pgi)) {
		//	//clear_all_load_hole();

		//	// Resposta para o Finish Load Hole
		//	sendReplyFinishLoadHole();

		//	// Send Rates of VersusBase, Mascot, voice Club e etc
		//	sendRatesOfVersusBase();
		//}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void VersusBase::requestFinishCharIntro(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishCharIntro");

	packet p;

	try {

		INIT_PLAYER_INFO("requestFinishCharIntro", "tentou finalizar intro do char no jogo", &_session);

		// Zera todas as tacada num dos players
		pgi->data.tacada_num = 0u;
		
		// Giveup Flag
		pgi->data.giveup = 0u;

		//pgi->finish_char_intro = 1;

		if (setFinishCharIntroAndCheckAllFinishCharIntroAndClear(pgi)) {
			//clear_all_finish_char_intro();

			sendReplyFinishCharIntro();
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestFinishHoleData(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishHoleData");

	try {

		UserInfoEx ui{ 0 };

		_packet->readBuffer(&ui, sizeof(UserInfo));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishHoleData][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] UserInfo[" + ui.toString() + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// aqui o cliente passa mad_conduta com o hole_in, trocados, mad_conduto <-> hole_in

		requestTranslateFinishHoleData(_session, ui);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishHoleData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestInitShotSended(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShotSended");

	try {

		INIT_PLAYER_INFO("requestInitShotSended", "player recebeu o pacote de InitShot", &_session);

#if defined(_WIN32)
		EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs_sync_shot);
#endif

#if defined(_WIN32)
		if (!QueryPerformanceCounter(&pgi->tick_sync_shot.tick))
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitShotSended][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] nao conseguiu pegar o QueryPerformanceCounter para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
		if (clock_gettime(CLOCK_MONOTONIC_RAW, &pgi->tick_sync_shot.tick) == -1)
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitShotSended][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] nao conseguiu pegar o QueryPerformanceCounter para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
#endif

		// Player recebeu o pacote55, agora o checkVersusTurn pode verifica se ele enviou o pacote no tempo
		pgi->tick_sync_shot.active = 1u;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitShotSended][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestInitShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShot");

	packet p;

	try {

		INIT_PLAYER_INFO("requestInitShot", "tentou iniciar tacada no jogo", &_session);

#if defined(_WIN32)
		EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs_sync_shot);
#endif

		if (pgi->init_shot == 1u) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitShot][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] o server ja recebeu o pacote12 Init Shot. ignora esse.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs_sync_shot);
#endif

			return;

		}else
			pgi->init_shot = 1u;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_sync_shot);
#endif

		// Stop time turn
		pgi->bar_space.setState(0);	// Volta para 1 depois que taca, era esse meu comentário no antigo

		// para o tempo da tacada ele acabou de tacar
		stopTime();

		pgi->tempo = 0;	// Reseta o tempo
		// end

		ShotDataEx sd{ 0 };

		sd.option = _packet->readUint16();

		// Power Shot
		if (sd.option == 1)
			_packet->readBuffer(&sd.power_shot, sizeof(sd.power_shot));

		_packet->readBuffer(&sd, sizeof(ShotData));

		pgi->shot_data = sd;

		m_state_vs.setStateWithLock(STATE_VERSUS::SHOTING);

#ifdef _DEBUG
		// Log Shot Data Ex
		_smp::message_pool::getInstance().push(new message("Log Shot Data Ex:\n\r" + sd.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Aqui não manda resposta no TourneyBase ou Practice, mas outro modos(VS, MATCH) manda e outros também não(TOURNEY)
		p.init_plain((unsigned short)0x55);

		p.addUint32(_session.m_oid);

		p.addBuffer(&sd, sizeof(ShotData));

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestSyncShot(player& _session, packet *_packet) {

	REQUEST_BEGIN("SyncShot");

	try {

		INIT_PLAYER_INFO("requestSyncShot", "tentou sincronizar a tacada no jogo", &_session);

#if defined(_WIN32)
		EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs_sync_shot);
#endif

		if (pgi->sync_shot_flag2 == 1u) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::requestSyncShot][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] o server ja recebeu o pacote1B sync shot do player. ignora esse.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs_sync_shot);
#endif

			return;

		}else
			pgi->sync_shot_flag2 = 1u;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_sync_shot);
#endif

		ShotSyncData ssd{ 0 };

		Game::requestReadSyncShotData(_session, _packet, ssd);

		requestTranslateSyncShotData(_session, ssd);

		requestReplySyncShotData(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestSyncShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestInitShotArrowSeq(player& _session, packet *_packet) {
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
			throw exception("[VersusBase::requestInitShotArrowSeq][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializar as sequencia de setas, mas nao enviou nenhuma seta. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 5, 0));

		std::vector< uArrow > setas;

		for (auto i = 0u; i < count_seta; ++i)
			setas.push_back({ _packet->readUint32() });

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestInitShotArrowSeq][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestShotEndData(player& _session, packet *_packet) {
	REQUEST_BEGIN("requestShotEndData");

	packet p;

	try {

		// ----------------- LEMBRETE --------------
		// Aqui vou usar para as tacadas do spinning cube que gera no course

		ShotEndLocationData seld{ 0 };

		_packet->readBuffer(&seld, sizeof(seld));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Log Shot End Location Data:\n\r" + seld.toString(), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		if (m_player_turn == nullptr)
			throw exception("[VersusBase::requestShotEndData][Error] m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1500, 0));

		// Tava esse aqui antes
		//INIT_PLAYER_INFO("requestShotEndData", "tentou finalizar local da tacada no jogo", &_session);

		//pgi->shot_data_for_cube = seld;
		if (m_player_turn->uid == _session.m_pi.uid)
			m_player_turn->shot_data_for_cube = seld;

		// Resposta para Shot End Data
		p.init_plain((unsigned short)0x1F7);

		/*p.addUint32(pgi->oid);
		p.addUint8(pgi->hole);*/
		p.addUint32(m_player_turn->oid);
		p.addUint8(m_player_turn->hole);
		
		p.addBuffer(&seld, sizeof(seld));

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestShotEndData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

RetFinishShot VersusBase::requestFinishShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishShot");

	packet p;

	RetFinishShot ret{ 0 };

	try {

		INIT_PLAYER_INFO("requestFinishShot", "tentou sincronizar o termino da tacada no jogo", &_session);

		// Essa parte tem que vir antes, mas estou fazendo teste de dc
#if defined(_WIN32)
		EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs_sync_shot);
#endif

		if (pgi->finish_shot2 == 1u) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishShot][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] o server ja recebeu o pacote1C sync end shot do player. ignora esse.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs_sync_shot);
#endif

			return ret;

		}else
			pgi->finish_shot2 = 1u;

		// Recebeu o primeiro finish shot libera a contagem, para verifica se o cliente recebeu a resposta em menos de 10 segundos
		// só entra se o timer do player no tick_sync_end_shot não estiver ativado, por que quando ativa 1 ativa todos
		if (!pgi->tick_sync_end_shot.active) {

			std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

				try {

					INIT_PLAYER_INFO("requestFinishShot", " tentou ativar all tick sync end shot do jogo", _el);

					// Limpa o tick_sync_shot para a próxima tacada(shot)
#if defined(_WIN32)
					QueryPerformanceCounter(&pgi->tick_sync_end_shot.tick);
#elif defined(__linux__)
					clock_gettime(CLOCK_MONOTONIC_RAW, &pgi->tick_sync_end_shot.tick);
#endif

					pgi->tick_sync_end_shot.active = 1u;

				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			});
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_sync_shot);
#endif

		// Time Finish Shot Initialization frequency
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
			ret.p = findSessionByPlayerGameInfo(m_player_turn);

#if defined(_WIN32)
		QueryPerformanceCounter(&end_tick);
#elif defined(__linux__)
		clock_gettime(CLOCK_MONOTONIC_RAW, &end_tick);
#endif

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishShot][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] finalizar tacada, o Server demorou "
				+ std::to_string(
#if defined(_WIN32)
					(end_tick.QuadPart - tick.QuadPart) * 1000000 / frequency.QuadPart
#elif defined(__linux__)
					DIFF_TICK_MICRO(end_tick, tick, frequency)
#endif
				) + " micro-segundos", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void VersusBase::requestChangeMira(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeMira");

	packet p;

	try {

		float mira = _packet->readFloat();

		INIT_PLAYER_INFO("requestChangeMira", "tentou mudar a mira[MIRA=" + std::to_string(mira) + "] no jogo", &_session);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeMira][Log] Request player[UID=" + std::to_string(_session.m_pi.uid) + "] mira[VALUE=" 
				+ std::to_string(mira) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		pgi->location.r = mira;

		// Resposta para o Change mira
		p.init_plain((unsigned short)0x56);

		p.addUint32(pgi->oid);
		p.addFloat(pgi->location.r);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeMira][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestChangeStateBarSpace(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateBarSpace");

	packet p;

	try {

		unsigned char state = _packet->readUint8();
		float point = _packet->readFloat();

		INIT_PLAYER_INFO("requestChangeStateBarSpace", "tentou mudar o estado[STATE=" + std::to_string((unsigned short)state) + ", POINT=" + std::to_string(point) + "] da barra de espaco no jogo", &_session);

		if (!pgi->bar_space.setStateAndPoint(state, point))
			throw exception("[VersusBase::requestChangeStateBarSpace][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mudar o estado da barra de espaco[STATE=" 
					+ std::to_string((unsigned short)state) + ", POINT=" + std::to_string(point) + "] no jogo, mas o estado eh desconhecido, Hacker ou Bug. packet: " + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 5, 0));

		if (state == 0 && pgi->tempo == 1) {

			try {

				pgi->tempo = 0;

				if (++pgi->data.time_out == 3) {

					auto hole = m_course->findHole(pgi->hole);

					if (hole == nullptr)
						throw exception("[VersusBase::requestChangeStateBarSpace][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mudar o estado da barra de espaco[STATE="
								+ std::to_string((unsigned short)state) + ", POINT=" + std::to_string(point) + "] no jogo, mas tentou encontrar o hole[HOLE="
								+ std::to_string((short)pgi->hole) + "] no course mas, nao encontrou. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 100, 0));

					pgi->shot_sync.state_shot.display.stDisplay.acerto_hole = 1u;

					pgi->data.tacada_num = hole->getPar().total_shot;

					// Derruba player, tem que fazer isso no channel ou na sala, com o retorno dessa função
					pgi->data.bad_condute = 3;
				}
			
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeStateBarSpace][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}

			// Time Out
			p.init_plain((unsigned short)0x5C);

			p.addUint32(_session.m_oid);

			packet_func::game_broadcast(*this, p, 1);

		}
		// Agora esse está no requestInitShot
		/*else if (state == 3 && pgi->bar_space.getPoint()[3] <= 250.f/*só pode da bad entre 100 a 250 (ACHO, não sei o valor certo ainda)*//*) {

			pgi->bar_space.setState(0);	// Volta para 1 depois que taca, era esse meu comentário no antigo

			// para o tempo da tacada ele acabou de tacar
			stopTime();

			pgi->tempo = 0;	// Reseta o tempo
		}*/

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeStateBarSpace][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActivePowerShot(player& _session, packet *_packet) {
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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActivePowerShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestChangeClub(player& _session, packet *_packet) {
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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeClub][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestUseActiveItem(player& _session, packet *_packet) {
	REQUEST_BEGIN("UseActiveItem");

	packet p;

	try {

		uint32_t item_typeid = _packet->readUint32();

		INIT_PLAYER_INFO("requestUseActiveItem", "tentou usar item ativo no jogo", &_session);

		if (item_typeid == 0)
			throw exception("[VersusBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID=" 
					+ std::to_string(item_typeid) + "] no jogo, mas o item_typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 7, 0));

		auto iffItem = sIff::getInstance().findCommomItem(item_typeid);
		
		if (iffItem == nullptr)
			throw exception("[VersusBase::requestActiveItem][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + " tentou usar active item[TYPEID="
				+ std::to_string(item_typeid) + "] no jogo, mas o item nao tem no IFF_STRUCT. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 77, 0));
		
		if (sIff::getInstance().getItemGroupIdentify(item_typeid) != iff::ITEM || !sIff::getInstance().IsItemEquipable(item_typeid))
			throw exception("[VersusBase::requestActiveItem][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
				+ std::to_string(item_typeid) + "] no jogo, mas o item nao eh equipavel(usar). Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 78, 0));
		
		if (item_typeid == MULLIGAN_ROSE_TYPEID)
			throw exception("[VersusBase::requestActiveItem][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
				+ std::to_string(item_typeid) + "] no jogo, mas o item Mulligan Rose nao pode usar no VersusBase, so em TourneyBase. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 79, 0));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(item_typeid);

		if (pWi == nullptr)
			throw exception("[VersusBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas ele nao tem esse item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 8, 0));

		auto it = pgi->used_item.v_active.find(pWi->_typeid);

		if (it == pgi->used_item.v_active.end())
			throw exception("[VersusBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas ele nao equipou esse item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 9, 0));

		if (it->second.count >= it->second.v_slot.size())
			throw exception("[VersusBase::requestActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item[TYPEID="
					+ std::to_string(item_typeid) + "] no jogo, mas ele ja usou todos os item desse que ele equipou. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 10, 0));

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
		
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestUseActiveItem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestChangeStateTypeing(player& _session, packet *_packet) {
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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeStateTypeing][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestMoveBall(player& _session, packet *_packet) {
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

		// para o tempo do da tacada do player, que ele vai recolocar e come�a um novo tempo depois
		stopTime();

		// Resposta para Move Ball
		p.init_plain((unsigned short)0x60);

		p.addFloat(pgi->location.x);
		p.addFloat(pgi->location.y);
		p.addFloat(pgi->location.z);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestMoveBall][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestChangeStateChatBlock(player& _session, packet *_packet) {
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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestChangeStateChatBlock][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveBooster(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveBooster");

	packet p;

	try {

		float velocidade = _packet->readFloat();

		INIT_PLAYER_INFO("requestActiveBooster", "tentou ativar Time Booster no jogo", &_session);

		if ((_session.m_pi.m_cap.stBit.premium_user/* & (1 << 14)/*0x00004000/*PREMIUM USER*/) == 0) { // (não é)!PREMIUM USER

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(TIME_BOOSTER_TYPEID);

			if (pWi == nullptr)
				throw exception("[VersusBase::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele nao tem o item passive. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 11, 0));

			if (pWi->STDA_C_ITEM_QNTD <= 0)
				throw exception("[VersusBase::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele nao tem quantidade suficiente[VALUE=" 
						+ std::to_string(pWi->STDA_C_ITEM_QNTD) + ", REQUEST=1] do item de time booster.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 12, 0));

			auto it = pgi->used_item.v_passive.find(pWi->_typeid);

			if (it == pgi->used_item.v_passive.end())
				throw exception("[VersusBase::requestActiveBooster][Error] player[UID = " + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele nao tem ele no item passive usados do server. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 13, 0));

			if ((short)it->second.count >= pWi->STDA_C_ITEM_QNTD)
				throw exception("[VersusBase::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster, mas ele ja usou todos os time booster. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 14, 0));

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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveBooster][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveReplay(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveReplay");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[VersusBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID=" 
					+ std::to_string(_typeid) + "], mas o typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 200, 0));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[VersusBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem o item. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 201, 0));

		if (pWi->STDA_C_ITEM_QNTD <= 0)
			throw exception("[VersusBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem quantidade suficiente[VALUE=" + std::to_string(pWi->STDA_C_ITEM_QNTD) + ", REQUEST=1] do item. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 202, 0));

		// UPDATE ON SERVER AND DB
		stItem item{ 0 };

		item.type = 2;
		item._typeid = pWi->_typeid;
		item.id = pWi->id;
		item.qntd = 1;
		item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

		if (item_manager::removeItem(item, _session) <= 0)
			throw exception("[VersusBase::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay[TYPEID="
					+ std::to_string(_typeid) + "], nao conseguiu deletar ou atualizar qntd do item[TYPEID=" + std::to_string(item._typeid) + ", ID=" + std::to_string(item.id) + "]", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 203, 0));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveReplay][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] ativou replay e ficou com " 
				+ std::to_string(item.stat.qntd_dep) + " + fita(s)", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// UPDATE ON GAME
		// Resposta para o Active Replay
		p.init_plain((unsigned short)0xA4);

		p.addUint16((unsigned short)item.stat.qntd_dep);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveReplay][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveCutin(player& _session, packet *_packet) {
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
			throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID=" 
					+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
					+ std::to_string(ac.uid) + "], mas o jogador nao esta no jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 0x5200101));

		if (s->m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
					+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
					+ std::to_string(ac.uid) + "], mas o jogador nao tem um character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 2, 0x5200102));

		IFF::CutinInfomation *pCutin = nullptr;

		// Cutin Padrão que o player equipa, quando o cliente envia o cutin type é que é efeito por roupas equipadas
		if (sIff::getInstance().getItemGroupIdentify(ac.char_typeid) == iff::CHARACTER && ac.active) {

			if (s->m_pi.ei.char_info->_typeid != ac.char_typeid)
				throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
						+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
						+ std::to_string(ac.uid) + "], mas o character typeid passado nao eh igual ao equipado do player. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 4, 0x5200104));

			WarehouseItemEx *pWi = nullptr;

			auto end = (sizeof(s->m_pi.ei.char_info->cut_in) / sizeof(s->m_pi.ei.char_info->cut_in[0]));

			for (auto i = 0u; i < end; ++i) {

				if (s->m_pi.ei.char_info->cut_in[i] > 0) {

					if ((pWi = _session.m_pi.findWarehouseItemById(s->m_pi.ei.char_info->cut_in[i])) != nullptr) {

						if ((pCutin = sIff::getInstance().findCutinInfomation(pWi->_typeid)) == nullptr)
							throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
									+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ", ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
									+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
									+ std::to_string(pWi->_typeid) + ", ID=" + std::to_string(pWi->id) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 3, 0x5200103));

						if (pCutin->tipo.ulCondition == ac.tipo)
							break;
						else if ((i + 1) == end)
							throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
									+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
									+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
									+ std::to_string(pWi->_typeid) + ", ID=" + std::to_string(pWi->id) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 3, 0x5200103));
					}
				}
			}

		}else if (sIff::getInstance().getItemGroupIdentify(ac.char_typeid) == iff::SKIN/*Cutin é uma Skin*/ && ac.active == 0) {

			// Verificar se ele tem os itens para ativar esse Cutin

			if ((pCutin = sIff::getInstance().findCutinInfomation(ac.char_typeid)) == nullptr)
				throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
						+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
						+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
						+ std::to_string(ac.char_typeid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 3, 0x5200103));

			// Esses que passa o cutin typeid, pode ativar com tipo 1 e 2, 1 PS e 2 PS
			/*if (pCutin->tipo != ac.tipo)
				throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
						+ std::to_string(ac.char_typeid) + ", TIPO=" + std::to_string(ac.tipo) + ", OPT=" + std::to_string(ac.opt) + ",  ACTIVE=" + std::to_string(ac.active) + "] de um player[UID="
						+ std::to_string(ac.uid) + "], mas o jogador nao tem esse cutin[TYPEID="
						+ std::to_string(ac.char_typeid) + "]. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 3, 0x5200103));*/
		}

		if (pCutin == nullptr)
			throw exception("[VersusBase::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou activar cutin[CHAR_TYPEID="
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

		packet_func::game_broadcast(*this, p, 1);

		// No Modo GrandZodic, não envia Cutin, então envia o pacote18D com option 0(Uint8), e valor 3(Uint16)

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveCutin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x18D);

		p.addUint8(0);	// OPT

		p.addUint16(1);	// Error

		packet_func::session_send(p, &_session, 1);
	}
}

void VersusBase::requestActiveRing(player& _session, packet *_packet) {
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
			throw exception("[VersusBase::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID=" 
					+ std::to_string(r._typeid) + "], mas o typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 30, 0x330001));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(r._typeid);

		if (pWi == nullptr)
			throw exception("[VersusBase::requestActiveRing][Error] player[UID = " + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID = " 
					+ std::to_string(r._typeid) + "], mas ele nao tem o anel. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 31, 0x330002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID="
					+ std::to_string(r._typeid) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 32, 0x330003));

		if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), r._typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
			throw exception("[VersusBase::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel[TYPEID="
					+ std::to_string(r._typeid) + "], mas ele nao esta equipado com o anel. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 33, 0x330004));

		// Adiciona o efeito que foi ativado
		checkEffectItemAndSet(_session, r._typeid);

		// Resposta para o cliente
		p.init_plain((unsigned short)0x237);

		p.addUint32(0);	// OK

		p.addUint32(_session.m_pi.uid);

		p.addUint32(r._typeid);
		p.addUint8(r.efeito);

		packet_func::game_broadcast(*this, p, 1);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRing][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x237);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::VERSUS_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x330000);

		packet_func::session_send(p, &_session, 1);
	}
}

void VersusBase::requestActiveRingGround(player& _session, packet *_packet) {
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
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRingGround][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] ativou o efeito 0x1F(31) com os itens[TYPEID_1=" + std::to_string(rg.ring[0])
					+ ", TYPEID_2=" + std::to_string(rg.ring[1]) + "] e OPTION=" + std::to_string(rg.option), CL_FILE_LOG_AND_CONSOLE));

		if (!rg.isValid())
			throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE=" 
					+ std::to_string(rg.efeito) +", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
					+ std::to_string(rg.option) + "], mas os typeid's nao sao validos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 50, 0x340001));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
					+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
					+ std::to_string(rg.option) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 51, 0x340002));

		if (sIff::getInstance().getItemGroupIdentify(rg.ring[0]) == iff::AUX_PART) {	// Anel

			auto pRing = _session.m_pi.findWarehouseItemByTypeid(rg.ring[0]);

			if (pRing == nullptr)
				throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao tem o Anel[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 52, 0x340002));

			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rg.ring[0]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao esta com o Anel[0] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 53, 0x340003));

			if (rg.ring[0] != rg.ring[1]) {	// Ativou Habilidade em conjunto 2 aneis

				auto pRing2 = _session.m_pi.findWarehouseItemByTypeid(rg.ring[1]);

				if (pRing2 == nullptr)
					throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao tem o Anel[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 52, 0x340002));

				if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
					throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao esta com o Anel[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 53, 0x340003));
			}

		}else if (sIff::getInstance().getItemGroupIdentify(rg.ring[0]) == iff::PART) {	// Part

			auto pRing = _session.m_pi.findWarehouseItemByTypeid(rg.ring[0]);

			if (pRing == nullptr)
				throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao tem o Part[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 52, 0x340002));

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), rg.ring[0]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao esta com o Part[0] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 53, 0x340003));

			if (rg.ring[0] != rg.ring[1]) {	// Ativou Habilidade em conjunto 2 aneis

				auto pRing2 = _session.m_pi.findWarehouseItemByTypeid(rg.ring[1]);

				if (pRing2 == nullptr)
					throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao tem o Part[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 52, 0x340002));

				if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), rg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
					throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao esta com o Part[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 53, 0x340003));
			}
		
		}else if (sIff::getInstance().getItemGroupIdentify(rg.ring[0]) == iff::MASCOT) {

			auto pMascot = _session.m_pi.findMascotByTypeid(rg.ring[0]);

			if (pMascot == nullptr)
				throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
						+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
						+ std::to_string(rg.option) + "], mas ele nao tem o Mascot[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 52, 0x340002));

			if (rg.ring[0] != rg.ring[1]) { // Ativou Habilidade em conjunto 2 aneis

				auto pPart2 = _session.m_pi.findWarehouseItemByTypeid(rg.ring[1]);

				if (pPart2 == nullptr)
					throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao tem o Part[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 52, 0x340002));

				if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), rg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
					throw exception("[VersusBase::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno[TYPE="
							+ std::to_string(rg.efeito) + ", RING[0]=" + std::to_string(rg.ring[0]) + ", RING[1]=" + std::to_string(rg.ring[1]) + ", OPTION="
							+ std::to_string(rg.option) + "], mas ele nao esta com o Part[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 53, 0x340003));
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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRingGround][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x266);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::VERSUS_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x340000);

		packet_func::session_send(p, &_session, 1);
	}
}

void VersusBase::requestActiveRingPawsRainbowJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPawsRainbowJP");

	packet p;

	try {

		// Efeito patinha não passa o TYPEID do item que ativou
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_ACCUMULATE));

		// Resposta para o Active Ring Paws Rainbow JP
		p.init_plain((unsigned short)0x27E);

		p.addUint32(_session.m_pi.uid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRingPawsRainbowJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveRingPawsRingSetJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPawsRingSetJP");

	packet p;

	try {

		// Efeito patinha não passa o TYPEID do item que ativou
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_NOT_ACCUMULATE));

		// Resposta para o Active Ring Paws Ring Set JP
 		p.init_plain((unsigned short)0x281);

		p.addUint32(_session.m_pi.uid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRingPawsRingSetJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveRingPowerGagueJP(player& _session, packet *_packet) {
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
			throw exception("[VersusBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas os typeid's nao sao validos. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 150, 0x390001));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 151, 0x390002));

		auto pRing = _session.m_pi.findWarehouseItemByTypeid(rpg.ring[0]);

		if (pRing == nullptr)
			throw exception("[VersusBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas ele nao tem o Anel[0]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 152, 0x390002));

		if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rpg.ring[0]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
			throw exception("[VersusBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
					+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
					+ std::to_string(rpg.option) + "], mas ele nao esta com o Anel[0] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 153, 0x390003));

		if (rpg.ring[0] != rpg.ring[1]) {	// Ativou Habilidade em conjunto 2 aneis

			auto pRing2 = _session.m_pi.findWarehouseItemByTypeid(rpg.ring[1]);

			if (pRing2 == nullptr)
				throw exception("[VersusBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
						+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
						+ std::to_string(rpg.option) + "], mas ele nao tem o Anel[1]. hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 152, 0x390002));

			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), rpg.ring[1]) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[VersusBase::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Barra de PS [JP] [TYPE="
						+ std::to_string(rpg.efeito) + ", RING[0]=" + std::to_string(rpg.ring[0]) + ", RING[1]=" + std::to_string(rpg.ring[1]) + ", OPTION="
						+ std::to_string(rpg.option) + "], mas ele nao esta com o Anel[1] equipado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 153, 0x390003));
		}

		// Effect
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::POWER_GAUGE_FREE));

		// Resposta para o Active Ring Power Gague JP
		p.init_plain((unsigned short)0x27F);

		p.addUint32(_session.m_pi.uid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRingPowerGagueJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveRingMiracleSignJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingMiracleSign");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[VersusBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID=" 
					+ std::to_string(_typeid) + "] Olho Magico JP, mas o typeid eh invalido(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 70, 0x350001));

		WarehouseItemEx *pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[VersusBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
					+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao tem o 'Anel'. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 71, 0x350002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
					+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 72, 0x350003));

		if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::AUX_PART) {	// Anel
			
			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[VersusBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
						+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao esta com o Anel equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 0x73, 0x350004));

		}else if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::PART) {	// Part

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[VersusBase::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar 'Anel'[TYPEID="
						+ std::to_string(_typeid) + "] Olho Magico JP, mas ele nao esta com a Part equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 74, 0x350005));

		}	// else Item Passive, o item do assist, mas acho que ele n�o chame esse, ele chama o proprio pacote dele

		// Effect
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::MIRACLE_SIGN_RANDOM));

		// Resposta para o Active Ring Miracle Sign JP
		p.init_plain((unsigned short)0x280);

		p.addUint32(0);	// OK;

		p.addUint32(_typeid);
		p.addUint32(_session.m_pi.uid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveRingMiracleSign][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x280);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::VERSUS_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x350000);

		packet_func::session_send(p, &_session, 1);
	}
}

void VersusBase::requestActiveWing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveWing");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[VersusBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID=" 
					+ std::to_string(_typeid) + "], mas o typeid eh invalido(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 90, 0x360001));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[VersusBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem esse item 'Asa', Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 91, 0x360002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 92, 0x360003));

		if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
			throw exception("[VersusBase::ActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao esta com o item 'Asa' equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 93, 0x360004));

		// Adiciona o efeito que foi ativado
		checkEffectItemAndSet(_session, _typeid);

		// Resposta para o Active Wing
		p.init_plain((unsigned short)0x203);

		p.addUint32(_session.m_pi.uid);

		p.addUint32(_typeid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::ActiveWing][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActivePaws(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActivePaws");

	packet p;

	try {

		// Efeito patinha não passa o TYPEID do item que ativou, Animal Ring(Anel) ou Patinha
		setEffectActiveInShot(_session, enumToBitValue<IFF::Ability::eEFFECT_TYPE, uint64_t>(IFF::Ability::eEFFECT_TYPE::PAWS_NOT_ACCUMULATE));

		// Resposta para o Active Paws
		p.init_plain((unsigned short)0x236);

		p.addUint32(_session.m_pi.uid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActivePaws][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestActiveGlove(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveGlove");

	packet p;

	try {

		uint32_t _typeid = _packet->readUint32();

		if (_typeid == 0)
			throw exception("[VersusBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID=" 
					+ std::to_string(_typeid) + "], mas o typeid eh invalido(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 110, 0x370001));

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(_typeid);

		if (pWi == nullptr)
			throw exception("[VersusBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao tem esse item 'Luva'. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 111, 0x370002));

		if (_session.m_pi.ei.char_info == nullptr)
			throw exception("[VersusBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
					+ std::to_string(_typeid) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 112, 0x370003));

		if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::PART) {	// Luva

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[VersusBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
						+ std::to_string(_typeid) + "], mas ele nao tem a Luva equipada. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 113, 0x370004));

		}else if (sIff::getInstance().getItemGroupIdentify(_typeid) == iff::AUX_PART) {	// Anel

			if (std::find(_session.m_pi.ei.char_info->auxparts, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts), _typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->auxparts))
				throw exception("[VersusBase::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Luva[TYPEID="
						+ std::to_string(_typeid) + "], mas ele nao tem o Anel equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 114, 0x370005));
		}

		// Adiciona o efeito que foi ativado
		checkEffectItemAndSet(_session, _typeid);

		// Resposta para o Active Glove
		p.init_plain((unsigned short)0x265);

		p.addUint32(0);	// OK

		p.addUint32(_typeid);

		p.addUint32(_session.m_pi.uid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveGlove][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x265);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::VERSUS_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x370000);

		packet_func::session_send(p, &_session, 1);
	}
}

void VersusBase::requestActiveEarcuff(player& _session, packet *_packet) {
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
			throw exception("[VersusBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff'Mascot'[TYPEID=" 
					+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE=" 
					+ std::to_string(ec.x_point_angle) + "], mas o typeid eh invalido. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 130, 0x380001));

		if (sIff::getInstance().getItemGroupIdentify(ec._typeid) == iff::PART) {	// Earcuff

			if (_session.m_pi.ei.char_info == nullptr)
				throw exception("[VersusBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao esta com um Character equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 131, 0x380002));

			auto pWi = _session.m_pi.findWarehouseItemByTypeid(ec._typeid);

			if (pWi == nullptr)
				throw exception("[VersusBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao tem o Part. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 132, 0x380003));

			if (std::find(_session.m_pi.ei.char_info->parts_typeid, LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid), ec._typeid) == LAST_ELEMENT_IN_ARRAY(_session.m_pi.ei.char_info->parts_typeid))
				throw exception("[VersusBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao esta com o Part equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 133, 0x380004));

		}else if (sIff::getInstance().getItemGroupIdentify(ec._typeid) == iff::MASCOT)	{	// Mascot Dragon

			auto pMi = _session.m_pi.findMascotByTypeid(ec._typeid);

			if (pMi == nullptr)
				throw exception("[VersusBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao tem esse Mascot. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 134, 0x380005));

			if (_session.m_pi.ei.mascot_info == nullptr)
				throw exception("[VersusBase::ActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff'Mascot'[TYPEID="
						+ std::to_string(ec._typeid) + ", ANGLE_SENTIDO=" + std::to_string((unsigned short)ec.angle) + ", X_ANGLE="
						+ std::to_string(ec.x_point_angle) + "], mas ele nao esta com o Mascot equipado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 135, 0x380006));
		}

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

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestActiveEarcuff][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta Error
		p.init_plain((unsigned short)0x24C);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::VERSUS_BASE) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x380000);

		packet_func::session_send(p, &_session, 1);
	}
}

void VersusBase::requestMarkerOnCourse(player& _session, packet *_packet) {
	REQUEST_BEGIN("MarkerOnCourse");

	packet p;

#if defined(__linux__)
#pragma pack(1)
#endif

	struct stMarkerOnCourse {
		void clear() { memset(this, 0, sizeof(stMarkerOnCourse)); };
		float x;
		float y;
		float z;
	};

#if defined(__linux__)
#pragma pack()
#endif

	try {

		stMarkerOnCourse moc{ 0 };

		_packet->readBuffer(&moc, sizeof(stMarkerOnCourse));

		// Resposta para MarkerOnCourse
		p.init_plain((unsigned short)0x1F8);

		p.addUint32(_session.m_oid);

		p.addBuffer(&moc, sizeof(stMarkerOnCourse));

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestMarkerOnCourse][ErrorSystem] " + e.getFullMessageError()));
	}
}

void VersusBase::requestLoadGamePercent(player& _session, packet *_packet) {
	REQUEST_BEGIN("LoadGamePercent");

	packet p;

	try {

		unsigned char percent = _packet->readUint8();

		// Resposta para o Load Game Percent
		p.init_plain((unsigned short)0xA3);

		p.addUint32(_session.m_oid);

		p.addUint8(percent);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestLoadGamePercent][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestStartTurnTime(player& _session, packet *_packet) {
	REQUEST_BEGIN("StartTurnTime");

	try {

		// Começa a contar o tempo do turno do player no Jogo
		startTime(&_session);

		m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_HIT_SHOT);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestStartTurnTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestUnOrPause(player& _session, packet *_packet) {
	REQUEST_BEGIN("UnOrPause");

	packet p;

	try {

		unsigned char opt = _packet->readUint8();

		if (m_timer == nullptr)
			throw exception("[VersusBase::requestUnOrPause][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pausar ou despausar[OPT=" 
					+ std::to_string((unsigned short)opt) + "] um VersusBase, que nao tem timer inicializado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 300, 0));

		if (opt == 0) {	// Despausa

			if (m_timer->getState() != timer::TIMER_STATE::PAUSED)
				throw exception("[VersusBase::requestUnOrPause][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pausar ou despausar[OPT="
						+ std::to_string((unsigned short)opt) + "] um VersusBase, que o timer nao esta pausado, esta em outro estado[ESTADO=" + std::to_string(m_timer->getState()) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 301, 0));

			resumeTime();

			packet p((unsigned short)0x8B);

			p.addUint32(_session.m_oid);

			p.addUint8(0/*Despausa*/);

			packet_func::game_broadcast(*this, p, 1);

			// Log
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestUnOrPause][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] despausou o tempo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		}else if (opt == 1) {	// Pausa

			if (m_timer->getState() != timer::TIMER_STATE::RUNNING)
				throw exception("[VersusBase::requestUnOrPause][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pausar ou despausar[OPT="
						+ std::to_string((unsigned short)opt) + "] um VersusBase, que o timer nao esta rodando, esta em outro estado[ESTADO=" + std::to_string(m_timer->getState()) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 301, 0));

			if (m_count_pause++ >= 3)
				throw exception("[VersusBase::requestUnOrPause][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "], tentou pausar ou despausar[OPT=" 
						+ std::to_string((unsigned short)opt) + "] um VersusBase, mas o Versus Base ja foi pausado 3x. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE);

			pauseTime();

			packet p((unsigned short)0x8B);

			p.addUint32(_session.m_oid);

			p.addUint8(1/*Pausa*/);

			packet_func::game_broadcast(*this, p, 1);

			// Log
#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestUnOrPause][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] pausou o tempo na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestUnOrPause][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestReplyContinue() {

	packet p;

	try {

		// Troca o Turno
		changeTurn();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestReplyContinue][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestExecCCGChangeWind(player& _session, packet *_packet) {
	REQUEST_BEGIN("ExecCCGChangeWind");

	try {

		unsigned char wind = _packet->readUint8();
		unsigned short degree = _packet->readUint8();

		CCGChangeWind(_session, wind, degree);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestExecCCGChangeWind][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}
}

void VersusBase::requestExecCCGChangeWeather(player& _session, packet *_packet) {
	REQUEST_BEGIN("ExecCCGChangeWeather");

	try {

		if (m_player_turn == nullptr)
			throw exception("[VersusBase::requestExecCCGChangeWeather][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou executar o comando de troca de tempo(weather) no versus na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o player_turn do versus eh invalido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 0x5700100));

		auto hole = m_course->findHole(m_player_turn->hole);

		if (hole == nullptr)
			throw exception("[VersusBase::requestExecCCGChangeWeather][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou executar o comando de troca de tempo(weather) no versus na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o nao encontrou o hole[VALUE=" + std::to_string((short)m_player_turn->hole) + "] no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 2, 0x5700100));

		auto weather = _packet->readUint8();

		// Change Weather of Hole
		hole->setWeather(weather);

		// Log
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestExecCCGChangeWeather][Log] [GM] player[UID=" + std::to_string(_session.m_pi.uid) + "] trocou o tempo(weather) da sala[NUMERO="
				+ std::to_string(m_ri.numero) + ", WEATHER=" + std::to_string((unsigned short)weather) + "]", CL_FILE_LOG_AND_CONSOLE));

		// UPDATE ON GAME
		packet p((unsigned short)0x9E);

		p.addUint16(hole->getWeather());
		p.addUint8(1);						// Acho que seja flag, não sei, vou deixar 1 por ser o GM que mudou

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestExecCCGChangeWeather][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}
}

bool VersusBase::requestFinishGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishGame");

	packet p;
	
	bool ret = false;

	try {

		UserInfoEx ui{ 0 };

		_packet->readBuffer(&ui, sizeof(UserInfo));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishGame][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] UserInfo[" + ui.toString() + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// aqui o cliente passa mad_conduta com o hole_in, trocados, mad_conduto <-> hole_in

		INIT_PLAYER_INFO("requestFinishGame", "tentou terminar o jogo", &_session);

		pgi->ui = ui;

		// Packet06
		ret = finish_game(_session, 6);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void VersusBase::startTime(void* _quem) {

	try {
		// Aqui tem tem ser come�a o tempo do player do turno e soma +1 ao n�mero de tacadas dele
		INIT_PLAYER_INFO("startTime", "tentou comecar o tempo do player turno no jogo", (player*)_quem);

		// Soma +1 na tacada do player
		pgi->data.tacada_num++;

		// Para Tempo se j� estiver 1 timer
		if (m_timer != nullptr)
			stopTime();

		job j(VersusBase::end_time, this, _quem);

		//if (sgs::gs != nullptr)
			m_timer = sgs::gs::getInstance().makeTime(m_ri.time_vs/*milliseconds*/, j);	// j� est� em minutos milliseconds
		/*else
			_smp::message_pool::getInstance().push(new message("[VersusBase::startTime][Error] tentou inicializar um timer, mas a variavel global estatica do Server eh invalida.", CL_FILE_LOG_AND_CONSOLE));*/

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[VersusBase::startTime][Log] Criou o Timer[Tempo=" + std::to_string((m_ri.time_30s > 0) ? m_ri.time_30s / 60000 : m_ri.time_vs / 1000) + (m_ri.time_30s > 0 ? "min" : "seg") 
				+ ", STATE=" + std::to_string(m_timer->getState()) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[VersusBase::startTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::timeIsOver(void* _quem) {

	if (_quem == nullptr)
		_smp::message_pool::getInstance().push(new message("[VersusBase::timeIsOver][WARNING] time is over executed without _quem, _quem is invalid(nullptr). Bug", CL_FILE_LOG_AND_CONSOLE));

	// Tempo do player do turno acabou, troca para end_shot
	m_state_vs.setStateWithLock(STATE_VERSUS::END_SHOT);
}

bool VersusBase::init_game() {

	// Inicializar Treasure Hunter Info do Versus Base
	init_treasure_hunter_info();

	return true;
}

void VersusBase::requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) {
	CHECK_SESSION_BEGIN("requestTranslateSyncShotData");

	try {

		auto s = findSessionByOID(_ssd.oid);

		if (s == nullptr)
			throw exception("[VersusBase::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada do player[OID="
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

			// J� s� na fun��o que come�a o tempo do player do turno
			//pgi->data.tacada_num++;

			if (_ssd.state == ShotSyncData::OUT_OF_BOUNDS || _ssd.state == ShotSyncData::UNPLAYABLE_AREA)
				pgi->data.tacada_num++;

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[VersusBase::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada no hole[NUMERO="
						+ std::to_string((unsigned short)pgi->hole) + "], mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 12, 0));

			// Conta j� a pr�xima tacada, no give up
			if (!_ssd.state_shot.display.stDisplay.acerto_hole && hole->getPar().total_shot <= (pgi->data.tacada_num + 1)) {

				// +1 que � giveup, s� add se n�o passou o n�mero de tacadas
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

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestTranslateSyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestReplySyncShotData(player& _session) {
	CHECK_SESSION_BEGIN("requestReplySyncShotData");

	try {

		INIT_PLAYER_INFO("requestReplySyncShotData", "tentou sincronizar a tacada no jogo", &_session);

		//pgi->sync_shot_flag = 1u;

		setSyncShot(pgi);

		//if (setSyncShotAndCheckAllSyncShotAndClear(pgi)) {
		//	//clear_all_sync_shot();

		//	if (m_player_turn == nullptr)
		//		throw exception("[VersusBase::requestReplySyncShotData][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 1201, 0));

		//	auto _session_turn = findSessionByPlayerGameInfo(m_player_turn);

		//	if (_session_turn != nullptr)
		//		drawDropItem(*_session_turn);
		//	else
		//		_smp::message_pool::getInstance().push(new message("[VersusBase::requestReplySyncShotData][Error] PlayerGameInfo *m_player_turn[UID=" + std::to_string(m_player_turn->uid) +
		//				"], nao conseguiu encontrar o player dele no map de player_info. Bug", CL_FILE_LOG_AND_CONSOLE));

		//	// Resposta Sync Shot
		//	sendSyncShot();
		//}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::requestReplySyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void VersusBase::requestTranslateFinishHoleData(player& _session, UserInfoEx& _ui) {
	CHECK_SESSION_BEGIN("requestTranslateFinishHole");

	try {

		INIT_PLAYER_INFO("requestTranslateFinishHoleData", "tentou finalizar hole dados no jogo", &_session);

		pgi->ui = _ui;

		if (!pgi->shot_sync.state_shot.display.stDisplay.acerto_hole) {	// Terminou o Hole sem acerta ele, Give Up

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[VersusBase::requestFinishHoleData][Error] player[UID=" + std::to_string(pgi->uid) + "] tentou finalizar os dados do hole no jogo, mas o hole[NUMERO="
						+ std::to_string(pgi->hole) + "] nao existe no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 400, 0));

			// +1 que é giveup, só add se não passou o número de tacadas
			if (pgi->data.tacada_num < hole->getPar().total_shot)
				pgi->data.tacada_num++;

			// Ainda não colocara o give up, o outro pacote, coloca nesse(muito difícil, não colocar só se estiver com bug)
			if (!pgi->data.giveup) {
				pgi->data.giveup = 1;

				// Incrementa o Bad Condute
				pgi->data.bad_condute++;
			}
		}

		// Aqui Salva os dados do Pgi, os best Chipin, Long putt e best drive(max distância)
		// Não sei se precisa de salvar aqui, já que estou salvando no pgi User Info
		pgi->progress.best_chipin = _ui.best_chip_in;
		pgi->progress.best_long_puttin = _ui.best_long_putt;
		pgi->progress.best_drive = _ui.best_drive;

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestTranslateFinishHoleData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool VersusBase::checkNextStepGame(player& _session) {

	auto ret = false;

	try {
		
		INIT_PLAYER_INFO("checkNextStepGame", "tentou verificar o proximo passo do jogo", &_session);

		auto seq = m_course->findHoleSeq(pgi->hole);

		if (seq == 0 || seq == (unsigned short)~0)
			throw exception("[VersusBase::checkNextStepGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pegar sequencia do hole[NUMERO=" 
					+ std::to_string(pgi->hole) + ", SEQ=" + std::to_string(seq) + "], mas nao encontrou course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 500, 0));

		if (m_players.size() == 2 && seq >= 4) {

			if (m_ri.qntd_hole == 18) {	// 18 Envia a pergunta se o player quer continuar o VS Sozinho

				if (m_player_turn == nullptr) {

					// Player Turn ainda não foi decidido, termina o jogo
					m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

					ret = true;	// Termina o Game

				}else if (m_player_turn == pgi) {

					// só tem 2 na sala, então só retorna uma session
					auto sessions = getSessions(&_session);

					packet p((unsigned short)0x92);

					if (sessions.size() > 1)
						packet_func::vector_send(p, sessions, 1);
					else
						packet_func::session_send(p, *sessions.begin(), 1);

				}else if (!checkPlayerTurnExistOnGame()) {

					// Player Turn não está mais no jogo, termina o jogo
					m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

					ret = true;	// Termina o Game

				}else
					m_flag_next_step_game = 1;	// Pergunta se quer continuar

			}else if (m_player_turn == nullptr) {

				// Player Turn ainda não foi decidido, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else if (m_player_turn == pgi) {
				
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game
			
			}else if (!checkPlayerTurnExistOnGame()) {

				// Player Turn não está mais no jogo, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else
				m_flag_next_step_game = 2;	// Termina o game

		}else if (m_players.size() == 2) {

			if (m_player_turn == nullptr) {

				// Player Turn ainda não foi decidido, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else if (m_player_turn == pgi) {
				
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game
			
			}else if (!checkPlayerTurnExistOnGame()) {

				// Player Turn não está mais no jogo, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else
				m_flag_next_step_game = 2;	// Termina o game
		
		}else if (m_players.size() == 1) {	// Player quitou mesmo sendo o ultimo no jogo

			if (m_player_turn == nullptr) {

				// Player Turn ainda não foi decidido, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else if (m_player_turn == pgi) {
				
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game
			
			}else if (!checkPlayerTurnExistOnGame()) {

				// Player Turn não está mais no jogo, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else
				m_flag_next_step_game = 2;
		
		}else if(m_player_turn == nullptr) {

			// Player Turn ainda não foi decidido, termina o jogo
			m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

			ret = true;	// Termina o Game

		}else
			m_flag_next_step_game = 3;	// Player quitou

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::checkNextStepGame][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

bool VersusBase::checkEndGame(player& _session) {

	INIT_PLAYER_INFO("checkEndGame", "tentou verificar se eh o final do jogo", &_session);

	return (m_course->findHoleSeq(pgi->hole) == m_ri.qntd_hole || (m_players.size() == 1 && m_course->findHoleSeq(pgi->hole) < 4));
}

bool VersusBase::checkPlayerTurnExistOnGame() {

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("checkPlayerTurnExistOnGame", "verifica se o player turno existe no jogo", el);

		// Existe
		if (m_player_turn == pgi)
			return true;
	}

	// Não existe
	return false;
}

bool VersusBase::checkAllClearHole() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("checkAllClearHole", "tentou verificar se todos os player terminaram o hole no jogo", _el);

			if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || pgi->data.giveup)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::checkAllClearHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearAllClearHole() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_clear_hole();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool VersusBase::checkAllClearHoleAndClear() {
	
	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("checkAllClearHoleAndClear", "tentou verificar se todos os player terminaram o hole no jogo", _el);

			if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || pgi->data.giveup)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::checkAllClearHoleAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_clear_hole();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void VersusBase::setLoadHole(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setLoadHole][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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
	if (m_hEvent_chk_turn_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_chk_turn_pulse);
#elif defined(__linux__)
	if (m_hEvent_chk_turn_pulse != nullptr)
		m_hEvent_chk_turn_pulse->set();
#endif
}

bool VersusBase::checkAllLoadHole() {
	
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

			_smp::message_pool::getInstance().push(new message("[VersusBase::CheckAllLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearLoadHole() {

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

bool VersusBase::setLoadHoleAndCheckAllLoadHoleAndClear(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setLoadHoleAndCheckAllLoadHoleAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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

			_smp::message_pool::getInstance().push(new message("[VersusBase::setLoadHoleAndCheckAllLoadHoleAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
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

void VersusBase::setFinishCharIntro(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishCharIntro][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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
	if (m_hEvent_chk_turn_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_chk_turn_pulse);
#elif defined(__linux__)
	if (m_hEvent_chk_turn_pulse != nullptr)
		m_hEvent_chk_turn_pulse->set();
#endif
}

bool VersusBase::checkAllFinishCharIntro() {

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

			_smp::message_pool::getInstance().push(new message("[VersusBase::CheckAllFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearFinishCharIntro() {

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

bool VersusBase::setFinishCharIntroAndCheckAllFinishCharIntroAndClear(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishCharIntroAndCheckAllFinishCharIntroAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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

			_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishCharIntroAndCheckAllFinishCharIntroAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
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

void VersusBase::setFinishShot(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishShot][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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
	if (m_hEvent_chk_turn_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_chk_turn_pulse);
#elif defined(__linux__)
	if (m_hEvent_chk_turn_pulse != nullptr)
		m_hEvent_chk_turn_pulse->set();
#endif
}

bool VersusBase::checkAllFinishShot() {
	
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

			_smp::message_pool::getInstance().push(new message("[VersusBase::CheckAllFinishShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearFinishShot() {

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

bool VersusBase::setFinishShotAndCheckAllFinishShotAndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {
		
		_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishShotAndCheckAllFinishShotAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));
		
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

			_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishShotAndCheckAllFinishShotAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
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

void VersusBase::setFinishShot2(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishShot2][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->finish_shot2 = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool VersusBase::checkAllFinishShot2() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("CheckAllFinishShot2", "tentou verificar se todos os player terminaram a Tacada no jogo", _el);

			if (pgi->finish_shot2)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::CheckAllFinishShot2][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearFinishShot2() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_finish_shot2();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool VersusBase::setFinishShot2AndCheckAllFinishShot2AndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {
		
		_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishShotAndCheckAllFinishShotAndClear][Error] PlayerGameInfo* _pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));
		
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
	_pgi->finish_shot2 = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("setFinishShot2AndCheckAllFinishShot2AndClear", "tentou verificar se todos os player terminaram a Tacada no jogo", _el);

			if (pgi->finish_shot2)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::setFinishShot2AndCheckAllFinishShot2AndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_finish_shot2();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void VersusBase::setSyncShot(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setSyncShot[Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

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
	if (m_hEvent_chk_turn_pulse != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent_chk_turn_pulse);
#elif defined(__linux__)
	if (m_hEvent_chk_turn_pulse != nullptr)
		m_hEvent_chk_turn_pulse->set();
#endif
}

bool VersusBase::checkAllSyncShot() {
	
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

			_smp::message_pool::getInstance().push(new message("[VersusBase::CheckAllSyncShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearSyncShot() {

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

bool VersusBase::setSyncShotAndCheckAllSyncShotAndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setSyncShotAndCheckAllSyncShotAndClear][Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));
		
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

			_smp::message_pool::getInstance().push(new message("[VersusBase::setSyncShotAndCheckAllSyncShotAndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
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

void VersusBase::setSyncShot2(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setSyncShot2][Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->sync_shot_flag2 = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool VersusBase::checkAllSyncShot2() {
	
	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("CheckAllSyncShot2", "tentou verificar se todos os player sincronizaram a Tacada no jogo", _el);

			if (pgi->sync_shot_flag2)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::CheckAllSyncShot2][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return (count == m_players.size());
}

void VersusBase::clearSyncShot2() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_sync_shot2();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

bool VersusBase::setSyncShot2AndCheckAllSyncShot2AndClear(PlayerGameInfo* _pgi) {
	
	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setSyncShot2AndCheckAllSyncShot2AndClear][Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));
		
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
	_pgi->sync_shot_flag2 = 1u;

	// Check
	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("setSyncShot2AndCheckAllSyncShot2AndClear", "tentou verificar se todos os player sincronizaram a Tacada no jogo", _el);

			if (pgi->sync_shot_flag2)
				count++;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::setSyncShot2AndCheckAllSyncShot2AndClear][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});

	ret = (count == m_players.size());

	// Clear
	if (ret)
		clear_all_sync_shot2();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

void VersusBase::setInitShot(PlayerGameInfo* _pgi) {

	if (_pgi == nullptr) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::setInitShot][Error] PlayerGameInfo *_pgi is invalid(nullptr).", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Set
	_pgi->init_shot = 1u;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

void VersusBase::clearInitShot() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_init_shot();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

void VersusBase::clear_all_clear_hole() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_clear_hole", " tentou limpar all clear hole no jogo", _el);

			pgi->shot_sync.state_shot.display.stDisplay.acerto_hole = 0u;
			pgi->data.giveup = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_load_hole() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {

			INIT_PLAYER_INFO("clear_all_load_hole", " tentou limpar all load hole no jogo", _el);

			pgi->finish_load_hole = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_load_hole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_finish_char_intro() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_finish_char_intro", " tentou limpar all finish char intro no jogo", _el);

			pgi->finish_char_intro = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_finish_char_intro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});;
}

void VersusBase::clear_all_finish_shot() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_finish_shot", " tentou limpar all finish tacada no jogo", _el);

			pgi->finish_shot = 0u;

			// Limpa o tick_sync_end_shot para a próxima tacada(shot)
			pgi->tick_sync_end_shot.clear();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_finish_shot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_finish_shot2() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_finish_shot2", " tentou limpar all finish tacada no jogo", _el);

			pgi->finish_shot2 = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_finish_shot2][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_sync_shot() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_sync_shot", " tentou limpar all sync shot do jogo", _el);

			pgi->sync_shot_flag = 0u;

			// Limpa o tick_sync_shot para a próxima tacada(shot)
			pgi->tick_sync_shot.clear();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_sync_shot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_sync_shot2() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_sync_shot2", " tentou limpar all sync shot2 do jogo", _el);

			pgi->sync_shot_flag2 = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_sync_shot2][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_init_shot() {

	std::for_each(m_players.begin(), m_players.end(), [&](auto& _el) {

		try {
			
			INIT_PLAYER_INFO("clear_all_init_shot", " tentou limpar all init shot do jogo", _el);

			pgi->init_shot = 0u;

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[VersusBase::clear_all_ini_shot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	});
}

void VersusBase::clear_all_flag_sync() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	clear_all_load_hole();
	clear_all_finish_char_intro();
	clear_all_finish_shot();
	clear_all_finish_shot2();
	clear_all_sync_shot();
	clear_all_sync_shot2();
	clear_all_init_shot();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif
}

void VersusBase::init_treasure_hunter_info() {

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("ini_treasure_hunter_info", "tentou inicializar o treasure hunter info do versus base", el);

		m_thi += pgi->thi;
	}
}

void VersusBase::updateFinishHole() {

	packet p((unsigned short)0x65);

	// !!@@@
	// No Pang Battle tem o oid do player aqui ele passa
	//p.addUint32(oid);

	packet_func::game_broadcast(*this, p, 1);
}

void VersusBase::updateTreasureHunterPoint() {

	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	// Calcule Treasure Pontos
	for (auto& el : m_players) {

		INIT_PLAYER_INFO("updateTreasureHunterPoint", "tentou atualizar os pontos do Treasure Hunter no jogo", el);

		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			throw exception("[VersusBase::updateTreasureHunterPoint][Error] player[UID=" + std::to_string(el->m_pi.uid) + "] tentou atualizar os pontos do Treasure Hunter no hole[NUMERO="
					+ std::to_string((unsigned short)pgi->hole) + "], mas o hole nao existe. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 30, 0));

		/*m_thi.treasure_point += TreasureHunterSystem::calcPointNormal(pgi->data.tacada_num, hole->getPar().par) + m_thi.getPoint(pgi->data.tacada_num, hole->getPar().par);*/
		m_thi.treasure_point += sTreasureHunterSystem::getInstance().calcPointNormal(pgi->data.tacada_num, hole->getPar().par) + m_thi.getPoint(pgi->data.tacada_num, hole->getPar().par);
	}

	// Mostra score board
	packet p((unsigned short)0x132);

	p.addUint32(m_thi.treasure_point);

	// No Modo Match passa outro valor tbm

	packet_func::game_broadcast(*this, p, 1);
}

void VersusBase::requestDrawTreasureHunterItem() {

	// Sorteia os itens ganho do Treasure ponto do player
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	// Treasure Hunter Info do Versus
	/*auto v_item = TreasureHunterSystem::drawItem(m_thi.treasure_point, m_ri.course & 0x7F);*/
	auto v_item = sTreasureHunterSystem::getInstance().drawItem(m_thi.treasure_point, m_ri.course & 0x7F);

	if (v_item.empty())
		_smp::message_pool::getInstance().push(new message("[VersusBase::requestDrawTreasureHunterItem][WARNING] tentou sortear os item(ns) do Treasure Hunter do jogo," +
				std::string("mas o Treasure Hunter Item nao conseguiu sortear nenhum item"), CL_FILE_LOG_AND_CONSOLE));

	// tem que distribuir pelos jogadores do versus
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto item_it = v_item.begin(); item_it != v_item.end();) {
		
		for (auto it = m_player_order.begin(); it != m_player_order.end() && item_it != v_item.end(); ++it, ++item_it) {
			
			// Treasure Hunter Item Versus Base
			m_thi.v_item.push_back({ (*it)->uid, *item_it });

			// Treasure Hunter Item Player
			(*it)->thi.v_item.push_back(*item_it);
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void VersusBase::sendSyncShot() {

	if (m_player_turn == nullptr)
		throw exception("[VersusBase::sendSyncShot][Error] PlayerGameInfo* m_player_turn is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1200, 0));

	packet p((unsigned short)0x64);

	p.addBuffer(&m_player_turn->shot_sync, sizeof(m_player_turn->shot_sync));

	packet_func::game_broadcast(*this, p, 1);
}

void VersusBase::sendEndShot(player& _session, DropItemRet& _cube) {

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

	packet_func::game_broadcast(*this, p, 1);
}

void VersusBase::sendDropItem(player& _session) {

	packet p((unsigned short)0xFA);

	p.addUint16((unsigned short)m_players.size());

	for (auto& el : m_players) {
	
		INIT_PLAYER_INFO("sendDropItem", "tentou enviar os itens dropado do player no jogo", el);
		
		p.addUint32(el->m_oid);

		p.addUint8(0);	// OK

		p.addUint16((unsigned short)pgi->drop_list.v_drop.size());

		for (auto& el : pgi->drop_list.v_drop)
			p.addUint32(el._typeid);
	}

	packet_func::session_send(p, &_session, 1);
}

void VersusBase::sendPlacar(player& _session) {

	packet p((unsigned short)0x66);

	p.addUint8((unsigned char)m_players.size());

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("sendPlacar", "tentou enviar o placar do jogo", el);

		p.addUint32(el->m_oid);
		p.addUint8((unsigned char)getRankPlace(*el));
		p.addInt8((char)pgi->data.score);
		p.addInt8((unsigned char)pgi->data.total_tacada_num);

		p.addUint16((unsigned short)pgi->data.exp);
		p.addUint64(pgi->data.pang);
		p.addUint64(pgi->data.bonus_pang);

		// Valor que usa no Pang Battle, valor de pang que ganhou ou perdeu
		// Como aqui é vs Base deixa o valor 0
		p.addUint64(0ull);
	}

	packet_func::session_send(p, &_session, 1);
}

void VersusBase::sendTreasureHunterItemDrawGUI(player& _session) {

	INIT_PLAYER_INFO("sendTreasureHunterItemDrawGUI", "tentou enviar os itens ganho no Treasure Hunter(so o Visual) do jogo", &_session);

	packet p((unsigned short)0x133);

	p.addUint8((unsigned char)m_thi.v_item.size());

	// No VS aqui os itens são dividido entres os players do versus
	for (auto& el : m_thi.v_item) {
		p.addUint32(el.uid);	// UID do player que ganhou o item
		p.addUint32(el.thi._typeid);
		p.addUint16((unsigned short)el.thi.qntd);
		p.addUint8(0);	// Acho que sejá opção ou dizendo que acabou o struct de Treasure Hunter Item Draw
	}

	packet_func::session_send(p, &_session, 1);
}

void VersusBase::sendReplyFinishLoadHole() {

	try {

		init_turn_hole_start();

		PlayerGameInfo *pgi = requestCalculePlayerTurn();

		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			throw exception("[VersusBase::requestFinishLoadHole][Error] player[UID=" + std::to_string(pgi->uid) + "] tentou finalizar carregamento do hole[NUMERO="
					+ std::to_string(pgi->hole) + "], mas nao conseguiu encontrar o hole no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 201, 0));

		// Resposta de tempo do hole
		packet p((unsigned short)0x9E);

		p.addUint16(hole->getWeather());
		p.addUint8(0);	// Option do tempo, sempre peguei zero aqui dos pacotes que vi

		packet_func::game_broadcast(*this, p, 1);

		auto wind_flag = initCardWindPlayer(m_player_turn, hole->getWind().wind);

		// Resposta do vento do hole
		p.init_plain((unsigned short)0x5B);

		p.addUint8(hole->getWind().wind + wind_flag);
		p.addUint8((wind_flag < 0) ? 1 : 0);	// Flag de card de vento, aqui é a qnd diminui o vento, 1 Vento azul
		p.addUint16(m_player_turn->degree);
		p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original, Também é flag para trocar o vento no Pang Battle se mandar o valor 0

		packet_func::game_broadcast(*this, p, 1);

		// Resposta passa o oid do player que vai começa o Hole
		p.init_plain((unsigned short)0x53);

		if (m_player_turn == nullptr) {
			_smp::message_pool::getInstance().push(new message("[VersusBase::requestFinishLoadHole][Error] player_turn is invalid(nullptr)", CL_FILE_LOG_AND_CONSOLE));

			p.addUint32(0);
		}else
			p.addUint32(m_player_turn->oid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[VersusBase::sendReplyFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}
