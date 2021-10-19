// Arquivo guild_battle.cpp
// Criado em 28/12/2019 as 17:37 por Acrisio
// Implementa��o da classe GuildBattle

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "guild_battle.hpp"
#include "treasure_hunter_system.hpp"

#include "../PACKET/packet_func_sv.h"

#define REQUEST_BEGIN(method) if (!_session.getState()) \
									throw exception("[GuildBattle::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GUILD_BATTLE, 1, 0)); \
							  if (_packet == nullptr) \
									throw exception("[GuildBattle::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GUILD_BATTLE, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[GuildBattle::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GUILD_BATTLE, 1, 4)); \

using namespace stdA;

GuildBattle::GuildBattle(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie, GuildRoomManager& _guild_manager)
	: TourneyBase(_players, _ri, _rv, _channel_rookie), m_guild_manager(_guild_manager) {

	// Atualiza Treasure Hunter System Course
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	auto course = sTreasureHunterSystem::getInstance().findCourse(m_ri.course & 0x7F);

	if (course == nullptr)
		_smp::message_pool::getInstance().push(new message("[GuildBattle::GuildBattle][Error] tentou pegar o course do Treasure Hunter System, mas o course[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "] nao existe no sistema", CL_FILE_LOG_AND_CONSOLE));
	else
		sTreasureHunterSystem::getInstance().updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado

	// Aqui tem que inicializar os players info
	initAllPlayerInfo();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("GuildBattle", "tentou inicializar o counter item do GuildBattle", el);

		initAchievement(*el);

		// Counter Item do Team Tournament, para n�o ficar sem fazer esse achievement por que n�o tem team tournament no Fresh UP!
		// Vou colocar para o player fazer esse achievement com o Guild Battle, j� que ele � um tournament de team tamb�m
		pgi->sys_achieve.incrementCounter(0x6C40003Au/*Team Tournament*/);

		// Counter Item do Guild Battle
		pgi->sys_achieve.incrementCounter(0x6C40003Bu/*Guild Battle*/);
	}

	// inicializa as duplas do Guild Battle
	init_duplas();

	// Inicializa o Guild Battle
	m_guild_battle_state = init_game();
}

GuildBattle::~GuildBattle() {

	m_guild_battle_state = false;

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
	_smp::message_pool::getInstance().push(new message("[GuildBattle::~GuildBattle][Log] GuildBattle destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

bool GuildBattle::deletePlayer(player *_session, int _option) {
	
	if (_session == nullptr)
		throw exception("[GuildBattle::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GUILD_BATTLE, 50, 0));
	
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

			Dupla *dup = nullptr;
			uint32_t dup_p_index = 0u;

			if (m_game_init_state == 1/*Come�ou*/) {

				packet p;

				INIT_PLAYER_INFO("deletePlayer", "tentou sair do jogo", _session);

				auto sessions = getSessions(*it);

				requestFinishItemUsedGame(*(*it));	// Salva itens usados no Guild Battle

				requestSaveInfo(*(*it), (_option == 0x800) ? 5/*N�o conta quit*/ : 1); // Quitou ou tomou DC

				setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::QUIT);

				// Update dados da dupla
				dup = m_guild_manager.findDupla(*_session);

				if (dup != nullptr) {

					dup_p_index = (dup->p[0/*P1*/] == _session) ? 0u/*P1*/ : 1u/*P2*/;

					dup->state[dup_p_index] = Dupla::eSTATE::OUT_GAME;

					for (auto i = 0u; i < m_course->findHoleSeq(dup->hole[!dup_p_index]); ++i) {
							
						if (!dup->dados[dup_p_index][i].finish) {
							dup->dados[!dup_p_index][i].score = 2;
							dup->dados[!dup_p_index][i].finish = 1u;
						}
					}

					// Atualiza dados do GuildRoomManager
					m_guild_manager.update();

					// send update player dupla guild
					sendFinishHoleDupla(*_session);

				}else
					_smp::message_pool::getInstance().push(new message("[GuildBattle::deletePlayer][WARNING] Player[UID=" + std::to_string(_session->m_pi.uid) 
							+ "], nao esta em nenhuma dupla no Guild Battle na sala[NUMERO=" + std::to_string(m_ri.numero) + "]. Bug.", CL_FILE_LOG_AND_CONSOLE));

				// Resposta Player saiu do Jogo, tira ele do list de score
				p.init_plain((unsigned short)0x61);

				p.addUint32((*it)->m_oid);

				packet_func::vector_send(p, sessions, 1);

				// Resposta Player saiu do jogo
				sendUpdateState(*_session, opt);

				// Att dados de mapas e info do player
				sendUpdateInfoAndMapStatistics(*_session, -1);

			}else
				requestSaveInfo(*(*it), (_option == 0x800) ? 5/*N�o conta quit*/ : 1);

			// Delete Player
			m_players.erase(it);

			if (AllCompleteGameAndClear() || AllTeamQuit()) {

				for (auto& el : m_player_info)
					if (el.first != nullptr && 
#if defined(_WIN32)
						el.first->m_sock != INVALID_SOCKET
#elif defined(__linux__)
						el.first->m_sock.fd != INVALID_SOCKET
#endif
						&& el.first != _session && el.second->flag == PlayerGameInfo::eFLAG_GAME::PLAYING
						&& findSessionByUID(el.second->uid) != nullptr)
						finish_guild_battle(*el.first, 0);	// Acabou o Guild Battle

				ret = true;	// Termina o Guild Battle
			
			}else if (dup != nullptr) // Termina o jogo para o outro player da dupla
				finish_guild_battle(*dup->p[!dup_p_index], 0/*Termina o jogo para o outro player da dupla*/);

		}else
			_smp::message_pool::getInstance().push(new message("[GuildBattle::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GuildBattle::deletePlayer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Libera Critical Section
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
	}

	return ret;
}

void GuildBattle::deleteAllPlayer() {

	while (!m_players.empty())
		deletePlayer(*m_players.begin(), 0);
}

void GuildBattle::sendInitialData(player& _session) {

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

			// Guild Battle Duplas
			m_guild_manager.initPacketDuplas(p);

			packet_func::game_broadcast(*this, p, 1);

			// Course
			// Send Individual Packet to all players in game
			for (auto& el : m_players)
				Game::sendInitialData(*el);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GuildBattle::sendInitialData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GuildBattle::requestInitHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitHole");

	try {

		TourneyBase::requestInitHole(_session, _packet);

		INIT_PLAYER_INFO("requestInitHole", "tentou inicializar o hole", &_session);

		auto dup = m_guild_manager.findDupla(_session);

		if (dup != nullptr) {

			if (dup->p[0/*P1*/] == &_session)
				dup->hole[0/*P1*/] = pgi->hole;
			else
				dup->hole[1/*P2*/] = pgi->hole;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GuildBattle::requestInitHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GuildBattle::changeHole(player& _session) {

	updateTreasureHunterPoint(_session);

	if (checkEndGame(_session))
		finish_guild_battle(_session, 0);
	else
		// Resposta terminou o hole
		updateFinishHole(_session, 1);	// Terminou
}

void GuildBattle::finishHole(player& _session) {

	INIT_PLAYER_INFO("finishHole", "tentu finializar o hole", &_session);

	if (m_guild_manager.finishHoleDupla(*pgi, m_course->findHoleSeq(pgi->hole)))
		sendFinishHoleDupla(_session);

	requestFinishHole(_session, 0);

	requestUpdateItemUsedGame(_session);
}

void GuildBattle::finish_guild_battle(player& _session, int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		INIT_PLAYER_INFO("finish_guild_battle", "tentou terminar o guild battle no jogo", &_session);

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

				auto dup = m_guild_manager.findDupla(_session);

				if (dup != nullptr) {

					auto dup_p_index = (dup->p[0] == &_session) ? 0u/*P1*/ : 1u/*P2*/;

					dup->state[dup_p_index] = Dupla::eSTATE::OVER_TIME;

					for (auto i = 0u; i < m_ri.qntd_hole; ++i) {

						if (dup->dados[dup_p_index][i].finish && !dup->dados[!dup_p_index][i].finish)
							dup->dados[dup_p_index][i].score = 2;
						else if (!dup->dados[dup_p_index][i].finish && dup->dados[!dup_p_index][i].finish)
							dup->dados[!dup_p_index][i].score = 2;
					}

					// Atualiza dados das guilds
					m_guild_manager.update();

					// Finish hole Dupla
					sendFinishHoleDupla(_session);
				}

				requestFinishHole(_session, 1);		// Acabou o Tempo

				// Mostra msg que o player terminou o jogo
				sendFinishMessage(_session);

				// Resposta terminou o hole
				updateFinishHole(_session, 0);

				// Resposta para acabou o tempo do Guild Battle
				sendTimeIsOver(_session);
			
			}

			setGameFlag(pgi, (_option == 0) ? PlayerGameInfo::eFLAG_GAME::FINISH : PlayerGameInfo::eFLAG_GAME::END_GAME);
		
		}
		
		GetLocalTime(&pgi->time_finish);

		if ((AllCompleteGameAndClear() || AllTeamQuit()) && m_game_init_state == 1)
			finish();	// Envia os pacotes que termina o jogo Ex: 0xCE, 0x79 e etc
	}
}

void GuildBattle::timeIsOver() {

	if (m_game_init_state == 1 && m_players.size() > 0) {

		player* _session = nullptr;

		for (auto& el : m_player_info) {

			// S� os que n�o acabaram
			if (el.second->flag == PlayerGameInfo::eFLAG_GAME::PLAYING && (_session = findSessionByUID(el.second->uid)) != nullptr)
				finish_guild_battle(*_session, 1/*Acabou Tempo*/);
			else if (el.second->flag == PlayerGameInfo::eFLAG_GAME::FINISH && (_session = findSessionByUID(el.second->uid)) != nullptr)
				// Resposta para acabou o tempo do Guild Battle
				sendTimeIsOver(*_session);
		}

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GuildBattle::timeIsOver][Log] Tempo Acabou no GuildBattle. na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}
}

bool GuildBattle::init_game() {
	
	if (m_players.size() > 0) {

		// Cria o timer do Guild Battle
		startTime();

		// variavel que salva a data local do sistema
		initGameTime();

		m_game_init_state = 1;	// Come�ou

		m_guild_battle_state = true;
	}

	return true;
}

void GuildBattle::requestFinishExpGame() {

	if (m_players.size() > 0) {

		player *_session = nullptr;
		float stars = m_course->getStar();
		int32_t exp = 0, hole_seq = 0;

		for (auto i = 0u; i < m_player_order.size(); ++i) {

			hole_seq = (int)m_course->findHoleSeq(m_player_order[i]->hole);

			// Ele est� no primeiro hole e n�o acertou ele, s� da experi�ncia se ele tiver acertado o hole
			if (hole_seq == 1 && !m_player_order[i]->shot_sync.state_shot.display.stDisplay.acerto_hole)
				hole_seq = 0;

			if (m_player_order[i]->flag == PlayerGameInfo::eFLAG_GAME::FINISH) {

				if ((_session = findSessionByUID(m_player_order[i]->uid)) != nullptr) {

					exp = (int)(1 * m_player_order.size() * (hole_seq > 0 ? hole_seq : 0) * stars);
					exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(m_player_order[i]->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));
					exp = (int)(exp * (1 - (i / m_player_info.size())));

					if (m_player_order[i]->level < 70/*Ultimo level n�o ganha exp*/)
						m_player_order[i]->data.exp = exp;
				}

			}else if (m_player_order[i]->flag == PlayerGameInfo::eFLAG_GAME::END_GAME) {
				
				if ((_session = findSessionByUID(m_player_order[i]->uid)) != nullptr) {

					exp = (int)(1 * m_player_order.size() * (hole_seq > 0 ? hole_seq : 0) * stars);
					exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(m_player_order[i]->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));
					exp = (int)(exp * (1 - (i / m_player_info.size())));

					if (m_player_order[i]->level < 70/*Ultimo level n�o ganha exp*/)
						m_player_order[i]->data.exp = exp;
				}
			}

			// Log
			_smp::message_pool::getInstance().push(new message("[GuildBattle::requestFinishExpGame][Log] player[UID=" + std::to_string(m_player_order[i]->uid) 
					+ "] ganhou " + std::to_string(m_player_order[i]->data.exp) + " de experience.", CL_FILE_LOG_AND_CONSOLE));

		}
	}
}

void GuildBattle::finish() {

	m_game_init_state = 2;	// Acabou

	requestCalculeRankPlace();

	m_guild_manager.calcGuildWin();

	requestSaveGuildData();					// Salva os dados da guild

	requestFinishExpGame();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el);
	}
}

void GuildBattle::init_duplas() {

	m_guild_manager.init_duplas();
}

bool GuildBattle::AllTeamQuit() {

	if (m_players.size() == 0)
		return true;

	return m_guild_manager.oneGuildRest();
}

void GuildBattle::requestFinishData(player& _session) {

	// Finish Artefact Frozen Flame agora � direto no Finish Item Used Game
	requestFinishItemUsedGame(_session);

	requestSaveDrop(_session);

	rain_hole_consecutivos_count(_session);			// conta os achievement de chuva em holes consecutivas

	score_consecutivos_count(_session);				// conta os achievement de back-to-back(2 ou mais score iguais consecutivos) do player

	rain_count(_session);							// Aqui achievement de rain count

	achievement_top_3_1st(_session);				// Se o Player ficou em Top 3 add +1 ao contador de top 3, e se ele ficou em primeiro add +1 ao do primeiro

	// Resposta terminou game - Drop Itens
	sendDropItem(_session);

	// Resposta terminou game - Placar
	sendPlacar(_session);
}

void GuildBattle::requestSaveGuildData() {

	// Salva os dados das Guild que jogaram o Guild Battle
	m_guild_manager.saveGuildsData();
}

void GuildBattle::sendFinishHoleDupla(player& _session) {

	auto dup = m_guild_manager.findDupla(_session);

	if (dup != nullptr) {

		packet p((unsigned short)0xC2);

		p.addUint32(_session.m_oid);

		auto guild = m_guild_manager.findGuildByTeam(Guild::eTEAM::RED);

		p.addUint16(guild != nullptr ? guild->getPoint() : 0u);

		guild = m_guild_manager.findGuildByTeam(Guild::eTEAM::BLUE);

		p.addUint16(guild != nullptr ? guild->getPoint() : 0u);
		
		// P1
		if (dup->p[0] == &_session) {
			
			p.addUint8((unsigned char)dup->sumScoreP1());
			p.addUint8((unsigned char)dup->sumScoreP2());
		
		}else { // P2

			p.addUint8((unsigned char)dup->sumScoreP2());
			p.addUint8((unsigned char)dup->sumScoreP1());
		}

		packet_func::game_broadcast(*this, p, 1);
	}
}

void GuildBattle::sendPlacar(player& _session) {

	INIT_PLAYER_INFO("sendPlacar", "tentou enviar o placar do jogo", &_session);

	packet p((unsigned short)0x79);

	p.addUint32(pgi->data.exp);

	p.addUint32(m_ri.trofel);

	p.addUint8(0u);	// Trofel Que o Player Ganhou
	p.addUint8(m_guild_manager.getGuildWin());	// Team Win, 0 - vermelho, 1 - Azul, 2 nenhum

	auto dup = m_guild_manager.findDupla(_session);

	if (dup != nullptr) {
		p.addUint32((dup->p[0] == &_session) ? dup->pang_win[0] : dup->pang_win[1]);
		p.addUint32((dup->p[0] == &_session) ? dup->sumScoreP1() : dup->sumScoreP2());
	}else // 2 valores de int32 com o valor 0
		p.addUint64(0u);

	auto guild = m_guild_manager.findGuildByTeam(Guild::eTEAM::RED);

	if (guild != nullptr)
		p.addUint32(guild->getPangWin());
	else
		p.addUint32(0u);

	guild = m_guild_manager.findGuildByTeam(Guild::eTEAM::BLUE);

	if (guild != nullptr)
		p.addUint32(guild->getPangWin());
	else
		p.addUint32(0u);

	// Medalhas
	for (auto i = 0u; i < (sizeof(m_medal) / sizeof(m_medal[0])); ++i)
		p.addBuffer(&m_medal[i], sizeof(Medal));

	// N�o sei se � a geral ou se � s� a do Tourney, (DEIXEI A GERAL) todas as medalhas que ele tem
	p.addBuffer(&_session.m_pi.ui.medal, sizeof(stMedal));

	packet_func::session_send(p, &_session, 1);
}

bool GuildBattle::finish_game(player& _session, int option) {
	
	if (
#if defined(_WIN32)
		_session.m_sock != INVALID_SOCKET 
#elif defined(__linux__)
		_session.m_sock.fd != INVALID_SOCKET 
#endif
	&& _session.getState() && _session.isConnected() && m_players.size() > 0) {

		packet p;

		if (option == 6/*packet06 pacote que termina o game*/) {

			if (m_guild_battle_state)
				finish_guild_battle(_session, 1);	// Termina sem ter acabado de jogar

			INIT_PLAYER_INFO("finish_game", "tentou terminar o jogo", &_session);

			requestSaveInfo(_session, 0);

			// D� Exp para o Caddie E Mascot Tamb�m
			if (pgi->data.exp > 0) {	// s� add exp se for maior que 0

				// Add Exp para o player
				_session.addExp(pgi->data.exp, false/*N�o precisa do pacote para trocar de level*/);

				// D� Exp para o Caddie Equipado
				if (_session.m_pi.ei.cad_info != nullptr)	// Tem um caddie equipado
					_session.addCaddieExp(pgi->data.exp);

				// D� Exp para o Mascot Equipado
				if (_session.m_pi.ei.mascot_info != nullptr)
					_session.addMascotExp(pgi->data.exp);
			}

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

	return (PlayersCompleteGameAndClear() && m_guild_battle_state);
}
