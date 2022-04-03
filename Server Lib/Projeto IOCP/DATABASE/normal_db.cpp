// Arquivo normal_db.cpp
// Criado em 01/05/2018 as 13:01 por Acrisio
// Implementação da classe NormalDB

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "normal_db.hpp"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"
#include "../UTIL/message_pool.h"

#include "../UTIL/reader_ini.hpp"

#include "../DATABASE/mysql_db.h"
#include "../DATABASE/mssql.h"
#include "../DATABASE/postgresql.h"

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

NormalDB::NormalDB() : m_pExec(nullptr), m_pResponse(nullptr), m_ctx_db{0}, m_state(false) {

#if defined(_WIN32)
	InterlockedExchange(&m_continue_exec, 0u);
	InterlockedExchange(&m_continue_response, 0u);

	InterlockedExchange(&m_free_all_waiting, 0u);
#elif defined(__linux__)
    __atomic_store_n(&m_continue_exec, 0u, __ATOMIC_RELAXED);
	__atomic_store_n(&m_continue_response, 0u, __ATOMIC_RELAXED);

	__atomic_store_n(&m_free_all_waiting, 0u, __ATOMIC_RELAXED);
#endif
}

NormalDB::~NormalDB() {

    try {
       
		if (m_state || m_pExec != nullptr || m_pResponse != nullptr) {
            sendCloseAndWait();
            close();
        }
    
	}catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[NormalDB::~NormalDB][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }
}

void NormalDB::init() {

    try {

        if (m_state)
            throw exception("[NormalDB::init][Error] DB already initialized", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 2, 0));

		// Libera os Loop das threads
#if defined(_WIN32)
		InterlockedExchange(&m_continue_exec, 1u);
		InterlockedExchange(&m_continue_response, 1u);

		InterlockedExchange(&m_free_all_waiting, 0u);
#elif defined(__linux__)
        __atomic_store_n(&m_continue_exec, 1u, __ATOMIC_RELAXED);
		__atomic_store_n(&m_continue_response, 1u, __ATOMIC_RELAXED);

		__atomic_store_n(&m_free_all_waiting, 0u, __ATOMIC_RELAXED);
#endif

        m_pExec = new thread(TT_NORMAL_EXEC_QUERY, NormalDB::ThreadFunc, this);

        m_pResponse = new thread(TT_NORMAL_RESPONSE, NormalDB::ThreadFunc2, this);

        m_state = true;

#ifdef _DEBUG
        _smp::message_pool::getInstance().push(new message("NormalDB iniciado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#else
        _smp::message_pool::getInstance().push(new message("NormalDB iniciado com sucesso!", CL_ONLY_FILE_LOG));
#endif

    }catch (exception& e) {

        if (m_pExec != nullptr)
            delete m_pExec;

        if (m_pResponse != nullptr)
            delete m_pResponse;

		m_pExec = nullptr;
		m_pResponse = nullptr;

        m_state = false;

        throw exception("[NormalDB::init][Error] " + e.getFullMessageError(), STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 3, 0));
    }
}

void NormalDB::sendCloseAndWait() {

#if defined(_WIN32)
	InterlockedExchange(&m_continue_exec, 0u);
	InterlockedExchange(&m_continue_response, 0u);

	InterlockedExchange(&m_free_all_waiting, 1u);
#elif defined(__linux__)
    __atomic_store_n(&m_continue_exec, 0u, __ATOMIC_RELAXED);
	__atomic_store_n(&m_continue_response, 0u, __ATOMIC_RELAXED);

	__atomic_store_n(&m_free_all_waiting, 1u, __ATOMIC_RELAXED);
#endif

	if (m_pExec != nullptr) {

		m_unExec.push(nullptr);

		m_pExec->waitThreadFinish(INFINITE);
	}
    
	if (m_pResponse != nullptr) {
		
		m_unResponse.push(nullptr);
		
		m_pResponse->waitThreadFinish(INFINITE);
	}

	// acordar quem está dormindo e deletar que não está
	auto all = m_unExec.getAll();

	for (auto& el : all) {

		if (forceWakeMsg(el, "[NormalDB::sendCloseAndWait][Error] force wake"))
			continue;

		if (el != nullptr)
			delete el;

		el = nullptr;
	}

	all = m_unResponse.getAll();

	for (auto& el : all) {
		
		if (el != nullptr)
			delete el;

		el = nullptr;
	}
}

void NormalDB::close() {

    if (!m_state && m_pExec == nullptr && m_pResponse == nullptr)
        throw exception("[NormalDB::close][Error] DB already close", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 4, 0));

    if (m_pExec != nullptr)
        delete m_pExec;

    m_pExec = nullptr;

    if (m_pResponse != nullptr)
        delete m_pResponse;

    m_pResponse = nullptr;

    m_state = false;
}

bool NormalDB::forceWakeMsg(msg_t* _msg, std::string _exception_msg) {

	if (_msg == nullptr || !_msg->isWaitable())
		return false;

	_msg->setException(_exception_msg);

	_msg->wakeupWaiter();

	// delete msg
	delete _msg;

	_msg = nullptr;

	return true;
}

#if defined(_WIN32)
DWORD NormalDB::ThreadFunc(LPVOID lpParameter) {
#elif defined(__linux__)
void* NormalDB::ThreadFunc(LPVOID lpParameter) {
#endif
    BEGIN_THREAD_SETUP(NormalDB);

    result = pT->runExecQuery();

    END_THREAD_SETUP("NormalDB");
}

#if defined(_WIN32)
DWORD NormalDB::ThreadFunc2(LPVOID lpParameter) {
#elif defined(__linux__)
void* NormalDB::ThreadFunc2(LPVOID lpParameter) {
#endif
    BEGIN_THREAD_SETUP(NormalDB);

    result = pT->runResponse();

    END_THREAD_SETUP("NormalDB");
}

int NormalDB::add(msg_t* _msg) {

#if defined(_WIN32)
	if (InterlockedCompareExchange(&m_free_all_waiting, 1, 1) == 1
#elif defined(__linux__)
	int32_t check_m = 1; // Compare
	if (__atomic_compare_exchange_n(&m_free_all_waiting, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED) == 1
#endif
			&& forceWakeMsg(_msg, "[NormalDB::Add][Error] force wake"))
		return 1; // wake msg, don't push into list
    
	m_unExec.push(_msg);

	return 0;
}

int NormalDB::add(uint32_t _id, pangya_db *_pangya_db, callback_response _callback_response, void *_arg) {
	return add(new(msg_t){ _id, _pangya_db, _callback_response, _arg });
}

void NormalDB::checkIsDeadAndRevive() {

	if (m_pExec == nullptr)
		m_pExec = new thread(TT_NORMAL_EXEC_QUERY, NormalDB::ThreadFunc, this);
	else if (!m_pExec->isLive())
		m_pExec->init_thread();

	if (m_pResponse == nullptr)
		m_pResponse = new thread(TT_NORMAL_RESPONSE, NormalDB::ThreadFunc2, this);
	else if (!m_pResponse->isLive())
		m_pResponse->init_thread();

}

void NormalDB::freeAllWaiting(std::string _msg) {

#if defined(_WIN32)
	InterlockedExchange(&m_free_all_waiting, 1u);
#elif defined(__linux__)
	__atomic_store_n(&m_free_all_waiting, 1u, __ATOMIC_RELAXED);
#endif

	auto all = m_unExec.getAll();

	for (auto& el : all) {

		if (forceWakeMsg(el, _msg))
			continue;

		// push again msg not waitable
		m_unExec.push(el);
	}
}

#if defined(_WIN32)
DWORD NormalDB::runExecQuery() {
#elif defined(__linux__)
void* NormalDB::runExecQuery() {
#endif
    
	database *_db = nullptr;

    try {

		// Load ini to Database Config
        loadIni();

		// Select Database Engine
		if (
#if defined (_WIN32)
			_stricmp
#elif defined(__linux__)
			strcasecmp
#endif
				(m_ctx_db.engine, "mysql") == 0
		)
			_db = new mysql_db(m_ctx_db.ip, m_ctx_db.name, m_ctx_db.user, m_ctx_db.pass, m_ctx_db.port);
		else if (
#if defined(_WIN32)
			_stricmp
#elif defined(__linux__)
			strcasecmp
#endif
				(m_ctx_db.engine, "mssql") == 0
		)
			_db = new mssql(m_ctx_db.ip, m_ctx_db.name, m_ctx_db.user, m_ctx_db.pass, m_ctx_db.port);
		else if (
#if defined(_WIN32)
			_stricmp
#elif defined(__linux__)
			strcasecmp
#endif
				(m_ctx_db.engine, "postgresql") == 0
		)
			_db = new postgresql(m_ctx_db.ip, m_ctx_db.name, m_ctx_db.user, m_ctx_db.pass, m_ctx_db.port);
		else // Default MSSQL
			_db = new mssql(m_ctx_db.ip, m_ctx_db.name, m_ctx_db.user, m_ctx_db.pass, m_ctx_db.port);

		// Connect
        _db->connect();

#ifdef _DEBUG
        _smp::message_pool::getInstance().push(new message("NormalDB::runExecQuery Iniciado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#else
        _smp::message_pool::getInstance().push(new message("NormalDB::runExecQuery Iniciado com sucesso!", CL_ONLY_FILE_LOG));
#endif
        bool sai = false;
        msg_t* _msg = nullptr;

#if defined(_WIN32)
        while(!sai && InterlockedCompareExchange(&m_continue_exec, 1, 1)) {
#elif defined(__linux__)
        int32_t check_m = 1; // Compare
        while(!sai && __atomic_compare_exchange_n(&m_continue_exec, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif

            try {
                _msg = m_unExec.get(1000/*second*/);
            }catch (exception& e) {
                if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::LIST_ASYNC) {
                    if (STDA_ERROR_DECODE(e.getCodeError()) == 2/*Time Out*/)
                        continue;
                    else if (STDA_ERROR_DECODE(e.getCodeError()) == 3/*Error ao pegar signal condition variable*/)
                        throw;
                }else
                    throw;
            }

            if (_msg == nullptr) {
                sai = true;
                continue;
            }

			try {

				_msg->execQuery(*_db);

			}catch (exception& e) {

				// É erro de conexão
				if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::NORMAL_DB, 5)) {

					// Log
					_smp::message_pool::getInstance().push(new message("[NormalDB::runExecQuery][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

					// Coloca a msg no pool devo até ter uma conexão com o banco de dados para executar
					add(_msg);
				}

				// Relança
				throw;
			}

			// é para ativar quem estava esperando, ou manda para a thread que response pela função callback
			if (_msg->isWaitable()) {

				_msg->wakeupWaiter();

				// Delete a msg aqui, o pangya_db quem chamou deleta lá
				delete _msg;

			}else
				m_unResponse.push(_msg);
        }

    }catch (exception& e) {
        _smp::message_pool::getInstance().push(new message("[NormalDB::runExecQuery][Error][My] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }catch (std::exception& e) {
        _smp::message_pool::getInstance().push(new message("[NormalDB::runExecQuery][Error][std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
    }catch (...) {
        _smp::message_pool::getInstance().push(new message("[NormalDB::runExecQuery][Error] Unknown Exception ", CL_FILE_LOG_AND_CONSOLE));
    }

	// free memory
	if (_db != nullptr)
		delete _db;

    _smp::message_pool::getInstance().push(new message("Saindo do NormalDB::runExecQuery...", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
    return 0u;
#elif defined(__linux__)
    return (void*)0u;
#endif
}

#if defined(_WIN32)
DWORD NormalDB::runResponse() {
#elif defined(__linux__)
void* NormalDB::runResponse() {
#endif

    try {
        
#ifdef _DEBUG
        _smp::message_pool::getInstance().push(new message("NormalDB::runResponse iniciado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#else
        _smp::message_pool::getInstance().push(new message("NormalDB::runResponse iniciado com sucesso!", CL_ONLY_FILE_LOG));
#endif

        bool sai = false;
        msg_t *_msg = nullptr;

#if defined(_WIN32)
        while (!sai && InterlockedCompareExchange(&m_continue_response, 1, 1)) {
#elif defined(__linux__)
        int32_t check_m = 1; // Compare
        while (!sai && __atomic_compare_exchange_n(&m_continue_response, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
            
            try {
                _msg = m_unResponse.get(1000/*second*/);
            }catch (exception& e) {
                if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::LIST_ASYNC) {
                    if (STDA_ERROR_DECODE(e.getCodeError()) == 2/*Time Out*/)
                        continue;
                    else if (STDA_ERROR_DECODE(e.getCodeError()) == 3/*Error ao pegar signal condition variable*/)
                        throw;
                }else
                    throw;
            }

            if (_msg == nullptr) {
                sai = true;
                continue;
            }

            _msg->execFunc();

			// Libera Memória, o destrutor do msg_t libera a memória do pangya_db
			if (_msg != nullptr)
				delete _msg;
        }

    }catch (exception& e) {
        _smp::message_pool::getInstance().push(new message("[NormalDB::runResponse][Error][My] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }catch (std::exception& e) {
        _smp::message_pool::getInstance().push(new message("[NormalDB::runResponse][Error][std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
    }catch (...) {
        _smp::message_pool::getInstance().push(new message("[NormalDB::runResponse][Error] Unknown Exception ", CL_FILE_LOG_AND_CONSOLE));
    }

    _smp::message_pool::getInstance().push(new message("Saindo do NormalDB::runResponse...", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
    return 0u;
#elif defined(__linux__)
    return (void*)0u;
#endif
}

bool NormalDB::loadIni() {

#if defined(_WIN32)
    ReaderIni ini("\\server.ini");
#elif defined(__linux__)
    ReaderIni ini("/server.ini");
#endif

    ini.init();

#if defined(_WIN32)
    memcpy_s(m_ctx_db.ip, sizeof(m_ctx_db.ip), ini.readString("NORMAL_DB", "DBIP").c_str(), sizeof(m_ctx_db.ip));
    memcpy_s(m_ctx_db.name, sizeof(m_ctx_db.name), ini.readString("NORMAL_DB", "DBNAME").c_str(), sizeof(m_ctx_db.name));
    memcpy_s(m_ctx_db.user, sizeof(m_ctx_db.user), ini.readString("NORMAL_DB", "DBUSER").c_str(), sizeof(m_ctx_db.user));
    memcpy_s(m_ctx_db.pass, sizeof(m_ctx_db.pass), ini.readString("NORMAL_DB", "DBPASS").c_str(), sizeof(m_ctx_db.pass));
#elif defined(__linux__)
    memcpy(m_ctx_db.ip, ini.readString("NORMAL_DB", "DBIP").c_str(), sizeof(m_ctx_db.ip));
    memcpy(m_ctx_db.name, ini.readString("NORMAL_DB", "DBNAME").c_str(), sizeof(m_ctx_db.name));
    memcpy(m_ctx_db.user, ini.readString("NORMAL_DB", "DBUSER").c_str(), sizeof(m_ctx_db.user));
    memcpy(m_ctx_db.pass, ini.readString("NORMAL_DB", "DBPASS").c_str(), sizeof(m_ctx_db.pass));
#endif

	// Database Engine
	try {
#if defined(_WIN32)
		memcpy_s(m_ctx_db.engine, sizeof(m_ctx_db.engine), ini.readString("NORMAL_DB", "DBENGINE").c_str(), sizeof(m_ctx_db.engine));
#elif defined(__linux__)
		memcpy(m_ctx_db.engine, ini.readString("NORMAL_DB", "DBENGINE").c_str(), sizeof(m_ctx_db.engine));
#endif
	}catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

		// clear
#if defined(_WIN32)
		strcpy_s(m_ctx_db.engine, sizeof(m_ctx_db.engine), "");
#elif defined(__linux__)
		strcpy(m_ctx_db.engine, "");
#endif
	}

	// Database Port
    try {
        m_ctx_db.port = ini.readInt("NORMAL_DB", "DBPORT");
    }catch (exception& e) {
        m_ctx_db.port = 0;  // o db utiliza a padrão se o valor for 0

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[NormalDB::loadIni][WARNING] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
        _smp::message_pool::getInstance().push(new message("[NormalDB::loadIni][WARNING] usando a porta padrao do banco de dados", CL_FILE_LOG_AND_CONSOLE));
#else
		UNREFERENCED_PARAMETER(e);
#endif
    }

	return true;
}