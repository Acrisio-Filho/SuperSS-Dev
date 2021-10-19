// Arquivo thread.cpp
// Criado em 21/05/2017 por Acrisio
// Implementação da classe thread

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "thread.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../TYPE/stda_error.h"

// pthread_kill
#include <signal.h>

using namespace stdA;

thread::thread(DWORD tipo) : m_tipo(tipo) {
    m_thread = INVALID_HANDLE_VALUE;
    m_thread_id = 0;
	m_routine = NULL;
	m_parameter = NULL;
	m_flag_priority = 0;
};

thread::thread(DWORD tipo, LPTHREAD_START_ROUTINE routine, LPVOID parameter, DWORD flag_priority, DWORD mask_affinity) 
	: m_tipo(tipo), m_routine(routine), m_parameter(parameter), m_flag_priority(flag_priority), m_mask_affinity(mask_affinity) {
	m_thread = INVALID_HANDLE_VALUE;
	m_thread_id = 0;
	init_thread(routine, parameter, flag_priority, mask_affinity);
};

thread::~thread() {
    if (isLive())
		exit_thread();
};

void thread::init_thread(LPTHREAD_START_ROUTINE routine, LPVOID parameter, DWORD flag_priority, DWORD mask_affinity) {
	m_routine = routine;
	m_parameter = parameter;
	m_flag_priority = flag_priority;

	init_thread();
};

void thread::init_thread() {
	if (isLive())
		throw exception("Thread ja esta iniciada. thread::init_thread()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 1, 0));

	if (m_routine == NULL)
		throw exception("Routine Thread is null, call ini_thread with params, thread::init_thread()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 2, 0));

	//DWORD flag_create = (m_flag_priority != THREAD_PRIORITY_NORMAL) ?  : 0;

#if defined(_WIN32)
	if ((m_thread = CreateThread(NULL, 0, m_routine, m_parameter, /*flag_create*/ CREATE_SUSPENDED, &m_thread_id)) == INVALID_HANDLE_VALUE)
		throw exception("Erro na criacao da thread. thread::init_thread()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 3, GetLastError()));

	if (m_flag_priority != THREAD_PRIORITY_NORMAL && SetThreadPriority(m_thread, m_flag_priority) == 0)
		throw exception("Error ao set flag priority: " + std::to_string(m_flag_priority) + " para a thread: " + std::to_string((size_t)m_thread), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 8, GetLastError()));

	if (m_mask_affinity != 0 && SetThreadAffinityMask(m_thread, m_mask_affinity) == 0)
		throw exception("Error ao set affinity mask: " + std::to_string(m_mask_affinity) + " para a thread: " + std::to_string((size_t)m_thread), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 9, GetLastError()));
	
	//if (flag_create == CREATE_SUSPENDED)
	resume_thread();
#elif defined(__linux__)
	int32_t error = 0;

	pthread_attr_t attr;

	if ((error = pthread_attr_init(&attr)) != 0)
		throw exception("[thread::init_thread][Error] nao conseguiu inicializar pthread attribute.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 10, error));

	if ((int32_t)m_flag_priority != THREAD_PRIORITY_NORMAL) {

		// !@ Teste
		if ((int32_t)m_flag_priority == THREAD_PRIORITY_IDLE) {

			// Change to IDLE policy
			if ((error = pthread_attr_setschedpolicy(&attr, SCHED_IDLE)) != 0)
				throw exception("[thread::init_thread][Error] ao set flag priority IDLE.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 8, error));

		}else if ((int32_t)m_flag_priority > THREAD_PRIORITY_HIGHEST) {

			// Change to real-time schedule policy
			sched_param _sched_param{ 0u };

			if ((error = pthread_attr_getschedparam(&attr, &_sched_param)) != 0)
				throw exception("[thread::init_thread][Error] ao pegar pthread_attr_getschedparam.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 80, error));

			int sp_min = sched_get_priority_min(SCHED_RR);
			int sp_max = sched_get_priority_max(SCHED_RR);

			if (sp_min == -1 || sp_max == -1)
				throw exception("[thread::init_thread][Error] ao pegar o sched_get_priority_" + std::to_string(sp_min == -1 ? sp_min : sp_max) 
					+ " da schedule SCHED_RR.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 81, errno));

			// set schedule policy
			if ((error = pthread_attr_setschedpolicy(&attr, SCHED_RR)) != 0)
				throw exception("[thread::init_thread][Error] ao setar o schedule policy do pthread attr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 82, error));

			// set priority com 
			_sched_param.sched_priority = ((int32_t)m_flag_priority < sp_min ? sp_min : ((int32_t)m_flag_priority > sp_max ? sp_max : (int32_t)m_flag_priority));
			
			if ((error = pthread_attr_setschedparam(&attr, &_sched_param)) != 0)
				throw exception("[thread::init_thread][Error] ao setar flag priority: " + std::to_string(m_flag_priority), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 8, error));

			// set thread schedule inherit
			if ((error = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) != 0)
				throw exception("[thread::init_thread][Error] ao setar pthread attr inherit schedule.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 83, error));
		}
	}

	if (m_mask_affinity != 0) {

		cpu_set_t cpu_mask;
		CPU_ZERO(&cpu_mask);
		CPU_SET(m_mask_affinity, &cpu_mask);

		if ((error = pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpu_mask)) != 0)
			throw exception("[thread::init_thread][Error] ao set affinity mask: " + std::to_string(m_mask_affinity), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 9, error));
	}

	if ((error = pthread_create(&m_thread, &attr, m_routine, m_parameter)) != 0)
		throw exception("[thread::init_thread][Error] na criacao da thread.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 3, error));

	if ((error = pthread_attr_destroy(&attr)) != 0)
		throw exception("[thread::init_thread][Error] ao destruir o pthread_attr_t.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 11, error));
#endif

	_smp::message_pool::getInstance().push(new message("Inicializou a thread com sucesso!"));
};

void thread::pause_thread() {
	if (!isLive())
		throw exception("Thread nao incializada. thread::pause_thread()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 4, 0));

#if defined(_WIN32)
	SuspendThread(m_thread);
#elif defined(__linux__)
	// !@ Não tem o Suspend no pthread linux
	throw exception("[thread::pause_thread][WARNING] nao tem suspend thread no pthread linux.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 12, 0));
#endif

	_smp::message_pool::getInstance().push(new message("Thread pausada com sucesso!"));
};

void thread::resume_thread() {
	if (!isLive())
		throw exception("Thread nao inicializada. thread::resume_threa()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 5, 0));

#if defined(_WIN32)
	while (ResumeThread(m_thread) > 0);
#elif defined(__linux__)
	// !@ Não tem o Resume no pthread linux
	throw exception("[thread::resume_thread][WARNING] nao tem resume thread no pthread linux.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 12, 0));
#endif

	_smp::message_pool::getInstance().push(new message("Thread resumida com sucesso!"));
};

// Exit Thread Not Gracifull
void thread::exit_thread() {
	if (!isLive())
		throw exception("Thread nao inicializada. thread::exit_thread()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 6, 0));

#if defined(_WIN32)
	pause_thread();
#elif defined(__linux__)
	// Linux no pthread não tem o pause(Suspend) thread
#endif
	
#if defined(_WIN32)
	DWORD exit_code = 0;

	GetExitCodeThread(m_thread, &exit_code);
	
	TerminateThread(m_thread, exit_code);

	WaitForSingleObject(m_thread, INFINITE);
#elif defined(__linux__)
	pthread_kill(m_thread, SIGKILL);
#endif

	//CloseHandle(m_thread);
	m_thread = INVALID_HANDLE_VALUE;

	_smp::message_pool::getInstance().push(new message("Thread Terminada abortivamente com sucesso!"));
};

void thread::waitThreadFinish(DWORD dwMilleseconds) {
#if defined(_WIN32)
	if (m_thread != INVALID_HANDLE_VALUE && WaitForSingleObject(m_thread, dwMilleseconds) != WAIT_OBJECT_0)
		throw exception("Erro ao esperar a thread acabar em WaitForSingleObject(). thread::waitThreadFinish()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 7, GetLastError()));
#elif defined(__linux__)

	if (m_thread == (pthread_t)INVALID_HANDLE_VALUE)
		return;
	
	int32_t error = 0;
	
	if ((int32_t)dwMilleseconds == INFINITE) {

		if ((error = pthread_join(m_thread, nullptr)) != 0)
			throw exception("[thread::waitThreadFinish][Error] ao esperar a thread acabar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 7, error));

	}else {

		timespec time_wait = _milliseconds_to_timespec_clock_realtime(dwMilleseconds);

		 if ((error = pthread_timedjoin_np(m_thread, nullptr, &time_wait)) != 0 && error != ETIMEDOUT)
			throw exception("[thread::waitThreadFinish][Error] ao esperar a thread acabar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREAD, 7, error));
	}

	m_thread = (pthread_t)INVALID_HANDLE_VALUE;
#endif
};

const DWORD thread::getTipo() {
	return m_tipo;
};

bool thread::isLive() {
#if defined(_WIN32)
	return (m_thread != INVALID_HANDLE_VALUE && WaitForSingleObject(m_thread, 1) == WAIT_TIMEOUT);
#elif defined(__linux__)

	timespec time_wait = _milliseconds_to_timespec_clock_realtime(1);

	return (m_thread != (pthread_t)INVALID_HANDLE_VALUE && pthread_timedjoin_np(m_thread, nullptr, &time_wait) == ETIMEDOUT);
#endif
};