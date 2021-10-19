// Arquivo special_shuffle_course.cpp
// Criado em 16/10/2018 as 22:42 por Acrisio
// Implementa��o da classe SpecialShuffleCourse

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "special_shuffle_course.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "treasure_hunter_system.hpp"
#include "drop_system.hpp"

#include "../PACKET/packet_func_sv.h"

#include "item_manager.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_update_item_slot.hpp"
#include "../PANGYA_DB/cmd_update_clubset_workshop.hpp"

#define REQUEST_BEGIN(method) if (!_session.getState()) \
									throw exception("[SpecialShuffleCourse::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY, 1, 0)); \
							  if (_packet == nullptr) \
									throw exception("[SpecialShuffleCourse::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[SpecialShuffleCourse::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY, 1, 4)); \

#define SPECIAL_SHUFFLE_COURSE_COIN_TYPEID 0x1A0000F8ul

#define ART_ROGER_K_STEERING_WHEEL	0x1A0001BCu	// de 500 a 501000 pangs no Ultimo Hole do game de 18H

using namespace stdA;

SpecialShuffleCourse::SpecialShuffleCourse(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: TourneyBase(_players, _ri, _rv, _channel_rookie), m_SSC_state(false), m_coin_SSC(0u) {

	// Atualiza Treasure Hunter System Course
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	/*auto course = TreasureHunterSystem::findCourse(m_ri.course & 0x7F);*/
	auto course = sTreasureHunterSystem::getInstance().findCourse(m_ri.course & 0x7F);

	if (course == nullptr)
		_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::SpecialShuffleCourse][Error] tentou pegar o course do Treasure Hunter System, mas o course[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "] nao existe no sistema", CL_FILE_LOG_AND_CONSOLE));
	else
		//TreasureHunterSystem::updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado
		sTreasureHunterSystem::getInstance().updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado

	// Aqui tem que inicializar os players info
	initAllPlayerInfo();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("SpecialShuffleCourse", "tentou inicializar o counter item do Special Shuffle Course", el);

		initAchievement(*el);

		pgi->sys_achieve.incrementCounter(0x6C40001Fu/*Tourney*/);	// Por que ele � um Tourney
	}

	m_state = init_game();
}

SpecialShuffleCourse::~SpecialShuffleCourse() {

	m_SSC_state = false;

	if (m_game_init_state != 2)
		finish();

	while (!PlayersCompleteGameAndClear())
#if defined(_WIN32)
		Sleep(500);
#elif defined(__linux__)
		usleep(500000);
#endif

	deleteAllPlayer();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::~SpecialShuffleCourse][Log] Tourney destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

bool SpecialShuffleCourse::deletePlayer(player* _session, int _option) {
	UNREFERENCED_PARAMETER(_option);
	
	if (_session == nullptr)
		throw exception("[SpecialShuffleCourse::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME, 50, 0));
	
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

			if (m_game_init_state == 1/*Começou*/) {

				packet p;

				INIT_PLAYER_INFO("deletePlayer", "tentou sair do jogo", _session);

				auto sessions = getSessions(*it);

				requestFinishItemUsedGame(*(*it));	// Salva itens usados no Tourney

				// SSC não salva info
				//requestSaveInfo(*(*it), 1); // Quitou ou tomou DC

				//pgi->flag = PlayerGameInfo::eFLAG_GAME::QUIT;
				setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::QUIT);

				// Resposta Player saiu do Jogo, tira ele do list de score
				p.init_plain((unsigned short)0x61);

				p.addUint32((*it)->m_oid);

				packet_func::vector_send(p, sessions, 1);

				// Resposta Player saiu do jogo
				sendUpdateState(*_session, opt);

				// Salva Achievement do player

				if (AllCompleteGameAndClear())
					ret = true;	// Termina o Tourney

				sendUpdateInfoAndMapStatistics(*_session, -1);
			}

			// Delete Player
			m_players.erase(it);
		}else
			_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::deletePlayer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Critical Section
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
	}

	return ret;
}

void SpecialShuffleCourse::deleteAllPlayer() {

	while (!m_players.empty())
		deletePlayer(*m_players.begin(), 0);
}

void SpecialShuffleCourse::changeHole(player& _session) {

	updateTreasureHunterPoint(_session);

	if (checkEndGame(_session))
		finish_SSC(_session, 0);
	else
		// Resposta terminou o hole
		updateFinishHole(_session, 1);	// Terminou
}

void SpecialShuffleCourse::finishHole(player& _session) {

	requestFinishHole(_session, 0);

	requestUpdateItemUsedGame(_session);
}

void SpecialShuffleCourse::finish_SSC(player& _session, int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		INIT_PLAYER_INFO("finish_SSC", "tentou terminar o Special Shuffle Course no jogo", &_session);

		if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

			// Calcula os pangs que o player ganhou
			requestCalculePang(_session);

			// Atualizar os pang do player se ele estiver com assist ligado, e for maior que beginner E
			updatePlayerAssist(_session);

			if (m_game_init_state == 1 && _option == 0) {

				// Mostra msg que o player terminou o jogo
				sendFinishMessage(_session);

				// Resposta terminou o hole
				updateFinishHole(_session, 1);

				// Resposta Terminou o Jogo, ou Saiu
				sendUpdateState(_session, 2);

				// Achievement Counter
				pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

			}else if (m_game_init_state == 1 && _option == 1) {	// Acabou o Tempo

				requestFinishHole(_session, 1);		// Acabou o Tempo

				// adicionar as tacadas dos holes que ele n�o fez, por que acabou o tempo
				// ------ O Original n�o soma as tacadas do resto dos holes que o player n�o jogou, quando o tempo acaba -------
				//pgi->ui.tacada = pgi->data.total_tacada_num;

				// Mostra msg que o player terminou o jogo
				sendFinishMessage(_session);

				// Resposta terminou o hole
				updateFinishHole(_session, 0);

				// Resposta para acabou o tempo do Tourney
				sendTimeIsOver(_session);
			}
		}

		//pgi->flag = (_option == 0) ? PlayerGameInfo::eFLAG_GAME::FINISH : PlayerGameInfo::eFLAG_GAME::END_GAME;
		setGameFlag(pgi, (_option == 0) ? PlayerGameInfo::eFLAG_GAME::FINISH : PlayerGameInfo::eFLAG_GAME::END_GAME);
		
		GetLocalTime(&pgi->time_finish);

		if (AllCompleteGameAndClear() && m_game_init_state == 1)
			finish();	// Envia os pacotes que termina o jogo Ex: 0xCE, 0x79 e etc
	}
}

void SpecialShuffleCourse::timeIsOver() {

	if (m_game_init_state == 1 && m_players.size() > 0) {

		player* _session = nullptr;

		for (auto& el : m_player_info) {

			// Só os que n�o acabaram
			if (el.second->flag == PlayerGameInfo::eFLAG_GAME::PLAYING && (_session = findSessionByUID(el.second->uid)) != nullptr)
				finish_SSC(*_session, 1/*Acabou Tempo*/);
			else if (el.second->flag == PlayerGameInfo::eFLAG_GAME::FINISH && (_session = findSessionByUID(el.second->uid)) != nullptr)
				// Resposta para acabou o tempo do Tourney
				sendTimeIsOver(*_session);
		}

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::timeIsOver][Log] Tempo Acabou no Tourney. na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}
}

bool SpecialShuffleCourse::init_game() {
	
	if (m_players.size() > 0) {

		// Cria o timer do Tourney
		startTime();

		// variavel que salva a data local do sistema
		initGameTime();

		// Aqui achievement de rain count
		// Esse aqui tem que ser na hora que finaliza o jogo por que depende de quantos holes o player completou
		//rain_count_players();

		m_game_init_state = 1;	// Come�ou

		m_SSC_state = true;
	}

	return true;
}

void SpecialShuffleCourse::finish() {

	m_game_init_state = 2;	// Acabou

	requestCalculeRankPlace();

	requestMakeMasterCoin();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el);
	}
}

DropItemRet SpecialShuffleCourse::requestInitDrop(player& _session) {

	try {
		
		auto dir = Game::requestInitDrop(_session);

		INIT_PLAYER_INFO("requestInitDrop", "tentou sortear o Drop do SSC no jogo", &_session);

		// Verifica se � o ultimo hole e sortea os pangs do final do SSC
		// Artefact Pang Drop
		if (m_ri.qntd_hole == m_course->findHoleSeq(pgi->hole) && m_ri.qntd_hole == 18) {	// Ultimo Hole, de 18h Game
			
			DropSystem::stCourseInfo ci{ 0 };

			// Init Course Info Drop System
			ci.artefact = ART_ROGER_K_STEERING_WHEEL;	// Para da os Pangs do SSC � o mesmo que o artefact
			ci.char_motion = pgi->char_motion_item;
			ci.course = m_ri.course & 0x7F;
			ci.hole = pgi->hole;
			ci.qntd_hole = m_ri.qntd_hole;

			auto art_pang = sDropSystem::getInstance().drawArtefactPang(ci, (uint32_t)m_players.size());

			if (art_pang._typeid != 0) {	// Dropou

				dir.v_drop.push_back(art_pang);

				// add para o drop list do player
				pgi->drop_list.v_drop.insert(pgi->drop_list.v_drop.end(), art_pang);

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

		return dir;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::RequestInitDrop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return DropItemRet();
}

void SpecialShuffleCourse::requestUpdateItemUsedGame(player& _session) {

	INIT_PLAYER_INFO("requestUpdateItemUsedGame", "tentou atualizar itens usado no jogo", &_session);

	auto& ui = pgi->used_item;

	// Club Mastery // (m_ri.course == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE ? 1.5f : 1.f)
	ui.club.count += (uint32_t)(1.5f * 10.f * ui.club.rate * TRANSF_SERVER_RATE_VALUE(m_rv.clubset) * TRANSF_SERVER_RATE_VALUE(ui.rate.club));

	// Passive Item exceto Time Booster e Auto Command, que soma o contador por uso, o cliente passa o pacote, dizendo que usou o item
	for (auto& el : ui.v_passive) {

		// Passive Item no SSC só consome os item boost de pang e o Club Mastery Boost,
		// Consome todos os outros menos os de Experiência
		if (std::find(passive_item_exp, LAST_ELEMENT_IN_ARRAY(passive_item_exp), el.second._typeid) == LAST_ELEMENT_IN_ARRAY(passive_item_exp)) {
			
			if (CHECK_PASSIVE_ITEM(el.second._typeid) && el.second._typeid != TIME_BOOSTER_TYPEID/*Time Booster*/ && el.second._typeid != AUTO_COMMAND_TYPEID)
				el.second.count++;
			else if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::BALL	/*Ball*/
					|| sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::AUX_PART) /*AuxPart(Anel)*/
				el.second.count++;
		}
	}
}

void SpecialShuffleCourse::requestFinishData(player& _session) {

	// Finish Artefact Frozen Flame agora é direto no Finish Item Used Game
	requestFinishItemUsedGame(_session);

	requestSaveDrop(_session);

	requestDrawTreasureHunterItem(_session);

	rain_hole_consecutivos_count(_session);			// conta os achievement de chuva em holes consecutivas

	score_consecutivos_count(_session);				// conta os achievement de back-to-back(2 ou mais score iguais consecutivos) do player

	rain_count(_session);							// Aqui achievement de rain count

	achievement_top_3_1st(_session);				// Se o Player ficou em Top 3 add +1 ao contador de top 3, e se ele ficou em primeiro add +1 ao do primeiro

	INIT_PLAYER_INFO("requestFinishData", "tentou finalizar dados do jogo", &_session);

	// Resposta terminou game - Drop Itens
	sendDropItem(_session);

	// Resposta terminou game - Placar
	sendPlacar(_session);

	// Resposta Treasure Hunter Item Draw
	sendTreasureHunterItemDrawGUI(_session);
}

void SpecialShuffleCourse::requestDrawTreasureHunterItem(player& _session) {

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
		_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::requestDrawTreasureHunterItem][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sortear os item(ns) do Treasure Hunter do jogo," +
				"mas o Treasure Hunter Item nao conseguiu sortear nenhum item", CL_FILE_LOG_AND_CONSOLE));
}

void SpecialShuffleCourse::requestMakeMasterCoin() {

	// minimo 3 moedas, max 4 moedas por cada jogador na sala
	//std::srand((size_t)this * std::clock() / ((size_t)time(nullptr) + 1) * (size_t)GetCurrentThreadId());

	m_coin_SSC = 3 + ((m_player_info.size() == 0) ? 0 : sRandomGen::getInstance().rIbeMt19937_64_chrono() % (((uint32_t)m_player_info.size() * 4) - 3));
}

void SpecialShuffleCourse::requestSendMasterCoin(player& _session) {

	if (m_ri.master == _session.m_pi.uid && m_coin_SSC > 0) {

		// Send Coin to Master
		stItem item{ 0 };

		item.type = 2;
		item.id = -1;
		item._typeid = SPECIAL_SHUFFLE_COURSE_COIN_TYPEID;
		item.qntd = m_coin_SSC;
		item.STDA_C_ITEM_QNTD = (short)item.qntd;

		auto rt = item_manager::RetAddItem::T_INIT_VALUE;

		if ((rt = item_manager::addItem(item, _session, 0, 0)) < 0) {
			_smp::message_pool::getInstance().push(new message("[SpecialShuffleCourse::requestSendMasterCoiin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou adicionar SSC coin[TYPEID=" 
					+ std::to_string(SPECIAL_SHUFFLE_COURSE_COIN_TYPEID) + "] para o master, mas deu erro no item_manager::addItem. Bug", CL_FILE_LOG_AND_CONSOLE));

			return;
		}

		// Resposta para enviar SSC coin para o master da sala
		packet p((unsigned short)0x198);

		p.addUint32(SPECIAL_SHUFFLE_COURSE_COIN_TYPEID);
		p.addUint32(m_coin_SSC);

		packet_func::session_send(p, &_session, 1);

		if (rt != item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {

			//auto v_item = std::vector< stItem >{ item };

			// Update Item ON Game pacoteAA no JP n�o att as moedas na hora
			p.init_plain((unsigned short)0x216);
			
			p.addUint32((const uint32_t)GetSystemTimeAsUnix());
			p.addUint32(1u);	// Count

			p.addUint8(item.type);
			p.addUint32(item._typeid);
			p.addInt32(item.id);
			p.addUint32(item.flag);
			p.addBuffer(&item.stat, sizeof(item.stat));
			p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
			p.addZeroByte(25);
			//packet_func::pacote0AA(p, &_session, v_item);

			packet_func::session_send(p, &_session, 1);
		}
	}
}

bool SpecialShuffleCourse::finish_game(player& _session, int option) {
	
	if (
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.getState() && _session.isConnected() && m_players.size() > 0) {

		packet p;

		if (option == 6/*packet06 pacote que termina o game*/) {

			if (m_SSC_state)
				finish_SSC(_session, 1);	// Termina sem ter acabado de jogar

			INIT_PLAYER_INFO("finish_game", "tentou terminar o jogo", &_session);

			// N�o terminou o Jogo a tempo, add as tacadas dos outros holes que ele nao conseguiu terminar
			// ------ O Original n�o soma as tacadas do resto dos holes que o player n�o jogou, quando o tempo acaba -------
			//if (pgi->flag == PlayerGameInfo::eFLAG_GAME::END_GAME)
				//pgi->ui.tacada = pgi->data.total_tacada_num;

			requestSaveInfo(_session, 4);

			requestSendMasterCoin(_session);

			// Update Info Map Statistics
			sendUpdateInfoAndMapStatistics(_session, 0);

			// Resposta Treasure Hunter Item
			requestSendTreasureHunterItem(_session);

			// Update Mascot Info ON GAME, se o player estiver com um mascot equipado
			if (_session.m_pi.ei.mascot_info != nullptr) {
				packet_func::pacote06B(p, &_session, &_session.m_pi, 8);

				packet_func::session_send(p, &_session, 1);
			}

			// Achievement Aqui
			pgi->sys_achieve.finish_and_update(_session);

			// Resposta que tem sempre que acaba um jogo, não sei o que é ainda, esse só não tem no HIO Event
			p.init_plain((unsigned short)0x244);

			p.addUint32(0);	// OK

			packet_func::session_send(p, &_session, 1);

			// Esse é novo do JP, tem Tourney, VS, Grand Prix, HIO Event, não vi talvez tenha nos outros também
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

	return (PlayersCompleteGameAndClear() && m_SSC_state);
}
