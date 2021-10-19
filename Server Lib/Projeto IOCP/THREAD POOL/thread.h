// Arquivo thread.h
// Criado em 21/05/2017 por Acrisio
// Definição da classe thread

#pragma once
#ifndef _STDA_THREAD_H
#define _STDA_THREAD_H

#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)

	#include "../UTIL/WinPort.h"

	#include <pthread.h>
#endif

namespace stdA {
    class thread {
        public:
            thread(DWORD tipo);
			thread(DWORD tipo, LPTHREAD_START_ROUTINE routine, LPVOID parameter, DWORD flag_priority = THREAD_PRIORITY_NORMAL, DWORD mask_affinity = 0);
            ~thread();

			void init_thread(LPTHREAD_START_ROUTINE routine, LPVOID parameter, DWORD flag_priority = THREAD_PRIORITY_NORMAL, DWORD mask_affinity = 0);
			void init_thread();
			void pause_thread();
			void resume_thread();
			void exit_thread();	// Exit Thread Not Gracifull

			void waitThreadFinish(DWORD dwMilleseconds);

			const DWORD getTipo();

			bool isLive();

        private:
#if defined(_WIN32)
            HANDLE m_thread;
#elif defined(__linux__)
			pthread_t m_thread;
#endif

            DWORD m_thread_id;
			LPTHREAD_START_ROUTINE m_routine;
			LPVOID m_parameter;
			DWORD m_flag_priority;
			DWORD m_mask_affinity;		// Afinidade mask com o CPU/Core

			DWORD m_tipo;
    };
}

#endif