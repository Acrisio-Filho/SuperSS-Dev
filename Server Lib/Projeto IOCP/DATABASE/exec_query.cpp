// Arquivo exec_query.cpp
// Criado em 11/08/2017 por Acrisio
// Implementação da classe exec_query

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "exec_query.h"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

#if defined(_WIN32)
LONG volatile exec_query::m_spin_count = 0;
#elif defined(__linux__)
int32_t volatile exec_query::m_spin_count = 0;
#endif

exec_query::exec_query() :
#if defined(_WIN32)
	m_hEvent(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	m_hEvent(nullptr)
#endif
{
	m_type = _QUERY;

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);

	if ((m_hEvent = CreateEvent(NULL, TRUE/*se true usa manual event*/, FALSE/*initial state do event*/, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 1, GetLastError()));
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 1, errno));
	}
#endif
};

exec_query::exec_query(std::string _query, unsigned char _type) : m_type(_type) 
#if defined(_WIN32)
	, m_hEvent(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	, m_hEvent(nullptr)
#endif
{
	
	setQuery(_query);
	
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);

	if ((m_hEvent = CreateEvent(NULL, TRUE/*se true usa manual event*/, FALSE/*initial state do event*/, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, GetLastError()));
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, errno));
	}
#endif
};

exec_query::exec_query(std::wstring _query, unsigned char _tipo) : m_query(_query), m_type(_tipo) 
#if defined(_WIN32)
	, m_hEvent(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	, m_hEvent(nullptr)
#endif
{

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);

	if ((m_hEvent = CreateEvent(NULL, TRUE/*se true usa manual event*/, FALSE/*initial state do event*/, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, GetLastError()));
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, errno));
	}
#endif
};

exec_query::exec_query(std::string _name_proc, std::string _params_proc, unsigned char _tipo)
	: m_type(_tipo) 
#if defined(_WIN32)
	, m_hEvent(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	, m_hEvent(nullptr)
#endif
{

	setQuery(_name_proc);
	setParam(_params_proc);

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);

	if ((m_hEvent = CreateEvent(NULL, TRUE/*se true usa manual event*/, FALSE/*initial state do event*/, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, GetLastError()));
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, errno));
	}
#endif

};

exec_query::exec_query(std::wstring _name_proc, std::wstring _params_proc, unsigned char _tipo)
	: m_query(_name_proc), m_params(_params_proc), m_type(_tipo) 
#if defined(_WIN32)
	, m_hEvent(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	, m_hEvent(nullptr)
#endif
{

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);

	if ((m_hEvent = CreateEvent(NULL, TRUE/*se true usa manual event*/, FALSE/*initial state do event*/, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, GetLastError()));
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("Erro nao conseguiu criar o evento. exec_query::exec_query()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 2, errno));
	}
#endif
};

exec_query::~exec_query() {

#if defined(_WIN32)
	if (m_hEvent != INVALID_HANDLE_VALUE) {
		resetEvent();
		CloseHandle(m_hEvent);
		m_hEvent = INVALID_HANDLE_VALUE;
	}
#elif defined(__linux__)
	if (m_hEvent != nullptr) {
		m_hEvent->reset();
		
		delete m_hEvent;

		m_hEvent = nullptr;
	}
#endif

	m_query.clear();
	m_query.shrink_to_fit();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
};

std::string exec_query::getQuery() {
	return WcToMb(m_query);
};

std::wstring& exec_query::getWQuery() {
	return m_query;
};

std::string exec_query::getParams() {
	return WcToMb(m_params);
};

std::wstring& exec_query::getWParams() {
	return m_params;
};

response *exec_query::getRes() {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif
	
	auto ret = m_res;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ret;
};

void exec_query::setQuery(std::string _query) {
	setQuery(MbToWc(_query));
};

void exec_query::setQuery(std::wstring _query) {
	m_query = _query;
};

void exec_query::setParam(std::string _params) {
	setParam(MbToWc(_params));
};

void exec_query::setParam(std::wstring _params) {
	m_params = _params;
};

void exec_query::setRes(response* _res) {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_res = _res;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

void exec_query::waitEvent(DWORD milliseconds) {

	if (exec_query::getSpinCount() <= 0 && milliseconds == INFINITE)
		throw exception("Erro nao tem nenhum leitor para tratar esse pedido. exec_query::waitEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 7, 0));

#if defined(_WIN32)
	if (WaitForSingleObject(m_hEvent, milliseconds) != WAIT_OBJECT_0)
		throw exception("Erro nao conseguiu esperar pelo evento. exec_query::waitEvent().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 3, GetLastError()));
#elif defined(__linux__)
	if (m_hEvent == nullptr || m_hEvent->wait(milliseconds) != WAIT_OBJECT_0)
		throw exception("Erro nao conseguiu esperar pelo evento. exec_query::waitEvent().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 3, errno));
#endif
};

void exec_query::setEvent() {
#if defined(_WIN32)
	if (!::SetEvent(m_hEvent))
		throw exception("Erro nao conseguiu setar o estado do evento. exec_query::setEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 4, GetLastError()));
#elif defined(__linux__)
	if (m_hEvent == nullptr || !m_hEvent->set())
		throw exception("Erro nao conseguiu setar o estado do evento. exec_query::setEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 4, errno));
#endif
};

void exec_query::resetEvent() {
#if defined(_WIN32)
	if (!::ResetEvent(m_hEvent))
		throw exception("Erro nao conseguiu resetar o estado do evento. exec_query::resetEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 5, GetLastError()));
#elif defined(__linux__)
	if (m_hEvent == nullptr || !m_hEvent->reset())
		throw exception("Erro nao conseguiu resetar o estado do evento. exec_query::resetEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 5, errno));
#endif
};

void exec_query::pulseEvent() {
#if defined(_WIN32)
	if (!::PulseEvent(m_hEvent))
		throw exception("Erro nao conseguiu pulsar o estado do evento. exec_query::pulseEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 6, GetLastError()));
#elif defined(__linux__)
	if (m_hEvent == nullptr || !m_hEvent->pulse())
		throw exception("Erro nao conseguiu pulsar o estado do evento. exec_query::pulseEvent()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::EXEC_QUERY, 6, errno));
#endif
};

unsigned char exec_query::getType() {
	return m_type;
};

void exec_query::enter() {
#if defined(_WIN32)
	InterlockedIncrement(&m_spin_count);
#elif defined(__linux__)
	__atomic_add_fetch(&m_spin_count, 1, __ATOMIC_RELAXED);
#endif
};

void exec_query::release() {
#if defined(_WIN32)
	InterlockedDecrement(&m_spin_count);
#elif defined(__linux__)
	__atomic_sub_fetch(&m_spin_count, 1, __ATOMIC_RELAXED);
#endif
};

int exec_query::getSpinCount() {
#if defined(_WIN32)
	return InterlockedExchange(&m_spin_count, m_spin_count);
#elif defined(__linux__)
	return __atomic_load_n(&m_spin_count, __ATOMIC_RELAXED);
#endif
};
