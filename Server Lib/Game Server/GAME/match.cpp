// Arquivo match.cpp
// Criado em 02/11/2018 as 14:39 por Acrisio
// Implementa��o da classe Match

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "match.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "treasure_hunter_system.hpp"

#include "../PACKET/packet_func_sv.h"

#include "../UTIL/map.hpp"

#include "../Game Server/game_server.h"
#include "../UTIL/club3d.hpp"

#if defined(__linux__)
#include <numbers> // Pi
#include <cmath>
#endif

#define CHECK_SESSION(method) if (!_session.getState()) \
									throw exception("[Match::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 1, 0)); \

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[Match::request" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[Match::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) auto pgi = getPlayerInfo((__session)); \
	if (pgi == nullptr) \
		throw exception("[Match::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 1, 4)); \

// Ponteiro de session
#define INIT_TEAM_INFO(_method, __session) Team *team = nullptr; \
{ \
	auto __it = std::find_if(m_teans.begin(), m_teans.end(), [&](auto& _el) { \
		return _el.findPlayerByUID((__session)->m_pi.uid) != nullptr; \
	}); \
	if (__it != m_teans.end()) \
		team = &(*__it); \
	else \
		throw exception("[Match::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] tentou encontrar o team(time) dele, mas no game nao tem o team(time) dele. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 100, 0)); \
} \

using namespace stdA;

Match::Match(std::vector<player*>& _players, RoomInfoEx & _ri, RateValue _rv, unsigned char _channel_rookie, std::vector< Team >& _teans)
	: VersusBase(_players, _ri, _rv, _channel_rookie), m_team_win(0u), m_match_state(false), m_teans(_teans), m_team_turn(nullptr), m_thi_blue{0} {

	// Atualiza Treasure Hunter System Course
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	/*auto course = TreasureHunterSystem::findCourse((m_ri.course & 0x7F));*/
	auto course = sTreasureHunterSystem::getInstance().findCourse((m_ri.course & 0x7F));

	if (course == nullptr)
		_smp::message_pool::getInstance().push(new message("[Match::Match][Error] tentou pegar o course do Treasure Hunter System, mas o course[COURSE="
				+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "] nao existe no sistema", CL_FILE_LOG_AND_CONSOLE));
	else
		//TreasureHunterSystem::updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado
		sTreasureHunterSystem::getInstance().updateCoursePoint(*course, -1);	// -1 ponto a cada jogo iniciado

	// Aqui tem que inicializar os players info
	initAllPlayerInfo();

	// Inicializar as posi��es dos jogadores do team que n� sala ele pode ser diferente, a ordem que entrou no team
	// Tem que sempre ficar na ordem da sala
	init_team_player_position();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("Match", "tentou inicializar o counter item do Match", el);

		initAchievement(*el);

		pgi->sys_achieve.incrementCounter(0x6C40001Eu/*Match*/);
	}

	m_match_state = init_game();
}

Match::~Match() {

	// Para o tempo do player Turn
	stopTime();

	// Salva os dados de todos os jogadores
	for (auto& el : m_players)
		finish_game(*el);

	clear_teans();

	m_team_win = 0u;

	deleteAllPlayer();
}

bool Match::deletePlayer(player* _session, int _option) {
	
	if (_session == nullptr)
		throw exception("[Match::deletePlayer][Error] tentou deletar um player, mas o seu endereco eh nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 50, 0));
	
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

				INIT_TEAM_INFO("deletePlayer", _session);

				packet p;

				// Player Turn Para o tempo dele
				if (m_player_turn == pgi)
					stopTime();

				auto sessions = getSessions(*it);

				requestFinishItemUsedGame(*(*it));	// Salva itens usados no Tourney

				requestSaveInfo(*(*it), (_option == 0x800) ? 5/*N�o conta quit*/ : 1); // Quitou ou tomou DC

				//pgi->flag = PlayerGameInfo::eFLAG_GAME::QUIT;
				setGameFlag(pgi, PlayerGameInfo::eFLAG_GAME::QUIT);

				// Player Quitou, o time desiste
				team->setQuit(1u);

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

				// Salva Achievement do player

				sendUpdateInfoAndMapStatistics(*_session, -1);

				ret = checkNextStepGame(*_session);

				if (!ret && m_players.size() > 0)
					ret = 1;

			}else if (m_game_init_state == 2 && !pgi->finish_game) {
	
				// Acabou
				requestSaveInfo(*(*it), 0);

				// Salva Achievement tbm
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
			_smp::message_pool::getInstance().push(new message("[Match::deletePlayer][WARNING] player ja foi excluido do game.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		// Evitar deadlock com a thread checkVersusTurn - Libera
		m_state_vs.unlock();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::deletePlayer][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

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

void Match::deleteAllPlayer() {

	while (!m_players.empty())
		deletePlayer(*m_players.begin(), 0);
}

void Match::requestInitHole(player& _session, packet *_packet) {
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

		INIT_TEAM_INFO("requestInitHole", &_session);

		// Update Location Player in Hole
		pgi->location.x = ctx_hole.tee.x;
		pgi->location.z = ctx_hole.tee.z;

		// Update Team Location
		team->setLocation(pgi->location);
		
		// N�mero do hole atual, que o player est� jogando
		pgi->hole = ctx_hole.numero;

		// Flag que marca se o player j� inicializou o primeiro hole do jogo
		if (!pgi->init_first_hole)
			pgi->init_first_hole = 1u;

		// Update Team Hole
		team->setHole(ctx_hole.numero);

		// Gera degree para o player ou pega o degree sem gerar que � do modo do hole repeat
		team->setDegree((m_ri.modo == Hole::M_REPEAT) ? hole->getWind().degree.getDegree() : hole->getWind().degree.getShuffleDegree());

		//pgi->degree = team->getDegree();

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Match::requestInitHole][Log] player[UID=" + std::to_string(pgi->uid) + "] Vento[Graus=" + std::to_string(pgi->degree) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::requestInitHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}
}

void Match::requestMoveBall(player& _session, packet* _packet) {
	REQUEST_BEGIN("MoveBall");

	packet p;

	try {

		float x = _packet->readFloat();
		float y = _packet->readFloat();
		float z = _packet->readFloat();

		INIT_PLAYER_INFO("requestMoveBall", "tentou recolocar a bola no jogo", &_session);

		INIT_TEAM_INFO("requestMoveBall", &_session);

		pgi->location.x = x;
		pgi->location.y = y;
		pgi->location.z = z;

		// Update Team Location
		team->setLocation(pgi->location);

		team->decrementPlayerStartHole();

		// para o tempo do da tacada do player, que ele vai recolocar e come�a um novo tempo depois
		stopTime();

		// Resposta para Move Ball
		p.init_plain((unsigned short)0x60);

		p.addFloat(pgi->location.x);
		p.addFloat(pgi->location.y);
		p.addFloat(pgi->location.z);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::requestMoveBall][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Match::changeHole() {

	updateTreasureHunterPoint();

	if (m_player_turn == nullptr)
		throw exception("[Match::changeHole][Error] PlayerGameInfo *m_player_turn is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 100, 0));

	auto hole_seq = m_course->findHoleSeq(m_player_turn->hole);

	int state = 0, hole_diff = m_ri.qntd_hole - hole_seq;

	if ((int)(m_teans[0].getPoint() - m_teans[1].getPoint()) > hole_diff)
		state = 1;
	else if ((int)(m_teans[1].getPoint() - m_teans[0].getPoint()) > hole_diff)
		state = 1;

	if (state || m_players.size() <= 0 || checkEndGame(**m_players.begin())) {

		// Resposta para o player que terminou o ultimo hole do Game
		packet p((unsigned short)0x199);

		packet_func::game_broadcast(*this, p, 1);

		// Fez o Ultimo Hole, Calcula Clear Bonus para o player
		if (!sMap::getInstance().isLoad())
			sMap::getInstance().load();

		auto map = sMap::getInstance().getMap(m_ri.course & 0x7F);

#ifdef _DEBUG
		auto clear_bonus = 0u;
#endif // _DEBUG

		// !!@@@ aqui pode ser que adicionar o clear bonus pros 2 mesmo que o outro n�o fez o ultimo hole,
		// j� estou add, s� vou deixar coment�rio para causo de erro ou mude de ideia a frente
		if (map == nullptr)
			_smp::message_pool::getInstance().push(new message("[Match::changeHole][Error][WARNING] tentou pegar o Map dados estaticos do course[COURSE="
					+ std::to_string((unsigned short)(m_ri.course & 0x7F)) + "], mas nao conseguiu encontra na classe do Server.", CL_FILE_LOG_AND_CONSOLE));
		else {
			for (auto& team : m_teans) {
#ifdef _DEBUG
				team.incrementBonusPang((clear_bonus = sMap::getInstance().calculeClearMatch(*map, (uint32_t)hole_seq)));

				_smp::message_pool::getInstance().push(new message("[Match::changeHole][Log] player_turn[UID=" + std::to_string(m_player_turn->uid) + "] do Team[ID="
						+ std::to_string(team.getId()) + "] fez o ultimo* hole do Match e ganhou " + std::to_string(clear_bonus) + " Clear Bonus", CL_FILE_LOG_AND_CONSOLE));
#else
				team.incrementBonusPang(sMap::getInstance().calculeClearMatch(*map, m_ri.qntd_hole));
#endif // _DEBUG
			}
		}

		// Finish Match
		finish_match(0);

	}else if (m_players.size() > 0)
		// Resposta terminou o hole
		updateFinishHole();	// Terminou
}

void Match::finishHole() {

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

void Match::finish_match(int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		// All All Pang and Bonus Pang All players of teans
		requestUpdateTeamPang();

		for (auto& el : m_players) {

			INIT_PLAYER_INFO("finish_match", "tentou finalizar o Match", el);

			pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

			requestCalculePang(*el);

			updatePlayerAssist(*el);

			sendFinishMessage(*el);
		}

		finish();
	}
}

void Match::requestTeamFinishHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("TeamFinishHole");

	try {

		// 9 putt, 10 Chip-in
		auto state_finish = _packet->readUint16();

		INIT_TEAM_INFO("requestTeamFinishHole", &_session);

		team->setStateFinish(state_finish);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Match::requestTeamFinishHole][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] do Team[ID=" + std::to_string(team->getId()) + ", STATE=" + std::to_string(team->getStateFinish()) + "] terminou o hole.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::requestTeamFinishHole][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

stGameShotValue Match::getGameShotValueToSmartCalculator(player& _session, unsigned char _club_index, unsigned char _power_shot_index) {
	CHECK_SESSION("getGameShotValueToSmartCalculator");

	stGameShotValue gsv{ 0u };

	try {

		INIT_PLAYER_INFO("getGameShotValueToSmartCalculator", "tentou executar Smart Calculator Command", &_session);
		
		INIT_TEAM_INFO("getGameShotValueToSmartCalculator", &_session);

		auto hole = m_course->findHole(team->getHole());

		if (hole == nullptr)
			throw exception("[Match::getGameShotValueToSmartCalculator][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou executar Smart Calculator command na sala[NUMERO=" + std::to_string(m_ri.numero)
					+ "], mas nao encontrou o Hole[NUMERO=" + std::to_string((short)team->getHole())
					+ "] no Course.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 10000, 0));

		auto wind_flag = initCardWindPlayer(pgi, hole->getWind().wind);

		auto wind = hole->getWind().wind + 1 + wind_flag;
		auto distance = hole->getPinLocation().diffXZ(const_cast<Location&>(team->getLocation())) * 0.3125f;
		auto ground = 1u;
		auto power_range = 230.f;
		auto slope_break = 1.f;
		auto power = _session.m_pi.getSlotPower();
		auto angTo_rad = -std::atan2(hole->getPinLocation().x - team->getLocation().x, hole->getPinLocation().z - team->getLocation().z);
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
			ang = fmodl((team->getDegree() / 255.f) * 360.f + -angTo, 360.f);

		bool pwr_by_condition_actived = pgi->effect_flag_shot.stFlag.SWITCH_TWO_EFFECT;

		if (pwr_by_condition_actived && _session.m_pi.ei.char_info != nullptr
			&& _session.m_pi.ei.char_info->isAuxPartEquiped(0x70210001u) && pgi->item_active_used_shot != 0u
			&& pgi->item_active_used_shot == POWER_MILK_TYPEID)
			pwr_by_condition_actived = false; // Usou Milk, encheu 1 ps perde a condi��o para ativar o efeito

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
		_smp::message_pool::getInstance().push(new message("[Match::getGameShotValueToSmartCalculator][Log] Wind=" + std::to_string(wind)
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

		_smp::message_pool::getInstance().push(new message("[Match::getGameShotValueToSmartCalculator][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return gsv;
}

void Match::startTime(void* _quem) {

	try {
		// Aqui tem tem ser come�a o tempo do player do turno e soma +1 ao n�mero de tacadas dele
		INIT_PLAYER_INFO("startTime", "tentou comecar o tempo do player turno no jogo", (player*)_quem);

		INIT_TEAM_INFO("startTime", (player*)_quem);

		// Soma +1 na tacada do player
		pgi->data.tacada_num++;

		team->incrementTacadaNum();

		team->incrementPlayerStartHole();

		// Para Tempo se j� estiver 1 timer
		if (m_timer != nullptr)
			stopTime();

		job j(VersusBase::end_time, this, _quem);

		/*if (sgs::gs != nullptr)*/
			m_timer = sgs::gs::getInstance().makeTime(m_ri.time_vs/*milliseconds*/, j);	// j� est� em minutos milliseconds
		/*else
			_smp::message_pool::getInstance().push(new message("[Match::startTime][Error] tentou inicializar um timer, mas a variavel global estatica do Server eh invalida.", CL_FILE_LOG_AND_CONSOLE));*/

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[Match::startTime][Log] Criou o Timer[Tempo=" + std::to_string((m_ri.time_30s > 0) ? m_ri.time_30s / 60000 : 0) + "min, STATE=" + std::to_string(m_timer->getState()) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::startTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Match::timeIsOver(void* _quem) {

	// Chama o timeIsOver da classe pai
	VersusBase::timeIsOver(_quem);

	if (_quem != nullptr) {

		player* p = reinterpret_cast< player* >(_quem);

		INIT_PLAYER_INFO("timeIsOver", "tentou acabar o tempo do turno no jogo", p);

		INIT_TEAM_INFO("timeIsOver", p);

		// set timeout do team
		team->setTimeout(1u);

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
		_smp::message_pool::getInstance().push(new message("[Match::timeIsOver][WARNING] time is over executed without _quem, _quem is invalid(nullptr). Bug" , CL_FILE_LOG_AND_CONSOLE));
}

bool Match::init_game() {

	auto lixo = VersusBase::init_game();
	
	if (m_players.size() > 0) {

		// variavel que salva a data local do sistema
		initGameTime();

		// Aqui achievement de rain count
		// Esse aqui tem que ser na hora que finaliza o jogo por que depende de quantos holes o player completou
		//rain_count_players();

		m_game_init_state = 1;	// Come�ou

		m_match_state = true;
	}

	return true;
}

void Match::requestTranslateSyncShotData(player& _session, ShotSyncData& _ssd) {
	CHECK_SESSION_BEGIN("requestTransateSyncShotData");

	try {

		auto s = findSessionByOID(_ssd.oid);

		if (s == nullptr)
			throw exception("[Match::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada do player[OID="
					+ std::to_string(_ssd.oid) + "], mas o player nao existe nessa jogo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 200, 0));

		// Update Sync Shot Player
		if (_session.m_pi.uid == s->m_pi.uid) {

			INIT_PLAYER_INFO("requestTranslateSyncShotData", "tentou sincronizar a tacada no jogo", &_session);

			INIT_TEAM_INFO("requestTranslateSyncShotData", &_session);

			pgi->shot_sync = _ssd;

			// Last Location Team(Player)
			auto last_location = team->getLocation();

			// Update Location Player
			pgi->location.x = _ssd.location.x;
			pgi->location.z = _ssd.location.z;

			// Update Team Location
			team->setLocation((const Location&)_ssd.location);

			// Update Pang and Bonus Pang
			pgi->data.pang = _ssd.pang;
			pgi->data.bonus_pang = _ssd.bonus_pang;

			// Update Pang and Bonus Pang Team
			team->setPang(_ssd.pang);
			team->setBonusPang(_ssd.bonus_pang);

			// J� s� na fun��o que come�a o tempo do player do turno
			//pgi->data.tacada_num++;

			if (_ssd.state == ShotSyncData::OUT_OF_BOUNDS || _ssd.state == ShotSyncData::UNPLAYABLE_AREA) {
				pgi->data.tacada_num++;

				// Update Tacada Num Team
				team->incrementTacadaNum();
			}

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[Match::requestTranslateSyncShotData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada no hole[NUMERO="
						+ std::to_string((unsigned short)pgi->hole) + "], mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 12, 0));

			// Update Team Acerto Hole
			team->setAcertoHole(_ssd.state_shot.display.stDisplay.acerto_hole);

			// Conta j� a pr�xima tacada, no give up
			if (!_ssd.state_shot.display.stDisplay.acerto_hole && hole->getPar().total_shot <= (pgi->data.tacada_num + 1)) {

				// +1 que � giveup, s� add se n�o passou o n�mero de tacadas
				if (pgi->data.tacada_num < hole->getPar().total_shot) {
					pgi->data.tacada_num++;

					// Update Tacada Num Team
					team->incrementTacadaNum();
				}

				pgi->data.giveup = 1;

				// Update Give Up Team
				team->setGiveUp(1u);

				// Soma +1 no Bad Condute
				pgi->data.bad_condute++;

				// Update Bad Condute Team
				team->incrementBadCondute();
			}

			// aqui os achievement de power shot int32_t putt beam impact e etc
			update_sync_shot_achievement(_session, last_location);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::requestTranslateSyncShotData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Match::requestTranslateFinishHoleData(player& _session, UserInfoEx& _ui) {
	CHECK_SESSION_BEGIN("requestTranslateFinishHole");

	try {

		INIT_PLAYER_INFO("requestTranslateFinishHoleData", "tentou finalizar hole dados no jogo", &_session);

		pgi->ui = _ui;

		// S� terminou o hole sem acerta se os 2 team n�o acertou o hole
		if (!pgi->shot_sync.state_shot.display.stDisplay.acerto_hole
			&& !m_teans[0].getAcertoHole() && !m_teans[1].getAcertoHole()) {	// Terminou o Hole sem acerta ele, Give Up

			INIT_TEAM_INFO("requestTranslateFinishHoleData", &_session);

			auto hole = m_course->findHole(pgi->hole);

			if (hole == nullptr)
				throw exception("[Match::requestFinishHoleData][Error] player[UID=" + std::to_string(pgi->uid) + "] tentou finalizar os dados do hole no jogo, mas o hole[NUMERO="
						+ std::to_string(pgi->hole) + "] nao existe no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS, 400, 0));

			// +1 que � giveup, s� add se n�o passou o n�mero de tacadas
			if (pgi->data.tacada_num < hole->getPar().total_shot) {
				pgi->data.tacada_num++;

				team->setTacadaNum(pgi->data.tacada_num);
			}

			// Ainda n�o colocara o give up, o outro pacote, coloca nesse(muito dif�cil, n�o colocar s� se estiver com bug)
			if (!pgi->data.giveup) {
				pgi->data.giveup = 1;

				team->setGiveUp(1);
			}

			// Incrementa o Bad Condute
			//pgi->data.bad_condute++;

			//team->setBadCondute(pgi->data.bad_condute);
		}

		// Aqui Salva os dados do Pgi, os best Chipin, Long putt e best drive(max dist�ncia)
		// N�o sei se precisa de salvar aqui, j� que estou salvando no pgi User Info
		pgi->progress.best_chipin = _ui.best_chip_in;
		pgi->progress.best_long_puttin = _ui.best_long_putt;
		pgi->progress.best_drive = _ui.best_drive;

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[Match::requestTranslateFinishHoleData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool Match::checkEndGame(player& _session) {
	
	INIT_PLAYER_INFO("checkEndGame", "tentou verificar se eh o final do jogo", &_session);

	return (m_course->findHoleSeq(pgi->hole) == m_ri.qntd_hole || ((m_players.size() % 2) == 1/*Quantidade de players impar na sala, acaba o jogo*/));
}

bool Match::checkAllClearHole() {
	
	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_teans.begin(), m_teans.end(), [&](auto& _el) {

		if (_el.getAcertoHole() || _el.getGiveUp() || _el.isQuit())
			count++;
	});

	ret = (count == m_teans.size());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_sync_shot);
#endif

	return ret;
}

bool Match::checkAllClearHoleAndClear() {
	
	uint32_t count = 0u;
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_sync_shot);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_sync_shot);
#endif

	// Check
	std::for_each(m_teans.begin(), m_teans.end(), [&](auto& _el) {

		if (_el.getAcertoHole() || _el.getGiveUp())
			count++;
	});

	ret = (count == m_teans.size());

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

void Match::clearAllClearHole() {

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

void Match::clear_all_clear_hole() {

	std::for_each(m_teans.begin(), m_teans.end(), [&](auto& _el) {
		
		_el.setAcertoHole(0u);
		_el.setGiveUp(0u);

	});
}

void Match::clear_teans() {

	if (!m_teans.empty()) {
		m_teans.clear();
		m_teans.shrink_to_fit();
	}
}

void Match::updateTreasureHunterPoint() {
	
	/*if (!TreasureHunterSystem::isLoad())
		TreasureHunterSystem::load();*/
	if (!sTreasureHunterSystem::getInstance().isLoad())
		sTreasureHunterSystem::getInstance().load();

	// Calcule Treasure Pontos - s� do team Red(vermelho)

	// Red team
	if (m_teans[0].getAcertoHole()) {
		// S� se ele acertou o hole ele add mais treasure hunter point
		auto hole = m_course->findHole(m_teans[0].getHole());

		if (hole == nullptr)
			throw exception("[VersusBase::updateTreasureHunterPoint][Error] tentou atualizar os pontos do Treasure Hunter no hole[NUMERO="
					+ std::to_string((unsigned short)m_teans[0].getHole()) + "], mas o hole nao existe. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 30, 0));

		// 2x o valor ganho por que s� ganha item do treasure hunter point do time RED(vermelho)
		/*m_thi.treasure_point += (TreasureHunterSystem::calcPointNormal(m_teans[0].getTacadaNum(), hole->getPar().par) + m_thi.getPoint(m_teans[0].getTacadaNum(), hole->getPar().par)) * 2;*/
		m_thi.treasure_point += (sTreasureHunterSystem::getInstance().calcPointNormal(m_teans[0].getTacadaNum(), hole->getPar().par) + m_thi.getPoint(m_teans[0].getTacadaNum(), hole->getPar().par)) * 2;
	}

	// Blue team
	if (m_teans[1].getAcertoHole()) {
		// S� se ele acertou o hole ele add mais treasure hunter point
		auto hole = m_course->findHole(m_teans[1].getHole());

		if (hole == nullptr)
			throw exception("[VersusBase::updateTreasureHunterPoint][Error] tentou atualizar os pontos do Treasure Hunter no hole[NUMERO="
					+ std::to_string((unsigned short)m_teans[1].getHole()) + "], mas o hole nao existe. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TOURNEY_BASE, 30, 0));

		/*m_thi_blue.treasure_point += (TreasureHunterSystem::calcPointNormal(m_teans[1].getTacadaNum(), hole->getPar().par) + m_thi_blue.getPoint(m_teans[1].getTacadaNum(), hole->getPar().par)) * 2;*/
		m_thi_blue.treasure_point += (sTreasureHunterSystem::getInstance().calcPointNormal(m_teans[1].getTacadaNum(), hole->getPar().par) + m_thi_blue.getPoint(m_teans[1].getTacadaNum(), hole->getPar().par)) * 2;
	}

	// no Match passa 2x o pacote132 treasure Hunter point, varia um pouco a valor
	// team Vermelho
	// Team Azul e Vermelho

	// Mostra score board
	packet p((unsigned short)0x132);

	p.addUint32(m_thi.treasure_point);

	// No Modo Match passa outro valor tbm

	packet_func::game_broadcast(*this, p, 1);

	// Mostra score board dos 2 teans
	p.init_plain((unsigned short)0x132);

	// Red Team
	p.addUint32(m_thi.treasure_point);

	// Blue Team
	p.addUint32(m_thi_blue.treasure_point);

	packet_func::game_broadcast(*this, p, 1);
}

bool Match::checkNextStepGame(player& _session) {
	
	auto ret = false;

	try {
		
		INIT_PLAYER_INFO("checkNextStepGame", "tentou verificar o proximo passo do jogo", &_session);

		if (m_players.size() > 0) {
			
			if (m_player_turn == nullptr) {

				// Player Turn ainda n�o foi decidido, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else if (m_player_turn == pgi) {

				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);
				
				ret = true;	// Termina o Game
			
			}else if (!checkPlayerTurnExistOnGame()) {

				// Player Turn n�o est� mais no jogo, termina o jogo
				m_state_vs.setStateWithLock(STATE_VERSUS::WAIT_END_GAME);

				ret = true;	// Termina o Game

			}else
				m_flag_next_step_game = 2;	// Termina o game
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::checkNextStepGame][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
}

void Match::requestSaveInfo(player& _session, int _option) {
	CHECK_SESSION_BEGIN("SaveInfo");

	try {

		INIT_PLAYER_INFO("requestSaveInfo", "tentou salvar o info do player no Match", &_session);

		INIT_TEAM_INFO("requestSaveInfo", &_session);

		if (_option == 0) {
			pgi->ui.team_game = 1l;
			pgi->ui.team_win = (m_team_win == team->getId()) ? 1l : 0l;
		}else {
			pgi->ui.team_game = 0l;
			pgi->ui.team_win = 0l;
		}

		auto hole_seq = m_course->findHoleSeq(pgi->hole);

		if (hole_seq == (unsigned short)~0)
			_smp::message_pool::getInstance().push(new message("[Match::requestSaveInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) +"] tentou pegar sequencia do hole[NUMERO=" 
					+ std::to_string(pgi->hole) + "] no course, mas nao encontrou o hole no course. Bug", CL_FILE_LOG_AND_CONSOLE));
		else
			pgi->ui.team_hole = hole_seq;

		Game::requestSaveInfo(_session, _option);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::requestSaveInfo][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Match::requestFinishExpGame() {
	
	if (m_players.size() > 0) {

		player *_session = nullptr;
		float stars = m_course->getStar(), temp = 0.f;
		int32_t exp = 0, hole_seq = 0;

		for (auto& team : m_teans) {

			temp = 1.f;

			if (m_team_win != 2 && team.getId() != m_team_win)
				temp -= 0.4f;

			for (auto& el : team.getPlayers()) {

				INIT_PLAYER_INFO("requestFinishExpGame", "tentou finalizar experiencia do jogo", el);

				hole_seq = (int)m_course->findHoleSeq(pgi->hole);

				// Ele est� no primeiro hole e n�o acertou ele, s� da experi�ncia se ele tiver acertado o hole
				if (hole_seq == 1 && !team.getAcertoHole())
					hole_seq = 0;

				if ((_session = findSessionByUID(pgi->uid)) != nullptr) {

					exp = (int)(1 * m_players.size() * (hole_seq > 0 ? hole_seq : 0) * stars);
					exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(pgi->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));
					exp = (int)((float)exp * temp);

					if (pgi->level < 70/*Ultimo level n�o ganha exp*/)
						pgi->data.exp = exp;
				}

				_smp::message_pool::getInstance().push(new message("[Match::requestFinishExpGame][Log] player[UID=" + std::to_string(pgi->uid) + "] ganhou " + std::to_string(pgi->data.exp) + " de experience.", CL_FILE_LOG_AND_CONSOLE));

			}
		}
	}
}

void Match::requestCalculeTeamWin() {

	m_team_win = 0;	// Red

	if (m_teans[0].getPoint() == m_teans[1].getPoint()) {

		if (m_teans[0].getPang() == m_teans[1].getPang())
			m_team_win = 2;	// Empate(Draw)
		else if (m_teans[1].getPang() > m_teans[0].getPang())
			m_team_win = 1;	// Blue

	}else if (m_teans[1].getPoint() > m_teans[0].getPoint())
		m_team_win = 1;	// Blue
}

void Match::requestUpdateTeamPang() {

	for (auto& team : m_teans) {

		for (auto& el : team.getPlayers()) {

			INIT_PLAYER_INFO("requestUpdateTeamPang", "tentou atualizar pangs do player[UID=" + std::to_string(el->m_pi.uid) + "] no Match", el);

			// Update Pang Player To Team Pang and Bonus Pang
			pgi->data.pang = team.getPang();
			pgi->data.bonus_pang = team.getBonusPang();
		}
	}
}

void Match::finish() {

	m_match_state = false;	// Terminou o versus

	m_game_init_state = 2; // Terminou o jogo

	requestCalculeRankPlace();

	// Calcula o team(time) que ganhou o Match
	requestCalculeTeamWin();

	requestFinishExpGame();

	requestDrawTreasureHunterItem();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el);
	}
}

void Match::requestFinishTeamHole() {

	finishHole();

	if (m_player_turn == nullptr)
		throw exception("[Match::requestFinishTeamHole][Error] m_player_turn is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 100, 0));

	auto hole = m_course->findHole(m_player_turn->hole);

	if (hole == nullptr)
		throw exception("[Match::requestFinishTeamHole][Error] player[UID=" + std::to_string(m_player_turn->uid) + "] tentou finalizar hole[NUMERO="
				+ std::to_string((unsigned short)m_player_turn->hole) + "] no jogo, mas o numero do hole is invalid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 20, 0));

	// Win Last And Point Team
	m_teans[0].setLastWin(0u);
	m_teans[1].setLastWin(0u);

	if (m_teans[0].getAcertoHole() && m_teans[0].getStateFinish() > 0 && m_teans[1].getStateFinish() == 0 
			&& m_teans[0].getTacadaNum() < (m_teans[1].getTacadaNum() + 1)) {	// Win
		m_teans[0].setLastWin(1u);
		m_teans[0].incrementPoint();
	}else if (m_teans[1].getAcertoHole() && m_teans[1].getStateFinish() > 0 && m_teans[0].getStateFinish() == 0 
			&& m_teans[1].getTacadaNum() < (m_teans[0].getTacadaNum() + 1)) {	// Win
		m_teans[1].setLastWin(1u);
		m_teans[1].incrementPoint();
	}

	m_teans[0].setStateFinish(0u);
	m_teans[1].setStateFinish(0u);

	for (auto& el : m_teans) {

		el.setStateFinish(0u);

		el.incrementTotalTacadaNum(el.getTacadaNum());
		
		el.setScore((el.getTacadaNum() - hole->getPar().par));

		// !!!@@ N�o sei por que estou zerando aqui, no antigo, mas vamos testar
		// Deu certo, mas vou deixar o coment�rio para duvidas futuras
		el.setPlayerStartHole(0u);
		//el.setTacadaNum(0u);
	}
}

void Match::requestFinishData(player& _session) {

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

void Match::sendPlacar(player& _session) {

	packet p((unsigned short)0x91);

	p.addUint8((unsigned char)m_players.size());

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("sendPlacar", "tentou enviar o placar do jogo", el);

		INIT_TEAM_INFO("sendPlacar", el);

		p.addUint32(el->m_oid);
		p.addUint8((unsigned char)getRankPlace(*el));
		p.addInt8(0x7F/*(char)pgi->data.score*/);
		p.addInt8((unsigned char)pgi->data.total_tacada_num);

		p.addUint16((unsigned short)pgi->data.exp);
		p.addUint64(team->getPang()/*pgi->data.pang*/);
		p.addUint64(team->getBonusPang()/*pgi->data.bonus_pang*/);

		// Valor que usa no Pang Battle, valor de pang que ganhou ou perdeu
		// Como aqui � vs Base deixa o valor 0
		p.addUint64(0ull);
	}

	p.addUint8((const unsigned char)m_teans[0].getPoint());
	p.addUint8((const unsigned char)m_teans[1].getPoint());
	p.addUint8(m_team_win);

	packet_func::session_send(p, &_session, 1);
}

void Match::sendFinishMessage(player& _session) {

	INIT_PLAYER_INFO("sendFinishMessage", "tentou enviar message no chat que o player terminou o jogo", &_session);

	INIT_TEAM_INFO("sendFinishMessage", &_session);

	packet p((unsigned short)0x40);

	p.addUint8(16);	// Msg que terminou o game

	p.addString(_session.m_pi.nickname);
	p.addUint16(0);	// Size Msg

	p.addInt32(team->getPoint());
	p.addUint64(team->getPang());
	p.addUint8(pgi->assist_flag);

	packet_func::game_broadcast(*this, p, 1);
}

void Match::sendReplyFinishLoadHole() {

	try {

		PlayerGameInfo *pgi = requestCalculePlayerTurn();

		auto hole = m_course->findHole(pgi->hole);

		if (hole == nullptr)
			throw exception("[Match::requestFinishLoadHole][Error] player[UID=" + std::to_string(pgi->uid) + "] tentou finalizar carregamento do hole[NUMERO="
					+ std::to_string(pgi->hole) + "], mas nao conseguiu encontrar o hole no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::VERSUS_BASE, 201, 0));

		INIT_TEAM_INFO("requestFinishLoadHole", findSessionByPlayerGameInfo(pgi));

		// Resposta de tempo do hole
		packet p((unsigned short)0x9E);

		p.addUint16(hole->getWeather());
		p.addUint8(0);	// Option do tempo, sempre peguei zero aqui dos pacotes que vi

		packet_func::game_broadcast(*this, p, 1);

		auto wind_flag = initCardWindPlayer(m_player_turn, hole->getWind().wind);

		// Resposta do vento do hole
		p.init_plain((unsigned short)0x5B);

		p.addUint8(hole->getWind().wind + wind_flag);
		p.addUint8((wind_flag < 0) ? 1 : 0);	// Flag de card de vento, aqui � a qnd diminui o vento, 1 Vento azul
		p.addUint16(team->getDegree()/*m_player_turn->degree*/);
		p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original

		packet_func::game_broadcast(*this, p, 1);

		// Resposta passa o oid do player que vai come�a o Hole
		p.init_plain((unsigned short)0x53);

		if (m_player_turn == nullptr) {
			_smp::message_pool::getInstance().push(new message("[Match::requestFinishLoadHole][Error] player_turn is invalid(nullptr)", CL_FILE_LOG_AND_CONSOLE));

			p.addUint32(0);
		}else
			p.addUint32(m_player_turn->oid);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::sendReplyFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Match::sendReplyFinishCharIntro() {

	packet p;

	try {

		for (auto& el : m_teans) {
			
			el.setTacadaNum(0u);

			el.setGiveUp(0u);
		}

		// Resposta para Finish Char Intro
		p.init_plain((unsigned short)0x90);

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::sendReplyFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

}

int Match::checkEndShotOfHole(player& _session) {
	CHECK_SESSION_BEGIN("checkEndShotOfHole");

	try {

		// Agora verifica o se ele acabou o hole e essas coisas
		INIT_PLAYER_INFO("checkEndShotOfHole", "tentou verificar a ultima tacada do hole no jogo", &_session);

		//pgi->finish_shot = 1u;

		if (pgi->data.bad_condute >= 3)
			return 2;
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

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::checkEndShotOfHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
}

void Match::changeTurn() {

	try {

		if (m_player_turn == nullptr)
			throw exception("[Match::changeTurn][Error] PlayerGameInfo *m_player_turn is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 100, 0));
		
		INIT_TEAM_INFO("changeTurn", findSessionByPlayerGameInfo(m_player_turn));

		// Para o tempo do player do turno
		stopTime();

		auto hole_seq = m_course->findHoleSeq(m_player_turn->hole);

		int state = 0, hole_diff = m_ri.qntd_hole - hole_seq;

		if (checkAllClearHole())
			state = 1;
		else if ((m_players.size() % 2) == 1/*Quantidade de player Impar acaba o jogo*/)
			state = 1;
		else if (m_teans[0].getAcertoHole() && !m_teans[1].getTimeout() && (m_teans[0].getTacadaNum() < (m_teans[1].getTacadaNum() + 1)))
			state = 1;
		else if (m_teans[1].getAcertoHole() && !m_teans[0].getTimeout() && (m_teans[1].getTacadaNum() < (m_teans[0].getTacadaNum() + 1)))
			state = 1;
		else if (m_teans[0].getAcertoHole() && !m_teans[1].getTimeout() && (m_teans[0].getTacadaNum() == (m_teans[1].getTacadaNum() + 1)) && (int)(m_teans[0].getPoint() - m_teans[1].getPoint()) > hole_diff)
			state = 1;
		else if (m_teans[1].getAcertoHole() && !m_teans[0].getTimeout() && (m_teans[1].getTacadaNum() == (m_teans[0].getTacadaNum() + 1)) && (int)(m_teans[1].getPoint() - m_teans[0].getPoint()) > hole_diff)
			state = 1;

		// Limpa dados que usa para cada tacada
		clearDataEndShot(m_player_turn);

		// Verifica se todos fizeram o hole, ou o outro team venceu por que chipo antes com menos tacada
		if (state) {
		
			clear_all_flag_sync();

			// clear teans timeout flag
			for (auto& el : m_teans)
				el.setTimeout(0u);

			//finishHole();
			requestFinishTeamHole();

			changeHole();
	
			// Clear Acerto hole e Give Up
			// Aqui zera mesmo se j� zero, por que n�o sei se foi por all clear ou outras regras
			clearAllClearHole();

		}else {	// Troca o Turno

			clear_all_flag_sync();

			// clear teans timeout flag
			for (auto& el : m_teans)
				el.setTimeout(0u);

			// Recalcula Turno
			requestCalculePlayerTurn();

			if (m_player_turn == nullptr)
				throw exception("[Match::changeTurn][Error] PlayerGameInfo *m_player_turn is invalid(nullptr). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 100, 1));

			auto hole = m_course->findHole(m_player_turn->hole);

			if (hole == nullptr)
				throw exception("[Match::changeTurn][Error] player[UID=" + std::to_string(m_player_turn->uid) + "] tentou encontrar o hole[NUMERO=" 
						+ std::to_string(m_player_turn->hole) + "] do course no jogo, mas nao foi encontrado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 101, 0));

			INIT_TEAM_INFO("changeTurn", findSessionByPlayerGameInfo(m_player_turn));

			auto wind_flag = initCardWindPlayer(m_player_turn, hole->getWind().wind);

			// Resposta do vento do hole
			packet p((unsigned short)0x5B);

			p.addUint8(hole->getWind().wind + wind_flag);
			p.addUint8((wind_flag < 0) ? 1 : 0);	// Flag de card de vento, aqui � a qnd diminui o vento, 1 Vento azul
			p.addUint16(team->getDegree()/*m_player_turn->degree*/);
			p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original

			packet_func::game_broadcast(*this, p, 1);

			// Resposta passa o oid do player que vai come�a o Hole
			p.init_plain((unsigned short)0x63);

			if (m_player_turn == nullptr) {
				_smp::message_pool::getInstance().push(new message("[Match::changeTurn][Error] player_turn is invalid(nullptr)", CL_FILE_LOG_AND_CONSOLE));

				p.addUint32(0);
			}else
				p.addUint32(m_player_turn->oid);

			// !!@@@
			// Aqui tem 2 bytes a+, int16 de um valor, que acho que acontece de ver em quando, ou s� no pang battle isso estava escrito no meu outro
			// acho que possa ser do pang battle, certaza, aqui acabei de ver na classe pang battle do antigo

			packet_func::game_broadcast(*this, p, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::changeTurn][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void Match::CCGChangeWind(player& _gm, unsigned char _wind, unsigned short _degree) {
	
	try {

		if (m_player_turn == nullptr)
			throw exception("[Match::CCGChangeWind][Error] player[UID=" + std::to_string(_gm.m_pi.uid) + "] tentou executar o comando de troca de vento no versus na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o player_turn do versus eh invalido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 1, 0x5700100));

		auto hole = m_course->findHole(m_player_turn->hole);

		if (hole == nullptr)
			throw exception("[Match::CCGChangeWind][Error] player[UID=" + std::to_string(_gm.m_pi.uid) + "] tentou executar o comando de troca de vento no versus na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o nao encontrou o hole[VALUE=" + std::to_string((short)m_player_turn->hole) + "] no course. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MATCH, 2, 0x5700100));

		auto wind = hole->getWind();

		// Change Wind of Hole
		wind.wind = _wind;

		hole->setWind(wind);

		// Change Degree of team
		m_team_turn->setDegree(_degree % LIMIT_DEGREE);

		// Log
		_smp::message_pool::getInstance().push(new message("[Match::CCGChangeWind][Log] [GM] player[UID=" + std::to_string(_gm.m_pi.uid) + "] trocou o vento e graus da sala[NUMERO="
				+ std::to_string(m_ri.numero) + ", VENTO=" + std::to_string((unsigned short)_wind + 1) + ", GRAUS=" + std::to_string(_degree) + "]", CL_FILE_LOG_AND_CONSOLE));

		auto wind_flag = initCardWindPlayer(m_player_turn, hole->getWind().wind);

		// UPDATE ON GAME
		packet p((unsigned short)0x5B);

		p.addUint8(hole->getWind().wind + wind_flag);	// Wind
		p.addUint8((wind_flag < 0) ? 1 : 0);			// Card Wind Flag, minus wind flag
		p.addUint16(m_team_turn->getDegree());			// Degree
		p.addUint8(1);									// Flag 1 = Reset Degree, 0 = Plus Degree

		packet_func::game_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[Match::CCGChangeWind][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

}

PlayerGameInfo* Match::requestCalculePlayerTurn() {
	
	auto team = requestCalculeTeamTurn();

	INIT_PLAYER_INFO("requestCalculePlayerTurn", "tentou calcular o player turno no Match", team->requestCalculePlayerTurn(m_course->findHoleSeq(team->getHole())));
	
	m_player_turn = pgi;

	return m_player_turn;
}

Team* Match::requestCalculeTeamTurn() {
	
	if (!m_player_info.empty()) {

		auto hole = m_course->findHole(m_player_info.begin()->second->hole);

		if (hole == nullptr) {
			_smp::message_pool::getInstance().push(new message("[Match::requestCalculeTeamTurn][Error] player[UID=" + std::to_string(m_player_info.begin()->second->uid) + "] o hole[NUMERO="
					+ std::to_string(m_player_info.begin()->second->hole) + "] nao foi encontrado no course. Bug", CL_FILE_LOG_AND_CONSOLE));
			
			m_player_turn = nullptr;

			return nullptr;
		}

		std::vector< TeamOrderTurnCtx > v_team_order_turn;

		for (auto& el : m_teans)
			v_team_order_turn.push_back({ &el, hole });
		
		if (v_team_order_turn.empty()) {
			_smp::message_pool::getInstance().push(new message("[Match::requestCalculeTeamTurn][Error] nao tem players, para calcular o turno. Bug", CL_FILE_LOG_AND_CONSOLE));

			m_player_turn = nullptr;

			return nullptr;
		}

		std::sort(v_team_order_turn.begin(), v_team_order_turn.end(), Match::sort_team_turn);

		m_team_turn = v_team_order_turn.begin()->team;
	}

	return m_team_turn;
}

void Match::init_team_player_position() {

	unsigned char red_flag = 0u, blue_flag = 0u;

	for (auto& el : m_players) {

		INIT_TEAM_INFO("init_team_player_position", el);

		if (team->getId() == 0/*RED*/ && !red_flag) {
			
			team->sort_player(el->m_pi.uid);

			red_flag = 1u;

		}else if (team->getId() == 1/*Blue*/ && !blue_flag) {

			team->sort_player(el->m_pi.uid);

			blue_flag = 1u;
		}
	}
	
}

bool Match::finish_game(player& _session, int option) {
	
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
			if (m_course->findHoleSeq(pgi->hole) == 1 && !checkAllClearHole()) {

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

				m_game_init_state = 2;	// Acabou o Match

				return true;

			}else {

				if (m_match_state)  // Deixa o cliente envia o pacote para finalizar o jogo, depois que ele mostrar os placares
					finish_match(1);
				else {

					for (auto& el : m_players) {

						INIT_PLAYER_INFO("finish_game", "tentou finalizar o versus", el);

						if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

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

					m_game_init_state = 2;	// Acabou o Match

					return true;
				}
			}
		}
	}

	return false;
}

bool Match::sort_team_turn(TeamOrderTurnCtx& _totc1, TeamOrderTurnCtx& _totc2) {

	auto diff = _totc1.hole->getPinLocation().diffXZ(const_cast<Location&>(_totc1.team->getLocation()));
	auto diff2 = _totc1.hole->getPinLocation().diffXZ(const_cast<Location&>(_totc2.team->getLocation()));

	// Os 2 Fez Give Up Ou Acerto o hole
	if ((_totc1.team->getAcertoHole() || _totc1.team->getGiveUp()) && (_totc2.team->getAcertoHole() || _totc2.team->getGiveUp()))
		return false;

	if (!_totc1.team->getAcertoHole() && (_totc2.team->getAcertoHole() || _totc2.team->getGiveUp()))
		return true;
	else if (_totc1.team->getTacadaNum() == 0 && _totc2.team->getTacadaNum() > 0)
		return true;
	else if (diff > diff2 && !_totc1.team->getAcertoHole() && !_totc1.team->getGiveUp())
		return true;
	else if (diff == diff2 && _totc1.team->getTacadaNum() < _totc2.team->getTacadaNum() && !_totc1.team->getAcertoHole() && !_totc1.team->getGiveUp())
		return true;
	else if (diff == diff2 && _totc1.team->getTacadaNum() == _totc2.team->getTacadaNum() && _totc1.team->getLastWin() > _totc2.team->getLastWin() && !_totc1.team->getAcertoHole() && !_totc1.team->getGiveUp())
		return true;
	else if (diff == diff2 && _totc1.team->getTacadaNum() == _totc2.team->getTacadaNum() && _totc1.team->getLastWin() == _totc2.team->getLastWin()
			&& _totc1.team->getPoint() > _totc2.team->getPoint() && !_totc1.team->getAcertoHole() && !_totc1.team->getGiveUp())
		return true;
	else if (diff == diff2 && _totc1.team->getTacadaNum() == _totc2.team->getTacadaNum() && _totc1.team->getLastWin() == _totc2.team->getLastWin()
			&& _totc1.team->getPoint() == _totc2.team->getPoint() && _totc1.team->getPang() > _totc2.team->getPang() && !_totc1.team->getAcertoHole() && !_totc1.team->getGiveUp())
		return true;

	return false;
}

void Match::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[Match::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[Match::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *game = reinterpret_cast<Game*>(_arg);

	switch (_msg_id) {
	case 0:
	default:
		break;
	}
}
