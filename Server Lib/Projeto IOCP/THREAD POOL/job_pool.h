// Arquivo job_pool.h
// Criado em 21/05/2017 por Acrisio
// Definição da classe job_pool

#pragma once
#ifndef _STDA_JOB_POOL_H
#define _STDA_JOB_POOL_H

#include "job.h"
#include <deque>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <pthread.h>
#endif

namespace stdA {
    class job_pool {
        public:
            job_pool();
            ~job_pool();

            void init();
            void destroy();

			void push(job *j);
			void push_front(job *j);
			void push_back(job *j);

			job* getJob();
            job* getFirstJob();
            job* getLastJob();

			job* peekJob();
            job* peekFirstJob();
            job* peekLastJob();

        private:
            std::deque< job* > m_jobs;

#if defined(_WIN32)
            CRITICAL_SECTION cs;
			CONDITION_VARIABLE cv;
#elif defined(__linux__)
            pthread_mutex_t cs;
            pthread_cond_t cv;
#endif
    };
}

#endif