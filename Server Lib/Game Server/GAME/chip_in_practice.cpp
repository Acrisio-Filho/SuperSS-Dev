// Arquivo chip_in_practice.cpp

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "chip_in_practice.hpp"

#include "../PACKET/packet_func_sv.h"

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[ChipInPractice" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHIP_IN_PRACTICE, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[ChipInPractice::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHIP_IN_PRACTICE, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) PlayerGrandZodiacInfo *pgi = reinterpret_cast< PlayerGrandZodiacInfo* >(getPlayerInfo((__session))); \
	if (pgi == nullptr) \
		throw exception("[ChipInPractice::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHIP_IN_PRACTICE, 1, 4)); \

using namespace stdA;

ChipInPractice::ChipInPractice(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: GrandZodiacBase(_players, _ri, _rv, _channel_rookie), m_chip_in_practice_state(false) {

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("ChipInPractice", "tentou inicializar o counter item do Chip-in Practice", el);

		initAchievement(*el);

		pgi->sys_achieve.incrementCounter(0x6C40003Fu/*Chip-in Practice*/);
	}

	m_state = init_game();
}

ChipInPractice::~ChipInPractice() {

	m_chip_in_practice_state = false;

	if (m_game_init_state != 2)
		finish(2);

	while (!PlayersCompleteGameAndClear())
#if defined(_WIN32)
		Sleep(500);
#elif defined(__linux__)
		usleep(500000);
#endif

	deleteAllPlayer();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[ChipInPractice::~ChipInPractice][Log] ChipInPractice destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

void ChipInPractice::changeHole(player& _session) {

	try {

		nextHole(_session);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[ChipInPractice::changeHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void ChipInPractice::finishHole(player& _session) {

	requestFinishHole(_session, 0);
}

void ChipInPractice::finish_chip_in_practice(player& _session, int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		packet p;

		INIT_PLAYER_INFO("finish_chip_in_practice", "tentou terminar o Chip-in Practice no jogo", &_session);

		if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

			// Calcula os pangs que o player ganhou
			requestCalculePang(_session);

			// Atualizar os pang do player se ele estiver com assist ligado, e for maior que beginner E
			updatePlayerAssist(_session);

			if (m_game_init_state == 1 && _option == 0) {

				// Achievement Counter, Chip-in Practice n�o precisa do counter de game complete
				//pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

			}//else if (m_game_init_state == 1 && _option == 1)	// Acabou o Tempo
		}

		setGameFlag(pgi, (_option == 0) ? PlayerGameInfo::eFLAG_GAME::FINISH : PlayerGameInfo::eFLAG_GAME::END_GAME);
		
		GetLocalTime(&pgi->time_finish);

		// Terminou o jogo no Grand Zodiac
		setEndGame(pgi);

		setFinishGameFlag(pgi, 1u);

		// End Game
		p.init_plain((unsigned short)0x1F2);

		packet_func::session_send(p, &_session, 1);

		if (AllCompleteGameAndClear() && m_game_init_state == 1)
			finish(_option);	// Envia os pacotes que termina o jogo Ex: 0xCE, 0x79 e etc
	}
}

void ChipInPractice::timeIsOver() {

	if (m_game_init_state == 1 && m_players.size() > 0) {

		player* _session = nullptr;

		for (auto& el : m_player_info) {

			// S� os que n�o acabaram
			if (el.second->flag == PlayerGameInfo::eFLAG_GAME::PLAYING && (_session = findSessionByUID(el.second->uid)) != nullptr) {
				
				// Send full time que o pr�prio cliente termina
				packet p((unsigned short)0x8D);

				p.addUint32(m_ri.time_30s);

				packet_func::session_send(p, _session, 1);
			}
		}

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[ChipInPractice::timeIsOver][Log] Tempo Acabou no Chip-in Practice. na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}
}

bool ChipInPractice::init_game() {
	
	if (m_players.size() > 0) {

		// variavel que salva a data local do sistema
		initGameTime();

		m_game_init_state = 1;	// Come�ou

		m_chip_in_practice_state = true;
	}

	return true;
}

void ChipInPractice::requestFinishExpGame() {

	int32_t exp = 0l;
	player* _session = nullptr;

	for (auto& el : m_player_info) {

		if (el.second != nullptr) {

			if (el.second->flag == PlayerGameInfo::eFLAG_GAME::FINISH) {

				if ((_session = findSessionByUID(el.second->uid)) != nullptr) {

					exp = 45;
					exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(el.second->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));

					if (el.second->level < 70/*Ultimo level n�o ganha exp*/)
						el.second->data.exp = exp;
				}

			}else if (el.second->flag == PlayerGameInfo::eFLAG_GAME::END_GAME) {

				exp = (int)(reinterpret_cast< PlayerGrandZodiacInfo* >(el.second)->m_gz.hole_in_one / 2);
				exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(el.second->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));

				if (el.second->level < 70/*Ultimo level n�o ganha exp*/)
					el.second->data.exp = exp;
			}
		}
	}
}

void ChipInPractice::finish(int option) {

	m_game_init_state = 2;	// Acabou

	requestCalculeRankPlace();

	requestMakeTrofel();

	requestFinishExpGame();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el, option);
	}
}

void ChipInPractice::drawDropItem(player& _session) {
	return; // N�o tem drop de item no Chip-in Practice
}

void ChipInPractice::requestFinishData(player& _session, int option) {

	packet p;

	try {

		INIT_PLAYER_INFO("requestFinishData", "tentou finalizar os dado do player no jogo", &_session);

		requestSaveInfo(_session, 0/*Terminou*/);

		// Atualiza itens usados no Grand Zodiac
		requestUpdateItemUsedGame(_session);

		// Finish Artefact Frozen Flame agora � direto no Finish Item Used Game
		requestFinishItemUsedGame(_session);

		requestSaveDrop(_session);

		sendTimeIsOver(_session);

		// Resposta terminou game - Placar
		sendPlacar(_session);

		// Resposta Update Pang
		p.init_plain((unsigned short)0xC8);

		p.addUint64(_session.m_pi.ui.pang);

		p.addUint64(0ull);

		packet_func::session_send(p, &_session, 1);

		// A7
		p.init_plain((unsigned short)0xA7);

		p.addUint8(0u); // Count

		packet_func::session_send(p, &_session, 1);

		// AA
		p.init_plain((unsigned short)0xAA);

		p.addUint16(0u);	// Count

		p.addUint64(_session.m_pi.ui.pang);
		p.addUint64(_session.m_pi.cookie);

		packet_func::session_send(p, &_session, 1);

		// Update Mascot Info ON GAME, se o player estiver com um mascot equipado
		if (_session.m_pi.ei.mascot_info != nullptr) {
			packet_func::pacote06B(p, &_session, &_session.m_pi, 8);

			packet_func::session_send(p, &_session, 1);
		}

		// Achievement Aqui
		pgi->sys_achieve.finish_and_update(_session);

		// Esse � novo do JP, tem Tourney, VS, Grand Prix, HIO Event, n�o vi talvez tenha nos outros tamb�m
		p.init_plain((unsigned short)0x24F);

		p.addUint32(0);	// OK

		packet_func::session_send(p, &_session, 1);

		// Exp
		if (pgi->data.exp > 0) // s� add exp se for maior que 0
			_session.addExp(pgi->data.exp, true); // Update in Game

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[ChipInPractice::requestFinishData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void ChipInPractice::updateFinishHole(player& _session, int _option) {

	try {

		INIT_PLAYER_INFO("updateFinishHole", "tentou atualizar o finish hole do grand zodiac", &_session);

		// Passa a localiza��o do player, esse � a primeira, vez ent�o passa os valores do init Hole sempre
		packet p((unsigned short)0x1EE);

		p.addUint32(_session.m_oid);
		p.addFloat(pgi->location.x);
		p.addFloat(pgi->location.z);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[ChipInPractice::updateFinishHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void ChipInPractice::requestMakeTrofel() {
	return; // N�o tem isso no Chip-in Practice
}

void ChipInPractice::startGoldenBeam() {
	return; // N�o tem isso no Chip-in Practice
}

void ChipInPractice::endGoldenBeam() {
	return; // N�o tem isso no Chip-in Practice
}

bool ChipInPractice::finish_game(player& _session, int option) {
	
	if (
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.getState() && _session.isConnected() && m_players.size() > 0) {

		packet p;

		if (option == 0x12C/*packet12C pacote que termina o Grand Zodiac*/ || option == 2/*Saiu*/) {

			bool is_hacker_or_bug = false;

			if (m_timer != nullptr) {

				is_hacker_or_bug = ((int)(m_ri.time_30s - m_timer->getElapsed()) / (60 * 1000/*Minuto*/)) >= 1 ? true : false;

				if (is_hacker_or_bug && option == 0x12C)
					_smp::message_pool::getInstance().push(new message("[ChipInPractice::finish_game][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "] na sala[NUMERO=" + std::to_string(m_ri.numero) + "] TEMPO[FINISH=" + std::to_string(m_timer->getElapsed()) + ", FINISH_CORRETO=" + std::to_string(m_ri.time_30s) 
							+ "] pediu para terminar o Chip-in Practice com tempo menor que o do sala, pelo pacote normal, ele que ganhar exp, com menos tempo. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
			}

			if (m_chip_in_practice_state)
				finish_chip_in_practice(_session, (option == 0x12C && !is_hacker_or_bug) ? 0 : 1);	// Termina sem ter acabado de jogar
		}
	}

	return (PlayersCompleteGameAndClear() && m_chip_in_practice_state);
}
