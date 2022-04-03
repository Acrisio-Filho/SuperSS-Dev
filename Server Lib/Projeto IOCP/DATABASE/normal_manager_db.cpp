// Arquivo normal_manager_db.cpp
// Criado em 01/05/2018 as 14:32 por Acrisio
// Implementação da classe NormalManagerDB

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "normal_manager_db.hpp"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"
#include "../UTIL/message_pool.h"

#include <ctime>

#include "../TYPE/set_se_exception_with_mini_dump.h"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

using namespace stdA;

#if defined(_WIN32)
#define BEGIN_THREAD_SETUP(_type_class) DWORD result = 0; \
						   STDA_SET_SE_EXCEPTION \
						   try { \
								_type_class *pT = reinterpret_cast<_type_class*>(lpParameter); \
								if (pT) { 
#elif defined(__linux__)
#define BEGIN_THREAD_SETUP(_type_class) void* result = (void*)0; \
						   try { \
								_type_class *pT = reinterpret_cast<_type_class*>(lpParameter); \
								if (pT) { 
#endif

#define END_THREAD_SETUP(name_thread)	  } \
							}catch (exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (std::exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (...) { \
								_smp::message_pool::getInstance().push(new message(std::string((name_thread)) + " -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE)); \
							} \
							_smp::message_pool::getInstance().push(new message("Saindo do thread->" + std::string((name_thread)))); \
						return result; \

NormalManagerDB::NormalManagerDB()
	:	m_state(false), m_dbs(), m_db_instance_num (NUM_DB_THREAD),
		m_pRevive(nullptr), m_continue_revive() 
{

	init();
}

NormalManagerDB::~NormalManagerDB() {

	destroy();
}

void NormalManagerDB::create(uint32_t _db_instance_num) {

	try {

		if (!m_state) {

			m_db_instance_num = _db_instance_num;

			// Verifica DB Instance Number
			checkDBInstanceNumAndFix();

			// Create Data base Instance and initialize
			for (auto i = 0u; i < m_db_instance_num; ++i) {

				// Make New Normal DB Instance if not reach of limit of instances
				if (m_dbs.empty() || m_dbs.size() < m_db_instance_num)
					m_dbs.push_back(new NormalDB());

				// Initialize DB Instance
				if (m_dbs[i] != nullptr)
					m_dbs[i]->init();
			}

			// Log
			_smp::message_pool::getInstance().push(new message("[NormalManagerDB::create][Log] create " + std::to_string(m_db_instance_num) + " instance do NormalDB", CL_FILE_LOG_AND_CONSOLE));

			// Initialize with Success
			m_state = true;
		}

	}catch (exception& e) {

		// Failed to initialize
		m_state = false;

		_smp::message_pool::getInstance().push(new message("[NormalManagerDB::create][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void NormalManagerDB::init() {

	if (!m_continue_revive.isLive())
		m_continue_revive.set(1);

	if (m_pRevive == nullptr)
		m_pRevive = new thread(0, NormalManagerDB::_Revive, this);
	else if (!m_pRevive->isLive())
		m_pRevive->init_thread();
}

void NormalManagerDB::destroy() {

	// Down thread revive
	m_continue_revive.set(0);

	if (m_pRevive != nullptr) {

		m_pRevive->waitThreadFinish(INFINITE);

		delete m_pRevive;

		m_pRevive = nullptr;
	}

	if (m_state) {

		// Verifica DB Instance Number
		checkDBInstanceNumAndFix();

		for (auto i = 0u; i < m_db_instance_num && i < m_dbs.size(); ++i) {
			
			if (m_dbs[i] != nullptr) {
				
				m_dbs[i]->sendCloseAndWait();

				// clean memory allocate
				delete m_dbs[i];

				m_dbs[i] = nullptr;
			}
		}

		// Clean vector
		m_dbs.clear();
		m_dbs.shrink_to_fit();
	}

	m_state = false;
}

void NormalManagerDB::checkIsDeadAndRevive() {

	// Verifica DB Instance Number
	checkDBInstanceNumAndFix();

	for (auto i = 0u; i < m_db_instance_num && i < m_dbs.size(); ++i)
		if (m_dbs[i] != nullptr)
			m_dbs[i]->checkIsDeadAndRevive();
}

#if defined(_WIN32)
DWORD CALLBACK NormalManagerDB::_Revive(LPVOID lpParameter) {
#elif defined(__linux__)
void* NormalManagerDB::_Revive(void* lpParameter) {
#endif
	BEGIN_THREAD_SETUP(NormalManagerDB);

	result = pT->Revive();

	END_THREAD_SETUP("Revive");
}

#if defined(_WIN32)
DWORD NormalManagerDB::Revive() {
#elif defined(__linux__)
void* NormalManagerDB::Revive() {
#endif

	try {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("NormalManagerDB::Revive iniciado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("NormalManagerDB::Revive iniciado com sucesso!", CL_ONLY_FILE_LOG));
#endif

		while (m_continue_revive.isLive()) {

			if (m_state)
				checkIsDeadAndRevive();

#if defined(_WIN32)
			Sleep(1000); // 1 second para próxima verificação
#elif defined(__linux__)
			usleep(1000000); // 1 second para próxima verificação
#endif
        }

    }catch (exception& e) {
        _smp::message_pool::getInstance().push(new message("[NormalManagerDB::Revive][Error][My] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }catch (std::exception& e) {
        _smp::message_pool::getInstance().push(new message("[NormalManagerDB::Revive][Error][std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
    }catch (...) {
        _smp::message_pool::getInstance().push(new message("[NormalManagerDB::Revive][Error] Unknown Exception ", CL_FILE_LOG_AND_CONSOLE));
    }

    _smp::message_pool::getInstance().push(new message("Saindo do NormalManagerDB::Revive...", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return (void*)0;
#endif
}

int NormalManagerDB::add(NormalDB::msg_t* _msg) {

	if (!m_state)	// DB not Create, cria ela
		create();

	uint32_t index = (uint32_t)(/*std::rand() **/ std::time(nullptr)) % m_dbs.size();

	if (m_dbs[index] != nullptr)
		m_dbs[index]->add(_msg);

	return 0;
}

int NormalManagerDB::add(uint32_t _id, pangya_db *_pangya_db, callback_response _callback_response, void* _arg) {

	add(new(NormalDB::msg_t){ _id, _pangya_db, _callback_response, _arg });

	return 0;
}

void NormalManagerDB::freeAllWaiting(std::string _msg) {

	for (auto& el : m_dbs)
		if (el != nullptr)
			el->freeAllWaiting(_msg);
}

// Methods que o normal db faz para o server e o channel
void NormalManagerDB::insertLoginLog(session& _session, NormalDB::msg_t* _msg) {
    // Ainda não implementei depois faço isso, só criei essa função de exemplo
    return;
}

inline void NormalManagerDB::checkDBInstanceNumAndFix() {

	// check m_db_instance_num is valid
	if ((int)m_db_instance_num <= 0)
		m_db_instance_num = NUM_DB_THREAD;
}

NormalManagerDB::DownEvent::DownEvent() {

#if defined(_WIN32)
	InterlockedExchange(&m_continue, 1);
#elif defined(__linux__)
	__atomic_store_n(&m_continue, 1, __ATOMIC_RELAXED);
#endif
}

void NormalManagerDB::DownEvent::set(_MY_LONG _value) {

#if defined(_WIN32)
	InterlockedExchange(&m_continue, _value);
#elif defined(__linux__)
	__atomic_store_n(&m_continue, _value, __ATOMIC_RELAXED);
#endif
}

bool NormalManagerDB::DownEvent::isLive() {
#if defined(_WIN32)
	return InterlockedCompareExchange(&m_continue, 1, 1) == 1;
#elif defined(__linux__)
	int32_t check_m = 1; // Compare
	return __atomic_compare_exchange_n(&m_continue, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED) == 1;
#endif
}
