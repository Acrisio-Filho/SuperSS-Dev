// Arquivo room_manager.cpp
// Criado em 14/02/2018 as 21:51 por Acrisio
// Implementação da classe RoomManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "room_manager.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

// Verifica se é um room válida e bloquea ela
#if defined(_WIN32)
#define WAIT_ROOM_UNLOCK(_method) DWORD lastError = 0u; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		if (SleepConditionVariableCS(&m_cv, &m_cs, 1000/*1 second*/) == 0 && (lastError = GetLastError()) != ERROR_TIMEOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = *it; \
	}
#elif defined(__linux__)
#define WAIT_ROOM_UNLOCK(_method) int lastError = 0; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		timespec wait_time = _milliseconds_to_timespec_clock_realtime(1000/*1 second*/); \
\
		if ((lastError = pthread_cond_timedwait(&m_cv, &m_cs, &wait_time)) != 0 && lastError != ETIMEDOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = *it; \
	}
#endif

// Verifica se é um room grand prix válida e bloquea ela
#if defined(_WIN32)
#define WAIT_ROOM_GP_UNLOCK(_method) DWORD lastError = 0u; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		if (SleepConditionVariableCS(&m_cv, &m_cs, 1000/*1 second*/) == 0 && (lastError = GetLastError()) != ERROR_TIMEOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = (RoomGrandPrix*)*it; \
	}
#elif defined(__linux__)
#define WAIT_ROOM_GP_UNLOCK(_method) int lastError = 0; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		timespec wait_time = _milliseconds_to_timespec_clock_realtime(1000/*1 second*/); \
\
		if ((lastError = pthread_cond_timedwait(&m_cv, &m_cs, &wait_time)) != 0 && lastError != ETIMEDOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = (RoomGrandPrix*)*it; \
	}
#endif

// Verifica se é um room grand zodiac event válida e bloquea ela
#if defined(_WIN32)
#define WAIT_ROOM_GZE_UNLOCK(_method) DWORD lastError = 0u; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		if (SleepConditionVariableCS(&m_cv, &m_cs, 1000/*1 second*/) == 0 && (lastError = GetLastError()) != ERROR_TIMEOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = (RoomGrandZodiacEvent*)*it; \
	}
#elif defined(__linux__)
#define WAIT_ROOM_GZE_UNLOCK(_method) int lastError = 0; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		timespec wait_time = _milliseconds_to_timespec_clock_realtime(1000/*1 second*/); \
\
		if ((lastError = pthread_cond_timedwait(&m_cv, &m_cs, &wait_time)) != 0 && lastError != ETIMEDOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = (RoomGrandZodiacEvent*)*it; \
	}
#endif

// Verifica se é um room bot gm event válida e bloquea ela
#if defined(_WIN32)
#define WAIT_ROOM_BGE_UNLOCK(_method) DWORD lastError = 0u; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		if (SleepConditionVariableCS(&m_cv, &m_cs, 1000/*1 second*/) == 0 && (lastError = GetLastError()) != ERROR_TIMEOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = (RoomBotGMEvent*)*it; \
	}
#elif defined(__linux__)
#define WAIT_ROOM_BGE_UNLOCK(_method) int lastError = 0; \
\
	while (r != nullptr && !r->tryLock()) { \
\
		timespec wait_time = _milliseconds_to_timespec_clock_realtime(1000/*1 second*/); \
\
		if ((lastError = pthread_cond_timedwait(&m_cv, &m_cs, &wait_time)) != 0 && lastError != ETIMEDOUT) \
			throw exception("[RoomManager::" + std::string((_method)) + "][Error] SleepConditionVariableCS wait room unlock. Error Code System: " + std::to_string(lastError), \
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 10, lastError)); \
\
		/* Check if room is valid */ \
		auto it = std::find_if(v_rooms.begin(), v_rooms.end(), [&](auto& _el) { \
			return _el == r; \
		}); \
\
		/* Sala ainda é válida */ \
		if (it != v_rooms.end()) \
			r = (RoomBotGMEvent*)*it; \
	}
#endif

using namespace stdA;

RoomManager::RoomManager(unsigned char _channel_id) : m_channel_id(_channel_id) {
	memset(m_map_index, 0, USHRT_MAX);

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
	InitializeConditionVariable(&m_cv);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	pthread_cond_init(&m_cv, nullptr);
#endif
};

RoomManager::~RoomManager() {
	destroy();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
	pthread_cond_destroy(&m_cv);
#endif
};

void RoomManager::destroy() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_rooms) {

		if (el != nullptr) {
			
			// Sala está destruindo
			el->setDestroying();

			// Libera a sala se ela estiver bloqueada
			el->unlock();

			// Deleta o Objeto
			delete el;

			// Limpa o ponteiro
			el = nullptr;
		}
	}

	v_rooms.clear();
	v_rooms.shrink_to_fit();

	m_channel_id = (unsigned char)~0;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

room* RoomManager::makeRoom(unsigned char _channel_owner, RoomInfoEx _ri, player* _session, int _option) {
	room *r = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (_session != nullptr && _session->m_pi.mi.sala_numero != -1)
			throw exception("[RoomManager::makeRoom][Error] Player[UID=" + std::to_string(_session->m_pi.uid)
					+ "] sala[NUMERO=" + std::to_string(_session->m_pi.mi.sala_numero) + "], ja esta em outra sala, nao pode criar outra. Hacker ou Bug.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 120, 0));

		_ri.numero = getNewIndex();

		if (_option == 0 && _session != nullptr)
			_ri.master = _session->m_pi.uid;
		else if (_option == 1)	// Room Sem Master Grand Prix ou Grand Zodiac Event Time
			_ri.master = -2;
		else // Room sem master
			_ri.master = -1;

		r = new room(_channel_owner, _ri);

		if (r == nullptr)
			throw exception("[RoomManager::makeRoom][Error] Player[UID=" + std::to_string(_session->m_pi.uid) 
					+ "] tentou criar a sala[TIPO=" + std::to_string((unsigned short)_ri.tipo) + "], mas nao conseguiu criar o objeto da classe room. Bug.", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 130, 0));

		// Verifica se é um room válida e bloquea ela
		WAIT_ROOM_UNLOCK("makeRoom");

		// Adiciona a sala no Vector
		v_rooms.push_back(r);

		if (_session != nullptr)
			r->enter(*_session);

		// Log
		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoom][Log] Channel[ID=" + std::to_string((unsigned short)m_channel_id)
				+ "] Maked Room[TIPO=" + std::to_string((unsigned short)r->getInfo()->tipo) + ", NUMERO="
				+ std::to_string(r->getNumero()) + ", MASTER=" + std::to_string((int)r->getMaster()) + "]", CL_FILE_LOG_AND_CONSOLE));

		//_smp::message_pool::getInstance().push(new message("Key Room Dump.\n\r" + hex_util::BufferToHexString((unsigned char*)r->getInfo()->key, 16), CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (r != nullptr) {
			
			// Destruindo a sala, não conseguiu 
			r->setDestroying();

			// Desbloqueia para
			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150))
				r->unlock();

			// Deletando o Objeto
			delete r;

			// Limpa o ponteiro
			r = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return r;
};

void RoomManager::destroyRoom(room* _room) {

	if (_room == nullptr)
		throw exception("[RoomManager::destroyRoom][Error] _room is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 4, 0));

	size_t index = findIndexRoom(_room);

	if (index == (size_t)~0)
		throw exception("[RoomManager::destroyRoom][Error] room nao existe no vector de salas.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 5, 0));

	std::string log = "";

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		clearIndex(_room->getNumero());

		v_rooms.erase(v_rooms.begin() + index);

		// Make Log
		log = "[RoomManager::destroyRoom][Log] Channel[ID=" + std::to_string((unsigned short)m_channel_id)
				+ "] Room[TIPO=" + std::to_string((unsigned short)_room->getInfo()->tipo) + ", NUMERO="
				+ std::to_string(_room->getNumero()) + ", MASTER=" + std::to_string((int)_room->getMaster()) + "] destroyed.";

		// Sala vai ser deletada
		_room->setDestroying();

		// Vai destruir(excluir) a sala, libera a sala
		_room->unlock();

		delete _room;		// Libera memória alocada para a sala

		// Deletou a sala;
		_room = nullptr;

		// Show Log, se destruiu a sala com sucesso
		_smp::message_pool::getInstance().push(new message(log, CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (_room != nullptr) {

			_room->setDestroying();

			_room->unlock();

			delete _room;

			_room = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::destroy][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

RoomGrandPrix* RoomManager::makeRoomGrandPrix(unsigned char _channel_owner, RoomInfoEx _ri, player* _session, IFF::GrandPrixData& _gp, int _option) {
	
	RoomGrandPrix *r = nullptr;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (_session != nullptr && _session->m_pi.mi.sala_numero != -1) 
			throw exception("[RoomManager::makeRoomGrandPrix][Error] Player[UID=" + std::to_string(_session->m_pi.uid)
					+ "] sala[NUMERO=" + std::to_string(_session->m_pi.mi.sala_numero) + "], ja esta em outra sala, nao pode criar outra. Hacker ou Bug.",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 120, 0));

		_ri.numero = getNewIndex();

		if (_option == 0 && _session != nullptr)
			_ri.master = _session->m_pi.uid;
		else if (_option == 1)	// Room Sem Master Grand Prix ou Grand Zodiac Event Time
			_ri.master = -2;
		else // Room sem master
			_ri.master = -1;

		r = new RoomGrandPrix(_channel_owner, _ri, _gp);

		if (r == nullptr)
			throw exception("[RoomManager::makeRoomGrandPrix][Error] Player[UID=" + std::to_string(_session->m_pi.uid) 
					+ "] tentou criar a sala[TIPO=" + std::to_string((unsigned short)_ri.tipo) + "], mas nao conseguiu criar o objeto da classe RoomGrandPrix. Bug.", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 130, 0));

		// Verifica se é um room grand prix válida e bloquea ela
		WAIT_ROOM_GP_UNLOCK("makeRoomGrandPrix");

		// Adiciona a sala ao vector
		v_rooms.push_back(r);

		if (_session != nullptr)
			r->enter(*_session);

		// Log
		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoomGrandPrix][Log] Channel[ID=" + std::to_string((unsigned short)m_channel_id)
				+ "] Maked Room[TIPO=" + std::to_string((unsigned short)r->getInfo()->tipo) + ", NUMERO="
				+ std::to_string(r->getNumero()) + ", MASTER=" + std::to_string((int)r->getMaster()) + ", PLAYER_REQUEST_CREATE=" 
				+ (_session != nullptr ? std::to_string(_session->m_pi.uid) : "NENHUMA-SYSTEM") + "]", CL_FILE_LOG_AND_CONSOLE));

		//_smp::message_pool::getInstance().push(new message("Key Room Dump.\n\r" + hex_util::BufferToHexString((unsigned char*)r->getInfo()->key, 16), CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (r != nullptr) {

			// Destruindo a sala, não conseguiu 
			r->setDestroying();

			// Desbloqueia para
			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150))
				r->unlock();

			// Deletando o Objeto
			delete r;

			// Limpa o ponteiro
			r = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoomGrandPrix][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return r;
}

RoomGrandZodiacEvent* RoomManager::makeRoomGrandZodiacEvent(unsigned char _channel_owner, RoomInfoEx _ri) {

	RoomGrandZodiacEvent *r = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		_ri.numero = getNewIndex();

		// Room Sem Master Grand Prix ou Grand Zodiac Event Time
		_ri.master = -2;

		r = new RoomGrandZodiacEvent(_channel_owner, _ri);

		if (r == nullptr)
			throw exception("[RoomManager::makeRoomGrandZodiacEvent][Error] tentou criar a sala[TIPO=" + std::to_string((unsigned short)_ri.tipo) + "] Grand Zodiac Event, mas nao conseguiu criar o objeto da classe room. Bug.", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 130, 0));

		// Verifica se é um room válida e bloquea ela
		WAIT_ROOM_GZE_UNLOCK("makeRoomGrandZodiacEvent");

		// Adiciona a sala no Vector
		v_rooms.push_back(r);

		// Log
		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoomGrandZodiacEvent][Log] Channel[ID=" + std::to_string((unsigned short)m_channel_id)
				+ "] Maked Room[TIPO=" + std::to_string((unsigned short)r->getInfo()->tipo) + ", NUMERO="
				+ std::to_string(r->getNumero()) + ", MASTER=" + std::to_string((int)r->getMaster()) + "] Grand Zodiac Event.", CL_FILE_LOG_AND_CONSOLE));

		//_smp::message_pool::getInstance().push(new message("Key Room Dump.\n\r" + hex_util::BufferToHexString((unsigned char*)r->getInfo()->key, 16), CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (r != nullptr) {
			
			// Destruindo a sala, não conseguiu 
			r->setDestroying();

			// Desbloqueia para
			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150))
				r->unlock();

			// Deletando o Objeto
			delete r;

			// Limpa o ponteiro
			r = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoomGrandZodiacEvent][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return r;
}

RoomBotGMEvent* RoomManager::makeRoomBotGMEvent(unsigned char _channel_owner, RoomInfoEx _ri, std::vector< stReward > _rewards) {
	
	RoomBotGMEvent *r = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		_ri.numero = getNewIndex();

		// Room Sem Master Grand Prix ou Grand Zodiac Event Time ou Bot GM Event
		_ri.master = -2;

		r = new RoomBotGMEvent(_channel_owner, _ri, _rewards);

		if (r == nullptr)
			throw exception("[RoomManager::makeRoomBotGMEvent][Error] tentou criar a sala[TIPO=" + std::to_string((unsigned short)_ri.tipo) 
					+ "] Bot GM Event, mas nao conseguiu criar o objeto da classe room. Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 130, 0));

		// Verifica se é um room válida e bloquea ela
		WAIT_ROOM_BGE_UNLOCK("makeRoomBotGMEvent");

		// Adiciona a sala no Vector
		v_rooms.push_back(r);

		// Log
		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoomBotGMEvent][Log] Channel[ID=" + std::to_string((unsigned short)m_channel_id)
				+ "] Maked Room[TIPO=" + std::to_string((unsigned short)r->getInfo()->tipo) + ", NUMERO="
				+ std::to_string(r->getNumero()) + ", MASTER=" + std::to_string((int)r->getMaster()) + "] Bot GM Event.", CL_FILE_LOG_AND_CONSOLE));

		//_smp::message_pool::getInstance().push(new message("Key Room Dump.\n\r" + hex_util::BufferToHexString((unsigned char*)r->getInfo()->key, 16), CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (r != nullptr) {
			
			// Destruindo a sala, não conseguiu 
			r->setDestroying();

			// Desbloqueia para
			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150))
				r->unlock();

			// Deletando o Objeto
			delete r;

			// Limpa o ponteiro
			r = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::makeRoomBotGMEvent][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return r;
}

room* RoomManager::findRoom(short _numero) {

	if (_numero == -1)
		return nullptr;

	room *r = nullptr;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		for (auto i = 0u; i < v_rooms.size(); ++i) {
			if (v_rooms[i]->getNumero() == _numero) {
				r = v_rooms[i];
				break;
			}
		}

		// Verifica se é um room válida e bloquea ela
		WAIT_ROOM_UNLOCK("makeRoom");

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (r != nullptr) {

			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150))
				r->unlock();

			r = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::findRoom][ErrorSystem] " + e.getFullMessageError()));
	}

	return r;
};

RoomGrandPrix* RoomManager::findRoomGrandPrix(uint32_t _typeid) {

	if (_typeid == 0u)
		return nullptr;

	RoomGrandPrix *r = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		for (auto& el : v_rooms) {

			if (el->getInfo()->grand_prix.active && el->getInfo()->grand_prix.dados_typeid != 0ul
				&& el->getInfo()->grand_prix.dados_typeid == _typeid) {

				r = (RoomGrandPrix*)el;

				break;
			}
		}

		// Verifica se é um room grand prix válida e bloquea ela
		WAIT_ROOM_GP_UNLOCK("findRoomGrandPrix");

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Libera Crictical Session do Room Manager
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		if (r != nullptr) {

			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::ROOM, 150))
				r->unlock();

			r = nullptr;
		}

		_smp::message_pool::getInstance().push(new message("[RoomManager::findRoomGrandPrix][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return r;
};

std::vector< RoomInfo > RoomManager::getRoomsInfo(bool _without_practice_room) {
	
	std::vector < RoomInfo > v_ri;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto i = 0u; i < v_rooms.size(); ++i)
		if (v_rooms[i] != nullptr && (!_without_practice_room || (v_rooms[i]->getInfo()->tipo != RoomInfo::TIPO::PRACTICE && v_rooms[i]->getInfo()->tipo != RoomInfo::TIPO::GRAND_ZODIAC_PRACTICE)))
			v_ri.push_back(*(RoomInfo*)v_rooms[i]->getInfo());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_ri;
};

std::vector< RoomGrandZodiacEvent* > RoomManager::getAllRoomsGrandZodiacEvent() {
	
	std::vector< RoomGrandZodiacEvent* > v_r;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_rooms)
		if (el != nullptr && (int)el->getMaster() == -2 && (el->getInfo()->tipo == RoomInfo::TIPO::GRAND_ZODIAC_ADV || el->getInfo()->tipo == RoomInfo::TIPO::GRAND_ZODIAC_INT))
			v_r.push_back(reinterpret_cast< RoomGrandZodiacEvent* >(el));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_r;
};

std::vector< RoomBotGMEvent* > RoomManager::getAllRoomsBotGMEvent() {
	
	std::vector< RoomBotGMEvent* > v_r;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : v_rooms)
		if (el != nullptr && (int)el->getMaster() == -2 && el->getInfo()->tipo == RoomInfo::TIPO::TOURNEY && el->getInfo()->flag_gm == 1u && el->getInfo()->trofel == TROFEL_GM_EVENT_TYPEID)
			v_r.push_back(reinterpret_cast< RoomBotGMEvent* >(el));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_r;
};

void RoomManager::unlockRoom(room *_r) {

	// _r is invalid
	if (_r == nullptr)
		return;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		for (auto& el : v_rooms) {
			
			if (el != nullptr && el == _r) {
				
				// Libera a sala
				el->unlock();

				// Acorda as outras threads que estão esperando
#if defined(_WIN32)
				WakeAllConditionVariable(&m_cv);
#elif defined(__linux__)
				pthread_cond_broadcast(&m_cv);
#endif

				// Sai do Loop para não consumir mais processos, 
				// que já encontrou o que estava procurando
				break;
			}
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RoomManager::unlockRoom][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

size_t RoomManager::findIndexRoom(room* _room) {

	if (_room == nullptr)
		throw exception("[RoomManager::findIndexRoom][Error] _room is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 4, 0));

	size_t index = ~0;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto i = 0u; i < v_rooms.size(); ++i) {
		if (v_rooms[i] == _room) {
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

unsigned short RoomManager::getNewIndex() {

	unsigned short index = 0;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto i = 0u; i < USHRT_MAX; ++i) {
		if (!m_map_index[i]) {
			index = i;
			m_map_index[i] = 1;	// Ocupado
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

void RoomManager::clearIndex(unsigned short _index) {

	if (_index >= USHRT_MAX)
		throw exception("[RoomManager::clearIndex][Error] _index maior que o limite do mapa de indexes.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::ROOM_MANAGER, 3, 0));
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_map_index[_index] = 0;	// Livre

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};
