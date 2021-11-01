// Arquivo guild_room_manager.cpp
// Criado em 26/12/2019 as 21:19 por Acrisio
// Implementa��o da classe GuildRoomManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "guild_room_manager.hpp"

#include <algorithm>
#include <random>

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_register_guild_match.hpp"
#include "../PANGYA_DB/cmd_update_guild_points.hpp"

using namespace stdA;

GuildRoomManager::GuildRoomManager() : m_dupla_manager(), v_guilds(), m_guild_win(eGUILD_WIN::DRAW) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

GuildRoomManager::~GuildRoomManager() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!v_guilds.empty()) {
		v_guilds.clear();
		v_guilds.shrink_to_fit();
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

Guild* GuildRoomManager::addGuild(Guild::eTEAM _team, uint32_t _uid) {

	Guild *guild = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	v_guilds.push_back(Guild(_uid, _team));

	guild = &(v_guilds.back());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return guild;
}

Guild* GuildRoomManager::addGuild(Guild& _guild) {

	Guild *guild = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	v_guilds.push_back(_guild);

	guild = &(v_guilds.back());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return guild;
}

void GuildRoomManager::deleteGuild(Guild* _guild) {

	if (_guild == nullptr) {

		_smp::message_pool::getInstance().push(new message("[GuildRoomManager::deleteGuild][Error] _guild is invalid(nullptr). Bug.", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_guilds.begin(), v_guilds.end(), [&](auto& _el) {
		return _el.getUID() == _guild->getUID() && (&_el) == _guild;
	});

	if (it != v_guilds.end())
		v_guilds.erase(it);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

uint32_t GuildRoomManager::getNumGuild() {
	return (uint32_t)v_guilds.size();
}

GuildRoomManager::eGUILD_WIN GuildRoomManager::getGuildWin() {
	return m_guild_win;
}

Guild* GuildRoomManager::findGuildByTeam(Guild::eTEAM _team) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_guilds.begin(), v_guilds.end(), [&](auto& _el) {
		return _el.getTeam() == _team;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_guilds.end()) ? &(*it) : nullptr;
}

Guild* GuildRoomManager::findGuildByUID(uint32_t _uid) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_guilds.begin(), v_guilds.end(), [&](auto& _el) {
		return _el.getUID() == _uid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_guilds.end()) ? &(*it) : nullptr;
}

Guild* GuildRoomManager::findGuildByPlayer(player& _session) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_guilds.begin(), v_guilds.end(), [&](auto& _el) {
		return _el.findPlayerByUID(_session.m_pi.uid) != nullptr;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_guilds.end()) ? &(*it) : nullptr;
}

Dupla* GuildRoomManager::findDupla(player& _session) {
	return m_dupla_manager.findDuplaByPlayer(_session);
}

void GuildRoomManager::init_duplas() {

	if (v_guilds.size() != 2) {

		_smp::message_pool::getInstance().push(new message("[GuildRoomManager::init_duplas][Error] nao tem duas guilds[NUM=" 
				+ std::to_string(v_guilds.size()) + "] para inicializar as duplas. Bug.", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	m_dupla_manager.init_duplas(v_guilds.front(), v_guilds.back());

	m_guild_win = eGUILD_WIN::DRAW;
}

int GuildRoomManager::isGoodToStart() {

	// S� tem uma ou nenhuma guild na sala
	if (v_guilds.size() <= 1)
		return 0;

	auto last_players = -1l;
	
	int ret = 1;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_guilds) {
		
		// Não tem o mesmo número de jogadores na sala as guilds
		if (last_players != -1l && last_players != el.numPlayers()) {
			
			ret = -1;
			
			break;
		}

		last_players = (int)el.numPlayers();

		// Uma Guild tem menos que 2 jogadores na sala
		if (last_players < 2) {
			
			ret = -2;

			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ret;
}

bool GuildRoomManager::oneGuildRest() {

	if (v_guilds.size() <= 1)
		return true;

	return m_dupla_manager.oneGuildRest();
}

void GuildRoomManager::update() {

	if (v_guilds.size() > 0)
		m_dupla_manager.updateGuildDados(&(v_guilds.front()), (v_guilds.size() == 2 ? &(v_guilds.back()) : nullptr));
}

void GuildRoomManager::calcGuildWin() {

	m_guild_win = eGUILD_WIN::DRAW;

	// Calcula Guild Win
	if (v_guilds.size() > 1u) {

		if (v_guilds.back().numPlayers() == 0u || m_dupla_manager.getNumPlayersQuitGuild(v_guilds.back().getUID()) == v_guilds.back().numPlayers()
				|| (v_guilds.front().getPoint() > v_guilds.back().getPoint() || (v_guilds.front().getPoint() == v_guilds.back().getPoint() && v_guilds.front().getPang() > v_guilds.back().getPang())))
			m_guild_win = eGUILD_WIN(v_guilds.front().getTeam());
		else if (v_guilds.front().numPlayers() == 0u || m_dupla_manager.getNumPlayersQuitGuild(v_guilds.front().getUID()) == v_guilds.front().numPlayers()
				|| (v_guilds.back().getPoint() > v_guilds.front().getPoint() || (v_guilds.back().getPoint() == v_guilds.front().getPoint() && v_guilds.back().getPang() > v_guilds.front().getPang())))
			m_guild_win = eGUILD_WIN(v_guilds.back().getTeam());

	}else if (v_guilds.size() > 0u)
		m_guild_win = eGUILD_WIN(v_guilds.front().getTeam());

	// Calcula Guild Pang Win
	uint32_t pang_winner = (m_dupla_manager.getNumDuplas() + m_dupla_manager.getNumPlayersQuit()) * 50u;
	uint32_t pang_loser = m_dupla_manager.getNumPlayersQuit() * 50u + 50u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_guilds)
		m_dupla_manager.updatePangWinDuplas(el.getUID(), ((unsigned char)el.getTeam() == (unsigned char)m_guild_win) ? pang_winner : pang_loser);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	update();
}

void GuildRoomManager::saveGuildsData() {

	// Update Guild Point and Pang Win
	GuildPoints gp{ 0u };

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_guilds) {

		gp.clear();

		gp.uid = el.getUID();
		gp.point = el.getPoint();
		gp.pang = el.getPangWin();
		gp.win = (m_guild_win == GuildRoomManager::eGUILD_WIN::DRAW ? GuildPoints::eGUILD_WIN::DRAW : ((unsigned char)el.getTeam() == (unsigned char)m_guild_win ? GuildPoints::eGUILD_WIN::WIN : GuildPoints::eGUILD_WIN::LOSE));

		NormalManagerDB::add(2, new CmdUpdateGuildPoints(gp), GuildRoomManager::SQLDBResponse, this);
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	// Update Guild Members Point and Pang Win
	m_dupla_manager.saveGuildMembersData();

	// Register Guild Match
	if (v_guilds.size() > 1) {
		
		GuildMatch match{ 0u };

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Guild 1
		match.uid[0] = v_guilds.front().getUID();
		match.pang[0] = (uint32_t)v_guilds.front().getPang();
		match.point[0] = v_guilds.front().getPoint();

		// Guild 2
		match.uid[1] = v_guilds.back().getUID();
		match.pang[1] = (uint32_t)v_guilds.back().getPang();
		match.point[1] = v_guilds.back().getPoint();

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		NormalManagerDB::add(1, new CmdRegisterGuildMatch(match), GuildRoomManager::SQLDBResponse, this);
	}
}

void GuildRoomManager::initPacketDuplas(packet& _p) {

	m_dupla_manager.initPacketDuplas(_p);
}

bool GuildRoomManager::finishHoleDupla(PlayerGameInfo& _pgi, unsigned short _seq_hole) {

	if (m_dupla_manager.finishHoleDupla(_pgi, _seq_hole)) {

		update();

		return true;
	}

	return false;
}

void GuildRoomManager::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[GuildRoomManager::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[GuildRoomManager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_channel = reinterpret_cast< GuildRoomManager* >(_arg);

	switch (_msg_id) {
	case 1:	// Register Guild Match
	{
		auto cmd_rgm = reinterpret_cast< CmdRegisterGuildMatch* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[GuildRoomManager::SQLDBResponse][Log] Registrou Guild Match[GUILD_1_UID=" + std::to_string(cmd_rgm->getInfo().uid[0]) 
				+ ", GUILD_1_POINT=" + std::to_string(cmd_rgm->getInfo().point[0]) + ", GUILD_1_PANG=" + std::to_string(cmd_rgm->getInfo().pang[0]) + ", GUILD_2_UID=" + std::to_string(cmd_rgm->getInfo().uid[1]) 
				+ ", GUILD_2_POINT=" + std::to_string(cmd_rgm->getInfo().point[1]) + ", GUILD_2_PANG=" + std::to_string(cmd_rgm->getInfo().pang[1]) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 2:	// Guild Update Points
	{
		auto cmd_ugp = reinterpret_cast< CmdUpdateGuildPoints* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[GuildRoomManager::SQLDBResponse][Log] Atualizou o Guild[UID=" + std::to_string(cmd_ugp->getInfo().uid) 
				+ ", WIN=" + std::to_string((unsigned short)cmd_ugp->getInfo().win) + "] Points[POINT=" + std::to_string(cmd_ugp->getInfo().point) 
				+ ", PANG=" + std::to_string(cmd_ugp->getInfo().pang) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
}
