// Arquivo timer_manager.h
// Criado em 01/01/2018 por Acrisio
// Definição da classe timer_manager

#pragma once
#ifndef _STD_TIMER_MANAGER_H
#define _STD_TIMER_MANAGER_H

#ifdef __linux__
	#include <pthread.h>
#endif

#include "timer.h"

namespace stdA {
    class timer_manager {
        public:
            timer_manager();
            ~timer_manager();

			timer *createTimer(DWORD _time, timer::timer_param *_arg, uint32_t _tipo = timer::NORMAL);
			timer *createTimer(DWORD _time, timer::timer_param *_arg, std::vector< DWORD > _table_interval, uint32_t _tipo = timer::PERIODIC);
			void deleteTimer(timer *_timer);

			bool isEmpty();

		protected:
			void check_event_end();
			void clear_timers();

			size_t findIndexTimer(timer *_timer);

		protected:
#if defined(_WIN32)
			HANDLE m_hTimerQueue;			// Fila de Timer
			HANDLE m_event;					// check event end
#elif defined(__linux__)
			QueueTimer* m_hTimerQueue;
			Event* m_event;
#endif

			std::vector< timer* > m_timers;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif

    };
}

#endif