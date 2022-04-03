// Arquivo versus.cpp
// Criado em 20/10/2018 as 22:49 por Acrisio
// Implementa��o da classe Versus

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "versus.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PACKET/packet_func_sv.h"

#include "treasure_hunter_system.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_update_last_player_game.hpp"

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[Versus::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[Versus::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[Versus::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 1, 4)); \

using namespace stdA;

Versus::Versus(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: VersusBase(_players, _ri, _rv, _channel_rookie), m_versus_state(false) {

	// Atualiza Treasure Hunter System Course
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	/*auto course = TreasureHunterSystem::findCourse((m_ri.course & 0x7F));*/
	auto course = sTreasureHunterSystem::getInstance().findCourse((m_ri.course & 0x7F));

	if (course == nullptr)
		_smp::message_pool::getInstance().push(new message("[Versus::Versus][Error] tentou pegar o course do Treasure Hunter System, mas o course[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "] nao existe no sistema", CL_FILE_LOG_AND_CONSOLE));
	else
		//TreasureHunterSystem::updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado
		sTreasureHunterSystem::getInstance().updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado

	// Aqui tem que inicializar os players info
	initAllPlayerInfo();

	// Last 5 Players Play, tem que salvar no server e no DB, and Achievement
	for (auto& el : m_players) {

		// Achievement
		INIT_PLAYER_INFO("Versus", "tentou inicializar o counter item do Versus", el);

		initAchievement(*el);

		pgi->sys_achieve.incrementCounter(0x6C40001Du/*Versus*/);
		// Fim de init Achievement

		for (auto& el2 : m_players) {

			if (el->m_pi.uid != el2->m_pi.uid)
				el->m_pi.l5pg.add(el2->m_pi, el->m_pi.mi.sexo);
		}

		// Update ON DB
		snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdateLastPlayerGame(el->m_pi.uid, el->m_pi.l5pg), Versus::SQLDBResponse, this);
	}

	m_versus_state = init_game();
}

Versus::~Versus() {

	// Para o tempo do player Turn
	stopTime();

	// Salva os dados de todos os jogadores
	for (auto& el : m_players)
		finish_game(*el);

	deleteAllPlayer();
}

bool Versus::deletePlayer(player* _session, int _option) {

	if (_session == nullptr)
		throw exception("[Versus::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 50, 0));
	
	bool ret = false;

	try {

		// Evitar deadlock com a thread checkVersusTurn - Bloqueia
		m_state_vs.lock();

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		auto it = std::find(m_players.begin(), m_players.end(), _session);

		if (it != m_players.end()) {
			unsigned char opt = 3;	// Saiu Quitou

			INIT_PLAYER_INFO("deletePlayer", "tentou sair do jogo", _session);

			if (m_game_init_state == 1/*Come�ou*/) {

				packet p;

				// Player Turn Para o tempo dele
				if (m_player_turn == pgi)
					stopTime();

				auto sessions = getSessions(*it);

				requestFinishItemUsedGame(*(*it));	// Salva itens usados no Tourney

				requestSaveInfo(*(*it), (_option == 0x800) ? 5/*N�o conta quit*/ : 1); // Quitou ou tomou DC

				//pgi->flag = PlayerGameInfo::eFLAG_GAME::QUIT;
				setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::QUIT);

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

				sendUpdateInfoAndMapStatistics(*_session, -1);

				ret = checkNextStepGame(*_session);

			}else if (m_game_init_state == 2 && !pgi->finish_game) {
	
				// Acabou
				requestSaveInfo(*(*it), 0);
			}
			
			// Deleta o player por give up ou time out, ele conta os achievements dele, tem o counter item 0x6C400004u Normal Game Complete
			// Envia os achievements para ele para ficar igual ao original
			if (m_game_init_state == 1/*Come�ou*/ && pgi->data.bad_condute >= 3 && (pgi->data.time_out >= 3 || pgi->data.giveup >= 3)) {

				// Achievements
				rain_hole_consecutivos_count(*_session);			// conta os achievement de chuva em holes consecutivas

				score_consecutivos_count(*_session);				// conta os achievement de back-to-back(2 ou mais score iguais consecutivos) do player

				rain_count(*_session);								// Aqui achievement de rain count

				pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

				// Achievement Aqui
				pgi->sys_achieve.finish_and_update(*_session);

				// Resposta que tem sempre que acaba um jogo, n�o sei o que � ainda, esse s� n�o tem no HIO Event
				packet p((unsigned short)0x244);

				p.addUint32(0);	// OK

				packet_func::session_send(p, _session, 1);

				// Esse � novo do JP, tem Tourney, VS, Grand Prix, HIO Event, n�o vi talvez tenha nos outros tamb�m
				p.init_plain((unsigned short)0x24F);

				p.addUint32(0);	// OK

				packet_func::session_send(p, _session, 1);
			}

			// Delete Player
			m_players.erase(it);
		}else
			_smp::message_pool::getInstance().push(new message("[Versus::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		// Evitar deadlock com a thread checkVersusTurn - Libera
		m_state_vs.unlock();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Versus::deletePlayer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Critical Section
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		// Evitar deadlock com a thread checkVersusTurn - Libera
		m_state_vs.unlock();
	}

	return ret;
}

void Versus::deleteAllPlayer() {

	while (!m_players.empty())
		deletePlayer(*m_players.begin(), 0);

}

void Versus::changeHole() {

	updateTreasureHunterPoint();

	if (m_players.size() <= 0 || checkEndGame(**m_players.begin()))
		finish_versus(0);
	else if (m_players.size() > 0)
		// Resposta terminou o hole
		updateFinishHole();	// Terminou
}

void Versus::finishHole() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_players) {

		requestFinishHole(*el, 0);

		requestUpdateItemUsedGame(*el);
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void Versus::finish_versus(int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		for (auto& el : m_players) {

			INIT_PLAYER_INFO("finish_versus", "tentou terminar o versus", el);

			pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

			requestCalculePang(*el);

			updatePlayerAssist(*el);

			sendFinishMessage(*el);
		}

		finish();
	}
}

void Versus::timeIsOver(void* _quem) {

	// Chama o timeIsOver da classe pai
	VersusBase::timeIsOver(_quem);

	if (_quem != nullptr) {

		player* p = reinterpret_cast< player* >(_quem);

		INIT_PLAYER_INFO("timeIsOver", "tentou acabar o tempo do turno no jogo", p);

		pgi->tempo = 1u;

		if (pgi->bar_space.getState() == 0 && pgi == m_player_turn) {

			pgi->tempo = 0u;

			if (++pgi->data.time_out >= 3)
				// 3 Time outs kika o jogado da sala
				pgi->data.bad_condute = 3;	// Kika Player

			// Time Out
			packet p((unsigned short)0x5C);

			p.addUint32(pgi->oid);

			packet_func::game_broadcast(*this, p, 1);
		}

	}else
		_smp::message_pool::getInstance().push(new message("[Versus::timeIsOver][WARNING] time is over executed without _quem, _quem is invalid(nullptr). Bug" , CL_FILE_LOG_AND_CONSOLE));
}

bool Versus::init_game() {

	auto lixo = VersusBase::init_game();

	if (m_players.size() > 0) {

		// variavel que salva a data local do sistema
		initGameTime();

		// Aqui achievement de rain count
		// Esse aqui tem que ser na hora que finaliza o jogo por que depende de quantos holes o player completou
		//rain_count_players();

		m_game_init_state = 1;	// Come�ou

		m_versus_state = true;
	}

	return true;
}

void Versus::requestFinishExpGame() {

	if (m_players.size() > 0) {

		player *_session = nullptr;
		float stars = m_course->getStar();
		int32_t exp = 0, hole_seq = 0;

		for (auto i = 0u; i < m_player_order.size(); ++i) {

			hole_seq = (int)m_course->findHoleSeq(m_player_order[i]->hole);

			// Ele est� no primeiro hole e n�o acertou ele, s� da experi�ncia se ele tiver acertado o hole
			if (hole_seq == 1 && !m_player_order[i]->shot_sync.state_shot.display.stDisplay.acerto_hole)
				hole_seq = 0;
			
			if ((_session = findSessionByUID(m_player_order[i]->uid)) != nullptr) {

				exp = (int)(1 * m_player_order.size() * (hole_seq > 0 ? hole_seq : 0) * stars);
				exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(m_player_order[i]->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));
				exp = (int)((float)exp * (float)(1.f - (i * 0.1f)));

				if (m_player_order[i]->level < 70/*Ultimo level n�o ganha exp*/)
					m_player_order[i]->data.exp = exp;
			}

			_smp::message_pool::getInstance().push(new message("[Versus::requestFinishExpGame][Log] player[UID=" + std::to_string(m_player_order[i]->uid) + "] ganhou " + std::to_string(m_player_order[i]->data.exp) + " de experience.", CL_FILE_LOG_AND_CONSOLE));

		}
	}
}

void Versus::finish() {

	m_versus_state = false;	// Terminou o versus

	m_game_init_state = 2; // Terminou o jogo

	requestCalculeRankPlace();

	requestFinishExpGame();

	requestDrawTreasureHunterItem();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el);
	}

}

void Versus::requestFinishData(player& _session) {

	// Finish Artefact Frozen Flame agora � direto no Finish Item Used Game
	requestFinishItemUsedGame(_session);

	requestSaveDrop(_session);

	rain_hole_consecutivos_count(_session);			// conta os achievement de chuva em holes consecutivas

	score_consecutivos_count(_session);				// conta os achievement de back-to-back(2 ou mais score iguais consecutivos) do player

	rain_count(_session);							// Aqui achievement de rain count

	//INIT_PLAYER_INFO("requestFinishData", "tentou finalizar dados do jogo", &_session);

	// Resposta Treasure Hunter Item Draw
	sendTreasureHunterItemDrawGUI(_session);

	// Resposta terminou game - Drop Itens
	sendDropItem(_session);

	// Resposta terminou game - Placar
	sendPlacar(_session);
}

bool Versus::finish_game(player& _session, int option) {
	
	if (
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.getState() && _session.isConnected() && m_players.size() > 0) {

		INIT_PLAYER_INFO("finish_game", "tentou finalizar o jogo", &_session);

		// Terminou o hole, finalizar o hole por ele
		if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole || pgi->data.giveup) {
			
			requestFinishHole(_session, 0);

			requestUpdateItemUsedGame(_session);
		}

		pgi->finish_game = 1u;

		if (PlayersCompleteGameAndClear() || option == 2/*Termina o jogo*/) {

			packet p;

			// Verifica se � o primeiro hole e se nem todos terminaram o hole
			if (m_course->findHoleSeq(pgi->hole) == 1 && !checkAllClearHole() && (pgi->progress.hole <= 0 || pgi->progress.finish_hole[pgi->progress.hole - 1] == 0/*N�o terminou o hole*/)) {

				for (auto& el : m_players) {

					INIT_PLAYER_INFO("finish_game", "tentou finalizar o versus", el);

					if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

						requestSaveInfo(*el, 2);

						if (pgi->finish_item_used == 0u)
							requestFinishItemUsedGame(*el);

						p.init_plain((unsigned short)0x67);

						packet_func::session_send(p, el, 1);

						//pgi->flag = PlayerGameInfo::eFLAG_GAME::END_GAME;
						setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::END_GAME);
					}
				}

				m_game_init_state = 2;	// Acabou o VS

				return true;

			}else {

				if (m_versus_state) // Deixa o cliente envia o pacote para finalizar o jogo, depois que ele mostrar os placares
					finish_versus(1);
				else {

					for (auto& el : m_players) {

						INIT_PLAYER_INFO("finish_game", "tentou finalizar o versus", el);

						if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

							requestSaveRecordCourse(*el, 0/*Normal Game*/, (m_ri.qntd_hole == 18 && m_course->findHoleSeq(pgi->hole) == 18) ? 1 : 0);

							requestSaveInfo(*el, 0);

							// D� Exp para o Caddie E Mascot Tamb�m
							if (pgi->data.exp > 0) {	// s� add exp se for maior que 0

								// Add Exp para o player
								el->addExp(pgi->data.exp, false/*N�o precisa do pacote para trocar de level*/);

								// D� Exp para o Caddie Equipado
								if (el->m_pi.ei.cad_info != nullptr)	// Tem um caddie equipado
									el->addCaddieExp(pgi->data.exp);

								// D� Exp para o Mascot Equipado
								if (el->m_pi.ei.mascot_info != nullptr)
									el->addMascotExp(pgi->data.exp);
							}

							sendUpdateInfoAndMapStatistics(*el, 0);

							// Resposta Treasure Hunter Item
							requestSendTreasureHunterItem(*el);

							// Update Mascot Info ON GAME, se o player estiver com um mascot equipado
							if (el->m_pi.ei.mascot_info != nullptr) {
								packet_func::pacote06B(p, el, &el->m_pi, 8);

								packet_func::session_send(p, el, 1);
							}

							// Achievement Aqui
							pgi->sys_achieve.finish_and_update(*el);

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

							p.addUint64(el->m_pi.ui.pang);

							p.addUint64(0ull);

							packet_func::session_send(p, el, 1);

							//pgi->flag = PlayerGameInfo::eFLAG_GAME::FINISH;
							setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::FINISH);
						}
					}

					m_game_init_state = 2;	// Acabou o VS

					return true;
				}
			}
		}
	}

	return m_players.size() == 0;
}

void Versus::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[Versus::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[Versus::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *game = reinterpret_cast<Game*>(_arg);

	switch (_msg_id) {
	case 1:	// Update Last 5 Player Game
	{
		auto cmd_l5pg = reinterpret_cast< CmdUpdateLastPlayerGame* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[Versus::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_l5pg->getUID()) + "] atualizou o Last 5 Player Game dele com sucesso!", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 0:
	default:
		break;
	}
}
