// Arquivo friend_manager.cpp
// Criado em 04/08/2018 as 19:52 por Acrisio
// Implementa��o da classe FriendManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "friend_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PANGYA_DB/cmd_friend_info.hpp"
#include "../PANGYA_DB/cmd_add_friend.hpp"
#include "../PANGYA_DB/cmd_delete_friend.hpp"
#include "../PANGYA_DB/cmd_update_friend.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include <algorithm>

using namespace stdA;

FriendManager::FriendManager() : m_pi{0}, m_friend(), m_state(false) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

FriendManager::FriendManager(player_info& _pi) : m_pi(_pi), m_friend(), m_state(false) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

FriendManager::~FriendManager() {
	clear();
	
#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void FriendManager::init(player_info& _pi) {

	if (isInitialized())
		clear();

	// Atualiza
	m_pi = _pi;

	if (m_pi.uid == 0)
		throw exception("[FriendManager::init][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FRIEND_MANAGER, 1, 0));

	CmdFriendInfo cmd_fi(m_pi.uid, CmdFriendInfo::ALL, 0u, true);	// Waiter

	NormalManagerDB::add(0, &cmd_fi, nullptr, nullptr);

	cmd_fi.waitEvent();

	if (cmd_fi.getException().getCodeError() != 0)
		throw cmd_fi.getException();

	m_friend = cmd_fi.getInfo();
	
	m_state = true;
}

void FriendManager::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_friend.empty())
		m_friend.clear();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	m_pi.clear();

	m_state = false;
}

bool FriendManager::isInitialized() {
	return m_state;
}

// Counters
uint32_t FriendManager::countAllFriend() {
	
	size_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	count = m_friend.size();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (uint32_t)count;
}

uint32_t FriendManager::countGuildMember() {

	size_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	count = std::count_if(m_friend.begin(), m_friend.end(), [&](auto& el) {
		return el.second.flag.stFlag.guild_member;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (uint32_t)count;
}

uint32_t FriendManager::countFriend() {

	size_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	count = std::count_if(m_friend.begin(), m_friend.end(), [&](auto& el) {
		return el.second.flag.stFlag._friend;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (uint32_t)count;
}

void FriendManager::requestAddFriend(FriendInfoEx& _fi) {
	
	// UPDATE ON SERVER
	addFriend(_fi);

	// UPDATE ON DB
	NormalManagerDB::add(1, new CmdAddFriend(m_pi.uid, _fi), FriendManager::SQLDBResponse, this);
}

void FriendManager::requestDeleteFriend(FriendInfoEx& _fi) {

	requestDeleteFriend(_fi.uid);
}

void FriendManager::requestDeleteFriend(uint32_t _uid) {

	// UPDATE ON SERVER
	deleteFriend(_uid);

	// UPDATE ON DB
	NormalManagerDB::add(2, new CmdDeleteFriend(m_pi.uid, _uid), FriendManager::SQLDBResponse, this);
}

void FriendManager::requestUpdateFriendInfo(FriendInfoEx& _fi) {

	if (_fi.uid == 0)
		throw exception("[FriendManager::requestUpdateFriendInfo][Error] _fi.uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FRIEND_MANAGER, 1, 0));

	// UPDATE ON DB
	NormalManagerDB::add(3, new CmdUpdateFriend(m_pi.uid, _fi), FriendManager::SQLDBResponse, this);
}

// add Friend
void FriendManager::addFriend(FriendInfoEx& _fi) {

	if (_fi.uid == 0)
		throw exception("[FriendManager::addFriend][Error] player[UID=" + std::to_string(m_pi.uid) + "] tentou adicionar um amigo[UID=" 
				+ std::to_string(_fi.uid) + "], mas o uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FRIEND_MANAGER, 1, 0));

	if (_fi.flag.stFlag.guild_member && m_pi.guild_uid == 0)
		throw exception("[FriendManager::addFriend][Error] player[UID=" + std::to_string(m_pi.uid) + "] tentou adicionar um Guild Member[UID=" 
				+ std::to_string(_fi.uid) + "], mas ele nao esta em nenhum Guild. Hacker ou Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FRIEND_MANAGER, 2, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = m_friend.find(_fi.uid);

	if (it == m_friend.end())	// add new friend ou Guild Member
		m_friend[_fi.uid] = _fi;
	else if (it->second.flag.ucFlag != 3 && it->second.flag.ucFlag != _fi.flag.ucFlag)	// Add Guild Member ou Friend
		it->second.flag.ucFlag |= _fi.flag.ucFlag;
	else // j� tem o amigo na guild e em amigos
		_smp::message_pool::getInstance().push(new message("[FriendManager::addFriend][Error][Warning] player[UID=" + std::to_string(m_pi.uid) + "] ja tem esse Amigo[UID=" + std::to_string(_fi.uid) + "]", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

// delete Friend
void FriendManager::deleteFriend(FriendInfoEx& _fi) {

	deleteFriend(_fi.uid);
}

void FriendManager::deleteFriend(uint32_t _uid) {

	if (_uid == 0)
		throw exception("[FriendManager::deleteFriend][Error] player[UID=" + std::to_string(m_pi.uid) + "] tentou adicionar um amigo[UID="
			+ std::to_string(_uid) + "], mas o uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::FRIEND_MANAGER, 1, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = m_friend.find(_uid);

	if (it != m_friend.end())
		m_friend.erase(it);
	else
		_smp::message_pool::getInstance().push(new message("[FriendManager::deleteFriend][Error][Warning] player[UID=" + std::to_string(m_pi.uid) + "] tentou deletat amigo[UID="
				+ std::to_string(_uid) + "] do map, mas ele nao existe no map.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

// Finders
FriendInfoEx* FriendManager::findFriendInAllFriend(uint32_t _uid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_friend.begin(), m_friend.end(), [&](auto& el) {
		return el.second.uid == _uid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it == m_friend.end() ? nullptr : &it->second);
}

FriendInfoEx* FriendManager::findGuildMember(uint32_t _uid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_friend.begin(), m_friend.end(), [&](auto& el) {
		return el.second.flag.stFlag.guild_member && el.second.uid == _uid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it == m_friend.end() ? nullptr : &it->second);
}

FriendInfoEx* FriendManager::findFriend(uint32_t _uid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_friend.begin(), m_friend.end(), [&](auto& el) {
		return el.second.flag.stFlag._friend && el.second.uid == _uid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it == m_friend.end() ? nullptr : &it->second);
}

std::vector< FriendInfoEx* > FriendManager::getAllFriend(bool _block) {
	
	std::vector< FriendInfoEx * > v_friend;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// Os Amigos que n�o estiverem bloqueados
	std::for_each(m_friend.begin(), m_friend.end(), [&](auto& el) {
		if (el.second.flag.stFlag._friend && (!_block || !el.second.state.stState.block))
			v_friend.push_back(&el.second);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_friend;
}

std::vector< FriendInfoEx* > FriendManager::getAllGuildMember() {
	
	std::vector< FriendInfoEx * > v_friend;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	std::for_each(m_friend.begin(), m_friend.end(), [&](auto& el) {
		if (el.second.flag.stFlag.guild_member)
			v_friend.push_back(&el.second);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_friend;
}

std::vector< FriendInfoEx* > FriendManager::getAllFriendAndGuildMember(bool _block) {

	std::vector< FriendInfoEx * > v_friend;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// Os Amigos que n�o estiverem bloqueados
	std::for_each(m_friend.begin(), m_friend.end(), [&](auto& el) {
		if (!_block || !el.second.state.stState.block)
			v_friend.push_back(&el.second);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_friend;
}

void FriendManager::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[FriendManager::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[FriendManager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_server = reinterpret_cast< FriendManager* >(_arg);

	switch (_msg_id) {
	case 1:	// Add Friend
	{
		auto cmd_af = reinterpret_cast< CmdAddFriend* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[FriendManager::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_af->getUID()) + "] adicionou Amigo[UID="
				+ std::to_string(cmd_af->getInfo().uid) + ", APELIDO=" + std::string(cmd_af->getInfo().apelido) + ", NICK="
				+ std::string(cmd_af->getInfo().nickname) + ", STATE=" + std::to_string((unsigned short)cmd_af->getInfo().state.ucState) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 2:	// Delete Friend
	{
		auto cmd_df = reinterpret_cast< CmdDeleteFriend* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[FriendManager::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_df->getUID()) + "] deletou Amigo[UID=" + std::to_string(cmd_df->getFriendUID()) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 3:	// Update Friend Info
	{
		auto cmd_ufi = reinterpret_cast< CmdUpdateFriend* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[FriendManager::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_ufi->getUID()) + "] atualizou Info do Amigo[UID="
				+ std::to_string(cmd_ufi->getInfo().uid) + ", APELIDO=" + std::string(cmd_ufi->getInfo().apelido) + ", UNK1="
				+ std::to_string(cmd_ufi->getInfo().lUnknown) + ", UNK2=" + std::to_string(cmd_ufi->getInfo().lUnknown2) + ", UNK3=" + std::to_string(cmd_ufi->getInfo().lUnknown3) + ", UNK4="
				+ std::to_string(cmd_ufi->getInfo().lUnknown4) + ", UNK5=" + std::to_string(cmd_ufi->getInfo().lUnknown5) + ", UNK6=" + std::to_string(cmd_ufi->getInfo().lUnknown6) + ", UNK_FLAG="
				+ std::to_string((short)cmd_ufi->getInfo().cUnknown_flag) + ", STATE=" + std::to_string((unsigned char)cmd_ufi->getInfo().state.ucState) + "]", CL_FILE_LOG_AND_CONSOLE));
		break;
	}
	case 0:
	default:
		break;
	}
};
