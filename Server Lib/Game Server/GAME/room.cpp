// Arquivo room.cpp
// Criado em 24/12/2017 por Acrisio
// Implementação da classe room

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "room.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../PACKET/packet_func_sv.h"

#include "../../Projeto IOCP/UTIL/message_pool.h"

#include <ctime>

#include "../Game Server/game_server.h"

#include "../UTIL/lottery.hpp"
#include "../GAME/item_manager.h"

#include "../../Projeto IOCP/PANGYA_DB/cmd_update_character_equiped.hpp"

#include "../PANGYA_DB/cmd_update_item_slot.hpp"
#include "../PANGYA_DB/cmd_update_caddie_equiped.hpp"
#include "../PANGYA_DB/cmd_update_ball_equiped.hpp"
#include "../PANGYA_DB/cmd_update_clubset_equiped.hpp"
#include "../PANGYA_DB/cmd_update_mascot_equiped.hpp"
#include "../PANGYA_DB/cmd_update_character_cutin_equiped.hpp"
#include "../PANGYA_DB/cmd_update_skin_equiped.hpp"
#include "../PANGYA_DB/cmd_update_poster_equiped.hpp"
#include "../PANGYA_DB/cmd_update_character_all_part_equiped.hpp"
#include "../PANGYA_DB/cmd_update_character_pcl.hpp"
#include "../PANGYA_DB/cmd_update_clubset_stats.hpp"
#include "../PANGYA_DB/cmd_update_character_mastey.hpp"
#include "../PANGYA_DB/cmd_update_clubset_workshop.hpp"
#include "../PANGYA_DB/cmd_update_mascot_info.hpp"

#include "practice.hpp"
#include "tourney.hpp"
#include "special_shuffle_course.hpp"
#include "versus.hpp"
#include "match.hpp"
#include "guild_battle.hpp"
#include "pang_battle.hpp"
#include "approach.hpp"
#include "chip_in_practice.hpp"
#include "grand_zodiac.hpp"

#include "mail_box_manager.hpp"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

using namespace stdA;

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[room::" + std::string((method)) + "][Error] player nao esta connectado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 12, 0)); \

#define REQUEST_BEGIN(method) CHECK_SESSION_BEGIN(std::string("request") + (method)) \
							  if (_packet == nullptr) \
									throw exception("[room::request" + std::string((method)) + "][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 12, 0)); \

room::room(unsigned char _channel_owner, RoomInfoEx _ri) 
	: m_ri(_ri), m_pGame(nullptr), m_channel_owner(_channel_owner), m_teans(), m_weather_lounge(0u), 
		m_destroying(false), m_bot_tourney(false), m_lock_spin_state(0l), m_personal_shop(m_ri) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);

	InitializeCriticalSection(&m_lock_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);

	INIT_PTHREAD_MUTEX_RECURSIVE(&m_lock_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	geraSecurityKey();

	// Calcula chuva(weather) se o tipo da sala for lounge
	calcRainLounge();

	// Atualiza tipo da sala
	setTipo(m_ri.tipo);

	// Att Exp rate, e Pang rate, que criou a sala, att ele também quando começa o jogo
	//if (sgs::gs != nullptr) {
		m_ri.rate_exp = sgs::gs::getInstance().getInfo().rate.exp;
		m_ri.rate_pang = sgs::gs::getInstance().getInfo().rate.pang;
		m_ri.angel_event = sgs::gs::getInstance().getInfo().rate.angel_event;
	//}
};

room::~room() {

	destroy();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_lock_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
	pthread_mutex_destroy(&m_lock_cs);
#endif
};

void room::destroy() {

	// Leave All Players
	leaveAll(0);

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (m_pGame != nullptr)
		delete m_pGame;

	m_pGame = nullptr;

	m_channel_owner = INVALID_CHANNEL;

	m_weather_lounge = 0u;

	if (!v_sessions.empty()) {
		v_sessions.clear();
		v_sessions.shrink_to_fit();
	}

	if (!m_player_info.empty())
		m_player_info.clear();

	clear_invite();

	clear_player_kicked();

	clear_teans();

	m_bot_tourney = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	m_personal_shop.destroy();

	// Destruindo a sala
	try {

		lock();

		m_destroying = true;

		unlock();

	}catch (exception& e) {

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150)) {
			
			unlock();

			_smp::message_pool::getInstance().push(new message("[room::destroy][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}
};

void room::enter(player& _session) {

	if (!_session.getState())
		throw exception("[room::enter][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 4, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (isFull()) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		throw exception("[room::enter][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na a sala[NUMERO="
				+ std::to_string(m_ri.numero) + "], mas a sala ja esta cheia.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0));
	}

	if (_session.m_pi.mi.sala_numero != -1) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		throw exception("[room::enter][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] sala[NUMERO=" + std::to_string(m_ri.numero) + "], ja esta em outra sala[NUMERO=" 
				+ std::to_string(_session.m_pi.mi.sala_numero) + "], nao pode entrar em outra. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 120, 0));
	}

	if (m_ri.tipo == RoomInfo::TIPO::GUILD_BATTLE && m_ri.guilds.guild_1_uid != 0 && m_ri.guilds.guild_2_uid != 0
			&& m_ri.guilds.guild_1_uid != _session.m_pi.gi.uid && m_ri.guilds.guild_2_uid != _session.m_pi.gi.uid) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		throw exception("[room::enter][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] sala[NUMERO=" + std::to_string(m_ri.numero) 
				+ "], ja tem duas guild e o player que quer entrar nao eh de nenhum delas. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 11000, 0));
	}

	try {

		_session.m_pi.mi.sala_numero = m_ri.numero;

		// Update Place player
		if (m_ri.tipo == RoomInfo::PRACTICE || m_ri.tipo == RoomInfo::GRAND_ZODIAC_PRACTICE)
			_session.m_pi.place = 2u;
		else
			_session.m_pi.place = 0u;

		v_sessions.push_back(&_session);

		++m_ri.num_player;

		// Update Trofel
		if (m_ri.trofel > 0)
			updateTrofel();

		// Acabou de criar a sala
		if (m_ri.master == _session.m_pi.uid && m_ri.tipo != RoomInfo::TIPO::GRAND_PRIX) {

			// Update Trofel
			if (_session.m_pi.m_cap.stBit.game_master/* & 4*/) {	// GM

				if ((m_ri.max_player > 30 && m_ri.tipo == RoomInfo::TIPO::TOURNEY) || (m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT && m_ri.tipo <= RoomInfo::TIPO::GRAND_ZODIAC_ADV)) {
				
					m_ri.flag_gm = 1;

					m_ri.state_flag = 0x100;

					m_ri.trofel = TROFEL_GM_EVENT_TYPEID;
				
				}else if (m_ri.tipo == RoomInfo::TIPO::TOURNEY || m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT)
					updateTrofel();

			}else if (m_ri.tipo == RoomInfo::TIPO::TOURNEY || m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT)
				updateTrofel();

		}else if (m_ri.tipo == RoomInfo::TIPO::GRAND_PRIX)
			updateTrofel();

		// Update Master
		// Só trocar o master da sala se não tiver nenhum jogo inicializado
		if (m_pGame == nullptr && v_sessions.size() > 0 && _session.m_pi.m_cap.stBit.game_master/* & 4*/ && m_ri.state_flag != 0x100/*GM*/
				&& m_ri.tipo != RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE && m_ri.tipo != RoomInfo::TIPO::GRAND_PRIX)
			updateMaster(&_session);

		// Add o player ao jogo
		if (m_pGame != nullptr) {

			m_pGame->addPlayer(_session);

			if (m_ri.trofel > 0)
				updateTrofel();
		}

		try {
		
			// Make Info Room Player
			makePlayerInfo(_session);

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[room::enter][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}

		if (m_ri.tipo == RoomInfo::TIPO::GUILD_BATTLE)
			updateGuild(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::enter][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

int room::leave(player& _session, int _option) {
	
	//if (!_session.getState() /*&& _option != 3/*Force*/)
		//throw exception("[room::leave][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 4, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	try {
		size_t index = findIndexSession(&_session);

		if (index == (size_t)~0) {
#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

			throw exception("[room::leave][Error] session[UID=" + std::to_string(_session.m_pi.uid) + "] nao existe no vector de sessions da sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 5, 0));
		}

		if (_option != 0 && _option != 1 && _option != 0x800/*GM Sai da sala*/ && _option != 10/*Saiu com ticket report*/)
			addPlayerKicked(_session.m_pi.uid);

		// Verifica se ele está em um jogo e tira ele
		try {
		
			if (m_pGame != nullptr)
				if (m_pGame->deletePlayer(&_session, _option) && m_pGame->finish_game(_session, 2)/*Deixa para o ultimo da sala finalizar o jogo*/)
					finish_game();

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[room::leave][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	
		v_sessions.erase(v_sessions.begin() + index);

		v_sessions.shrink_to_fit();

		if ((m_ri.num_player - 1) > 0 || v_sessions.size() == 0)
			--m_ri.num_player;

		// Sai do Team se for Match
		if (m_ri.tipo == RoomInfo::TIPO::MATCH) {

			if (m_teans.size() < 2)
				throw exception("[room::leave][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair da sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + "], mas a sala nao tem os 2 teans(times). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1502, 0));

			auto pPri = getPlayerInfo(&_session);

			if (pPri == nullptr)
				throw exception("[room::leave][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair da sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + "], mas a sala nao encontrou o info do player. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1503, 0));

			m_teans[pPri->state_flag.uFlag.stFlagBit.team].deletePlayer(&_session, _option);
		
		}else if (m_ri.tipo == RoomInfo::TIPO::GUILD_BATTLE) {

			auto pPri = getPlayerInfo(&_session);

			if (pPri == nullptr)
				throw exception("[room::leave][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair da sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + "], mas a sala nao encontrou o info do player. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1503, 0));

			auto guild = m_guild_manager.findGuildByPlayer(_session);

			if (guild == nullptr)
				throw exception("[room::leave][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair da sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + "], mas o player nao esta em nenhuma guild da sala. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1504, 0));

			// Deleta o player da guild  da sala
			guild->deletePlayer(&_session);

			// Deleta player do team
			m_teans[pPri->state_flag.uFlag.stFlagBit.team].deletePlayer(&_session, _option);

			// Limpa o team do player
			pPri->state_flag.uFlag.stFlagBit.team = 0u;
				
			// Limpa guild
			if (guild->numPlayers() == 0) {
				
				if (guild->getTeam() == Guild::eTEAM::RED) {

					// Red
					m_ri.guilds.guild_1_uid = 0u;
					m_ri.guilds.guild_1_index_mark = 0u;
					memset(m_ri.guilds.guild_1_mark, 0, sizeof(m_ri.guilds.guild_1_mark));
					memset(m_ri.guilds.guild_1_nome, 0, sizeof(m_ri.guilds.guild_1_nome));

				}else {

					// Blue
					m_ri.guilds.guild_2_uid = 0u;
					m_ri.guilds.guild_2_index_mark = 0u;
					memset(m_ri.guilds.guild_2_mark, 0, sizeof(m_ri.guilds.guild_2_mark));
					memset(m_ri.guilds.guild_2_nome, 0, sizeof(m_ri.guilds.guild_2_nome));

				}

				//delete Guild
				m_guild_manager.deleteGuild(guild);
			}
		}

		// Delete Player Info
		m_player_info.erase(&_session);

		// reseta(default) o número da sala no info do player
		_session.m_pi.mi.sala_numero = -1;
		_session.m_pi.place = 0u;

		// Excluiu personal shop do player se ele estiver com shop aberto
		m_personal_shop.destroyShop(_session);

		updatePosition();

		updateTrofel();

		// Isso é para o cliente saber que ele foi kickado pelo server sem ação de outro player
		if (_option == 0x800 || (_option != 0/*foi chutado da sala*/ && _option != 1/*Próprio player saiu*/ && _option != 3/*chutado da sala sem o jogo ter começado*/)) {

			uint32_t opt_kick = 0x800;

			switch (_option) {
			case 1:
				opt_kick = 4;
				break;
			case 2:
				opt_kick = 2;
				break;
			default:
				opt_kick = _option;
			}

			packet p((unsigned short)0x7E);

			p.addUint32(opt_kick);

			packet_func::session_send(p, &_session, 1);
		}

		if (m_ri.tipo == RoomInfo::LOUNGE) { // Zera State lounge of player
			
			_session.m_pi.state = 0u;
			_session.m_pi.state_lounge = 0u;
		}

		// Update Players State On Room
		if (v_sessions.size() > 0) {
			sendUpdate();

			sendCharacter(_session, 2);
		}
		// Fim Update Players State

		if ((m_pGame == nullptr && m_ri.tipo == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE && _session.m_pi.uid == m_ri.master)
			|| (_session.m_pi.m_cap.stBit.game_master/* & 4*/ && m_ri.master == _session.m_pi.uid && m_ri.tipo != RoomInfo::TIPO::LOUNGE && m_ri.trofel == TROFEL_GM_EVENT_TYPEID)) {

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

			return 0x801;	// deleta todos da sala

		}else if (m_pGame == nullptr/*Só atualiza o master da sala quando não tem nenhum jogo inicializado na sala*/)
			updateMaster(nullptr);	// update Master

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::leave][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (v_sessions.size() > 0 || (m_ri.master == -2 && (/*~Nega*/!isDropRoom() || m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT && m_ri.tipo <= RoomInfo::TIPO::GRAND_ZODIAC_ADV))) ? 0 : 1;
};

int room::leaveAll(int _option) {

	while (!v_sessions.empty()) {

		try {
			leave(*(*v_sessions.begin()), _option);
		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[room::leaveAll][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	return 0;
};

bool room::isInvited(player& _session) {
	
	auto it = m_player_info.find(&_session);

	return (it != m_player_info.end() && it->second.convidado);
};

InviteChannelInfo room::addInvited(uint32_t _uid_has_invite, player& _session) {

	if (!_session.getState())
		throw exception("[room::addInvited][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 4, 0));

	if (isFull())
		throw exception("[room::addInvited][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na a sala[NUMERO="
			+ std::to_string(m_ri.numero) + "], mas a sala ja esta cheia.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0));

	if (findIndexSession(_uid_has_invite) == (size_t)~0)
		throw exception("[room::addInvited][Error] quem convidou[UID=" + std::to_string(_uid_has_invite) + "] o player[UID="
			+ std::to_string(_session.m_pi.uid) + "] para a sala nao esta na sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2010, 0));

	auto s = findSessionByUID(_session.m_pi.uid);

	if (s != nullptr)
		throw exception("[room::addInvited][Error] player[UID=" + std::to_string(_uid_has_invite) + "] tentou adicionar o convidado[UID="
			+ std::to_string(_session.m_pi.uid) + "] a sala, mas ele ja esta na sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2001, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	_session.m_pi.mi.sala_numero = m_ri.numero;

	_session.m_pi.place = 70;	// Está sendo convidado	

	v_sessions.push_back(&_session);

	++m_ri.num_player;

	PlayerRoomInfoEx *pri = nullptr;

	try {

		// Make Info Room Player Invited
		pri = makePlayerInvitedInfo(_session);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[room::addInvited][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	if (pri == nullptr) {

		// Pop_back
		v_sessions.erase(v_sessions.end());

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		throw exception("[[room::addInvited][Error] player[UID=" + std::to_string(_uid_has_invite) + "] tentou adicionar o convidado[UID="
				+ std::to_string(_session.m_pi.uid) + "] a sala, nao conseguiu criar o Player Room Info Invited do player. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2002, 0));
	}

	// Add Invite Channel Info
	InviteChannelInfo ici{ 0 };

	ici.room_number = m_ri.numero;

	ici.invite_uid = _uid_has_invite;
	ici.invited_uid = _session.m_pi.uid;
	
	GetLocalTime(&ici.time);

	v_invite.push_back(ici);
	// End Add Invite Channel Info

	// Update Char Invited ON ROOM
	packet p((unsigned short)0x48);

	p.addUint8(1);
	p.addInt16(-1);

	p.addBuffer(pri, sizeof(PlayerRoomInfo));

	p.addUint8(0);	// Final Packet

	packet_func::room_broadcast(*this, p, 1);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ici;
};

InviteChannelInfo room::deleteInvited(player& _session) {

	// Por que se o player não estiver mais online não pode deletar o convidado
	//if (!_session.getState())
		//throw exception("[room::deleteInvited][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 4, 0));

	auto it = m_player_info.find(&_session);

	if (it == m_player_info.end())
		throw exception("[room::deleteInvited][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou deletar convidado," 
				+ " mas nao tem o info do convidado na sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2003, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	size_t index = findIndexSession(&_session);

	if (index == (size_t)~0) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		throw exception("[room::deleteInvited][Error] session[UID=" + std::to_string(_session.m_pi.uid) + "] nao existe no vector de sessions da sala.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 5, 0));
	}

	_session.m_pi.mi.sala_numero = -1;

	_session.m_pi.place = 0;	// Limpa Está sendo convidado	

	v_sessions.erase(v_sessions.begin() + index);

	v_sessions.shrink_to_fit();

	--m_ri.num_player;

	m_player_info.erase(it);

	// Update Position all players
	updatePosition();

	// Delete Invite Channel Info
	InviteChannelInfo ici{ 0 };

	auto itt = std::find_if(v_invite.begin(), v_invite.end(), [&](auto& _el) {
		return (_el.room_number == m_ri.numero && _el.invited_uid == _session.m_pi.uid);
	});

	if (itt != v_invite.end()) {

		ici = *itt;

		v_invite.erase(itt);

	}else
		_smp::message_pool::getInstance().push(new message("[room::deleteInvited][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem um convite.", CL_FILE_LOG_AND_CONSOLE));

	// End Delete Invite Channel Info

	// Resposta Delete Convidado
	packet p((unsigned short)0x130);

	p.addUint32(_session.m_pi.uid);

	packet_func::room_broadcast(*this, p, 1);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[room::deleteInvited][Log] Deleteou um convite[Convidado=" + std::to_string(_session.m_pi.uid) + "] na Sala[NUMERO=" 
			+ std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ici;
};

InviteChannelInfo room::deleteInvited(uint32_t _uid) {

	if (_uid == 0u)
		throw exception("[room::deleteInvited][Error] _uid is invalid(zero). Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2005, 0));

	auto it = std::find_if(m_player_info.begin(), m_player_info.end(), [&](auto& _el) {
		return (_el.second.convidado && _el.second.uid == _uid);
	});

	if (it == m_player_info.end())
		throw exception("[room::deleteInvited][Error] player[UID=" + std::to_string(_uid) + "] tentou deletar convidado,"
				+ " mas nao tem o info do convidado na sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2003, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	size_t index = findIndexSession(_uid);

	if (index == (size_t)~0) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		throw exception("[room::deleteInvited][Error] session[UID=" + std::to_string(_uid) + "] nao existe no vector de sessions da sala.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 5, 0));
	}

	v_sessions.erase(v_sessions.begin() + index);

	v_sessions.shrink_to_fit();

	--m_ri.num_player;

	m_player_info.erase(it);

	// Update Position all players
	updatePosition();

	// Delete Invite Channel Info
	InviteChannelInfo ici{ 0 };

	auto itt = std::find_if(v_invite.begin(), v_invite.end(), [&](auto& _el) {
		return (_el.room_number == m_ri.numero && _el.invited_uid == _uid);
	});

	if (itt != v_invite.end()) {

		ici = *itt;

		v_invite.erase(itt);

	}else
		_smp::message_pool::getInstance().push(new message("[room::deleteInvited][WARNING] player[UID=" + std::to_string(_uid) + "] nao tem um convite.", CL_FILE_LOG_AND_CONSOLE));

	// End Delete Invite Channel Info

	// Resposta Delete Convidado
	packet p((unsigned short)0x130);

	p.addUint32(_uid);

	packet_func::room_broadcast(*this, p, 1);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[room::deleteInvited][Log] Deleteou um convite[Convidado=" + std::to_string(_uid) + "] na Sala[NUMERO="
			+ std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ici;
};

const RoomInfoEx* room::getInfo() {
	return &m_ri;
};

const unsigned char room::getChannelOwenerId() {
	return m_channel_owner;
}

const short room::getNumero() {
	return m_ri.numero;
};

const uint32_t room::getMaster() {
	return m_ri.master;
};

const uint32_t room::getNumPlayers() {
	return m_ri.num_player;
};

const uint32_t room::getPosition(player *_session) {
	auto position = ~0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto i = 0u; i < v_sessions.size(); ++i) {
		if (v_sessions[i] == _session) {
			position = i;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return position;
};

PlayerRoomInfoEx* room::getPlayerInfo(player *_session) {

	if (_session == nullptr)
		throw exception("Error _session is nullptr. Em room::getPlayerInfo()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 7, 0));

	PlayerRoomInfoEx *pri = nullptr;
	std::map< player*, PlayerRoomInfoEx >::iterator i;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if ((i = m_player_info.find(_session)) != m_player_info.end())
		pri = &i->second;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return pri;
};

std::vector< player* > room::getSessions(player *_session, bool _with_invited) {
	std::vector< player* > v_session;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_sessions)
		if (el != nullptr && el->getState() && el->m_pi.mi.sala_numero != -1 
				&& (_session == nullptr || _session != el) && (_with_invited || !isInvited(*el)))
			v_session.push_back(el);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_session;
};

uint32_t room::getRealNumPlayersWithoutInvited() {
	
	uint32_t num = 0;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	try {
		
		num = _getRealNumPlayersWithoutInvited();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::getRealNumPlayerWithoutInvited][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return num;
};

bool room::haveInvited() {
	
	bool question = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	try {

		question = _haveInvited();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::haveInvited][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return question;
};

void room::setNome(std::string _nome) {
	
	if (_nome.empty())
		throw exception("Error _nome esta vazio. Em room::setNome()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 6, 0));

#if defined(_WIN32)
	memcpy_s(m_ri.nome, sizeof(m_ri.nome), _nome.c_str(), sizeof(m_ri.nome));
#elif defined(__linux__)
	memcpy(m_ri.nome, _nome.c_str(), sizeof(m_ri.nome));
#endif
};

void room::setSenha(std::string _senha) {

	if (_senha.empty()) {
		if (!m_ri.senha_flag) {
			memset(m_ri.senha, 0, sizeof(m_ri.senha));
			
			m_ri.senha_flag = 1;
		}
	}else {
#if defined(_WIN32)
		memcpy_s(m_ri.senha, sizeof(m_ri.senha), _senha.c_str(), sizeof(m_ri.senha));
#elif defined(__linux__)
		memcpy(m_ri.senha, _senha.c_str(), sizeof(m_ri.senha));
#endif

		m_ri.senha_flag = 0;
	}
};

void room::setTipo(unsigned char _tipo) {
	
	if (_tipo == RoomInfo::TIPO::MATCH || _tipo == RoomInfo::TIPO::GUILD_BATTLE)	
		init_teans();
	else if (_tipo != RoomInfo::TIPO::MATCH && m_ri.tipo == RoomInfo::TIPO::MATCH)
		clear_teans();

	m_ri.tipo = _tipo;
	
	// Atualizar tipo da sala
	if (m_ri.tipo > RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)
		m_ri.tipo_show = 4;
	else if (m_ri.tipo == RoomInfo::TIPO::GRAND_ZODIAC_ADV || m_ri.tipo == RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)
		m_ri.tipo_show = RoomInfo::TIPO::GRAND_ZODIAC_INT;
	else
		m_ri.tipo_show = m_ri.tipo;

	if (m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT)
		m_ri.tipo_ex = m_ri.tipo;
	else
		m_ri.tipo_ex = ~0;

	// Atualiza Trofel se for Tourney
	if (m_ri.tipo == RoomInfo::TIPO::TOURNEY || (m_ri.master != -2 && m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT && m_ri.tipo <= RoomInfo::TIPO::GRAND_ZODIAC_ADV)) {
		
		if ((m_ri.max_player > 30 && m_ri.tipo == RoomInfo::TIPO::TOURNEY) || (m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT && m_ri.tipo <= RoomInfo::TIPO::GRAND_ZODIAC_ADV)) {

			m_ri.flag_gm = 1;

			m_ri.state_flag = 0x100;

			m_ri.trofel = TROFEL_GM_EVENT_TYPEID;

		}else if (m_ri.tipo == RoomInfo::TIPO::TOURNEY || m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT)
			updateTrofel();

	}else
		m_ri.trofel = 0u;
};

void room::setCourse(unsigned char _course) {
	m_ri.course = RoomInfo::eCOURSE(_course);
};

void room::setQntdHole(unsigned char _qntd_hole) {
	m_ri.qntd_hole = _qntd_hole;
};

void room::setModo(unsigned char _modo) {
	m_ri.modo = _modo;
};

void room::setTempoVS(uint32_t _tempo) {
	m_ri.time_vs = _tempo;
};

void room::setMaxPlayer(unsigned char _max_player) {

	if (v_sessions.size() > _max_player)
		throw exception("[room::setMaxPlayer][Error] MASTER[UID=" + std::to_string(m_ri.master) + "] _max_player[VALUE=" + std::to_string(_max_player) 
				+ "] eh menor que o numero de jogadores[VALUE=" + std::to_string(v_sessions.size()) + "] na sala.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 250, 0x588000));

	// New Max player room
	m_ri.max_player = _max_player;

	// Atualiza Trofeu se for Tourney
	if (m_ri.tipo == RoomInfo::TIPO::TOURNEY || (m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT && m_ri.tipo <= RoomInfo::TIPO::GRAND_ZODIAC_ADV)) {

		if ((m_ri.max_player > 30 && m_ri.tipo == RoomInfo::TIPO::TOURNEY) || (m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT && m_ri.tipo <= RoomInfo::TIPO::GRAND_ZODIAC_ADV)) {

			m_ri.flag_gm = 1;

			m_ri.trofel = TROFEL_GM_EVENT_TYPEID;

		}else if (m_ri.tipo == RoomInfo::TIPO::TOURNEY || m_ri.tipo >= RoomInfo::TIPO::GRAND_ZODIAC_INT)
			updateTrofel();

	}
};

void room::setTempo30S(uint32_t _tempo) {
	m_ri.time_30s = _tempo;
};

void room::setHoleRepeat(unsigned char _hole_repeat) {
	m_ri.hole_repeat = _hole_repeat;
};

void room::setFixedHole(uint32_t _fixed_hole) {
	m_ri.fixed_hole = _fixed_hole;
};

void room::setArtefato(uint32_t _artefato) {
	m_ri.artefato = _artefato;
};

void room::setNatural(uint32_t _natural) {
	m_ri.natural.ulNaturalAndShortGame = _natural;
};

void room::setState(unsigned char _state) {
	m_ri.state = _state;
};

void room::setFlag(unsigned char _flag) {
	m_ri.flag = _flag;
};

void room::setStateAFK(unsigned char _state_afk) {
	m_ri.state_afk = _state_afk;
};

bool room::checkPass(std::string _pass) {
	
	if (!isLocked())
		throw exception("[Room::checkPass][Error] sala nao tem senha", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0));

	return strcmp(m_ri.senha, _pass.c_str()) == 0;
};

bool room::checkPersonalShopItem(player& _session, int32_t _item_id) {
	return m_personal_shop.isItemForSale(_session, _item_id);
};

bool room::isLocked() {
	return !m_ri.senha_flag;
};

bool room::isFull() {
	return m_ri.num_player >= m_ri.max_player;
};

bool room::isGaming() {
	return m_pGame != nullptr;
};

bool room::isGamingBefore(uint32_t _uid) {

	if (_uid == 0u)
		throw exception("[room::isGamingBefore][Error] _uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1000, 0));

	if (m_pGame == nullptr)
		throw exception("[room::isGamingBefore][Error] a sala[NUMERO=" + std::to_string(m_ri.numero) + "] nao tem um jogo inicializado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1001, 0));

	return m_pGame->isGamingBefore(_uid);
};

void room::updatePlayerInfo(player& _session) {
	PlayerRoomInfoEx pri{}, *_pri = nullptr;

	if ((_pri = getPlayerInfo(&_session)) == nullptr)
		throw exception("[room::updatePlayerInfo][Error] nao tem o player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] info dessa session na sala.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 8, 0));

	// Copia do que esta no map
	pri = *_pri;

	// Player Room Info Update
	pri.oid = _session.m_oid;
#if defined(_WIN32)
	memcpy_s(pri.nickname, sizeof(pri.nickname), _session.m_pi.nickname, sizeof(pri.nickname));
	memcpy_s(pri.guild_name, sizeof(pri.guild_name), _session.m_pi.gi.name, sizeof(pri.guild_name));
#elif defined(__linux__)
	memcpy(pri.nickname, _session.m_pi.nickname, sizeof(pri.nickname));
	memcpy(pri.guild_name, _session.m_pi.gi.name, sizeof(pri.guild_name));
#endif
	pri.position = (unsigned char)getPosition(&_session) + 1;	// posição na sala
	pri.capability = _session.m_pi.m_cap;
	pri.title = _session.m_pi.ue.m_title;
	
	if (_session.m_pi.ei.char_info != nullptr)
		pri.char_typeid = _session.m_pi.ei.char_info->_typeid;

#if defined(_WIN32)
	memcpy_s(pri.skin, sizeof(pri.skin), _session.m_pi.ue.skin_typeid, sizeof(pri.skin));
#elif defined(__linux__)
	memcpy(pri.skin, _session.m_pi.ue.skin_typeid, sizeof(pri.skin));
#endif
	pri.skin[4] = 0;		// Aqui tem que ser zero, se for outro valor não mostra a imagem do character equipado

	if (getMaster() == _session.m_pi.uid) {
		pri.state_flag.uFlag.stFlagBit.master = 1;
		pri.state_flag.uFlag.stFlagBit.ready = 1;	// Sempre está pronto(ready) o master
	}else {
		
		// Só troca o estado de pronto dele na sala, se anterior mente ele era Master da sala ou não estiver pronto
		if (pri.state_flag.uFlag.stFlagBit.master || !pri.state_flag.uFlag.stFlagBit.ready)
			pri.state_flag.uFlag.stFlagBit.ready = 0;

		pri.state_flag.uFlag.stFlagBit.master = 0;
	}

	pri.state_flag.uFlag.stFlagBit.sexo = _session.m_pi.mi.sexo;

	// Update Team se for Match
	if (m_ri.tipo == RoomInfo::TIPO::MATCH) {

		// Verifica se o player está em algum team para atualizar o team dele se ele não estiver em nenhum
		auto player_team = pri.state_flag.uFlag.stFlagBit.team;
		player* p_seg_team = nullptr;

		// atualizar o team do player a flag de team dele não bate com o team dele
		if (m_teans[player_team].findPlayerByUID(pri.uid) == nullptr && (p_seg_team = m_teans[~player_team].findPlayerByUID(pri.uid)) == nullptr) {

			// Player não está em nenhum team
			if (v_sessions.size() > 1) {

				if (m_teans[0].getCount() >= 2 && m_teans[1].getCount() >= 2)
					throw exception("[room::updatePlayerInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar em time para todos os times da sala estao cheios. Hacker ou Bug", 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1500, 0));
				else if (m_teans[0].getCount() >= 2)
					pri.state_flag.uFlag.stFlagBit.team = 1;	// Blue
				else if (m_teans[1].getCount() >= 2)
					pri.state_flag.uFlag.stFlagBit.team = 0;	// Red
				else {

					auto pPri = getPlayerInfo((v_sessions.size() == 2) ? *v_sessions.begin() : (v_sessions.size() > 2 ? *(v_sessions.rbegin() += 1) : nullptr));

					if (pPri == nullptr)
						throw exception("[room::updatePlayerInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar em um time, mas o ultimo player da sala, nao tem um info no sala. Bug", 
								STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1501, 0));

					pri.state_flag.uFlag.stFlagBit.team = ~pPri->state_flag.uFlag.stFlagBit.team;
				}

			}else
				pri.state_flag.uFlag.stFlagBit.team = 0;

			m_teans[pri.state_flag.uFlag.stFlagBit.team].addPlayer(&_session);
		
		}else if (p_seg_team != nullptr) {
		
			// a flag de team do player está errada, ele está no outro team, ajeita
			pri.state_flag.uFlag.stFlagBit.team = ~player_team;
		}

	}else if (m_ri.tipo != RoomInfo::TIPO::GUILD_BATTLE)	// O Guild Battle tem sua própria função para inicializar e atualizar o team e os dados da guild
		pri.state_flag.uFlag.stFlagBit.team = (pri.position - 1) % 2;

	// Só faz calculo de Quita rate depois que o player
	// estiver no level Beginner E e jogado 50 games
	if (_session.m_pi.level >= 6 && _session.m_pi.ui.jogado >= 50) {
		float rate = _session.m_pi.ui.getQuitRate();

		if (rate < GOOD_PLAYER_ICON)
			pri.state_flag.uFlag.stFlagBit.azinha = 1;
		else if (rate >= QUITER_ICON_1 && rate < QUITER_ICON_2)
			pri.state_flag.uFlag.stFlagBit.quiter_1 = 1;
		else if (rate >= QUITER_ICON_2)
			pri.state_flag.uFlag.stFlagBit.quiter_2 = 1;
	}

	pri.level = _session.m_pi.mi.level;

	if (_session.m_pi.ei.char_info != nullptr && _session.m_pi.ui.getQuitRate() < GOOD_PLAYER_ICON)
		pri.icon_angel = _session.m_pi.ei.char_info->AngelEquiped();
	else
		pri.icon_angel = 0u;

	pri.ucUnknown_0A = 10;	// 0x0A dec"10" _session.m_pi.place
	pri.guild_uid = _session.m_pi.gi.uid;
#if defined(_WIN32)
	memcpy_s(pri.guild_mark_img, sizeof(pri.guild_mark_img), _session.m_pi.gi.mark_emblem, sizeof(pri.guild_mark_img));
#elif defined(__linux__)
	memcpy(pri.guild_mark_img, _session.m_pi.gi.mark_emblem, sizeof(pri.guild_mark_img));
#endif
	pri.guild_mark_index = _session.m_pi.gi.index_mark_emblem;
	pri.uid = _session.m_pi.uid;
	pri.state_lounge = _session.m_pi.state_lounge;
	pri.usUnknown_flg = 0;			// Ví players com valores 2 e 4 e 0
	pri.state = _session.m_pi.state;
	pri.location = { _session.m_pi.location.x, _session.m_pi.location.z, _session.m_pi.location.r };
	
	// Personal Shop
	pri.shop = m_personal_shop.getPersonShop(_session);

	if (_session.m_pi.ei.mascot_info != nullptr)
		pri.mascot_typeid = _session.m_pi.ei.mascot_info->_typeid;

	pri.flag_item_boost = _session.m_pi.checkEquipedItemBoost();
	pri.ulUnknown_flg = 0;
	//pri.id_NT não estou usando ainda
	//pri.ucUnknown106
	
	// Só atualiza a flag de convidado se for diferente de 1, por que 1 ele é convidado
	if (pri.convidado != 1)
		pri.convidado = 0;	// Flag Convidado, [Não sei bem por que os que entra na sala normal tem valor igual aqui, já que é flag de convidado waiting], Valor constante da sala para os players(ACHO)
	
	pri.avg_score = _session.m_pi.ui.getMediaScore();
	//pri.ucUnknown3
	
	if (_session.m_pi.ei.char_info != nullptr)
		pri.ci = *_session.m_pi.ei.char_info;
	
	// Salva novamente
	*_pri = pri;
};

player* room::findSessionByOID(uint32_t _oid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto i = VECTOR_FIND_PTR_ITEM(v_sessions, m_oid, ==, _oid);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	if (i != v_sessions.end())
		return *i;

	return nullptr;
};

player* room::findSessionByUID(uint32_t _uid) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto i = VECTOR_FIND_PTR_ITEM(v_sessions, m_pi.uid, ==, _uid);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	if (i != v_sessions.end())
		return *i;

	return nullptr;
};

player* room::findMaster() {

	player* master = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto pMaster = std::find_if(v_sessions.begin(), v_sessions.end(), [&](auto& _el) {
		return _el->m_pi.uid == m_ri.master;
	});

	if (pMaster != v_sessions.end())
		master = *pMaster;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return master;
};

void room::makeBot(player& _session) {
	CHECK_SESSION_BEGIN("makeBot");

	packet p;

	try {

// Bot Ticket TypeId
#define TICKET_BOT_TYPEID 0x1A000401ul

		// Premium User Não precisa de ticket não
		if (_session.m_pi.m_cap.stBit.premium_user) {

			// Add Bot Tourney Visual para a sala
			addBotVisual(_session);

			// Send Message
			p.init_plain((unsigned short)0x40);	// Msg to Chat of player

			p.addUint8(7);	// Notice

			p.addString("@SuperSS");
			p.addString("[ \\2Premium ] \\c0xff00ff00\\cBot was created.");

			packet_func::session_send(p, &_session, 1);

		}else {

			// Verifica se ele tem o ticket para criar o Bot se não manda mensagem dizenho que ele não tem ticket para criar o bot
			auto pWi = _session.m_pi.findWarehouseItemByTypeid(TICKET_BOT_TYPEID);

			if (pWi != nullptr && pWi->STDA_C_ITEM_QNTD > 1) {

				stItem item{ 0 };

				item.type = 2;
				item.id = pWi->id;
				item._typeid = pWi->_typeid;
				item.qntd = 1;
				item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

				if (item_manager::removeItem(item, _session) > 0) {

					// Atualiza o item no Jogo e Add o Bot e manda a mensagem que o bot foi add
					p.init_plain((unsigned short)0x216);

					p.addUint32((const uint32_t)GetSystemTimeAsUnix());
					p.addUint32(1);	// Count;

					p.addUint8(item.type);
					p.addUint32(item._typeid);
					p.addInt32(item.id);
					p.addUint32(item.flag_time);
					p.addBuffer(&item.stat, sizeof(item.stat));
					p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
					p.addZeroByte(25);

					packet_func::session_send(p, &_session, 1);

					// Add Bot
					addBotVisual(_session);

					// Send Message
					p.init_plain((unsigned short)0x40);	// Msg to Chat of player

					p.addUint8(7);	// Notice

					p.addString("@SuperSS");
					p.addString("\\c0xff00ff00\\cBot was created 1 ticket has been consumed.");

					packet_func::session_send(p, &_session, 1);

				}else {
							
					_smp::message_pool::getInstance().push(new message("[room::makeBot][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "] nao conseguiu deletar o TICKET_BOT[TYPEID=" + std::to_string(TICKET_BOT_TYPEID) + ", ID=" + std::to_string(item.id) + "]", CL_FILE_LOG_AND_CONSOLE));

					// Send Message
					p.init_plain((unsigned short)0x40);	// Msg to Chat of player

					p.addUint8(7);	// Notice

					p.addString("@SuperSS");
					p.addString("\\c0xffff0000\\cError creating Bot.");

					packet_func::session_send(p, &_session, 1);
				}
					
			}else {

				// Não tem ticket bot suficiente, manda mensagem
				// Send Message
				p.init_plain((unsigned short)0x40);	// Msg to Chat of player

				p.addUint8(7);	// Notice

				p.addString("@SuperSS");
				p.addString("\\c0xffff0000\\cYou do not have enough ticket to create the Bot.");

				packet_func::session_send(p, &_session, 1);
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::makeBot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Send Message
		p.init_plain((unsigned short)0x40);	// Msg to Chat of player

		p.addUint8(7);	// Notice

		p.addString("@SuperSS");
		p.addString("\\c0xffff0000\\cError creating Bot.");

		packet_func::session_send(p, &_session, 1);
	}
};

bool room::requestChangeInfoRoom(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeInfoRoom");

	bool ret = false;

	try {

		unsigned char num_info;
		short flag;

		if (m_ri.master != _session.m_pi.uid)
			throw exception("[room::requestChangeInfoRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar o info da sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) + "], mas nao pode trocar o info da sala sem ser master.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 11, 0));

		flag = _packet->readInt16();

		num_info = _packet->readUint8();

		if (num_info <= 0)
			throw exception("[room::requestChangeInfoRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar o info da sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) + "], mas nao tem nenhum info para trocar do buffer do cliente.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 8, 0));

		for (auto i = 0u; i < num_info; ++i) {

			switch (_packet->readUint8()) {
			case RoomInfo::INFO_CHANGE::NAME:
				setNome(_packet->readString());
				break;
			case RoomInfo::INFO_CHANGE::SENHA:
				setSenha(_packet->readString());
				break;
			case RoomInfo::INFO_CHANGE::TIPO:
				setTipo(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::COURSE:
				setCourse(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::QNTD_HOLE:
				setQntdHole(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::MODO:
				setModo(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::TEMPO_VS:	// Passa em Segundos
				setTempoVS(_packet->readUint16() * 1000);
				break;
			case RoomInfo::INFO_CHANGE::MAX_PLAYER:
				setMaxPlayer(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::TEMPO_30S:	// Passa em Minutos
				setTempo30S(_packet->readUint8() * 60000);
				break;
			case RoomInfo::INFO_CHANGE::STATE_FLAG:
				// Esse não usa mais
				// Aqui posso usar para começar o jogo, se a sala estiver(AFK) => "isso acontece quando o master está AFK"
				// Então vou salver esse valor aqui
				setStateAFK(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::HOLE_REPEAT:
				setHoleRepeat(_packet->readUint8());
				break;
			case RoomInfo::INFO_CHANGE::FIXED_HOLE:
				setFixedHole(_packet->readUint32());
				break;
			case RoomInfo::INFO_CHANGE::ARTEFATO:
				setArtefato(_packet->readUint32());
				break;
			case RoomInfo::INFO_CHANGE::NATURAL:
			{
				uNaturalAndShortGame natural = (uint32_t)_packet->readUint32();

				if (sgs::gs::getInstance().getInfo().propriedade.stBit.natural)	// Natural não deixa desabilitar o Natural da sala, por que o server é natural
					natural.stBit.natural = 1u;

				setNatural(natural.ulNaturalAndShortGame);

				break;
			}
			default:
				throw exception("[room::requestChangeInfoRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar info da sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) + "], mas info change eh desconhecido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 9, 0));
			}
		}

		// send to clients update room info
		sendUpdate();

		ret = true;	// Trocou o info da sala com sucesso

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeInfoRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta para o cliente
		packet p;

		packet_func::pacote04A(p, m_ri, 25/*Não mostra erro mas eu sei que foi erro pela opção que enviei*/);
		packet_func::session_send(p, &_session, 1);
	}

	return ret;
};

void room::requestChatTeam(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChatTeam");

	packet p;

	try {

		auto msg = _packet->readString();

		// Verifica a mensagem com palavras proibida e manda para o log e bloquea o chat dele
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[room::requestChatTeam][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + ", MESSAGE=" + msg + "]", CL_ONLY_FILE_LOG));
#endif

		if (msg.empty())
			throw exception("[room::requestChatTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mandar messsage[MSG=" + msg + "] no chat do team na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a msg esta vazia. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2000, 0));

		if (m_ri.tipo != RoomInfo::TIPO::MATCH && m_ri.tipo != RoomInfo::TIPO::GUILD_BATTLE)
			throw exception("[room::requestChatTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mandar messsage[MSG=" + msg + "] no chat do team na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao eh MATCH ou GUILD_BATTLE. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2001, 0));

		if (m_teans.empty())
			throw exception("[room::requestChatTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mandar messsage[MSG=" + msg + "] no chat do team na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum team. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2002, 0));

		auto pri = getPlayerInfo(&_session);

		if (pri == nullptr)
			throw exception("[room::requetChatTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mandar messsage[MSG=" + msg + "] no chat do team na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem o info dele. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2003, 0));

		auto team = m_teans[pri->state_flag.uFlag.stFlagBit.team];

		if (team.findPlayerByUID(_session.m_pi.uid) == nullptr)
			throw exception("[room::requestChatTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mandar messsage[MSG=" + msg + "] no chat do team na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas ele nao esta no team que a flag de team dele diz. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2004, 0));

		// LOG GM
		// Envia para todo os GM do server essa message
		auto c = sgs::gs::getInstance().findChannel(_session.m_pi.channel);

		if (c != nullptr) {

			auto gm = sgs::gs::getInstance().findAllGM();

			if (!gm.empty()) {

				std::string msg_gm = "\\5" + std::string(_session.m_pi.nickname) + ": '" + msg + "'";
				std::string from = "\\1[Channel=" + std::string(c->getInfo()->name) + ", \\1ROOM=" + std::to_string(_session.m_pi.mi.sala_numero) + "][Team" 
						+ (!pri->state_flag.uFlag.stFlagBit.team ? "R" : "B") +"]";

				auto index = from.find(' ');

				if (index != std::string::npos)
					from.replace(index, 1, " \\1");

				for (auto& el : gm) {

					if (((el->m_gi.channel && el->m_pi.channel == c->getInfo()->id) || el->m_gi.whisper || el->m_gi.isOpenPlayerWhisper(_session.m_pi.uid))
						&& /* Check SAME Channel and Room*/(el->m_pi.channel != _session.m_pi.channel || el->m_pi.mi.sala_numero != _session.m_pi.mi.sala_numero
							||/*não está no mesmo team que enviou a mensagem*/ team.findPlayerByUID(el->m_pi.uid) == nullptr)) {

						// Responde no chat do player
						p.init_plain((unsigned short)0x40);

						p.addUint8(0);

						p.addString(from);		// Nickname

						p.addString(msg_gm);	// Message

						packet_func::session_send(p, el, 1);
					}
				}
			}

		}else
			_smp::message_pool::getInstance().push(new message("[room::requestChatTeam][WARNING] Log GM nao encontrou o Channel[ID=" 
					+ std::to_string((unsigned short)_session.m_pi.channel) + "] no server. Bug", CL_FILE_LOG_AND_CONSOLE));

		// Manda message para o team da sala
		p.init_plain((unsigned short)0xB0);

		p.addString(_session.m_pi.nickname);
		p.addString(msg);

		for (auto& el : team.getPlayers())
			packet_func::session_send(p, el, 1);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChatTeam][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestChangePlayerItemRoom(player& _session, ChangePlayerItemRoom& _cpir) {
	CHECK_SESSION_BEGIN("ChangePlayerItemRoom");

	packet p;

	try {

		int error = 0/*SUCCESS*/;

		switch (_cpir.type) {
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_CADDIE:	// Caddie
		{
			CaddieInfoEx *pCi = nullptr;

			// Caddie
			if (_cpir.caddie != 0 && (pCi = _session.m_pi.findCaddieById(_cpir.caddie)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE) {
					
				// Check if item is in map of update item
				auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(pCi->_typeid, pCi->id);
					
				if (!v_it.empty()) {

					for (auto& el : v_it) {

						if (el->second.type == UpdateItem::CADDIE) {
								
							// Desequipa o caddie
							_session.m_pi.ei.cad_info = nullptr;
							_session.m_pi.ue.caddie_id = 0;

							_cpir.caddie = 0;

						}else if (el->second.type == UpdateItem::CADDIE_PARTS) {

							// Limpa o caddie Parts
							pCi->parts_typeid = 0u;
							pCi->parts_end_date_unix = 0;
							pCi->end_parts_date = { 0 };

							_session.m_pi.ei.cad_info = pCi;
							_session.m_pi.ue.caddie_id = _cpir.caddie;
						}

						// Tira esse Update Item do map
						_session.m_pi.mp_ui.erase(el);
					}

				}else {

					// Caddie is Good, Update caddie equiped ON SERVER AND DB
					_session.m_pi.ei.cad_info = pCi;
					_session.m_pi.ue.caddie_id = _cpir.caddie;

					// Verifica se o Caddie pode ser equipado
					if (_session.checkCaddieEquiped(_session.m_pi.ue))
						_cpir.caddie = _session.m_pi.ue.caddie_id;

				}

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCaddieEquiped(_session.m_pi.uid, _cpir.caddie), room::SQLDBResponse, this);

			}else if (_session.m_pi.ue.caddie_id > 0 && _session.m_pi.ei.cad_info != nullptr) {	// Desequipa Caddie
			
				error = (_cpir.caddie == 0) ? 1/*client give invalid item id*/ : (pCi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				if (error > 1) {
					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o Caddie[ID=" + std::to_string(_cpir.caddie) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], desequipando o caddie. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				}

				// Check if item is in map of update item
				auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(_session.m_pi.ei.cad_info->_typeid, _session.m_pi.ei.cad_info->id);
					
				if (!v_it.empty()) {

					for (auto& el : v_it) {

						// Caddie já vai se desequipar, só verifica o parts
						if (el->second.type == UpdateItem::CADDIE_PARTS) {

							// Limpa o caddie Parts
							_session.m_pi.ei.cad_info->parts_typeid = 0u;
							_session.m_pi.ei.cad_info->parts_end_date_unix = 0;
							_session.m_pi.ei.cad_info->end_parts_date = { 0 };
						}

						// Tira esse Update Item do map
						_session.m_pi.mp_ui.erase(el);
					}

				}

				_session.m_pi.ei.cad_info = nullptr;
				_session.m_pi.ue.caddie_id = 0;

				_cpir.caddie = 0;

				// Zera o Error para o cliente desequipar o caddie que o server desequipou
				error = 0;

				// Att No DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCaddieEquiped(_session.m_pi.uid, _cpir.caddie), room::SQLDBResponse, this);
			}

			packet_func::pacote04B(p, &_session, _cpir.type, error);
			packet_func::room_broadcast(*this, p, 1);
			break;
		}
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_BALL: // Ball
		{
			WarehouseItemEx *pWi = nullptr;

			if (_cpir.ball != 0 && (pWi = _session.m_pi.findWarehouseItemByTypeid(_cpir.ball)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL) {
			
				_session.m_pi.ei.comet = pWi;
				_session.m_pi.ue.ball_typeid = _cpir.ball;		// Ball(Comet) é o typeid que o cliente passa

				// Verifica se a bola pode ser equipada
				if (_session.checkBallEquiped(_session.m_pi.ue))
					_cpir.ball = _session.m_pi.ue.ball_typeid;

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);

			}else if (_cpir.ball == 0) { // Bola 0 coloca a bola padrão para ele, se for premium user coloca a bola de premium user

				// Zera para equipar a bola padrão
				_session.m_pi.ei.comet = nullptr;
				_session.m_pi.ue.ball_typeid = 0l;

				// Verifica se a Bola pode ser equipada (Coloca para equipar a bola padrão
				if (_session.checkBallEquiped(_session.m_pi.ue))
					_cpir.ball = _session.m_pi.ue.ball_typeid;

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);

			}else {

				error = (pWi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				pWi = _session.m_pi.findWarehouseItemByTypeid(DEFAULT_COMET_TYPEID);

				if (pWi != nullptr) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar a Ball[TYPEID=" + std::to_string(_cpir.ball) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], colocando a Ball Padrao do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
					_session.m_pi.ei.comet = pWi;
					_cpir.ball = _session.m_pi.ue.ball_typeid = pWi->_typeid;

					// Zera o Error para o cliente equipar a Ball Padrão que o server equipou
					error = 0;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);
			
				}else {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar a Ball[TYPEID=" + std::to_string(_cpir.ball) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], ele nao tem a Ball Padrao, adiciona a Ball pardrao para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = DEFAULT_COMET_TYPEID;
					bi.qntd = 1;
				
					item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0) {

						if ((_cpir.ball = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

							// Equipa a Ball padrao
							pWi = _session.m_pi.findWarehouseItemById(_cpir.ball);

							if (pWi != nullptr) {

								_session.m_pi.ei.comet = pWi;
								_session.m_pi.ue.ball_typeid = pWi->_typeid;

								// Zera o Error para o cliente equipar a Ball Padrão que o server equipou
								error = 0;

								// Update ON DB
								snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);

								// Update ON GAME
								p.init_plain((unsigned short)0x216);

								p.addUint32((const uint32_t)GetSystemTimeAsUnix());
								p.addUint32(1);	// Count

								p.addUint8(item.type);
								p.addUint32(item._typeid);
								p.addInt32(item.id);
								p.addUint32(item.flag_time);
								p.addBuffer(&item.stat, sizeof(item.stat));
								p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
								p.addZeroByte(25);

								packet_func::session_send(p, &_session, 1);

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu achar a Ball[ID=" 
										+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu adicionar a Ball[TYPEID=" 
									+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					}else
						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao conseguiu inicializar a Ball[TYPEID=" 
								+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			}

			packet_func::pacote04B(p, &_session, _cpir.type, error);
			packet_func::room_broadcast(*this, p, 1);
			break;
		}
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_CLUBSET: // ClubSet
		{
			WarehouseItemEx *pWi = nullptr;

			// ClubSet
			if (_cpir.clubset != 0 && (pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET) {
			
				auto c_it = _session.m_pi.findUpdateItemByIdAndType(_cpir.clubset, UpdateItem::WAREHOUSE);

				if (c_it == _session.m_pi.mp_ui.end()) {

					_session.m_pi.ei.clubset = pWi;

					// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
					// que no original fica no warehouse msm, eu só confundi quando fiz
					_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

					IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

					if (cs != nullptr) {

						for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
							_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

						_session.m_pi.ue.clubset_id = _cpir.clubset;

						// Verifica se o ClubSet pode ser equipado
						if (_session.checkClubSetEquiped(_session.m_pi.ue))
							_cpir.clubset = _session.m_pi.ue.clubset_id;

						// Update ON DB
						snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

					}else {
						
						error = 5/*Item Not Found ON IFF_STRUCT SERVER*/;

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] tentou Atualizar Clubset[TYPEID=" + std::to_string(pWi->_typeid) + ", ID=" + std::to_string(pWi->id) 
								+ "] equipado, mas ClubSet Not exists on IFF structure. Equipa o ClubSet padrao. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

						// Coloca o ClubSet CV1 no lugar do ClubSet que acabou o tempo
						pWi = _session.m_pi.findWarehouseItemByTypeid(AIR_KNIGHT_SET);

						if (pWi != nullptr) {

							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
									+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) 
									+ "] para comecar o jogo, mas acabou o tempo do ClubSet[ID=" + std::to_string(_cpir.clubset) 
									+ "], colocando o ClubSet Padrao\"CV1\" do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
						
							// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
							// que no original fica no warehouse msm, eu só confundi quando fiz
							_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

							IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

							if (cs != nullptr)
								for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
									_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

							_session.m_pi.ei.clubset = pWi;
							_cpir.clubset = _session.m_pi.ue.clubset_id = pWi->id;

							// Zera o Error para o cliente equipar a "CV1" que o server equipou
							error = 0;

							// Update ON DB
							snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);
			
						}else {

							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
									+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
									+ std::to_string(_cpir.clubset) + "], ele nao tem o ClubSet Padrao\"CV1\", adiciona o ClubSet pardrao\"CV1\" para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

							BuyItem bi{ 0 };
							stItem item{ 0 };

							bi.id = -1;
							bi._typeid = AIR_KNIGHT_SET;
							bi.qntd = 1;
				
							item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

							if (item._typeid != 0) {

								if ((_cpir.clubset = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

									// Equipa o ClubSet CV1
									pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset);

									if (pWi != nullptr) {

										// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
										// que no original fica no warehouse msm, eu só confundi quando fiz
										_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

										IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

										if (cs != nullptr)
											for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
												_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

										_session.m_pi.ei.clubset = pWi;
										_session.m_pi.ue.clubset_id = pWi->id;

										// Zera o Error para o cliente equipar a "CV1" que o server equipou
										error = 0;

										// Update ON DB
										snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

										// Update ON GAME
										p.init_plain((unsigned short)0x216);

										p.addUint32((const uint32_t)GetSystemTimeAsUnix());
										p.addUint32(1);	// Count

										p.addUint8(item.type);
										p.addUint32(item._typeid);
										p.addInt32(item.id);
										p.addUint32(item.flag_time);
										p.addBuffer(&item.stat, sizeof(item.stat));
										p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
										p.addZeroByte(25);

										packet_func::session_send(p, &_session, 1);

									}else
										_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
												+ "] nao conseguiu achar o ClubSet\"CV1\"[ID=" 
												+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

								}else
									_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
											+ "] nao conseguiu adicionar o ClubSet[TYPEID=" 
											+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu inicializar o ClubSet[TYPEID=" 
										+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
						}
					}
			
				}else {	// ClubSet Acabou o tempo

					error = 6;	// Acabou o tempo do item

					// Coloca o ClubSet CV1 no lugar do ClubSet que acabou o tempo
					pWi = _session.m_pi.findWarehouseItemByTypeid(AIR_KNIGHT_SET);

					if (pWi != nullptr) {

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
								+ std::to_string(_cpir.clubset) + "], colocando o ClubSet Padrao\"CV1\" do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

						// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
						// que no original fica no warehouse msm, eu só confundi quando fiz
						_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

						IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

						if (cs != nullptr)
							for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
								_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

						_session.m_pi.ei.clubset = pWi;
						_cpir.clubset = _session.m_pi.ue.clubset_id = pWi->id;

						// Zera o Error para o cliente equipar a "CV1" que o server equipou
						error = 0;

						// Update ON DB
						snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);
			
					}else {

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
								+ std::to_string(_cpir.clubset) + "], ele nao tem o ClubSet Padrao\"CV1\", adiciona o ClubSet pardrao\"CV1\" para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

						BuyItem bi{ 0 };
						stItem item{ 0 };

						bi.id = -1;
						bi._typeid = AIR_KNIGHT_SET;
						bi.qntd = 1;
				
						item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

						if (item._typeid != 0) {

							if ((_cpir.clubset = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

								// Equipa o ClubSet CV1
								pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset);

								if (pWi != nullptr) {

									// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
									// que no original fica no warehouse msm, eu só confundi quando fiz
									_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

									IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

									if (cs != nullptr)
										for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
											_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

									_session.m_pi.ei.clubset = pWi;
									_session.m_pi.ue.clubset_id = pWi->id;

									// Zera o Error para o cliente equipar a "CV1" que o server equipou
									error = 0;

									// Update ON DB
									snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

									// Update ON GAME
									p.init_plain((unsigned short)0x216);

									p.addUint32((const uint32_t)GetSystemTimeAsUnix());
									p.addUint32(1);	// Count

									p.addUint8(item.type);
									p.addUint32(item._typeid);
									p.addInt32(item.id);
									p.addUint32(item.flag_time);
									p.addBuffer(&item.stat, sizeof(item.stat));
									p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
									p.addZeroByte(25);

									packet_func::session_send(p, &_session, 1);

								}else
									_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
											+ "] nao conseguiu achar o ClubSet\"CV1\"[ID=" 
											+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu adicionar o ClubSet[TYPEID=" 
										+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu inicializar o ClubSet[TYPEID=" 
									+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
					}
				}
		
			}else {

				error = (_cpir.clubset == 0) ? 1/*client give invalid item id*/ : (pWi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				pWi = _session.m_pi.findWarehouseItemByTypeid(AIR_KNIGHT_SET);

				if (pWi != nullptr) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], colocando o ClubSet Padrao\"CV1\" do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
					// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
					// que no original fica no warehouse msm, eu só confundi quando fiz
					_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

					IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

					if (cs != nullptr)
						for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
							_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

					_session.m_pi.ei.clubset = pWi;
					_cpir.clubset = _session.m_pi.ue.clubset_id = pWi->id;

					// Zera o Error para o cliente equipar a "CV1" que o server equipou
					error = 0;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);
			
				}else {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], ele nao tem o ClubSet Padrao\"CV1\", adiciona o ClubSet pardrao\"CV1\" para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = AIR_KNIGHT_SET;
					bi.qntd = 1;
				
					item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0) {

						if ((_cpir.clubset = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

							// Equipa o ClubSet CV1
							pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset);

							if (pWi != nullptr) {

								// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
								// que no original fica no warehouse msm, eu só confundi quando fiz
								_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

								IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

								if (cs != nullptr)
									for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
										_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

								_session.m_pi.ei.clubset = pWi;
								_session.m_pi.ue.clubset_id = pWi->id;

								// Zera o Error para o cliente equipar a "CV1" que o server equipou
								error = 0;

								// Update ON DB
								snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

								// Update ON GAME
								p.init_plain((unsigned short)0x216);

								p.addUint32((const uint32_t)GetSystemTimeAsUnix());
								p.addUint32(1);	// Count

								p.addUint8(item.type);
								p.addUint32(item._typeid);
								p.addInt32(item.id);
								p.addUint32(item.flag_time);
								p.addBuffer(&item.stat, sizeof(item.stat));
								p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
								p.addZeroByte(25);

								packet_func::session_send(p, &_session, 1);

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu achar o ClubSet\"CV1\"[ID=" 
										+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu adicionar o ClubSet[TYPEID=" 
									+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					}else
						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao conseguiu inicializar o ClubSet[TYPEID=" 
								+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			}

			packet_func::pacote04B(p, &_session, _cpir.type, error);
			packet_func::room_broadcast(*this, p, 1);
			break;
		}
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_CHARACTER: // Character
		{
			CharacterInfo *pCe = nullptr;

			if (_cpir.character != 0 && (pCe = _session.m_pi.findCharacterById(_cpir.character)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER) {
				
				_session.m_pi.ei.char_info = pCe;
				_session.m_pi.ue.character_id = _cpir.character;

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCharacterEquiped(_session.m_pi.uid, _cpir.character), room::SQLDBResponse, this);

				// Update Player Info Channel and Room
				updatePlayerInfo(_session);

				PlayerRoomInfoEx *pri = getPlayerInfo(&_session);

				if (getInfo()->tipo != RoomInfo::TIPO::PRACTICE && getInfo()->tipo != RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)
					if (packet_func::pacote048(p, &_session, std::vector< PlayerRoomInfoEx > { (pri == nullptr) ? PlayerRoomInfoEx() : *pri }, 0x103))
						packet_func::room_broadcast(*this, p, 0);

				if (getInfo()->tipo == RoomInfo::TIPO::LOUNGE) {
					
					auto it = (_session.m_pi.ei.char_info == nullptr) ? _session.m_pi.mp_scl.end() : _session.m_pi.mp_scl.find(_session.m_pi.ei.char_info->id);

					if (it == _session.m_pi.mp_scl.end()) {

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao tem os estados do character na lounge. Criando um novo para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						// Add New State Character Lounge
						auto pair = _session.m_pi.mp_scl.insert(std::make_pair(_session.m_pi.ei.char_info->id, StateCharacterLounge{}));

						it = pair.first;
					}

					p.init_plain((unsigned short)0x196);

					p.addUint32(_session.m_oid);

					p.addBuffer(&it->second, sizeof(StateCharacterLounge));

					packet_func::room_broadcast(*this, p, 0);
				}
					
			}else {
				
				error = (_cpir.character == 0) ? 1/*client give invalid item id*/ : (pCe == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				if (_session.m_pi.mp_ce.size() > 0) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o Character[ID=" + std::to_string(_cpir.character) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], colocando o primeiro character do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
					_session.m_pi.ei.char_info = &_session.m_pi.mp_ce.begin()->second;
					_cpir.character = _session.m_pi.ue.character_id = _session.m_pi.ei.char_info->id;

					// Zera o Error para o cliente equipar o Primeiro Character do map de character do player, que o server equipou
					error = 0;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCharacterEquiped(_session.m_pi.uid, _cpir.character), room::SQLDBResponse, this);

					// Update Player Info Channel and Room
					updatePlayerInfo(_session);

					PlayerRoomInfoEx *pri = getPlayerInfo(&_session);

					if (getInfo()->tipo != RoomInfo::TIPO::PRACTICE && getInfo()->tipo != RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)
						if (packet_func::pacote048(p, &_session, std::vector< PlayerRoomInfoEx > { (pri == nullptr) ? PlayerRoomInfoEx() : *pri }, 0x103))
							packet_func::room_broadcast(*this, p, 0);

					if (getInfo()->tipo == RoomInfo::TIPO::LOUNGE) {

						auto it = (_session.m_pi.ei.char_info == nullptr) ? _session.m_pi.mp_scl.end() : _session.m_pi.mp_scl.find(_session.m_pi.ei.char_info->id);

						if (it == _session.m_pi.mp_scl.end()) {

							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] nao tem os estados do character na lounge. Criando um novo para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

							// Add New State Character Lounge
							auto pair = _session.m_pi.mp_scl.insert(std::make_pair(_session.m_pi.ei.char_info->id, StateCharacterLounge{}));

							it = pair.first;
						}

						p.init_plain((unsigned short)0x196);

						p.addUint32(_session.m_oid);

						p.addBuffer(&it->second, sizeof(StateCharacterLounge));

						packet_func::room_broadcast(*this, p, 0);
					}
			
				}else {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o Character[ID=" + std::to_string(_cpir.character) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], ele nao tem nenhum character, adiciona o Nuri para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = iff::CHARACTER << 26;	// Nuri
					bi.qntd = 1;
				
					item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0) {

						// Add Item já atualiza o Character equipado
						if ((_cpir.character = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

							// Zera o Error para o cliente equipar o Nuri que o server equipou
							error = 0;

							// Update ON GAME
							p.init_plain((unsigned short)0x216);

							p.addUint32((const uint32_t)GetSystemTimeAsUnix());
							p.addUint32(1);	// Count

							p.addUint8(item.type);
							p.addUint32(item._typeid);
							p.addInt32(item.id);
							p.addUint32(item.flag_time);
							p.addBuffer(&item.stat, sizeof(item.stat));
							p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
							p.addZeroByte(25);

							packet_func::session_send(p, &_session, 1);

							// Update Player Info Channel and Room
							updatePlayerInfo(_session);

							PlayerRoomInfoEx *pri = getPlayerInfo(&_session);

							if (getInfo()->tipo != RoomInfo::TIPO::PRACTICE && getInfo()->tipo != RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)
								if (packet_func::pacote048(p, &_session, std::vector< PlayerRoomInfoEx > { (pri == nullptr) ? PlayerRoomInfoEx() : *pri }, 0x103))
									packet_func::room_broadcast(*this, p, 0);

							if (getInfo()->tipo == RoomInfo::TIPO::LOUNGE) {

								auto it = (_session.m_pi.ei.char_info == nullptr) ? _session.m_pi.mp_scl.end() : _session.m_pi.mp_scl.find(_session.m_pi.ei.char_info->id);

								if (it == _session.m_pi.mp_scl.end()) {

									_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid)
										+ "] nao tem os estados do character na lounge. Criando um novo para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

									// Add New State Character Lounge
									auto pair = _session.m_pi.mp_scl.insert(std::make_pair(_session.m_pi.ei.char_info->id, StateCharacterLounge{}));

									it = pair.first;
								}

								p.init_plain((unsigned short)0x196);

								p.addUint32(_session.m_oid);

								p.addBuffer(&it->second, sizeof(StateCharacterLounge));

								packet_func::room_broadcast(*this, p, 0);
							}

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu adicionar o Character[TYPEID=" 
									+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					}else
						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao conseguiu inicializar o Character[TYPEID=" 
								+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			}

			packet_func::pacote04B(p, &_session, _cpir.type, error);
			packet_func::room_broadcast(*this, p, 1);
			break;
		}
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_MASCOT: // Mascot
		{
			MascotInfoEx *pMi = nullptr;

			if (_cpir.mascot != 0) {
				
				if ((pMi = _session.m_pi.findMascotById(_cpir.mascot)) != nullptr && sIff::getInstance().getItemGroupIdentify(pMi->_typeid) == iff::MASCOT) {

					auto m_it = _session.m_pi.findUpdateItemByIdAndType(_session.m_pi.ue.mascot_id, UpdateItem::MASCOT);

					if (m_it != _session.m_pi.mp_ui.end()) {

						// Desequipa o Mascot que acabou o tempo dele
						_session.m_pi.ei.mascot_info = nullptr;
						_session.m_pi.ue.mascot_id = 0;

						_cpir.mascot = 0;

					}else {

						// Mascot is good, update on server, DB and game
						_session.m_pi.ei.mascot_info = pMi;
						_session.m_pi.ue.mascot_id = _cpir.mascot;

						// Verifica se o mascot pode equipar
						if (_session.checkMascotEquiped(_session.m_pi.ue))
							_cpir.mascot = _session.m_pi.ue.mascot_id;

					}

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateMascotEquiped(_session.m_pi.uid, _cpir.mascot), room::SQLDBResponse, this);
					
				}else {

					error = (_cpir.mascot == 0) ? 1/*client give invalid item id*/ : (pMi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

					if (error > 1) {
						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] tentou trocar o Mascot[ID=" + std::to_string(_cpir.mascot) + "] para comecar o jogo, mas deu Error[VALUE="
								+ std::to_string(error) + "], desequipando o Mascot. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
					}

					_session.m_pi.ei.mascot_info = nullptr;
					_session.m_pi.ue.mascot_id = 0;

					_cpir.mascot = 0;

					// Att No DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateMascotEquiped(_session.m_pi.uid, _cpir.mascot), room::SQLDBResponse, this);
				}

			}else if (_session.m_pi.ue.mascot_id > 0 && _session.m_pi.ei.mascot_info != nullptr) {	// Desequipa Mascot

				_session.m_pi.ei.mascot_info = nullptr;
				_session.m_pi.ue.mascot_id = 0;

				_cpir.mascot = 0;

				// Att No DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateMascotEquiped(_session.m_pi.uid, _cpir.mascot), room::SQLDBResponse, this);

			} // else Não tem nenhum mascot equipado, para desequipar, então o cliente só quis atualizar o estado
			

			packet_func::pacote04B(p, &_session, _cpir.type, error);
			packet_func::room_broadcast(*this, p, 1);
			break;
		}
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_ITEM_EFFECT_LOUNGE: // Itens Active, Jester x2 velocidade no lounge, e Harmes tamanho da cabeça
		{
			// ignora o item_id por que ele envia 0

			// Valor 1 Cabeca
			// Valor 2 Velocidade
			// Valor 3 Twilight

			if (!sIff::getInstance().isLoad())
				sIff::getInstance().load();

			if (_session.m_pi.ei.char_info == nullptr)
				throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] nao tem nenhum character equipado. Hacker ou Bug.", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1000, 0x57007/*player não tem nenhum character equipado*/));

			if (_cpir.effect_lounge.effect != ChangePlayerItemRoom::stItemEffectLounge::TYPE_EFFECT::TE_TWILIGHT) {

				auto it = (_session.m_pi.ei.char_info == nullptr) ? _session.m_pi.mp_scl.end() : _session.m_pi.mp_scl.find(_session.m_pi.ei.char_info->id);

				if (it == _session.m_pi.mp_scl.end()) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] nao tem os estados do character na lounge. Criando um novo para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					// Add New State Character Lounge
					auto pair = _session.m_pi.mp_scl.insert(std::make_pair(_session.m_pi.ei.char_info->id, StateCharacterLounge{}));

					it = pair.first;
				}

				switch (_cpir.effect_lounge.effect) {
				case ChangePlayerItemRoom::stItemEffectLounge::TYPE_EFFECT::TE_BIG_HEAD: // Jester (Big head)
				{

					auto ccj = std::find_if(cadie_cauldron_Jester_item_typeid, LAST_ELEMENT_IN_ARRAY(cadie_cauldron_Jester_item_typeid), [&](auto& el) {
						return sIff::getInstance().getItemCharIdentify(el) == (_session.m_pi.ei.char_info->_typeid & 0x000000FF);
					});

					if (ccj == LAST_ELEMENT_IN_ARRAY(cadie_cauldron_Jester_item_typeid))
						throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] o Character[TYPEID=" + std::to_string(_session.m_pi.ei.char_info->_typeid) 
								+"] equipado nao tem o item Jester no server. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1001, 0x57008));

					if (!_session.m_pi.ei.char_info->isPartEquiped(*ccj))
						throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] o Character[TYPEID=" + std::to_string(_session.m_pi.ei.char_info->_typeid) 
								+ "] nao esta com o item[TYPEID=" + std::to_string(*ccj) + "] Jester equipado. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1002, 0x57009));

					it->second.scale_head = (it->second.scale_head > 1.f) ? 1.f : 2.f;

					break;
				}
				case ChangePlayerItemRoom::stItemEffectLounge::TYPE_EFFECT::TE_FAST_WALK:	// Hermes (Velocidade x2)
				{

					auto cch = std::find_if(cadie_cauldron_Hermes_item_typeid, LAST_ELEMENT_IN_ARRAY(cadie_cauldron_Hermes_item_typeid), [&](auto& el) {
						return sIff::getInstance().getItemCharIdentify(el) == (_session.m_pi.ei.char_info->_typeid & 0x000000FF);
					});

					if (cch == LAST_ELEMENT_IN_ARRAY(cadie_cauldron_Hermes_item_typeid))
						throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] o Character[TYPEID=" + std::to_string(_session.m_pi.ei.char_info->_typeid) 
								+ "] equipado nao tem o item Hermes no server. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1001, 0x57008));

					if (!_session.m_pi.ei.char_info->isPartEquiped(*cch))
						throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] o Character[TYPEID=" + std::to_string(_session.m_pi.ei.char_info->_typeid) 
								+ "] nao esta com o item[TYPEID=" + std::to_string(*cch) + "] Hermes equipado. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1002, 0x57009));

					it->second.walk_speed = (it->second.walk_speed > 1.f) ? 1.f : 2.f;

					break;
				}
				} // End Switch

			}else {
				// else == 3 // Twilight (Fogos de artifícios em cima da cabeça do player)
				// Valor 1 pass para fazer o fogos

				auto cct = std::find_if(cadie_cauldron_Twilight_item_typeid, LAST_ELEMENT_IN_ARRAY(cadie_cauldron_Twilight_item_typeid), [&](auto& el) {
					return sIff::getInstance().getItemCharIdentify(el) == (_session.m_pi.ei.char_info->_typeid & 0x000000FF);
				});

				if (cct == LAST_ELEMENT_IN_ARRAY(cadie_cauldron_Twilight_item_typeid))
					throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] o Character[TYPEID=" + std::to_string(_session.m_pi.ei.char_info->_typeid) 
							+ "] equipado nao tem o item Twilight no server. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1001, 0x57008));

				if (!_session.m_pi.ei.char_info->isPartEquiped(*cct))
					throw exception("[room::requestChangePlayerItemRoom][Error] Player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] o Character[TYPEID=" + std::to_string(_session.m_pi.ei.char_info->_typeid) 
							+ "] nao esta com o item[TYPEID=" + std::to_string(*cct) + "] Twilight equipado. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 1002, 0x57009));
			}

			packet_func::pacote04B(p, &_session, _cpir.type, error, _cpir.effect_lounge.effect);
			packet_func::room_broadcast(*this, p, 1);
			break;
		}
		case ChangePlayerItemRoom::TYPE_CHANGE::TC_ALL: // Todos
		{
			// Aqui se não tiver os itens, algum hacker, gera Log, e coloca item padrão ou nenhum
			CharacterInfo *pCe = nullptr;
			CaddieInfoEx *pCi = nullptr;
			WarehouseItemEx *pWi = nullptr;
			uint32_t error = 0;

			// Character
			if (_cpir.character != 0 && (pCe = _session.m_pi.findCharacterById(_cpir.character)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pCe->_typeid) == iff::CHARACTER) {
				
				_session.m_pi.ei.char_info = pCe;
				_session.m_pi.ue.character_id = _cpir.character;

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCharacterEquiped(_session.m_pi.uid, _cpir.character), room::SQLDBResponse, this);

			}else {

				error = (_cpir.character == 0) ? 1/*client give invalid item id*/ : (pCe == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				if (_session.m_pi.mp_ce.size() > 0) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o Character[ID=" + std::to_string(_cpir.character) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], colocando o primeiro character do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
					_session.m_pi.ei.char_info = &_session.m_pi.mp_ce.begin()->second;
					_cpir.character = _session.m_pi.ue.character_id = _session.m_pi.ei.char_info->id;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCharacterEquiped(_session.m_pi.uid, _cpir.character), room::SQLDBResponse, this);
			
				}else {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o Character[ID=" + std::to_string(_cpir.character) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], ele nao tem nenhum character, adiciona o Nuri para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = iff::CHARACTER << 26;	// Nuri
					bi.qntd = 1;
				
					item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0) {

						// Add Item já atualiza o Character equipado
						if ((_cpir.character = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

							// Update ON GAME
							p.init_plain((unsigned short)0x216);

							p.addUint32((const uint32_t)GetSystemTimeAsUnix());
							p.addUint32(1);	// Count

							p.addUint8(item.type);
							p.addUint32(item._typeid);
							p.addInt32(item.id);
							p.addUint32(item.flag_time);
							p.addBuffer(&item.stat, sizeof(item.stat));
							p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
							p.addZeroByte(25);

							packet_func::session_send(p, &_session, 1);

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu adicionar o Character[TYPEID=" 
									+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					}else
						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao conseguiu inicializar o Character[TYPEID=" 
								+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			}

			// Caddie
			if (_cpir.caddie != 0 && (pCi = _session.m_pi.findCaddieById(_cpir.caddie)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pCi->_typeid) == iff::CADDIE) {
					
				// Check if item is in map of update item
				auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(pCi->_typeid, pCi->id);
					
				if (!v_it.empty()) {

					for (auto& el : v_it) {

						if (el->second.type == UpdateItem::CADDIE) {
								
							// Desequipa o caddie
							_session.m_pi.ei.cad_info = nullptr;
							_session.m_pi.ue.caddie_id = 0;

							_cpir.caddie = 0;

						}else if (el->second.type == UpdateItem::CADDIE_PARTS) {

							// Limpa o caddie Parts
							pCi->parts_typeid = 0u;
							pCi->parts_end_date_unix = 0;
							pCi->end_parts_date = { 0 };

							_session.m_pi.ei.cad_info = pCi;
							_session.m_pi.ue.caddie_id = _cpir.caddie;
						}

						// Tira esse Update Item do map
						_session.m_pi.mp_ui.erase(el);
					}

				}else {

					// Caddie is Good, Update caddie equiped ON SERVER AND DB
					_session.m_pi.ei.cad_info = pCi;
					_session.m_pi.ue.caddie_id = _cpir.caddie;

					// Verifica se o Caddie pode equipar
					if (_session.checkCaddieEquiped(_session.m_pi.ue))
						_cpir.caddie = _session.m_pi.ue.caddie_id;

				}

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCaddieEquiped(_session.m_pi.uid, _cpir.caddie), room::SQLDBResponse, this);

			}else if (_session.m_pi.ue.caddie_id > 0 && _session.m_pi.ei.cad_info != nullptr) {	// Desequipa Caddie
			
				error = (_cpir.caddie == 0) ? 1/*client give invalid item id*/ : (pCi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				if (error > 1) {
					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o Caddie[ID=" + std::to_string(_cpir.caddie) 
							+ "] para comecar o jogo, mas deu Error[VALUE=" + std::to_string(error) + "], desequipando o caddie. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				}

				// Check if item is in map of update item
				auto v_it = _session.m_pi.findUpdateItemByTypeidAndId(_session.m_pi.ei.cad_info->_typeid, _session.m_pi.ei.cad_info->id);
					
				if (!v_it.empty()) {

					for (auto& el : v_it) {

						// Caddie já vai se desequipar, só verifica o parts
						if (el->second.type == UpdateItem::CADDIE_PARTS) {

							// Limpa o caddie Parts
							_session.m_pi.ei.cad_info->parts_typeid = 0u;
							_session.m_pi.ei.cad_info->parts_end_date_unix = 0;
							_session.m_pi.ei.cad_info->end_parts_date = { 0 };
						}

						// Tira esse Update Item do map
						_session.m_pi.mp_ui.erase(el);
					}

				}

				_session.m_pi.ei.cad_info = nullptr;
				_session.m_pi.ue.caddie_id = 0;

				_cpir.caddie = 0;

				// Att No DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateCaddieEquiped(_session.m_pi.uid, _cpir.caddie), room::SQLDBResponse, this);
			}

			// ClubSet
			if (_cpir.clubset != 0 && (pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::CLUBSET) {

				auto c_it = _session.m_pi.findUpdateItemByIdAndType(_cpir.clubset, UpdateItem::WAREHOUSE);

				if (c_it == _session.m_pi.mp_ui.end()) {

					_session.m_pi.ei.clubset = pWi;

					// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
					// que no original fica no warehouse msm, eu só confundi quando fiz
					_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

					IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

					if (cs != nullptr) {

						for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
							_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

						_session.m_pi.ue.clubset_id = _cpir.clubset;

						// Verifica se o ClubSet pode equipar
						if (_session.checkClubSetEquiped(_session.m_pi.ue))
							_cpir.clubset = _session.m_pi.ue.clubset_id;

						// Update ON DB
						snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

					}else {
						
						error = 5/*Item Not Found ON IFF_STRUCT SERVER*/;

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou Atualizar Clubset[TYPEID="
								+ std::to_string(pWi->_typeid) + ", ID=" + std::to_string(pWi->id) 
								+ "] equipado, mas ClubSet Not exists on IFF structure. Equipa o ClubSet padrao. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

						// Coloca o ClubSet CV1 no lugar do ClubSet que acabou o tempo
						pWi = _session.m_pi.findWarehouseItemByTypeid(AIR_KNIGHT_SET);

						if (pWi != nullptr) {

							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
									+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
									+ std::to_string(_cpir.clubset) + "], colocando o ClubSet Padrao\"CV1\" do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
							
							// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
							// que no original fica no warehouse msm, eu só confundi quando fiz
							_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

							IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

							if (cs != nullptr)
								for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
									_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

							_session.m_pi.ei.clubset = pWi;
							_cpir.clubset = _session.m_pi.ue.clubset_id = pWi->id;

							// Update ON DB
							snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);
			
						}else {

							_smp::message_pool::getInstance().push(new message("[channel::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
									+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
									+ std::to_string(_cpir.clubset) + "], ele nao tem o ClubSet Padrao\"CV1\", adiciona o ClubSet pardrao\"CV1\" para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

							BuyItem bi{ 0 };
							stItem item{ 0 };

							bi.id = -1;
							bi._typeid = AIR_KNIGHT_SET;
							bi.qntd = 1;
				
							item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

							if (item._typeid != 0) {

								if ((_cpir.clubset = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

									// Equipa o ClubSet CV1
									pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset);

									if (pWi != nullptr) {

										// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
										// que no original fica no warehouse msm, eu só confundi quando fiz
										_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

										IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

										if (cs != nullptr)
											for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
												_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

										_session.m_pi.ei.clubset = pWi;
										_session.m_pi.ue.clubset_id = pWi->id;

										// Update ON DB
										snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

										// Update ON GAME
										p.init_plain((unsigned short)0x216);

										p.addUint32((const uint32_t)GetSystemTimeAsUnix());
										p.addUint32(1);	// Count

										p.addUint8(item.type);
										p.addUint32(item._typeid);
										p.addInt32(item.id);
										p.addUint32(item.flag_time);
										p.addBuffer(&item.stat, sizeof(item.stat));
										p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
										p.addZeroByte(25);

										packet_func::session_send(p, &_session, 1);

									}else
										_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
												+ "] nao conseguiu achar o ClubSet\"CV1\"[ID=" 
												+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

								}else
									_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
											+ "] nao conseguiu adicionar o ClubSet[TYPEID=" 
											+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu inicializar o ClubSet[TYPEID=" 
										+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
						}
					}
			
				}else {	// ClubSet Acabou o tempo

					// Coloca o ClubSet CV1 no lugar do ClubSet que acabou o tempo
					pWi = _session.m_pi.findWarehouseItemByTypeid(AIR_KNIGHT_SET);

					if (pWi != nullptr) {

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
								+ std::to_string(_cpir.clubset) + "], colocando o ClubSet Padrao\"CV1\" do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
						// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
						// que no original fica no warehouse msm, eu só confundi quando fiz
						_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

						IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

						if (cs != nullptr)
							for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
								_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

						_session.m_pi.ei.clubset = pWi;
						_cpir.clubset = _session.m_pi.ue.clubset_id = pWi->id;

						// Update ON DB
						snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);
			
					}else {

						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas acabou o tempo do ClubSet[ID="
								+ std::to_string(_cpir.clubset) + "], ele nao tem o ClubSet Padrao\"CV1\", adiciona o ClubSet pardrao\"CV1\" para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

						BuyItem bi{ 0 };
						stItem item{ 0 };

						bi.id = -1;
						bi._typeid = AIR_KNIGHT_SET;
						bi.qntd = 1;
				
						item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

						if (item._typeid != 0) {

							if ((_cpir.clubset = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

								// Equipa o ClubSet CV1
								pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset);

								if (pWi != nullptr) {

									// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
									// que no original fica no warehouse msm, eu só confundi quando fiz
									_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

									IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

									if (cs != nullptr)
										for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
											_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

									_session.m_pi.ei.clubset = pWi;
									_session.m_pi.ue.clubset_id = pWi->id;

									// Update ON DB
									snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

									// Update ON GAME
									p.init_plain((unsigned short)0x216);

									p.addUint32((const uint32_t)GetSystemTimeAsUnix());
									p.addUint32(1);	// Count

									p.addUint8(item.type);
									p.addUint32(item._typeid);
									p.addInt32(item.id);
									p.addUint32(item.flag_time);
									p.addBuffer(&item.stat, sizeof(item.stat));
									p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
									p.addZeroByte(25);

									packet_func::session_send(p, &_session, 1);

								}else
									_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
											+ "] nao conseguiu achar o ClubSet\"CV1\"[ID=" 
											+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu adicionar o ClubSet[TYPEID=" 
										+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu inicializar o ClubSet[TYPEID=" 
									+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
					}
				}
		
			}else {

				error = (_cpir.clubset == 0) ? 1/*client give invalid item id*/ : (pWi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				pWi = _session.m_pi.findWarehouseItemByTypeid(AIR_KNIGHT_SET);

				if (pWi != nullptr) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], colocando o ClubSet Padrao\"CV1\" do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
					// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
					// que no original fica no warehouse msm, eu só confundi quando fiz
					_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

					IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

					if (cs != nullptr)
						for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
							_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

					_session.m_pi.ei.clubset = pWi;
					_cpir.clubset = _session.m_pi.ue.clubset_id = pWi->id;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);
			
				}else {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar o ClubSet[ID=" + std::to_string(_cpir.clubset) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], ele nao tem o ClubSet Padrao\"CV1\", adiciona o ClubSet pardrao\"CV1\" para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = AIR_KNIGHT_SET;
					bi.qntd = 1;
				
					item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0) {

						if ((_cpir.clubset = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

							// Equipa o ClubSet CV1
							pWi = _session.m_pi.findWarehouseItemById(_cpir.clubset);

							if (pWi != nullptr) {

								// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
								// que no original fica no warehouse msm, eu só confundi quando fiz
								_session.m_pi.ei.csi = { pWi->id, pWi->_typeid, pWi->c };

								IFF::ClubSet *cs = sIff::getInstance().findClubSet(pWi->_typeid);

								if (cs != nullptr)
									for (auto j = 0u; j < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(short)); ++j)
										_session.m_pi.ei.csi.enchant_c[j] = cs->slot[j] + pWi->clubset_workshop.c[j];

								_session.m_pi.ei.clubset = pWi;
								_session.m_pi.ue.clubset_id = pWi->id;

								// Update ON DB
								snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateClubsetEquiped(_session.m_pi.uid, _cpir.clubset), room::SQLDBResponse, this);

								// Update ON GAME
								p.init_plain((unsigned short)0x216);

								p.addUint32((const uint32_t)GetSystemTimeAsUnix());
								p.addUint32(1);	// Count

								p.addUint8(item.type);
								p.addUint32(item._typeid);
								p.addInt32(item.id);
								p.addUint32(item.flag_time);
								p.addBuffer(&item.stat, sizeof(item.stat));
								p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
								p.addZeroByte(25);

								packet_func::session_send(p, &_session, 1);

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu achar o ClubSet\"CV1\"[ID=" 
										+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu adicionar o ClubSet[TYPEID=" 
									+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					}else
						_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao conseguiu inicializar o ClubSet[TYPEID=" 
								+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			}

			// Ball(Comet)
			if (_cpir.ball != 0 && (pWi = _session.m_pi.findWarehouseItemByTypeid(_cpir.ball)) != nullptr 
					&& sIff::getInstance().getItemGroupIdentify(pWi->_typeid) == iff::BALL) {
			
				_session.m_pi.ei.comet = pWi;
				_session.m_pi.ue.ball_typeid = _cpir.ball;		// Ball(Comet) é o typeid que o cliente passa

				// Verifica se a Bola pode ser equipada
				if (_session.checkBallEquiped(_session.m_pi.ue))
					_cpir.ball = _session.m_pi.ue.ball_typeid;

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);

			}else if (_cpir.ball == 0) { // Bola 0 coloca a bola padrão para ele, se for premium user coloca a bola de premium user

				// Zera para equipar a bola padrão
				_session.m_pi.ei.comet = nullptr;
				_session.m_pi.ue.ball_typeid = 0l;

				// Verifica se a Bola pode ser equipada (Coloca para equipar a bola padrão
				if (_session.checkBallEquiped(_session.m_pi.ue))
					_cpir.ball = _session.m_pi.ue.ball_typeid;

				// Update ON DB
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);

			}else {

				error = (pWi == nullptr ? 2/*Item Not Found*/ : 3/*Erro item typeid invalid*/);

				pWi = _session.m_pi.findWarehouseItemByTypeid(DEFAULT_COMET_TYPEID);

				if (pWi != nullptr) {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar a Ball[TYPEID=" + std::to_string(_cpir.ball) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], colocando a Ball Padrao do player. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
				
					_session.m_pi.ei.comet = pWi;
					_cpir.ball = _session.m_pi.ue.ball_typeid = pWi->_typeid;

					// Update ON DB
					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);
			
				}else {

					_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] tentou trocar a Ball[TYPEID=" + std::to_string(_cpir.ball) + "] para comecar o jogo, mas deu Error[VALUE="
							+ std::to_string(error) + "], ele nao tem a Ball Padrao, adiciona a Ball pardrao para ele. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = DEFAULT_COMET_TYPEID;
					bi.qntd = 1;
				
					item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0) {

						if ((_cpir.ball = item_manager::addItem(item, _session, 2/*Padrão Item*/, 0)) != item_manager::RetAddItem::T_ERROR) {

							// Equipa a Ball padrao
							pWi = _session.m_pi.findWarehouseItemById(_cpir.ball);

							if (pWi != nullptr) {

								_session.m_pi.ei.comet = pWi;
								_session.m_pi.ue.ball_typeid = pWi->_typeid;

								// Update ON DB
								snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateBallEquiped(_session.m_pi.uid, _cpir.ball), room::SQLDBResponse, this);

								// Update ON GAME
								p.init_plain((unsigned short)0x216);

								p.addUint32((const uint32_t)GetSystemTimeAsUnix());
								p.addUint32(1);	// Count

								p.addUint8(item.type);
								p.addUint32(item._typeid);
								p.addInt32(item.id);
								p.addUint32(item.flag_time);
								p.addBuffer(&item.stat, sizeof(item.stat));
								p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
								p.addZeroByte(25);

								packet_func::session_send(p, &_session, 1);

							}else
								_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
										+ "] nao conseguiu achar a Ball[ID=" 
										+ std::to_string(item.id) + "] que acabou de adicionar para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

						}else
							_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
									+ "] nao conseguiu adicionar a Ball[TYPEID=" 
									+ std::to_string(item._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));

					}else
						_smp::message_pool::getInstance().push(new message("[channel::requestChangePlayerItemRoom][Log][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) 
								+ "] nao conseguiu inicializar a Ball[TYPEID=" 
								+ std::to_string(bi._typeid) + "] para ele. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			}

			// Verifica se o Mascot Equipado acabou o tempo
			if (_session.m_pi.ue.mascot_id != 0 && _session.m_pi.ei.mascot_info != nullptr) {

				auto m_it = _session.m_pi.findUpdateItemByIdAndType(_session.m_pi.ue.mascot_id, UpdateItem::MASCOT);

				if (m_it != _session.m_pi.mp_ui.end()) {

					// Desequipa o Mascot que acabou o tempo dele
					_session.m_pi.ei.mascot_info = nullptr;
					_session.m_pi.ue.mascot_id = 0;

					snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateMascotEquiped(_session.m_pi.uid, 0/*Mascot_id == 0 not equiped*/), room::SQLDBResponse, this);

					// Update on GAME se não o cliente continua com o mascot equipado
					packet_func::pacote04B(p, &_session, ChangePlayerItemRoom::TYPE_CHANGE::TC_MASCOT, 0);
					packet_func::session_send(p, &_session, 0);

				}
			}

			// Começa jogo
			startGame(_session);

			break;
		}
		default:
			throw exception("[room::requestChangePlayerItemRoom][Error] sala[NUMERO=" + std::to_string(getNumero()) + "] type desconhecido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 13, 1));
		}

		updatePlayerInfo(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangePlayerItemRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		packet_func::pacote04B(p, &_session, _cpir.type, (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error*/));
		packet_func::session_send(p, &_session, 0);
	}
};

void room::requestOpenEditSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("OpenEditSaleShop");

	packet p;

	try {

		if (m_personal_shop.openShopToEdit(_session, p))
			packet_func::room_broadcast(*this, p, 1);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestOpenEditSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestCloseSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("CloseSaleShop");

	packet p;

	try {

		if (m_personal_shop.closeShop(_session, p))
			packet_func::room_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestCloseSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestChangeNameSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeNameSaleShop");

	packet p;

	try {

		if (m_personal_shop.changeShopName(_session, _packet->readString(), p))
			packet_func::room_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeNameSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestOpenSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("OpenSaleShop");

	try {

		m_personal_shop.openShop(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestOpenSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestVisitCountSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("VisitCountSaleShop");

	try {

		m_personal_shop.visitCountShop(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestVisitCount][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestPangSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("PangSaleShop");

	try {

		m_personal_shop.pangShop(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestPangSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestCancelEditSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("CancelEditSaleShop");

	packet p;

	try {

		if (m_personal_shop.cancelEditShop(_session, p))
			packet_func::room_broadcast(*this, p, 1);

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[room::reuqestCancelEditSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestViewSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("ViewSaleShop");

	try {

		m_personal_shop.viewShop(_session, _packet->readUint32());

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestViewSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestCloseViewSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("CloseViewSaleShop");

	try {

		m_personal_shop.closeViewShop(_session, _packet->readUint32());

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestCloseViewSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestBuyItemSaleShop(player& _session, packet *_packet) {
	REQUEST_BEGIN("BuyItemSaleShop");

	try {

		m_personal_shop.buyInShop(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestBuyItemSaleShop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestToggleAssist(player& _session, packet *_packet) {
	REQUEST_BEGIN("ToggleAssist");

	packet p;

	try {

		stItem item{ 0 };

		auto rt = item_manager::RetAddItem::T_INIT_VALUE;

		item.type = 2;
		item.id = -1;
		item._typeid = ASSIST_ITEM_TYPEID;

		auto pWi = _session.m_pi.findWarehouseItemByTypeid(ASSIST_ITEM_TYPEID);

		if (pWi == nullptr) {	// Não tem ativa o Assist
			item.qntd = 1;
			item.STDA_C_ITEM_QNTD = 1;

			if ((rt = item_manager::addItem(item, _session, 0, 0)) < 0)
				throw exception("[room::requestToggleAssist][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar o Assist[TYPEID=" 
						+ std::to_string(ASSIST_ITEM_TYPEID) + "], mas nao conseguiu adicionar o item. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200801));

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[room::requestToggleAssist][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] Ligou o Assist Modo", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[room::requestToggleAssist][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] Ligou o Assist Modo", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		}else {	// Já tem, Desativa
			
			item.id = pWi->id;
			item.qntd = (pWi->STDA_C_ITEM_QNTD <= 0) ? 1 : pWi->STDA_C_ITEM_QNTD;
			item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

			if (item_manager::removeItem(item, _session) <= 0)
				throw exception("[room::requestToggleAssist][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desativar o Assist[TYPEID="
						+ std::to_string(ASSIST_ITEM_TYPEID) + "], mas nao conseguiu remover o item. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0x5200802));

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[room::requestToggleAssist][Log] player[UID=" + std::to_string(_session.m_pi.uid) +"] Desligou o Assist Modo", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[room::requestToggleAssist][Log] player[UID=" + std::to_string(_session.m_pi.uid) +"] Desligou o Assist Modo", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		}

		// UPDATE ON GAME
		if (rt != item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH) {
			
			p.init_plain((unsigned short)0x216);

			p.addUint32((const uint32_t)GetSystemTimeAsUnix());
			p.addUint32(1);	// Count

			p.addUint8(item.type);
			p.addUint32(item._typeid);
			p.addInt32(item.id);
			p.addUint32(item.flag_time);
			p.addBuffer(&item.stat, sizeof(item.stat));
			p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
			p.addZeroByte(25);

			packet_func::session_send(p, &_session, 1);
		}

		// Resposta ao Toggle Assist
		p.init_plain((unsigned short)0x26A);

		p.addUint32(0);	// OK

		packet_func::session_send(p, &_session, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestToggleAssist][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		p.init_plain((unsigned short)0x26A);

		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5200800);

		packet_func::session_send(p, &_session, 1);
	}
};

void room::requestChangeTeam(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeTeam");

	packet p;

	try {

		unsigned char team = _packet->readUint8();

		PlayerRoomInfoEx *pPri = getPlayerInfo(&_session);

		if (pPri == nullptr)
			throw exception("[room::requestChangeTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar o team(time) na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem o info do player. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1505, 0));

		if (m_teans.size() < 2)
			throw exception("[room::requestChangeTeam][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar o team(time) na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem teans(times) suficiente. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1506, 0));

		// Sai do outro team(time) se ele estiver
		try {

			m_teans[pPri->state_flag.uFlag.stFlagBit.team].deletePlayer(&_session, 3/*force*/);

		}catch (exception& e) {

			_smp::message_pool::getInstance().push(new message("[room::requestChangeTeam][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}

		// Add o Player ao (team)time
		m_teans[team].addPlayer(&_session);

		pPri->state_flag.uFlag.stFlagBit.team = team; // ~pri->state_flag.uFlag.stFlagBit.team;

		packet p((unsigned short)0x7D);

		p.addUint32(_session.m_oid);

		p.addUint8(team);

		packet_func::room_broadcast(*this, p, 0);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeTeam][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool room::requestStartGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("StartGame");

	packet p;

	bool ret = true;

	try {

		if (m_ri.master != _session.m_pi.uid)
			throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o jogo na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas ele nao eh o master da sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5900201));

		// Verifica se já tem um jogo inicializado e lança error se tiver, para o cliente receber uma resposta
		if (m_pGame != nullptr)
			throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid)
					+ "] tentou comecar o jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + "], mas ja tem um jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 8, 0x5900202));

		// Verifica se todos estão prontos se não da erro
		if (!isAllReady())
			throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou comecar o jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) 
					+ "], mas nem todos jogadores estao prontos. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 8, 0x5900202));

		// Coloquei para verificar se a flag de Bot tourney não está ativo verifica o resto das condições
		if (!m_bot_tourney && v_sessions.size() == 1 && m_ri.tipo != RoomInfo::PRACTICE && m_ri.tipo != RoomInfo::GRAND_PRIX
			&& m_ri.tipo != RoomInfo::GRAND_ZODIAC_INT && m_ri.tipo != RoomInfo::GRAND_ZODIAC_ADV && m_ri.tipo != RoomInfo::GRAND_ZODIAC_PRACTICE)
			throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas nao tem quantidade de jogadores suficiente para da comecar. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5900202));

		// Match
		if (m_ri.tipo == RoomInfo::TIPO::MATCH) {

			if (m_teans.empty())
				throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Match na sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + "], mas o vector do teans esta vazio. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0x5900202));

			if (m_teans.size() == 1)
				throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Match na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas o vector do teans só tem um team. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 3, 0x5900202));

			if (v_sessions.size() % 2 == 1)
				throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Match na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas o numero de jogadores na sala eh impar. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 4, 0x5900202));

			if (v_sessions.size() == 2 && (m_teans[0].getNumPlayers() == 0 || m_teans[1].getNumPlayers() == 0))
				throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Match na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas um team nao tem jogador. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 5, 0x5900202));

			if (v_sessions.size() == 4 && (m_teans[0].getNumPlayers() < 2 || m_teans[1].getNumPlayers() < 2))
				throw exception("[room::requestStartGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Match na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas um team nao tem jogador suficiente. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 6, 0x5900202));

			if (m_ri.max_player == 4 && v_sessions.size() < 4)
				throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Match na sala[NUMERO=" 
						+ std::to_string(m_ri.numero) + "], mas o max player sala eh 4, mas nao tem os 4 jogadores na sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 7, 0x5900202));
		}

		// Guild Battle
		if (m_ri.tipo == RoomInfo::TIPO::GUILD_BATTLE) {

			if (v_sessions.size() % 2 == 1)
				throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Guild Battle na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas o numero de jogadores na sala eh impar. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 9, 0x5900202));

			auto error_check = m_guild_manager.isGoodToStart();

			if (error_check <= 0) {
				
				switch (error_check) {
					case 0:		// Não tem duas guilds na sala
						throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Guild Battle na sala[NUMERO="
								+ std::to_string(m_ri.numero) + "], mas nao tem guilds suficientes para comecar o jogo. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 10, 0x5900202));
						break;
					case -1:	// Não tem o mesmo número de jogadores na sala as duas guilds
						throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Guild Battle na sala[NUMERO="
								+ std::to_string(m_ri.numero) + "], mas as duas guilds nao tem o mesmo numero de jogadores na sala. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 11, 0x5900202));
						break;
					case -2:	// Uma das Guilds ou as duas não tem 2 jogadores
						throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Guild Battle na sala[NUMERO="
								+ std::to_string(m_ri.numero) + "], mas uma ou as duas guilds tem menos que 2 jogadores na sala. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 12, 0x5900202));
						break;
				}
			}
		}

		// Chip-in Practice
		if (m_ri.tipo == RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE) {

			auto pTicket = _session.m_pi.findWarehouseItemByTypeid(CHIP_IN_PRACTICE_TICKET_TYPEID);

			if (pTicket == nullptr)
				throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Chip-in Practice na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas ele nao tem ticket[TYPEID=" + std::to_string(CHIP_IN_PRACTICE_TICKET_TYPEID) 
						+ "] do Chip-in Practice para comecar o jogo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 50, 500017));

			if (pTicket->STDA_C_ITEM_QNTD < 1)
				throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Chip-in Practice na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas ele nao tem ticket[TYPEID=" + std::to_string(CHIP_IN_PRACTICE_TICKET_TYPEID) 
						+ ", ID=" + std::to_string(pTicket->id) + ", QNTD=" + std::to_string(pTicket->STDA_C_ITEM_QNTD)
						+ "] do Chip-in Practice suficiente para comecar o jogo. Ticket necessario \"1\".", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 51, 500017));

			stItem item{ 0u };

			item.type = 2;
			item.id = pTicket->id;
			item._typeid = pTicket->_typeid;
			item.qntd = 1;
			item.STDA_C_ITEM_QNTD = (short)item.qntd * -1;

			// UPDATE ON SERVER AND DB
			if (item_manager::removeItem(item, _session) <= 0)
				throw exception("[room::requestStartGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o Chip-in Practice na sala[NUMERO="
						+ std::to_string(m_ri.numero) + "], mas nao conseguiu deletar o Ticket[TYPEID=" + std::to_string(item._typeid)
						+ ", ID=" + std::to_string(item.id) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 52, 500017));

			// UPDATE ON GAME
			p.init_plain((unsigned short)0x216);

			p.addUint32((const uint32_t)GetSystemTimeAsUnix());
			p.addUint32(1);	// Count;

			p.addUint8(item.type);
			p.addUint32(item._typeid);
			p.addInt32(item.id);
			p.addUint32(item.flag_time);
			p.addBuffer(&item.stat, sizeof(item.stat));
			p.addUint32((item.STDA_C_ITEM_TIME > 0) ? item.STDA_C_ITEM_TIME : item.STDA_C_ITEM_QNTD);
			p.addZeroByte(25);

			packet_func::session_send(p, &_session, 1);
		}

		if (m_ri.course >= 0x7Fu) {

			// Special Shuffle Course
			if (m_ri.tipo == RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE && m_ri.modo == Hole::eMODO::M_SHUFFLE_COURSE) {

				m_ri.course = RoomInfo::eCOURSE(0x80 | RoomInfo::eCOURSE::CHRONICLE_1_CHAOS/*SPECIAL SHUFFLE COURSE*/);

			}else {	// Random normal

				Lottery lottery((uint64_t)this);

				for (auto& el : sIff::getInstance().getCourse()) {
				
					auto course_id = sIff::getInstance().getItemIdentify(el.second._typeid);

					if (course_id != 17/*SSC*/ && course_id != 0x40/*GRAND ZODIAC*/)
						lottery.push(100, course_id);
				}

				auto lc = lottery.spinRoleta();

				if (lc != nullptr)
					m_ri.course = RoomInfo::eCOURSE(0x80u | (unsigned char)lc->value);
			}
		}

		RateValue rv{ 0 };

		// Att Exp rate, e Pang rate, que começou o jogo
		//if (sgs::gs != nullptr) {

			rv.exp = m_ri.rate_exp = sgs::gs::getInstance().getInfo().rate.exp;
			rv.pang = m_ri.rate_pang = sgs::gs::getInstance().getInfo().rate.pang;

			// Angel Event
			m_ri.angel_event = sgs::gs::getInstance().getInfo().rate.angel_event;

			rv.clubset = sgs::gs::getInstance().getInfo().rate.club_mastery;
			rv.rain = sgs::gs::getInstance().getInfo().rate.chuva;
			rv.treasure = sgs::gs::getInstance().getInfo().rate.treasure;

			rv.persist_rain = 0u;	// Persist rain flag isso é feito na classe game
		//}

		switch (m_ri.tipo) {
		case RoomInfo::TIPO::STROKE:
			m_pGame = new Versus(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
			break;
		case RoomInfo::TIPO::MATCH:
			m_pGame = new Match(v_sessions, m_ri, rv, m_ri.channel_rookie, m_teans);
			break;
		case RoomInfo::TIPO::PANG_BATTLE:	// Ainda não está feio, usa o  Versus Normal
			m_pGame = new PangBattle(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
			break;
		case RoomInfo::TIPO::APPROCH:
			m_pGame = new Approach(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
			break;
		case RoomInfo::TIPO::PRACTICE:
			m_pGame = new Practice(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
			break;
		case RoomInfo::TIPO::TOURNEY:
			m_pGame = new Tourney(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
			break;
		case RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE:
			m_pGame = new SpecialShuffleCourse(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
			break;
		case RoomInfo::TIPO::GUILD_BATTLE:
			m_pGame = new GuildBattle(v_sessions, m_ri, rv, m_ri.channel_rookie, m_guild_manager);
			break;
		case RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE:
			m_pGame = new ChipInPractice(v_sessions, m_ri, rv, m_ri.channel_rookie);
			break;
		case RoomInfo::TIPO::GRAND_ZODIAC_INT:
		case RoomInfo::TIPO::GRAND_ZODIAC_ADV:
			m_pGame = new GrandZodiac(v_sessions, m_ri, rv, m_ri.channel_rookie);
			break;
		default:
			m_pGame = new Practice(v_sessions, m_ri, rv, m_ri.channel_rookie/*Channel Rookie Wind From 1m At 5m*/);
		}

		// Update Room State
		m_ri.state = 0;	// IN GAME

		// Update on GAME
		//packet_func::pacote04A(p, m_ri, -1);

		//packet_func::room_broadcast(*this, p, 1);

		p.init_plain((unsigned short)0x230);

		packet_func::room_broadcast(*this, p, 1);

		p.init_plain((unsigned short)0x231);

		packet_func::room_broadcast(*this, p, 1);

		uint32_t rate_pang = sgs::gs::getInstance().getInfo().rate.pang;

		p.init_plain((unsigned short)0x77);

		p.addUint32(rate_pang);	// Rate Pang

		packet_func::room_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestStartGame][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Error
		p.init_plain((unsigned short)0x253);
		
		p.addUint32((STDA_SOURCE_ERROR_DECODE(e.getCodeError() == STDA_ERROR_TYPE::ROOM)) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 0x5900200);

		packet_func::session_send(p, &_session, 1);

		ret = false;	// Error ao inicializar o Jogo
	}

	return ret;
};

void room::requestInitHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitHole");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestInitHole][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializer o hole do jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200201));

		m_pGame->requestInitHole(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestInitHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool room::requestFinishLoadHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishLoadHole");

	bool ret = false;

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestFinishLoadHole][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar carregamento do hole do jogo na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200301));

		ret = m_pGame->requestFinishLoadHole(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestFinishLoadHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
};

void room::requestFinishCharIntro(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishCharIntro");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestFinishCharIntro][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar Char Intro do jogo na sala[NUMEROR=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200401));

		m_pGame->requestFinishCharIntro(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestFinishCharIntro][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestFinishHoleData(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishHoleData");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestFinishHoleData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar dados do hole, no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201901));

		m_pGame->requestFinishHoleData(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestFinishHoleData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestInitShotSended(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShotSended");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestInitShotSended][Error] Player[UID=" + std::to_string(_session.m_pi.uid) + "] o server enviou o pacote de InitShot para o cliente, mas na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "] nao tem mais nenhum jogo inicializado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5905001));

		m_pGame->requestInitShotSended(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestInitShotSended][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestInitShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShot");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestInitShot][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializar o shot no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201401));

		m_pGame->requestInitShot(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestInitShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestSyncShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("SyncShot");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestSyncShot][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sincronizar tacada no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201501));

		m_pGame->requestSyncShot(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestSyncShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestInitShotArrowSeq(player& _session, packet *_packet) {
	REQUEST_BEGIN("InitShotArrowSeq");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestInitShotArrowSeq][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou inicializar a sequencia de setas no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado, Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201601));

		m_pGame->requestInitShotArrowSeq(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestInitShotArrowSeq][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestShotEndData(player& _session, packet *_packet) {
	REQUEST_BEGIN("ShotEndData");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestShotEndData][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar local da tacada no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201701));

		m_pGame->requestShotEndData(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestShotEndData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

RetFinishShot room::requestFinishShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishShot");

	RetFinishShot rfs;

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestFinishShot][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar tacada no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201801));

		rfs = m_pGame->requestFinishShot(_session, _packet);

		if (rfs.ret > 0) {

			// Acho que não usa mais isso então vou deixar ai, e o ret == 2 vou deixar no channel, 
			// por que se ele for o ultimo da sala tem que excluir ela
			if (rfs.ret == 1/*Finaliza Jogo*/)
				finish_game();

		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestFinishShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return rfs;
};

void room::requestChangeMira(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeMira");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestChangeMira][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar a mira no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200501));

		m_pGame->requestChangeMira(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeMira][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestChangeStateBarSpace(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateBarSpace");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestChangeStateBarSpace][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar estado da barra de espaco no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200601));

		m_pGame->requestChangeStateBarSpace(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeStateBarSpace][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActivePowerShot(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActivePowerShot");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActivePowerShot][Error] player[UID=" + std::to_string(_session.m_pi.uid) +"] tentou ativar power shot no jogo na sala[NUMEROR=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200701));

		m_pGame->requestActivePowerShot(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActivePowerShot][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestChangeClub(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeClub");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestChangeClub][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar taco no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200801));

		m_pGame->requestChangeClub(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeClub][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestUseActiveItem(player& _session, packet *_packet) {
	REQUEST_BEGIN("UseActiveItem");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestUseActiveItem][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar active item no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200901));

		m_pGame->requestUseActiveItem(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestUseActiveItem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestChangeStateTypeing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateTypeing");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestChangeStateTypeing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mudar estado do escrevendo icon no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201001));

		m_pGame->requestChangeStateTypeing(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeStateTypeing][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestMoveBall(player& _session, packet *_packet) {
	REQUEST_BEGIN("MoveBall");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestMoveBall][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou recolocar a bola no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201101));

		m_pGame->requestMoveBall(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestMoveBall][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestChangeStateChatBlock(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeStateChatBlock");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestChangeStateChatBlock][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mudar estado do chat block no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201201));

		m_pGame->requestChangeStateChatBlock(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeStateChatBlock][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActiveBooster(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveBooster");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveBooster][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar time booster no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201301));

		m_pGame->requestActiveBooster(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveBooster][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActiveReplay(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveReplay");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveReplay][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Replay no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5301001));

		m_pGame->requestActiveReplay(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveReplay][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActiveCutin(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveCutin");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveCutin][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar cutin no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201701));

		m_pGame->requestActiveCutin(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveCutin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActiveAutoCommand(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveAutoCommand");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveAutoCommand][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Auto Command no jogo na sala[NUMEROR=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x550001));

		m_pGame->requestActiveAutoCommand(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveAutoCommand][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActiveAssistGreen(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveAssistGreen");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveAssistGreen][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Assist Green no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5201801));

		m_pGame->requestActiveAssistGreen(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveAssistGreen][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestLoadGamePercent(player& _session, packet *_packet) {
	REQUEST_BEGIN("LoadGamePercent");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestLoadGamePercent][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou mandar a porcentagem carregada do jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x551001));

		m_pGame->requestLoadGamePercent(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestLoadGamePercent][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestMarkerOnCourse(player& _session, packet *_packet) {
	REQUEST_BEGIN("MarkerOnCourse");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestMarkerOnCourse][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou marcar no course no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x552001));

		m_pGame->requestMarkerOnCourse(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestMarkerOnCourse][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestStartTurnTime(player& _session, packet *_packet) {
	REQUEST_BEGIN("StartTurnTime");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestStartTurnTime][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o tempo do turno no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x553001));

		m_pGame->requestStartTurnTime(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestStartTurnTime][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestUnOrPauseGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("UnOrPauseGame");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestUnOrPauseGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pausar ou despausar o jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x554001));

		m_pGame->requestUnOrPause(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestUnOrPauseGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool room::requestLastPlayerFinishVersus(player& _session, packet *_packet) {
	REQUEST_BEGIN("LastPlayerFinishVersus");

	bool ret = false;

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestLastPlayerFinishVersus][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar Versus na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x555001));

		// Finaliza o Versus
		if (m_pGame->getSessions().size() > 0) {

			if (m_pGame->finish_game(**m_pGame->getSessions().begin(), 2/*Termina*/))
				finish_game();

		}else
			finish_game();

		ret = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestLastPlayerFinishVersus][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
};

bool room::requestReplyContinueVersus(player& _session, packet *_packet) {
	REQUEST_BEGIN("ReplyContinueVersus");

	bool ret = false;

	try {

		unsigned char opt = _packet->readUint8();

		if (m_pGame == nullptr)
			throw exception("[room::requestReplyContinueVersus][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou responder se quer continuar o versus ou nao na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x556001));

		if (opt == 0/*Finaliza o Versus*/) {
			
			if (m_pGame->getSessions().size() > 0) {
				
				if (m_pGame->finish_game(**m_pGame->getSessions().begin(), 2/*Termina*/))
					finish_game();
			
			}else
				finish_game();

			ret = true;

		}else if (opt == 1/*Continua o Versus*/)
			m_pGame->requestReplyContinue();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestReplyContinueVersus][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
};

void room::requestTeamFinishHole(player& _session, packet *_packet) {
	REQUEST_BEGIN("TeamFinishHole");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestTeamFinishHole][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar hole do Match na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x562001));

		m_pGame->requestTeamFinishHole(_session, _packet);
		
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestTeamFinishHole][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::testeDegree() {

	try {

		if (m_pGame == nullptr)
			throw exception("[room::testeDegree][Error] sala[NUMERO=" + std::to_string(m_ri.numero) + "] nao tem um jogo inicializado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2000, 0));

		auto pgi = m_pGame->getPlayerInfo(*v_sessions.begin());

		if (pgi == nullptr)
			throw exception("[room::testeDegree][Error] o jogo na sala[NUMERO=" + std::to_string(m_ri.numero) + "], nao tem o player[UID=" 
					+ std::to_string((*v_sessions.begin())->m_pi.uid) + "]. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2001, 0));

		pgi->degree += 1;

		pgi->degree %= LIMIT_DEGREE;

#ifdef _DEBUG
		// Log
		_smp::message_pool::getInstance().push(new message("[room::testeDegree][Log] Player[UID=" + std::to_string(pgi->uid) + "] Vento[Graus=" + std::to_string(pgi->degree) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		packet p((unsigned short)0x5B);

		p.addUint8(1);
		p.addUint8(0);	// Flag de card de vento, aqui é a qnd diminui o vento, 1 Vento azul
		p.addUint16(pgi->degree);
		p.addUint8(1/*Reseta*/);	// Flag do vento, 1 Reseta o Vento, 0 soma o vento que nem o comando gm \wind do pangya original

		packet_func::room_broadcast(*this, p, 1);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::testeDegree][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestLeavePractice(player& _session, packet *_packet) {
	REQUEST_BEGIN("LeavePractice");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestLeavePractice][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair do Practice na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6202001));

		if (m_ri.tipo != RoomInfo::TIPO::PRACTICE)
			throw exception("[room::requestLeavePractice][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair do Practice na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas TIPO=" + std::to_string((unsigned short)m_ri.tipo) + " de jogo da sala nao eh Practice", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0x6202002));
	
		// Acabou o tempo /*Sai do Practice*/
		reinterpret_cast< TourneyBase* >(m_pGame)->timeIsOver();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestLeavePractice][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool room::requestUseTicketReport(player& _session, packet *_packet) {
	REQUEST_BEGIN("UseTicketReport");

	bool ret = false;

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestUseTicketReport][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou usar Ticket Report no Tourney no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6301001));

		ret = m_pGame->requestUseTicketReport(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestUseTicketReport][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
};

void room::requestLeaveChipInPractice(player& _session, packet *_packet) {
	REQUEST_BEGIN("LeaveChipInPractice");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestLeaveChipInPractice][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair do Chip-in Practice na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6701001));

		if (m_ri.tipo != RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)
			throw exception("[room::requestLeaveChipInPractice][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou sair do Chip-in Practice na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas TIPO=" + std::to_string((unsigned short)m_ri.tipo) + " de jogo da sala nao eh Chip-in Practice", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0x6701002));

		// Acabou o tempo /*Sai do Chip-in Practice*/
		if (m_pGame->finish_game(_session, 2)/*Deixa para o ultimo da sala finalizar o jogo*/)
			finish_game();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestLeaveChipInPractice][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestStartFirstHoleGrandZodiac(player& _session, packet *_packet) {
	REQUEST_BEGIN("StartFirstHoleGrandZodiac");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestStartFisrtHoleGrandZodiac][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o primeiro hole do Grand Zodiac game na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6702001));

		m_pGame->requestStartFirstHoleGrandZodiac(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestStartFirstHoleGrandZodiac][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestReplyInitialValueGrandZodiac(player& _session, packet *_packet) {
	REQUEST_BEGIN("ReplyInitialValueGrandZodiac");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestReplyInitialValueGrandZodiac][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou reponder o valor inicial do Grand Zodiac game na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6703001));

		m_pGame->requestReplyInitialValueGrandZodiac(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestReplyInitialValueGrandZodiac][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestActiveRing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRing");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveRing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201001));

		m_pGame->requestActiveRing(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveRing][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveRingGround(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingGround");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveRingGround][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Terreno no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201101));

		m_pGame->requestActiveRingGround(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveRingGround][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveRingPawsRainbowJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPawsRainbowJP");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveRingPawsRainbowJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel Patinha Arco-iris no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201201));

		m_pGame->requestActiveRingPawsRainbowJP(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveRingPawsRainbowJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveRingPawsRingSetJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPawsRingSetJP");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveRingPawsRingSetJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel de Patinha de conjunto de Aneis [JP] no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala n ao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201301));

		m_pGame->requestActiveRingPawsRingSetJP(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveRingPawsRingSetJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveRingPowerGagueJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingPowerGagueJP");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveRingPowerGagueJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel Barra de PS [JP] no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201401));

		m_pGame->requestActiveRingPowerGagueJP(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveRingPowerGagueJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveRingMiracleSignJP(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveRingMiracleSignJP");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveRingMiracleSignJP][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Anel Olho Magico [JP] no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201501));

		m_pGame->requestActiveRingMiracleSignJP(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveRingMiracleSignJP][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveWing(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveWing");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveWing][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Asa no joga na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201601));

		m_pGame->requestActiveWing(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveWing][ErrorSystem] " + e.getMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActivePaws(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActivePaws");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActivePaws][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Patinha no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201701));

		m_pGame->requestActivePaws(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActivePaws][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveGlove(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveGlove");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveGlove][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativat Luva 1m no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201801));

		m_pGame->requestActiveGlove(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveGlove][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestActiveEarcuff(player& _session, packet *_packet) {
	REQUEST_BEGIN("ActiveEarcuff");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestActiveEarcuff][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou ativar Earcuff no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x6201901));

		m_pGame->requestActiveEarcuff(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestActiveEarcuff][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestSendTimeGame(player& _session) {
	CHECK_SESSION_BEGIN("requestSendTimeGame");

	packet p;

	try {

		if (isKickedPlayer(_session.m_pi.uid))
			throw exception("[room::requestSendTimeGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas o player foi chutado da sala antes de comecar o jogo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2704, 7));

		if (m_pGame == nullptr)
			throw exception("[room::requestSendTimeGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pegar o tempo do tourney que comecou na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2705, 1));

		m_pGame->requestSendTimeGame(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestSendTimeGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta erro
		p.init_plain((unsigned short)0x113);

		p.addUint8(6);		// Option Error

		// Error Code
		p.addUint8((unsigned char)((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error System*/));

		packet_func::session_send(p, &_session, 1);
	}
}

bool room::requestEnterGameAfterStarted(player& _session) {
	CHECK_SESSION_BEGIN("requestEnterGameAfterStarted");

	packet p;

	bool ret = false;

	try {

		if (isKickedPlayer(_session.m_pi.uid))
			throw exception("[room::requestEnterGameAfterStarted][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas o player foi chutado da sala antes de comecar o jogo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2704, 7));

		if (m_pGame == nullptr)
			throw exception("[room::requestEnterGameAfterStarted][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2705, 1));

		if (isGamingBefore(_session.m_pi.uid))
			throw exception("[room::requestEnterGameAfterStarted][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas o player ja tinha jogado nessa sala e saiu, e nao pode mais entrar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2703, 6));

		int64_t tempo = (m_ri.qntd_hole == 18) ? 10 * 60000 : 5 * 60000;

		auto remain = getLocalTimeDiff(m_pGame->getTimeStart());

		if (remain > 0)
			remain /= STDA_10_MICRO_PER_MILLI;	// miliseconds

		if (remain >= tempo)
			throw exception("[room::requestEnterGameAfrerStarted][WARNING] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas o tempo de entrar no tourney acabou.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2706, 2));	// Acabou o tempo de entrar na sala

		// Add Player a sala
		enter(_session);

		ret = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestEnterGameAfterStarted][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Excluí player da sala se adicionou ele antes
		if (findSessionByUID(_session.m_pi.uid) != nullptr)
			leave(_session, 0);

		// Resposta erro
		p.init_plain((unsigned short)0x113);

		p.addUint8(6);		// Option Error

		// Error Code
		p.addUint8((unsigned char)((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error System*/));

		packet_func::session_send(p, &_session, 1);
	}

	return ret;
}

void room::requestUpdateEnterAfterStartedInfo(player& _session, EnterAfterStartInfo& _easi) {
	CHECK_SESSION_BEGIN("requestUpdateEnterAfterStartedInfo");

	packet p;

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestUpdateEnterAfterStartedInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou atualizar info do player que entrou depois na sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] ja em jogo, mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2705, 1));

		m_pGame->requestUpdateEnterAfterStartedInfo(_session, _easi);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestUpdateEnterAfterStartedInfo][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Resposta erro
		p.init_plain((unsigned short)0x113);

		p.addUint8(6);		// Option Error

		// Error Code
		p.addUint8((unsigned char)((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::ROOM) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 1/*Unknown Error System*/));

		packet_func::session_send(p, &_session, 1);
	}
}

bool room::requestFinishGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("FinishGame");

	bool ret = false;

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestFinishGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou finalizar jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5202101));

		if (m_pGame->requestFinishGame(_session, _packet)) {	// Terminou o Jogo

			finish_game();

			ret = true;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestFinishGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ret;
};

void room::requestChangeWindNextHoleRepeat(player& _session, packet *_packet) {
	REQUEST_BEGIN("ChangeWindNextHoleRepeat");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestChangeWindNextHoleRepeat][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou trocar vento dos proximos holes repeat no jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5202201));

		m_pGame->requestChangeWindNextHoleRepeat(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestChangeWindNextHoleRepeat][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestPlayerReportChatGame(player& _session, packet *_packet) {
	REQUEST_BEGIN("PlayerReportChatGame");

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestPlayerReportChatGame][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] tentou reporta o chat do jogo na sala[NUMERO=" + std::to_string(m_ri.numero) 
					+ "], mas nao tem nenhum jogo inicializado na sala. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 0x580200, 0));
	
		// Report Chat Game
		m_pGame->requestPlayerReportChatGame(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestPlayerReportChatGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::GAME)
			throw;
	}
};

void room::requestExecCCGChangeWindVersus(player& _session, packet *_packet) {
	REQUEST_BEGIN("ExecCCGChangeWindVersus");

	try {

		if (!(m_ri.tipo == RoomInfo::TIPO::STROKE || m_ri.tipo == RoomInfo::TIPO::MATCH))
			throw exception("[room::requestExecCCGChangeWindVersus][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou executar o comando de troca de vento na sala[NUMERO="
					+ std::to_string(m_ri.numero) + ", TIPO=" + std::to_string(m_ri.tipo) + "], mas o tipo da sala nao eh Stroke ou Match modo. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5700100));

		if (m_pGame == nullptr)
			throw exception("[room::requestExecCCGChangeWindVersus][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou executar o comando de troca de vento na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo inicializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 2, 0x5700100));

		m_pGame->requestExecCCGChangeWind(_session, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestExecCCGChangeWindVersus][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}
};

void room::requestExecCCGChangeWeather(player& _session, packet *_packet) {
	REQUEST_BEGIN("ExecCCGChangeWeather");

	try {

		// Update on Flag Lounge or Game Course->hole->weather
		if (m_ri.tipo == RoomInfo::TIPO::LOUNGE) {

			m_weather_lounge = _packet->readUint8();

			// UPDATE ON GAME
			packet p((unsigned short)0x9E);

			p.addUint16(m_weather_lounge);
			p.addUint8(1);			// Acho que seja flag, não sei, vou deixar 1 por ser o GM que mudou

			packet_func::room_broadcast(*this, p, 1);
		
		}else if (m_pGame != nullptr) {
			
			m_pGame->requestExecCCGChangeWeather(_session, _packet);

		}else
			throw exception("[room::requestExecCCGChangeWeather][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou executar o comando de troca de tempo(weather) na sala[NUMERO="
				+ std::to_string(m_ri.numero) + "], mas a sala nao eh lounge ou nao tem um jogo iniclializado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 3, 0x5700100));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestExecCCGChangeWeather][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}
};

void room::requestExecCCGGoldenBell(player& _session, packet *_packet) {
	REQUEST_BEGIN("ExecCCGGoldenBell");

	try {

		uint32_t item_typeid = _packet->readUint32();
		uint32_t item_qntd = _packet->readUint32();

		if (item_typeid == 0)
			throw exception("[room::requestExecCCGGoldenBell][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar presente para todos da sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "] o Item[TYPEID=" + std::to_string(item_typeid) + "QNTD = " 
					+ std::to_string(item_qntd) + "], mas item is invalid. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 3, 0x5700100));

		if (item_qntd > 20000u)
			throw exception("[room::requestExecCCGGoldenBell][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar presente para todos da sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] o Item[TYPEID=" + std::to_string(item_typeid) + "QNTD = " 
					+ std::to_string(item_qntd) + "], mas a quantidade passa de 20mil. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 4, 0x5700100));

		auto base = sIff::getInstance().findCommomItem(item_typeid);

		if (base == nullptr)
			throw exception("[room::requestExecCCGGoldenBell][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar presente para todos da sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] o Item[TYPEID=" + std::to_string(item_typeid) + "QNTD = " 
					+ std::to_string(item_qntd) + "], mas o item nao existe no IFF_STRUCT do Server. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 6, 0));

		stItem item{ 0 };
		BuyItem bi{ 0 };

		bi.id = -1;
		bi._typeid = item_typeid;
		bi.qntd = item_qntd;

		auto msg = std::string("GM enviou um item para voce: item[ " + std::string(base->name) + " ]");

		for (auto& el : v_sessions) {

			// Limpa item
			item.clear();

			item_manager::initItemFromBuyItem(el->m_pi, item, bi, false, 0, 0, 1/*~nao Check Level*/);

			if (item._typeid == 0)
				throw exception("[room::requestExecCCGGoldenBell][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar presente para todos da sala[NUMERO="
					+ std::to_string(m_ri.numero) + "] o Item[TYPEID=" + std::to_string(item_typeid) + "QNTD = "
					+ std::to_string(item_qntd) + "], mas nao conseguiu inicializar o item. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 5, 0));

			if (MailBoxManager::sendMessageWithItem(0, el->m_pi.uid, msg, item) <= 0)
				throw exception("[room::requestExecCCGGoldenBell][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou enviar presente para o player[UID="
						+ std::to_string(el->m_pi.uid) + "] o Item[TYPEID=" + std::to_string(item_typeid) + ", QNTD=" 
						+ std::to_string(item_qntd) + "], mas nao conseguiu colocar o item no mail box dele. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 7, 0));

			// Log
			_smp::message_pool::getInstance().push(new message("[room::requestExecCCGGoldenBell][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] enviou um Item[TYPEID="
					+ std::to_string(item_typeid) + ", QNTD=" + std::to_string(item_qntd) + "] para o player[UID=" 
					+ std::to_string(el->m_pi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));
		}

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message("[room::requestExecCCGGoldenBell][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}
};

bool room::execSmartCalculatorCmd(player& _session, std::string& _msg, eTYPE_CALCULATOR_CMD _type) {
	CHECK_SESSION_BEGIN("execSmartCalculatorCmd");

	bool ret = false;

	try {

		if (!isGaming())
			throw exception("[room::execSmartCalculatorCmd][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] Channel[ID=" + std::to_string((unsigned short)m_channel_owner) + "] tentou executar Smart Calculator Command na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas ele nao esta em jogo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 10000, 0));

		ret = m_pGame->execSmartCalculatorCmd(_session, _msg, _type);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::execSmartCalculatorCmd][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		ret = false;
	}

	return ret;
};

unsigned char room::requestPlace(player& _session) {

	try {

		if (m_pGame != nullptr)
			return m_pGame->requestPlace(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestPlacePlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0u;
}

void room::startGame(player& _session) {

	try {

		if (m_pGame == nullptr)
			throw exception("[room::startGame][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou comecar o jogo na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo iniciado. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 0x5200101));

		if (m_ri.flag == 0) {
			
			m_pGame->sendInitialData(_session);

			if (m_ri.tipo == RoomInfo::TIPO::STROKE || m_ri.tipo == RoomInfo::TIPO::MATCH)
				sendCharacter(_session, 0x103);

		}else {	// Entra depois
			
			try {

				std::vector< PlayerRoomInfoEx > v_element;
				PlayerRoomInfoEx* pri = nullptr;

				std::for_each(v_sessions.begin(), v_sessions.end(), [&](auto& _el) {
					if ((pri = getPlayerInfo(_el)) != nullptr)
						v_element.push_back(*pri);
				});

				// Send Make Room
				packet p((unsigned short)0x113);

				p.addUint8(4);	// Cria sala
				p.addUint8(0);

				p.addBuffer(&m_ri, sizeof(RoomInfo));

				packet_func::session_send(p, &_session, 1);

				// Send All Player Of Room
				p.init_plain((unsigned short)0x113);

				p.addUint8(4);
				p.addUint8(1);

				p.addInt8((unsigned char)v_element.size());

				for (auto i = 0u; i < v_element.size(); i++)
					p.addBuffer(&v_element[i], sizeof(PlayerRoomInfo));

				packet_func::session_send(p, &_session, 1);

				// Rate Pang
				p.init_plain((unsigned short)0x113);

				p.addUint8(4);
				p.addUint8(2);

				p.addUint32(m_ri.rate_pang);

				packet_func::session_send(p, &_session, 1);

				// Send Initial of Game
				m_pGame->sendInitialDataAfter(_session);

				// Add Player ON GAME to ALL players
				p.init_plain((unsigned short)0x113);

				p.addUint8(7);
				p.addUint8(0);

				p.addString(_session.m_pi.nickname);

				p.addBuffer(&m_ri, sizeof(RoomInfo));

				p.addInt8((unsigned char)v_element.size());

				p.addUint32((uint32_t)v_sessions.size());

				for (auto i = 0u; i < v_element.size(); i++)
					p.addBuffer(&v_element[i], sizeof(PlayerRoomInfo));

				// Send ALL players of room exceto ele
				packet_func::vector_send(p, getSessions(&_session), 1);
			
			}catch (exception& e) {
				UNREFERENCED_PARAMETER(e);

				throw;
			}
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::startGame][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::requestStartAfterEnter(job& _job) {

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestStartAfterEnter][Error] tentou comecar o tempo que pode entrar no jogo depois que ele comecou na sala[NUMERO="
				+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo iniciado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1200, 0));

		m_pGame->requestStartAfterEnter(_job);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestStartAfterEnter][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void room::requestEndAfterEnter() {

	try {

		if (m_pGame == nullptr)
			throw exception("[room::requestEndAfterEnter][Error] tentou terminar o tempo que pode entrar no jogo depois que ele comecou na sala[NUMERO=" 
					+ std::to_string(m_ri.numero) + "], mas a sala nao tem nenhum jogo iniciado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1201, 0));

		m_pGame->requestEndAfterEnter();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[room::requestEndAfterEnter][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void room::sendMake(player& _session) {

	packet p;

	packet_func::pacote049(p, this, 0);
	packet_func::session_send(p, &_session, 0);

};

void room::sendUpdate() {

	packet p;

	packet_func::pacote04A(p, m_ri, -1/*valor constante*/);
	packet_func::room_broadcast(*this, p, 0);
};

void room::sendCharacter(player& _session, int _option) {

	int option = !(RoomInfo::TIPO::STROKE == m_ri.tipo || RoomInfo::TIPO::MATCH == m_ri.tipo || RoomInfo::TIPO::LOUNGE == m_ri.tipo || RoomInfo::TIPO::PANG_BATTLE == m_ri.tipo) ? 0x100 : 0;

	option += _option;

	if (option == 0 && m_ri.tipo == RoomInfo::TIPO::LOUNGE)
		option = 7;

	std::vector< PlayerRoomInfoEx > v_element;
	PlayerRoomInfoEx* pri = nullptr;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		std::for_each(v_sessions.begin(), v_sessions.end(), [&](auto& _el) {
			if ((pri = getPlayerInfo(_el)) != nullptr)
				v_element.push_back(*pri);
		});

		pri = getPlayerInfo(&_session);

		if (pri == nullptr && _option != 2)
			throw exception("[room::sendCharacter][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pegar o info do player na sala[NUMERO="
				+ std::to_string(m_ri.numero) + "], mas nao tem o info dele na sala. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 5000));

		packet p;

		if (packet_func::pacote048(p, &_session, ((_option == 1 || _option == 4 || _option == 0x103) ? std::vector< PlayerRoomInfoEx > { *pri  } : v_element), option))
			packet_func::room_broadcast(*this, p, 1);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch(exception& e) {
		UNREFERENCED_PARAMETER(e);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		// Relança
		throw;
	}
};

void room::sendCharacterStateLounge(player& _session) {

	if (m_ri.tipo == RoomInfo::TIPO::LOUNGE) {
		auto it = (_session.m_pi.ei.char_info == nullptr) ? _session.m_pi.mp_scl.end() : _session.m_pi.mp_scl.find(_session.m_pi.ei.char_info->id);

		if (it == _session.m_pi.mp_scl.end())
			throw exception("[room::sendCharacterStateLounge][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] nao tem os estados do character na lounge.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CHANNEL, 13, 0));

		packet p((unsigned short)0x196);

		p.addUint32(_session.m_oid);

		p.addBuffer(&it->second, sizeof(StateCharacterLounge));

		packet_func::room_broadcast(*this, p, 0);
	}
};

void room::sendWeatherLounge(player& _session) {

	if (m_ri.tipo == RoomInfo::TIPO::LOUNGE) {

		// Envia o tempo(weather) do lounge só se ele for diferente de tempo bom
		if (m_weather_lounge != 0u/*Good Weather*/) {
		
			packet p((unsigned short)0x9E);

			p.addUint16(m_weather_lounge);
			p.addUint8(0);	// Flag (acho), vou colocar 0 o padrão, colocou 1 aqui só quando eu mudou com o comando GM

			packet_func::session_send(p, &_session, 1);
		}
	}
};

void room::lock() {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_lock_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_lock_cs);
#endif

	if (m_destroying) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_lock_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_lock_cs);
#endif

		throw exception("[room::lock][Error] room esta no estado para ser destruida, nao pode bloquear ela.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 150, 0));
	}

	m_lock_spin_state++;	// Bloqueado
};

bool room::tryLock() {

	bool ret = false;

#if defined(_WIN32)
	if ((ret = TryEnterCriticalSection(&m_lock_cs))) {
#elif defined(__linux__)
	int err_ret = 0; // provável erro é EBUSY, mas pode falhar com outros também, mas muito menos provável

	if ((err_ret = pthread_mutex_trylock(&m_lock_cs)) == 0) {

		ret = true;
#endif

		if (m_destroying) {

#if defined(_WIN32)
			LeaveCriticalSection(&m_lock_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_lock_cs);
#endif

			throw exception("[room::tryLock][Error] room esta no estado para ser destruida, nao pode bloquear ela.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 150, 0));
		}

		m_lock_spin_state++;	// Bloqueado
	}

	return ret;
};

void room::unlock() {

	if (--m_lock_spin_state < 0l) {

		_smp::message_pool::getInstance().push(new message("[room::unlock][WARNING] a sala[NUMERO=" + std::to_string(m_ri.numero) + "] ja esta desbloqueada.", CL_FILE_LOG_AND_CONSOLE));

		//return;
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_lock_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_lock_cs);
#endif
};

std::vector< InviteChannelInfo >& room::getAllInvite() {
	return v_invite;
};

void room::setDestroying() {

#if defined(_WIN32)
	EnterCriticalSection(&m_lock_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_lock_cs);
#endif

	// Destruindo a sala
	m_destroying = true;

#if defined(_WIN32)
	LeaveCriticalSection(&m_lock_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_lock_cs);
#endif
};

void room::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_channel = reinterpret_cast< channel* >(_arg);

	switch (_msg_id) {
	case 7:	// Update Character PCL
	{
		auto cmd_ucp = reinterpret_cast< CmdUpdateCharacterPCL* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][Log] Atualizou Character[TYPEID=" + std::to_string(cmd_ucp->getInfo()._typeid) + ", ID=" 
				+ std::to_string(cmd_ucp->getInfo().id) + "] PCL[C0=" + std::to_string((unsigned short)cmd_ucp->getInfo().pcl[CharacterInfo::S_POWER]) + ", C1="
				+ std::to_string((unsigned short)cmd_ucp->getInfo().pcl[CharacterInfo::S_CONTROL]) + ", C2=" + std::to_string((unsigned short)cmd_ucp->getInfo().pcl[CharacterInfo::S_ACCURACY]) + ", C3="
				+ std::to_string((unsigned short)cmd_ucp->getInfo().pcl[CharacterInfo::S_SPIN]) + ", C4=" + std::to_string((unsigned short)cmd_ucp->getInfo().pcl[CharacterInfo::S_CURVE]) + "] do Player[UID=" 
				+ std::to_string(cmd_ucp->getUID()) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 8:	// Update ClubSet Stats
	{
		auto cmd_ucss = reinterpret_cast<CmdUpdateClubSetStats*>(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][Log] Atualizou ClubSet[TYPEID=" + std::to_string(cmd_ucss->getInfo()._typeid) + ", ID="
				+ std::to_string(cmd_ucss->getInfo().id) + "] Stats[C0=" + std::to_string((unsigned short)cmd_ucss->getInfo().c[CharacterInfo::S_POWER]) + ", C1="
				+ std::to_string((unsigned short)cmd_ucss->getInfo().c[CharacterInfo::S_CONTROL]) + ", C2=" + std::to_string((unsigned short)cmd_ucss->getInfo().c[CharacterInfo::S_ACCURACY]) + ", C3="
				+ std::to_string((unsigned short)cmd_ucss->getInfo().c[CharacterInfo::S_SPIN]) + ", C4=" + std::to_string((unsigned short)cmd_ucss->getInfo().c[CharacterInfo::S_CURVE]) + "] do Player[UID=" 
				+ std::to_string(cmd_ucss->getUID()) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 9:	// Update Character Mastery
	{
		auto cmd_ucm = reinterpret_cast< CmdUpdateCharacterMastery* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][Log] Atualizou Character[TYPEID=" + std::to_string(cmd_ucm->getInfo()._typeid) + ", ID=" 
				+ std::to_string(cmd_ucm->getInfo().id) + "] Mastery[value=" + std::to_string(cmd_ucm->getInfo().mastery) + "] do player[UID=" + std::to_string(cmd_ucm->getUID()) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 12:	// Update ClubSet Workshop
	{
		auto cmd_ucw = reinterpret_cast< CmdUpdateClubSetWorkshop* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_ucw->getUID()) + "] Atualizou ClubSet[TYPEID=" + std::to_string(cmd_ucw->getInfo()._typeid) + ", ID=" 
				+ std::to_string(cmd_ucw->getInfo().id) + "] Workshop[C0=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[0]) + ", C1=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[1]) + ", C2=" 
				+ std::to_string(cmd_ucw->getInfo().clubset_workshop.c[2]) + ", C3=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[3]) + ", C4=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.c[4]) 
				+ ", Level=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.level) + ", Mastery=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.mastery) + ", Rank=" 
				+ std::to_string(cmd_ucw->getInfo().clubset_workshop.rank) + ", Recovery=" + std::to_string(cmd_ucw->getInfo().clubset_workshop.recovery_pts) + "] Flag=" + std::to_string(cmd_ucw->getFlag()) + "", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 26:	// Update Mascot Info
	{

		auto cmd_umi = reinterpret_cast< CmdUpdateMascotInfo* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[room::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_umi->getUID()) + "] Atualizar Mascot Info[TYPEID=" 
				+ std::to_string(cmd_umi->getInfo()._typeid) + ", ID=" + std::to_string(cmd_umi->getInfo().id) + ", LEVEL=" + std::to_string((unsigned short)cmd_umi->getInfo().level) 
				+ ", EXP=" + std::to_string(cmd_umi->getInfo().exp) + ", FLAG=" + std::to_string((unsigned short)cmd_umi->getInfo().flag) + ", TIPO=" 
				+ std::to_string(cmd_umi->getInfo().tipo) + ", IS_CASH=" + std::to_string((unsigned short)cmd_umi->getInfo().is_cash) + ", PRICE="
				+ std::to_string(cmd_umi->getInfo().price) + ", MESSAGE=" + std::string(cmd_umi->getInfo().message) + ", END_DT=" + _formatDate(cmd_umi->getInfo().data) + "]", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:	// 25 é update item equipado slot
		break;
	}
};

size_t room::findIndexSession(player* _session) {
	
	if (_session == nullptr)
		throw exception("[room::findIndexSession][Error] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 3, 0));

	size_t index = ~0;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto i = 0u; i < v_sessions.size(); ++i) {
		if (v_sessions[i] == _session) {
			index = i;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return index;
};

size_t room::findIndexSession(uint32_t _uid) {

	if (_uid == 0u)
		throw exception("[room::findIndexSession][Error] _uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 3, 0));

	size_t index = ~0;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto i = 0u; i < v_sessions.size(); ++i) {
		if (v_sessions[i]->m_pi.uid == _uid) {
			index = i;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return index;
};

PlayerRoomInfoEx* room::makePlayerInfo(player& _session) {
	
	PlayerRoomInfoEx pri{};

	// Player Room Info Init
	pri.oid = _session.m_oid;
#if defined(_WIN32)
	memcpy_s(pri.nickname, sizeof(pri.nickname), _session.m_pi.nickname, sizeof(pri.nickname));
	memcpy_s(pri.guild_name, sizeof(pri.guild_name), _session.m_pi.gi.name, sizeof(pri.guild_name));
#elif defined(__linux__)
	memcpy(pri.nickname, _session.m_pi.nickname, sizeof(pri.nickname));
	memcpy(pri.guild_name, _session.m_pi.gi.name, sizeof(pri.guild_name));
#endif
	pri.position = (unsigned char)getPosition(&_session) + 1;	// posição na sala
	pri.capability = _session.m_pi.m_cap;
	pri.title = _session.m_pi.ue.m_title;

	if (_session.m_pi.ei.char_info != nullptr)
		pri.char_typeid = _session.m_pi.ei.char_info->_typeid;

#if defined(_WIN32)
	memcpy_s(pri.skin, sizeof(pri.skin), _session.m_pi.ue.skin_typeid, sizeof(pri.skin));
#elif defined(__linux__)
	memcpy(pri.skin, _session.m_pi.ue.skin_typeid, sizeof(pri.skin));
#endif
	pri.skin[4] = 0;		// Aqui tem que ser zero, se for outro valor não mostra a imagem do character equipado

	if (getMaster() == _session.m_pi.uid) {
		pri.state_flag.uFlag.stFlagBit.master = 1;
		pri.state_flag.uFlag.stFlagBit.ready = 1;	// Sempre está pronto(ready) o master
	}

	pri.state_flag.uFlag.stFlagBit.sexo = _session.m_pi.mi.sexo;

	// Update Team se for Match
	if (m_ri.tipo == RoomInfo::TIPO::MATCH) {

		if (v_sessions.size() > 1) {

			if (m_teans[0].getCount() >= 2 && m_teans[1].getCount() >= 2)
				throw exception("[room::makePlayerInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar em time para todos os times da sala estao cheios. Hacker ou Bug", 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1500, 0));
			else if (m_teans[0].getCount() >= 2)
				pri.state_flag.uFlag.stFlagBit.team = 1;	// Blue
			else if (m_teans[1].getCount() >= 2)
				pri.state_flag.uFlag.stFlagBit.team = 0;	// Red
			else {

				auto pPri = getPlayerInfo((v_sessions.size() == 2) ? *v_sessions.begin() : (v_sessions.size() > 2 ? *(v_sessions.rbegin() += 1) : nullptr));

				if (pPri == nullptr)
					throw exception("[room::makePlayerInfo][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou entrar em um time, mas o ultimo player da sala, nao tem um info no sala. Bug", 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1501, 0));

				pri.state_flag.uFlag.stFlagBit.team = ~pPri->state_flag.uFlag.stFlagBit.team;
			}

		}else
			pri.state_flag.uFlag.stFlagBit.team = 0;

		m_teans[pri.state_flag.uFlag.stFlagBit.team].addPlayer(&_session);

	}else if (m_ri.tipo != RoomInfo::TIPO::GUILD_BATTLE)	// O Guild Battle tem sua própria função para inicializar e atualizar o team e os dados da guild
		pri.state_flag.uFlag.stFlagBit.team = (pri.position - 1) % 2;
	
	// Só faz calculo de Quit rate depois que o player
	// estiver no level Beginner E e jogado 50 games
	if (_session.m_pi.level >= 6 && _session.m_pi.ui.jogado >= 50) {
		float rate = _session.m_pi.ui.getQuitRate();

		if (rate < GOOD_PLAYER_ICON)
			pri.state_flag.uFlag.stFlagBit.azinha = 1;
		else if (rate >= QUITER_ICON_1 && rate < QUITER_ICON_2)
			pri.state_flag.uFlag.stFlagBit.quiter_1 = 1;
		else if (rate >= QUITER_ICON_2)
			pri.state_flag.uFlag.stFlagBit.quiter_2 = 1;
	}

	pri.level = _session.m_pi.mi.level;

	if (_session.m_pi.ei.char_info != nullptr && _session.m_pi.ui.getQuitRate() < GOOD_PLAYER_ICON)
		pri.icon_angel = _session.m_pi.ei.char_info->AngelEquiped();
	else
		pri.icon_angel = 0u;

	pri.ucUnknown_0A = 10;	// 0x0A dec"10" _session.m_pi.place, pode ser lugar[place]
	pri.guild_uid = _session.m_pi.gi.uid;
#if defined(_WIN32)
	memcpy_s(pri.guild_mark_img, sizeof(pri.guild_mark_img), _session.m_pi.gi.mark_emblem, sizeof(pri.guild_mark_img));
#elif defined(__linux__)
	memcpy(pri.guild_mark_img, _session.m_pi.gi.mark_emblem, sizeof(pri.guild_mark_img));
#endif
	pri.guild_mark_index = _session.m_pi.gi.index_mark_emblem;
	pri.uid = _session.m_pi.uid;
	pri.state_lounge = _session.m_pi.state_lounge;
	pri.usUnknown_flg = 0;			// Ví players com valores 2 e 4 e 0
	pri.state = _session.m_pi.state;
	pri.location = { _session.m_pi.location.x, _session.m_pi.location.z, _session.m_pi.location.r };
	
	// Personal Shop
	pri.shop = m_personal_shop.getPersonShop(_session);

	if (_session.m_pi.ei.mascot_info != nullptr)
		pri.mascot_typeid = _session.m_pi.ei.mascot_info->_typeid;

	pri.flag_item_boost = _session.m_pi.checkEquipedItemBoost();
	pri.ulUnknown_flg = 0;
	//pri.id_NT não estou usando ainda
	//pri.ucUnknown106
	pri.convidado = 0;	// Flag Convidado, [Não sei bem por que os que entra na sala normal tem valor igual aqui, já que é flag de convidado waiting], Valor constante da sala para os players(ACHO)
	pri.avg_score = _session.m_pi.ui.getMediaScore();
	//pri.ucUnknown3

	if (_session.m_pi.ei.char_info != nullptr)
		pri.ci = *_session.m_pi.ei.char_info;

	auto it = m_player_info.insert(std::make_pair(&_session, pri));

	// Check inset pair in map of room player info
	if (!it.second) {

		if (it.first != m_player_info.end() && it.first->first != nullptr && it.first->first == (&_session)) {

			if (it.first->second.uid != _session.m_pi.uid) {

				// Add novo PlayerRoomInfo para a (session*), que tem um novo player conectado na session.
				// Isso pode acontecer quando chama essa função 2x com a mesma session e o mesmo player

				try {

					// pega o antigo PlayerRoomInfo para usar no Log
					auto pri_ant = m_player_info.at(&_session);

					// Novo PlayerRoomInfo
					m_player_info.at(&_session) = pri;

					// Log de que trocou o PlayerChannelInfo da session
					_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][WARNING][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] esta trocando o PlayerRoomInfo[UID=" + std::to_string(pri_ant.uid) + "] do player anterior que estava conectado com essa session, pelo o PlayerRoomInfo[UID=" 
							+ std::to_string(pri.uid) + "] do player atual da session.", CL_FILE_LOG_AND_CONSOLE));

				}catch (std::out_of_range& e) {
					UNREFERENCED_PARAMETER(e);

					_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][Error][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "], nao conseguiu atualizar o PlayerRoomInfo da session para o novo PlayerRoomInfo do player atual da session. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			
			}else
				_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] nao conseguiu adicionar o PlayerRoomInfo da session, por que ja tem o mesmo PlayerRoomInfo no map.", CL_FILE_LOG_AND_CONSOLE));
		
		}else
			_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][Error] nao conseguiu inserir o pair de PlayerInfo do player[UID="
					+ std::to_string(_session.m_pi.uid) + "] no map de player info do room. Bug", CL_FILE_LOG_AND_CONSOLE));
	}

	return ((it.second || (it.first != m_player_info.end() && it.first->first != nullptr)) ? &it.first->second : nullptr);
};

PlayerRoomInfoEx* room::makePlayerInvitedInfo(player& _session) {

	PlayerRoomInfoEx pri{};

	// Player Room Info Init
	pri.oid = _session.m_oid;
	pri.position = (unsigned char)(getPosition(&_session) + 1);	// posição na sala

	pri.ucUnknown_0A = 10;	// 0x0A dec"10" _session.m_pi.place, pode ser lugar[place]
	
	pri.uid = _session.m_pi.uid;

	pri.convidado = 1;	// Flag Convidado, [Não sei bem por que os que entra na sala normal tem valor igual aqui, já que é flag de convidado waiting], Valor constante da sala para os players(ACHO)

	auto it = m_player_info.insert(std::make_pair(&_session, pri));

	// Check inset pair in map of room player info
	if (!it.second) {

		if (it.first != m_player_info.end() && it.first->first != nullptr && it.first->first == (&_session)) {

			if (it.first->second.uid != _session.m_pi.uid) {

				// Add novo PlayerRoomInfo para a (session*), que tem um novo player conectado na session.
				// Isso pode acontecer quando chama essa função 2x com a mesma session e o mesmo player

				try {

					// pega o antigo PlayerRoomInfo para usar no Log
					auto pri_ant = m_player_info.at(&_session);

					// Novo PlayerRoomInfo
					m_player_info.at(&_session) = pri;

					// Log de que trocou o PlayerChannelInfo da session
					_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][WARNING][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
							+ "] esta trocando o PlayerRoomInfo[UID=" + std::to_string(pri_ant.uid) + "] do player anterior que estava conectado com essa session, pelo o PlayerRoomInfo[UID=" 
							+ std::to_string(pri.uid) + "] do player atual da session.", CL_FILE_LOG_AND_CONSOLE));

				}catch (std::out_of_range& e) {
					UNREFERENCED_PARAMETER(e);

					_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][Error][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
							+ "], nao conseguiu atualizar o PlayerRoomInfo da session para o novo PlayerRoomInfo do player atual da session. Bug", CL_FILE_LOG_AND_CONSOLE));
				}
			
			}else
				_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
						+ "] nao conseguiu adicionar o PlayerRoomInfo da session, por que ja tem o mesmo PlayerRoomInfo no map.", CL_FILE_LOG_AND_CONSOLE));
		
		}else
			_smp::message_pool::getInstance().push(new message("[room::makePlayerInfo][Error] nao conseguiu inserir o pair de PlayerInfo do player[UID="
					+ std::to_string(_session.m_pi.uid) + "] no map de player info do room. Bug", CL_FILE_LOG_AND_CONSOLE));
	}

	return ((it.second || (it.first != m_player_info.end() && it.first->first != nullptr)) ? &it.first->second : nullptr);
};

void room::geraSecurityKey() {
	//std::srand(std::clock() * 255 * std::clock() * 127);

	for (auto i = 0u; i < 16u; ++i)
		m_ri.key[i] = (char)((sRandomGen::getInstance().rIbeMt19937_64_chrono() % 195) + 60);

	m_ri.key[0];
};

void room::updatePosition() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	std::map< player*, PlayerRoomInfoEx >::iterator it;

	for (auto i = 0u; i < v_sessions.size(); ++i) {
		if ((it = m_player_info.find(v_sessions[i])) != m_player_info.end())
			it->second.position = (unsigned char)i + 1;
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

void room::updateTrofel() {

	if (v_sessions.size() > 0 && (m_ri.trofel != TROFEL_GM_EVENT_TYPEID || m_ri.max_player <= 30) && (m_ri.time_30s > 0 && m_ri.tipo != RoomInfo::TIPO::GUILD_BATTLE) 
			&& m_ri.master != -2 || (m_ri.tipo == RoomInfo::GRAND_PRIX && m_ri.grand_prix.dados_typeid > 0)) {

		if (m_pGame != nullptr)
			m_pGame->requestUpdateTrofel();
		else {

			uint32_t soma = 0u;

			std::for_each(v_sessions.begin(), v_sessions.end(), [&](auto& _el) {
				if (_el != nullptr)
					soma += (_el->m_pi.level > 60) ? 60 : (_el->m_pi.level > 0 ? _el->m_pi.level - 1 : 0);
			});

			uint32_t new_trofel = STDA_MAKE_TROFEL(soma, (uint32_t)v_sessions.size());

			if (new_trofel > 0 && new_trofel != m_ri.trofel) {

				// Check se o trofeu anterior era o GM e se o novo não é mais, aí tira a flag de GM da sala
				if (m_ri.trofel == TROFEL_GM_EVENT_TYPEID && new_trofel != TROFEL_GM_EVENT_TYPEID)
					m_ri.flag_gm = 0;

				if (m_ri.trofel > 0) {

					m_ri.trofel = new_trofel;

					packet p((unsigned short)0x97);

					p.addUint32(m_ri.trofel);

					packet_func::room_broadcast(*this, p, 1);

				}else
					m_ri.trofel = new_trofel;
			}
		}
	}
}

void room::updateMaster(player* _session) {

	player *master = findSessionByUID(m_ri.master);

	if (_session != nullptr && _session->m_pi.m_cap.stBit.game_master/* & 4*/ && m_ri.master != -2) { // GM Entrou na sala

		// Só troca o master se ele saiu da sala ou se ele não for GM
		if (master == nullptr || !(master->m_pi.m_cap.stBit.game_master/* & 4*/)) {
			
			packet p;

			m_ri.master = _session->m_pi.uid;

			m_ri.state_flag = 0x100;	// GM

			if (master != nullptr) {

				updatePlayerInfo(*master);

				// State Old Master Change
				p.init_plain((unsigned short)0x78);

				p.addUint32(master->m_oid);

				p.addUint8(!getPlayerInfo(master)->state_flag.uFlag.stFlagBit.ready);

				packet_func::room_broadcast(*this, p, 1);
			}

			// Master Change
			p.init_plain((unsigned short)0x7C);

			p.addUint32(_session->m_oid);

			p.addInt16(0);	// option ACHO

			packet_func::room_broadcast(*this, p, 0);
		}

	}else if (master == nullptr && v_sessions.size() > 0 && m_ri.master != -2) { // Master saiu da sala
		
		//só troca se for diferente de SSC e GRANDPRIX e GZ Event
		if (m_ri.tipo != RoomInfo::TIPO::SPECIAL_SHUFFLE_COURSE && m_ri.tipo != RoomInfo::TIPO::GRAND_PRIX) {

			// Find GM
#if defined(_WIN32)
			EnterCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_lock(&m_cs);
#endif

			auto i = VECTOR_FIND_PTR_ITEM(v_sessions, m_pi.m_cap.stBit.game_master, ==, 1);

#if defined(_WIN32)
			LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
			pthread_mutex_unlock(&m_cs);
#endif

			packet p((unsigned short)0x7C);

			if (i != v_sessions.end())
				master = *i;
			else
				master = v_sessions.front();

			m_ri.master = master->m_pi.uid;

			if (master->m_pi.m_cap.stBit.game_master/* & 4*/)
				m_ri.state_flag = 0x100;	// GM
			else
				m_ri.state_flag = 0;		// Normal Player

			updatePlayerInfo(*master);

			p.addUint32(master->m_oid);

			p.addInt16(0);	// option ACHO

			packet_func::room_broadcast(*this, p, 0);
		}
	}
};

void room::updateGuild(player& _session) {

	if (_session.m_pi.gi.uid == ~0u)
		throw exception("[channel::UpdateGuild][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] player nao esta em uma guild.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 9, 0));

	PlayerRoomInfoEx *pri = getPlayerInfo(&_session);

	if (pri == nullptr)
		throw exception("[channel::UpdateGuild][Error] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] nao tem o info do player na sala[NUMERO=" + std::to_string(m_ri.numero) + "]. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 10, 0));

	Guild *guild = nullptr;

	if (m_ri.guilds.guild_1_uid == 0 && m_ri.guilds.guild_2_uid != _session.m_pi.gi.uid) {

		m_ri.guilds.guild_1_uid = _session.m_pi.gi.uid;
#if defined(_WIN32)
		memcpy_s(m_ri.guilds.guild_1_nome, sizeof(m_ri.guilds.guild_1_nome), _session.m_pi.gi.name, sizeof(m_ri.guilds.guild_1_nome));
		memcpy_s(m_ri.guilds.guild_1_mark, sizeof(m_ri.guilds.guild_1_mark), _session.m_pi.gi.mark_emblem, sizeof(m_ri.guilds.guild_1_mark));
#elif defined(__linux__)
		memcpy(m_ri.guilds.guild_1_nome, _session.m_pi.gi.name, sizeof(m_ri.guilds.guild_1_nome));
		memcpy(m_ri.guilds.guild_1_mark, _session.m_pi.gi.mark_emblem, sizeof(m_ri.guilds.guild_1_mark));
#endif
		m_ri.guilds.guild_1_index_mark = (unsigned short)_session.m_pi.gi.index_mark_emblem;

		// Team Red
		pri->state_flag.uFlag.stFlagBit.team = 0;

		// Add a guild Vermelha
		guild = m_guild_manager.addGuild(Guild::eTEAM::RED, m_ri.guilds.guild_1_uid);

	}else if (m_ri.guilds.guild_1_uid == _session.m_pi.gi.uid) {

		// Team Red
		pri->state_flag.uFlag.stFlagBit.team = 0;

		// Find Guild Vermelha
		guild = m_guild_manager.findGuildByTeam(Guild::eTEAM::RED);

	}else if (m_ri.guilds.guild_2_uid == 0) {

		m_ri.guilds.guild_2_uid = _session.m_pi.gi.uid;
#if defined(_WIN32)
		memcpy_s(m_ri.guilds.guild_2_nome, sizeof(m_ri.guilds.guild_2_nome), _session.m_pi.gi.name, sizeof(m_ri.guilds.guild_2_nome));
		memcpy_s(m_ri.guilds.guild_2_mark, sizeof(m_ri.guilds.guild_2_mark), _session.m_pi.gi.mark_emblem, sizeof(m_ri.guilds.guild_2_mark));
#elif defined(__linux__)
		memcpy(m_ri.guilds.guild_2_nome, _session.m_pi.gi.name, sizeof(m_ri.guilds.guild_2_nome));
		memcpy(m_ri.guilds.guild_2_mark, _session.m_pi.gi.mark_emblem, sizeof(m_ri.guilds.guild_2_mark));
#endif
		m_ri.guilds.guild_2_index_mark = (unsigned short)_session.m_pi.gi.index_mark_emblem;

		// Team Blue
		pri->state_flag.uFlag.stFlagBit.team = 1;

		// Add a guild Azul
		guild = m_guild_manager.addGuild(Guild::eTEAM::BLUE, m_ri.guilds.guild_2_uid);

	}else {

		// Team Blue
		pri->state_flag.uFlag.stFlagBit.team = 1;

		// Find Guild Azul
		guild = m_guild_manager.findGuildByTeam(Guild::eTEAM::BLUE);
	}

	if (guild != nullptr)
		guild->addPlayer(_session);
	else
		_smp::message_pool::getInstance().push(new message("[room::updateGuild][WARNING] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] tentou entrar em uma guild da sala[NUMERO=" + std::to_string(m_ri.numero) 
				+  "], mas nao conseguiu criar ou achar nenhum guild na sala. Bug.", CL_FILE_LOG_AND_CONSOLE));

	// Add player ao team
	m_teans[pri->state_flag.uFlag.stFlagBit.team].addPlayer(&_session);
};

void room::clear_player_kicked() {

	if (!m_player_kicked.empty())
		m_player_kicked.clear();
};

void room::addPlayerKicked(uint32_t _uid) {

	if (isKickedPlayer(_uid))
		_smp::message_pool::getInstance().push(new message("[room::addPlayerKicked][Error][WARNING] player[UID=" + std::to_string(_uid) + "] ja foi chutado da sala[NUMERO="
				+ std::to_string(m_ri.numero) + "]", CL_FILE_LOG_AND_CONSOLE));
	else
		m_player_kicked[_uid] = true;
};

inline bool room::isKickedPlayer(uint32_t _uid) {
	return m_player_kicked.find(_uid) != m_player_kicked.end();
};

bool room::isAllReady() {

	auto master = findMaster();

	if (master == nullptr)
		return false;

	// Bot Tourney, Short Game and Special Shuffle Course
	if (m_bot_tourney && v_sessions.size() == 1 && (m_ri.tipo == RoomInfo::TOURNEY || m_ri.tipo == RoomInfo::SPECIAL_SHUFFLE_COURSE))
		return true;

	// se a sala for Practice, CHIP-IN Practice, e GRAND_PRIX_NOVICE não precisa o player está pronto
	if (m_ri.tipo == RoomInfo::TIPO::PRACTICE || m_ri.tipo == RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE
			|| (m_ri.tipo == RoomInfo::TIPO::GRAND_PRIX /*GP tem sua classe sobrecarregada, ele verifica sozinho o isAllready()*/))
		return true;

	// Se o master for GM então não precisar todos está ready(prontos)
	if (master->m_pi.m_cap.stBit.game_master && !_haveInvited())
		return true;

	auto count = std::count_if(v_sessions.begin(), v_sessions.end(), [&](auto& _el) {

		auto pri = getPlayerInfo(_el);

		return (pri != nullptr && pri->state_flag.uFlag.stFlagBit.ready);	// Pronto(Ready
	});

	// Conta com o master por que o master sempre está pronto(ready)
	return (count == v_sessions.size());
};

void room::finish_game() {

	if (m_pGame != nullptr) {
		packet p;

		// Deleta o jogo
		delete m_pGame;

		// Zera Player Flags
		for (auto& el : m_player_info) {
			
			// Update Place player
			if (m_ri.tipo == RoomInfo::PRACTICE || m_ri.tipo == RoomInfo::GRAND_ZODIAC_PRACTICE)
				el.first->m_pi.place = 2u;
			else
				el.first->m_pi.place = 0u;

			el.second.state_flag.uFlag.stFlagBit.away = 0;
			
			// Aqui só zera quem não é Master da sala, o master deixa sempre ready
			if (m_ri.master == el.first->m_pi.uid)
				el.second.state_flag.uFlag.stFlagBit.ready = 1;
			else
				el.second.state_flag.uFlag.stFlagBit.ready = 0/*Unready*/;

			// Update player info
			updatePlayerInfo(*el.first);

			// Send update on room
			sendCharacter(*el.first, 3);
		}

		// Atualiza flag da sala, só não atualiza se for GM evento ou GZ Event e SSC
		if (!(m_ri.trofel == TROFEL_GM_EVENT_TYPEID/*GM event não abre a sala de novo depois que acaba*/ || m_ri.tipo == RoomInfo::SPECIAL_SHUFFLE_COURSE || m_ri.master == -2/*GP e GZ por tempo*/))
			m_ri.state = 1;	/*em espera*/

		// Att Exp rate, e Pang rate, que criou a sala, att ele também quando começa o jogo
		//if (sgs::gs != nullptr) {
			m_ri.rate_exp = sgs::gs::getInstance().getInfo().rate.exp;
			m_ri.rate_pang = sgs::gs::getInstance().getInfo().rate.pang;
			m_ri.angel_event = sgs::gs::getInstance().getInfo().rate.angel_event;
		//}

		// Update Course of Hole
		if (m_ri.course >= 0x7Fu)					// Random Course With Course already draw
			m_ri.course = RoomInfo::eCOURSE(0x7Fu);	// Random Course standard

		// Update Master da sala
		updateMaster(nullptr);

		if (m_ri.master == -2)
			m_ri.master = -1; // pode deletar a sala quando sair todos

		if (v_sessions.size() > 0) {
			// Atualiza info da sala para quem está na sala
			packet_func::pacote04A(p, m_ri, -1);
			packet_func::room_broadcast(*this, p, 1);
		}

		// limpa lista de player kikados
		clear_player_kicked();

		// Verifica se o Bot Tourney está ativo, kika bot e limpa a flag
		if (m_bot_tourney) {

			auto pMaster = findMaster();
			
			if (pMaster != nullptr) {

				try {
					// Kick Bot
					// Atualiza os player que estão na sala que o Bot sai por que ele é só visual
					sendCharacter(*pMaster, 0);
				
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[room::finish_game::KickBotTourney][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			}

			m_bot_tourney = false;
		}

		// Terminou o jogo
		m_pGame = nullptr;
	}
};

void room::clear_invite() {

	if (!v_invite.empty()) {

		v_invite.clear();
		v_invite.shrink_to_fit();
	}
};

void room::init_teans() {

	// Limpa teans, se tiver teans inicilizados já
	clear_teans();

	// Init Teans
	m_teans.push_back(Team(0/*Red*/));
	m_teans.push_back(Team(1/*Blue*/));

	PlayerRoomInfo *pPri = nullptr;

	// Add Players All Seus Respectivos teans
	for (auto& el : v_sessions) {

		if ((pPri = getPlayerInfo(el)) == nullptr)
			throw exception("[room::init_teans][Error] nao encontrou o info do player[UID=" + std::to_string(el->m_pi.uid) + "] na sala. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1504, 0));

		m_teans[pPri->state_flag.uFlag.stFlagBit.team].addPlayer(el);
	}
}

void room::clear_teans() {

	if (!m_teans.empty()) {
		m_teans.clear();
		m_teans.shrink_to_fit();
	}
};

void room::addBotVisual(player& _session) {
	CHECK_SESSION_BEGIN("addBotVisual");

	// Add Bot
	std::vector< PlayerRoomInfoEx > v_element;
	PlayerRoomInfoEx pri, *tmp_pri = nullptr;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		std::for_each(v_sessions.begin(), v_sessions.end(), [&](auto& _el) {
			if ((tmp_pri = getPlayerInfo(_el)) != nullptr)
				v_element.push_back(*tmp_pri);
		});

		if (v_element.empty())
			throw exception("[room::makeBot][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou criar Bot na sala[NUMERO="
				+ std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) + "], mas nao nenhum player na sala. Bug", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM, 1, 5000));

		// Inicializa os dados do Bot
		pri.uid = _session.m_pi.uid;
		pri.oid = _session.m_oid;
		pri.position = 0;	// 0 Que é para ele ficar em primeiro e parece que tbm não deixa kick(ACHO)
		pri.state_flag.uFlag.stFlagBit.ready = 1u;
		pri.char_typeid = 0x4000000u;	// Nuri
		pri.title = 0x39800013u;		// Title Helper
#if defined(_WIN32)
		strcpy_s(pri.nickname, "\\1Bot");
#elif defined(__linux__)
		strcpy(pri.nickname, "\\1Bot");
#endif

		// Add o Bot a sala, só no visual
		v_element.push_back(pri);

		// Packet
		packet p;

		// Option 0, passa todos que estão na sala
		if (packet_func::pacote048(p, &_session, v_element, 0x100))
			packet_func::room_broadcast(*this, p, 1);

		// Option 1, passa só o player que entrou na sala, nesse caso foi o Bot
		if (packet_func::pacote048(p, &_session, std::vector< PlayerRoomInfoEx > { pri  }, 0x101))
			packet_func::room_broadcast(*this, p, 1);

		// Criou Bot com sucesso
		m_bot_tourney = true;

		// Log
		_smp::message_pool::getInstance().push(new message("[room::addBotVisual][Log] Player[UID=" + std::to_string(_session.m_pi.uid)
				+ "] Room[NUMBER=" + std::to_string(m_ri.numero) + ", MASTER=" + std::to_string(m_ri.master) + "] Bot criado com sucesso.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch(exception& e) {
		UNREFERENCED_PARAMETER(e);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		// Relança
		throw;
	}
};

bool room::isDropRoom() {
	return true; // class room normal é sempre true
};

uint32_t room::_getRealNumPlayersWithoutInvited() {
	return (uint32_t)std::count_if(v_sessions.begin(), v_sessions.end(), [this](auto _el) {

		if (_el == nullptr)
			return false;

		auto it = m_player_info.find(_el);

		return it != m_player_info.end() && !it->second.convidado;
	});
};

bool room::_haveInvited() {
	return std::find_if(v_sessions.begin(), v_sessions.end(), [this](auto _el) {
		
		// session invalid, pula ela
		if (_el == nullptr)
			return false;

		auto it = m_player_info.find(_el);

		return it != m_player_info.end() && it->second.convidado;
	}) != v_sessions.end();
};

void room::calcRainLounge() {

	// Só calcRainLounge se for lounge
	if (m_ri.tipo == RoomInfo::TIPO::LOUNGE) {

		m_weather_lounge = 0u;	// Good Weather

		unsigned short rate_rain = sgs::gs::getInstance().getInfo().rate.chuva;

		Lottery loterry((uint64_t)this);

		auto rate_good_weather = (rate_rain <= 0) ? 1000 : ((rate_rain < 1000) ? 1000 - rate_rain : 1);

		loterry.push(rate_good_weather, 0);
		loterry.push(rate_good_weather, 0);
		loterry.push(rate_good_weather, 0);
		loterry.push(rate_rain, 2/*Rain*/);

		auto lc = loterry.spinRoleta();

		if (lc != nullptr)
			m_weather_lounge = (unsigned char)lc->value;
	}
};
