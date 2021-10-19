// Arquivo grand_zodiac.cpp
// Criado em 25/06/2020 as 18:30 por Acrisio
// Implementa��o da classe GrandZodiac

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "grand_zodiac.hpp"

#include "item_manager.h"

#include "../PACKET/packet_func_sv.h"

#include "../Game Server/game_server.h"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include <algorithm>

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[GrandZodiac" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC, 1, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[GrandZodiac::request" + std::string((method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC, 6, 0)); \

// Ponteiro de session
#define INIT_PLAYER_INFO(_method, _msg, __session) PlayerGrandZodiacInfo *pgi = reinterpret_cast< PlayerGrandZodiacInfo* >(getPlayerInfo((__session))); \
	if (pgi == nullptr) \
		throw exception("[GrandZodiac::" + std::string((_method)) + "][Error] player[UID=" + std::to_string((__session)->m_pi.uid) + "] " + std::string((_msg)) + ", mas o game nao tem o info dele guardado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GRAND_ZODIAC, 1, 4)); \

using namespace stdA;

GrandZodiac::GrandZodiac(std::vector< player* >& _players, RoomInfoEx& _ri, RateValue _rv, unsigned char _channel_rookie)
	: GrandZodiacBase(_players, _ri, _rv, _channel_rookie), m_grand_zodiac_state(false) {

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("GrandZodiac", "tentou inicializar o counter item do Grand Zodiac", el);

		initAchievement(*el);

		pgi->sys_achieve.incrementCounter(0x6C40003Cu/*Grand Zodiac*/);
	}

	m_state = init_game();
}

GrandZodiac::~GrandZodiac() {

	m_grand_zodiac_state = false;

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
	_smp::message_pool::getInstance().push(new message("[GrandZodiac::~GrandZodiac][Log] Grand Zodiac destroyed on Room[Number=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
}

void GrandZodiac::changeHole(player& _session) {

	try {

		nextHole(_session);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiac::changeHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiac::finishHole(player& _session) {

	requestFinishHole(_session, 0);
}

void GrandZodiac::finish_grand_zodiac(player& _session, int _option) {

	if (m_players.size() > 0 && m_game_init_state == 1) {

		packet p;

		INIT_PLAYER_INFO("finish_grand_zodiac", "tentou terminar o Grand Zodiac no jogo", &_session);

		if (pgi->flag == PlayerGameInfo::eFLAG_GAME::PLAYING) {

			// Calcula os pangs que o player ganhou
			requestCalculePang(_session);

			// Atualizar os pang do player se ele estiver com assist ligado, e for maior que beginner E
			updatePlayerAssist(_session);

			if (m_game_init_state == 1 && _option == 0) {

				// Achievement Counter
				pgi->sys_achieve.incrementCounter(0x6C400004u/*Normal game complete*/);

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

void GrandZodiac::timeIsOver() {

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
		_smp::message_pool::getInstance().push(new message("[GrandZodiac::timeIsOver][Log] Tempo Acabou no Grand Zodiac. na sala[NUMERO=" + std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}
}

bool GrandZodiac::init_game() {
	
	if (m_players.size() > 0) {

		// variavel que salva a data local do sistema
		initGameTime();

		m_game_init_state = 1;	// Come�ou

		m_grand_zodiac_state = true;
	}

	return true;
}

void GrandZodiac::requestFinishExpGame() {

	int32_t exp = 0;
	player* _session = nullptr;

	PlayerGrandZodiacInfo *pgzi = nullptr;

	for (auto& el : m_player_info) {

		if ((pgzi = reinterpret_cast< PlayerGrandZodiacInfo* >(el.second)) != nullptr) {

			if (el.second->flag == PlayerGameInfo::eFLAG_GAME::FINISH) {

				if ((_session = findSessionByUID(el.second->uid)) != nullptr) {

					exp = (int)(45.f * ((121 - pgzi->m_gz.position) / 100.f));
					exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(el.second->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));

					if (el.second->level < 70/*Ultimo level n�o ganha exp*/)
						el.second->data.exp = exp;
				}

			}else if (el.second->flag == PlayerGameInfo::eFLAG_GAME::END_GAME) {

				exp = (int)(pgzi->m_gz.hole_in_one / 2);
				exp = (int)(exp * TRANSF_SERVER_RATE_VALUE(el.second->used_item.rate.exp) * TRANSF_SERVER_RATE_VALUE(m_rv.exp));

				if (el.second->level < 70/*Ultimo level n�o ganha exp*/)
					el.second->data.exp = exp;
			}
		}
	}
}

void GrandZodiac::finish(int option) {

	m_game_init_state = 2;	// Acabou

	requestCalculeRankPlace();

	requestMakeTrofel();

	requestCalculePontos();

	requestFinishExpGame();

	for (auto& el : m_players) {

		INIT_PLAYER_INFO("finish", "tentou finalizar os dados do jogador no jogo", el);

		if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT)
			requestFinishData(*el, option);
	}
}

void GrandZodiac::drawDropItem(player& _session) {

	try {

		INIT_PLAYER_INFO("drawDropItem", "tentou sortear item drop para o jogador no jogo", &_session);

		if (pgi->shot_sync.state_shot.display.stDisplay.acerto_hole) {

			auto seed = sRandomGen::getInstance().rIbeMt19937_64_chronoRange(1, 10000);

			if (seed > 9000/*10%*/) { // Dropou

				// 0x1800002C - Silent Nerver Stabilizer
				// 0x1800002D - Safe Silent
				DropItem di{ 0 };

				di.numero_hole = 1;
				di.course = m_ri.course & 0x7F;

				seed = sRandomGen::getInstance().rIbeMt19937_64_chronoRange(0, 1);

				di._typeid = 0x1800002C + (uint32_t)seed;
				di.qntd = (seed == 0) ? 5 : 3;
				di.type = DropItem::eTYPE::NORMAL_QNTD;

				// Add Droped item to pgi player
				pgi->drop_list.v_drop.push_back(di);

				// Update item game, show msg
				packet p((unsigned short)0x40);

				p.addUint8(15);			// Dropou item

				p.addString(_session.m_pi.nickname);
				p.addUint16(0u);		// Message empty

				p.addUint32(di._typeid);
				p.addUint32(di.qntd);

				// Envia para todos
				packet_func::game_broadcast(*this, p, 1);
				
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiac::drawDropItem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiac::requestFinishData(player& _session, int option) {

	packet p;

	try {

		INIT_PLAYER_INFO("requestFinishData", "tentou finalizar os dado do player no jogo", &_session);

		requestSaveInfo(_session, 0/*Terminou*/);

		// Atualiza itens usados no Grand Zodiac
		requestUpdateItemUsedGame(_session);

		// Finish Artefact Frozen Flame agora � direto no Finish Item Used Game
		requestFinishItemUsedGame(_session);

		// Salva pontos do Grand Zodiac ganho
		if (pgi->m_gz.pontos > 0)
			_session.m_pi.addGrandZodiacPontos(pgi->m_gz.pontos);

		// Salve trofe�
		sendTrofel(_session);

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

		_smp::message_pool::getInstance().push(new message("[GrandZodiac::requestFinishData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiac::updateFinishHole(player& _session, int _option) {

	try {

		INIT_PLAYER_INFO("updateFinishHole", "tentou atualizar o finish hole do grand zodiac", &_session);

		// Passa a localiza��o do player, esse � a primeira, vez ent�o passa os valores do init Hole sempre
		packet p((unsigned short)0x1EE);

		p.addUint32(_session.m_oid);
		p.addFloat(pgi->location.x);
		p.addFloat(pgi->location.z);

		if (m_ri.tipo == RoomInfo::TIPO::GRAND_ZODIAC_INT)
			packet_func::game_broadcast(*this, p, 1);
		else
			packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiac::updateFinishHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiac::requestMakeTrofel() {
	
	// Trofe� Grand Zodiac
	uint32_t trofeu_base = 0x2D0A6200u;
	uint32_t qntd = 0u;

	auto players_num = m_player_info.size();

	if (players_num >= 20u && players_num < 30u) // Bronza
		qntd = 1u;
	else if (players_num >= 30 && players_num < 50) // Silver and Bronza
		qntd = 2u;
	else if (players_num >= 50)	// Gold, Silver and Bronze
		qntd = 3u;

	trofeu_base = trofeu_base + ((3 - qntd) << 8);

	PlayerGrandZodiacInfo *pgzi = nullptr;

	for (auto& el : m_player_info) {

		if ((pgzi = reinterpret_cast< PlayerGrandZodiacInfo* >(el.second)) != nullptr && pgzi->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

			if (pgzi->m_gz.position <= qntd)
				pgzi->m_gz.trofeu = trofeu_base + ((pgzi->m_gz.position - 1) << 8);
		}
	}
}

void GrandZodiac::startGoldenBeam() {

	// Come�a o tempo do golden beam time
#if defined(_WIN32)
	InterlockedExchange(&m_golden_beam_state, 1u);
#elif defined(__linux__)
	__atomic_store_n(&m_golden_beam_state, 1u, __ATOMIC_RELAXED);
#endif

	// Golden Beam Start
	packet p((unsigned short)0x1F0);

	packet_func::game_broadcast(*this, p, 1);
}

void GrandZodiac::endGoldenBeam() {
	
	try {

		// Acabou o tempo do golden beam time
#if defined(_WIN32)
		InterlockedExchange(&m_golden_beam_state, 0u);
#elif defined(__linux__)
		__atomic_store_n(&m_golden_beam_state, 0u, __ATOMIC_RELAXED);
#endif

		// Golden Beam End
		packet p((unsigned short)0x1F1);

		packet_func::game_broadcast(*this, p, 1);

		if (!m_mp_golden_beam_player.empty()) {

			uint64_t jackpot = m_players.size() * (sRandomGen::getInstance().rIbeMt19937_64_chronoRange(1, 5) * 500); // rand de 500 a 2500 por player

			auto seed = sRandomGen::getInstance().rIbeMt19937_64_chronoRange(0, 1);

			if (m_mp_golden_beam_player.size() == 1 || seed == 0ull/*1 Ganhou tudo*/) {

				auto it = m_mp_golden_beam_player.begin();

				std::advance(it, (uint32_t)sRandomGen::getInstance().rIbeMt19937_64_chronoRange(0, m_mp_golden_beam_player.size() - 1));

				INIT_PLAYER_INFO("endGoldenBeam", "tentou enviar o prensete do Golden Beam", it->first);

				if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

					// Log
					_smp::message_pool::getInstance().push(new message("[GrandZodiac::endGoldenBeam][Log] Player[UID=" 
							+ std::to_string(it->first->m_pi.uid) + "] ganhou jackpot(" + std::to_string(jackpot) + ") sozinho.", CL_FILE_LOG_AND_CONSOLE));

					pgi->m_gz.jackpot = jackpot;

					p.init_plain((unsigned short)0x40);

					p.addUint8(13);		// 1 Ganhou sozinho o jackpot no Grand Zodiac

					p.addString(it->first->m_pi.nickname);
					p.addUint16(0u);	// Msg empty

					p.addUint32(0x1A000010u); // Jackpot Pangs Pouch
					p.addUint64(jackpot);

					packet_func::game_broadcast(*this, p, 1);
				
				}else
					_smp::message_pool::getInstance().push(new message("[GrandZodiac::endGoldenBeam][Log][WARNING] Player[UID="
							+ std::to_string(pgi->uid) + "] ganhou jackpot, mas ele nao esta mais no jogo, para receber o jackpot.", CL_FILE_LOG_AND_CONSOLE));

			}else if (seed == 1ull/*Todos ganham*/) {

				auto equal_jackpot = jackpot / m_mp_golden_beam_player.size();

				for (auto& el : m_mp_golden_beam_player) {

					INIT_PLAYER_INFO("endGoldenBeam", "tentou enviar o presente do Golden Beam", el.first);

					if (pgi->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

						// Log
						_smp::message_pool::getInstance().push(new message("[GrandZodiac::endGoldenBeam][Log] Player[UID="
							+ std::to_string(el.first->m_pi.uid) + "] ganhou jackpot(" + std::to_string(jackpot) + ") igual ao de todo mundo.", CL_FILE_LOG_AND_CONSOLE));

						pgi->m_gz.jackpot = equal_jackpot;

						p.init_plain((unsigned short)0x40);

						p.addUint8(14);		// Todos que fizeram hio no golden beam garanham o jackpot

						p.addString(el.first->m_pi.nickname);
						p.addUint16(0u);	// Msg Empty

						p.addUint32(0x1A000010u);	// Jackpot Pangs Pouch
						p.addUint64(equal_jackpot);

						packet_func::session_send(p, el.first, 1);
					
					}else
						_smp::message_pool::getInstance().push(new message("[GrandZodiac::endGoldenBeam][Log][WARNING] Player[UID=" 
								+ std::to_string(pgi->uid) + "] ganhou jackpot, mas ele nao esta mais no jogo, para receber o jackpot.", CL_FILE_LOG_AND_CONSOLE));
				}
			}
		}
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiac::endGoldenBeam][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void GrandZodiac::requestCalculePontos() {

	PlayerGrandZodiacInfo *pgzi = nullptr;

	float pontos_base = (m_ri.tipo == RoomInfo::TIPO::GRAND_ZODIAC_INT ? 3.5f : 5.f);
	
	for (auto& el : m_player_info) {

		if ((pgzi = reinterpret_cast< PlayerGrandZodiacInfo* >(el.second)) != nullptr
				&& pgzi->flag != PlayerGameInfo::eFLAG_GAME::QUIT) {

			pgzi->m_gz.pontos = (uint32_t)(pgzi->m_gz.total_score * pontos_base);
		}
	}
}

void GrandZodiac::sendTrofel(player& _session) {

	packet p;

	try {

		INIT_PLAYER_INFO("sendTrofel", "tentou enviar o trofeu do player no jogo", &_session);

		if (pgi->m_gz.trofeu > 0) {

			stItem item{ 0 };

			// Inicializa o Trof�u
			item.type = 2;
			item.id = -1;
			item._typeid = pgi->m_gz.trofeu;
			item.qntd = 1;
			item.STDA_C_ITEM_QNTD = (short)item.qntd;

			// Update on Server and Database
			if (item_manager::addItem(item, _session, 0, 0) >= item_manager::RetAddItem::TYPE::T_SUCCESS) {

				// Adicionou o Trof�u com sucesso para o player
				_smp::message_pool::getInstance().push(new message("[GrandZodiac::sendTrofel][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] ganhou Grand Zodiac Trofeu[TYPEID=" + std::to_string(pgi->m_gz.trofeu) + "] na Posicao[RANK=" + std::to_string(pgi->m_gz.position) + "].", CL_FILE_LOG_AND_CONSOLE));

				// Update Trof�u on Game
				p.init_plain((unsigned short)0x1FA);

				p.addUint32(item._typeid);

				p.addUint32(item.id);

				packet_func::session_send(p, &_session, 1);

			}else
				_smp::message_pool::getInstance().push(new message("[GrandZodiac::sendTrofel][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] tentou adicionar Grand Zodiac Trofeu[TYPEID=" + std::to_string(item._typeid) + "] na Posicao[RANK=" + std::to_string(pgi->m_gz.position) 
						+ "], mas nao conseguiu adicionar o item.", CL_FILE_LOG_AND_CONSOLE));
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiac::sendTrofel][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool GrandZodiac::finish_game(player& _session, int option) {
	
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

			if (m_timer != nullptr)
				is_hacker_or_bug = ((int)(m_ri.time_30s - m_timer->getElapsed()) / (60 * 1000/*Minuto*/)) >= 1 ? true : false;

			if (m_grand_zodiac_state)
				finish_grand_zodiac(_session, (option == 0x12C && !is_hacker_or_bug) ? 0 : 1);	// Termina sem ter acabado de jogar
		}
	}

	return (PlayersCompleteGameAndClear() && m_grand_zodiac_state);
}
