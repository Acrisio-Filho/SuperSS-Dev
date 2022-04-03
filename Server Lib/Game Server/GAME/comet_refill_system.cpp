// Arquivo comet_refill_system.cpp
// Criado em 08/07/2018 as 22:21 por Acrisio
// Implementa��o da classe CometRefillSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "comet_refill_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_comet_refill_info.hpp"
#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include <algorithm>
#include <ctime>

using namespace stdA;

#if defined(_WIN32)
#define TRY_CHECK			 try { \
								EnterCriticalSection(&m_cs);
#elif defined(__linux__)
#define TRY_CHECK			 try { \
								pthread_mutex_lock(&m_cs);
#endif

#if defined(_WIN32)
#define LEAVE_CHECK				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
#define LEAVE_CHECK				pthread_mutex_unlock(&m_cs);
#endif

#if defined(_WIN32)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								LeaveCriticalSection(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[CometRefillSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[CometRefillSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

//std::map< uint32_t, ctx_comet_refill > CometRefillSystem::m_comet_refill;
//bool CometRefillSystem::m_load = false;

CometRefillSystem::CometRefillSystem() : m_load(false) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Inicializa
	initialize();
}

CometRefillSystem::~CometRefillSystem() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void CometRefillSystem::initialize() {

	TRY_CHECK;

	CmdCometRefillInfo cmd_cri(true/*Waiter*/);

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_cri, nullptr, nullptr);

	cmd_cri.waitEvent();

	if (cmd_cri.getException().getCodeError() != 0)
		throw cmd_cri.getException();

	m_comet_refill = cmd_cri.getInfo();

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[CometRefillSystem::initialize][Log] Comet Refill System Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[CometRefillSystem::initialize][Log] Comet Refill System Carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server toma as provid�ncias
	throw;

	END_CHECK;
}

void CometRefillSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_comet_refill.empty())
		m_comet_refill.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void CometRefillSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool CometRefillSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = (m_load && !m_comet_refill.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

ctx_comet_refill* CometRefillSystem::findCometRefill(uint32_t _typeid) {

	if (!isLoad())
		throw exception("[CometRefillSystem::findCometRefill][Error] nao esta carregado. carregue ele primeiro antes de procurar por um Comet Refill.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMET_REFILL_SYSTEM, 3, 0));

	TRY_CHECK;

	auto it = std::find_if(m_comet_refill.begin(), m_comet_refill.end(), [&](auto& el) {
		return el.second._typeid == _typeid;
	});

	if (it != m_comet_refill.end()) {

		LEAVE_CHECK;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findCometRefill");
	END_CHECK;

	return nullptr;
}

uint32_t CometRefillSystem::drawsCometRefill(ctx_comet_refill& _ctx_cr) {

	if (_ctx_cr._typeid == 0)
		throw exception("[CometRefillSystem][Error] ctx_comet_refill[TYPEID=" + std::to_string(_ctx_cr._typeid) + "] is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMET_REFILL_SYSTEM, 1, 0));

	if (!_ctx_cr.qntd_range.isValid())
		throw exception("[CometRefillSystem][Error] ctx_comet_refill[TYPEID=" + std::to_string(_ctx_cr._typeid) + "] range[min=" 
				+ std::to_string(_ctx_cr.qntd_range.min) + ", max=" + std::to_string(_ctx_cr.qntd_range.max) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMET_REFILL_SYSTEM, 2, 0));

	uint32_t draw = 0l;

	TRY_CHECK;

	draw = (uint32_t)(_ctx_cr.qntd_range.min + (sRandomGen::getInstance().rIbeMt19937_64_chrono() % (_ctx_cr.qntd_range.max - _ctx_cr.qntd_range.min + 1)));

	LEAVE_CHECK;
	CATCH_CHECK("drawsCometRefill");
	END_CHECK;

	return draw;
}
