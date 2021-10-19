// Arquivo job_pool.cpp
// Criado em 21/05/2017 por Acrisio
// Implementação da classe job_pool

#include "job_pool.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

job_pool::job_pool() {
    init();
};

job_pool::~job_pool() {
    if (!m_jobs.empty()) {
        for (size_t i = 0; i < m_jobs.size(); i++)
            if (m_jobs[i] != nullptr)
                delete m_jobs[i];
        
        m_jobs.clear();
        m_jobs.shrink_to_fit();
    }

    destroy();
};

void job_pool::init() {
#if defined(_WIN32)
    InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cv);
#elif defined(__linux__)
    INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

    pthread_cond_init(&cv, nullptr);
#endif
};

void job_pool::destroy() {
#if defined(_WIN32)
    DeleteCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_destroy(&cs);
    pthread_cond_destroy(&cv);
#endif
};

void job_pool::push(job *j) {
    push_back(j);
};

void job_pool::push_front(job *j) {
#if defined(_WIN32)
    EnterCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_lock(&cs);
#endif

    m_jobs.push_front(j);

#if defined(_WIN32)
    LeaveCriticalSection(&cs);

	WakeConditionVariable(&cv);
#elif defined(__linux__)
    pthread_mutex_unlock(&cs);

    pthread_cond_signal(&cv);
#endif
};

void job_pool::push_back(job *j) {
#if defined(_WIN32)
    EnterCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_lock(&cs);
#endif

    m_jobs.push_back(j);

#if defined(_WIN32)
    LeaveCriticalSection(&cs);

	WakeConditionVariable(&cv);
#elif defined(__linux__)
    pthread_mutex_unlock(&cs);

    pthread_cond_signal(&cv);
#endif
};

job* job_pool::getJob() {
    return getFirstJob();
};

job* job_pool::getFirstJob() {
    job *j = nullptr;

#if defined(_WIN32)
    EnterCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_lock(&cs);
#endif

	while (m_jobs.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::getFirstJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 1, 0));
		}
#elif defined(__linux__)
        int32_t error = 0;

        if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
            pthread_mutex_unlock(&cs);
			throw exception("[job_pool::getFirstJob][Error] ao pegar o sinal do evento.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 1, error));
		}
#endif
	}

    j = m_jobs.front();
    m_jobs.pop_front();

#if defined(_WIN32)
    LeaveCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_unlock(&cs);
#endif

    return j;
};

job* job_pool::getLastJob() {
    job *j = nullptr;

#if defined(_WIN32)
    EnterCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_lock(&cs);
#endif

	while (m_jobs.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::getLastJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 2, 0));
		}
#elif defined(__linux__)
        int32_t error = 0;

        if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::getLastJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 2, error));
		}
#endif
	}	

	j = m_jobs.back();
	m_jobs.pop_back();
 

#if defined(_WIN32)
    LeaveCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_unlock(&cs);
#endif

    return j;
};

job* job_pool::peekJob() {
    return peekFirstJob();
};

job* job_pool::peekFirstJob() {
    job *j = nullptr;

#if defined(_WIN32)
    EnterCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_lock(&cs);
#endif

	while (m_jobs.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::peekFirstJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 3, 0));
		}
#elif defined(__linux__)
        int32_t error = 0;
        if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::peekFirstJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 3, error));
		}
#endif
	}
       
	j = m_jobs.front();

#if defined(_WIN32)
    LeaveCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_unlock(&cs);
#endif

    return j;
};

job* job_pool::peekLastJob() {
    job *j = nullptr;

#if defined(_WIN32)
    EnterCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_lock(&cs);
#endif

	while (m_jobs.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::peekLastJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 4, 0));
		}
#elif defined(__linux__)
        int32_t error = 0;
        
        if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao pegar o sinal do evento. job_pool::peekLastJob()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB_POOL, 4, error));
		}
#endif
	}
       
	j = m_jobs.back();

#if defined(_WIN32)
    LeaveCriticalSection(&cs);
#elif defined(__linux__)
    pthread_mutex_unlock(&cs);
#endif

    return j;
};