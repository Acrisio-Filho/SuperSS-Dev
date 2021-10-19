// Arquivo timer.h
// Criado em 01/01/2018 por Acrisio
// Definição da classe timer

#pragma once
#ifndef _STDA_TIMER_H
#define _STDA_TIMER_H

#include <cstdint>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "queue_timer.hpp"
#include "../UTIL/event.hpp"
#include <time.h>

#ifndef BOOLEAN
    #define BOOLEAN int
#endif

#ifndef CALLBACK
    #define CALLBACK // __stdcall
#endif

#endif

#include <vector>
#include "../TYPE/stdAType.h"
#include "../THREAD POOL/job.h"
#include "../THREAD POOL/job_pool.h"

namespace stdA {

#if defined(__linux__)
    constexpr uint32_t CLOCK_TICK_TO_TIMER = CLOCK_MONOTONIC_RAW;
#endif

    class timer {
        public:
            enum TIMER_STATE : uint32_t {
				STANDBY,				// Stand By, esperando a thread callback ser totalmente inicializada
				RUNNING,				// Rodando
                STOP,					// Request
				STOPPING,				// Executando o request
				STOPPED,				// Executado o request
				PAUSE,					// Request
				PAUSING,				// Executando o request
                PAUSED,					// Executado o request
            };

            enum TIMER_TYPE : uint32_t {
                NORMAL,                 // Normal
                PERIODIC,
				PERIODIC_INFINITE,
            };

			struct timer_param {
				job _job;
				job_pool& _job_pl;
			};

        public:
            timer(
#if defined(_WIN32)
                HANDLE _timerQueue, 
#elif defined(__linux__)
                QueueTimer* _timerQueue,
#endif
                DWORD _time, timer_param *_arg, uint32_t _tipo = NORMAL);
            timer(
#if defined(_WIN32)
                HANDLE _timerQueue, 
#elif defined(__linux__)
                QueueTimer* _timerQueue,
#endif
                DWORD _time, timer_param *_arg, std::vector< DWORD > _table_interval, uint32_t _tipo = PERIODIC);
            ~timer();

			static void CALLBACK callback(LPVOID lpParameter, BOOLEAN TimerOrWaitFired);

#if defined(_WIN32)
            HANDLE getTimer();
#elif defined(__linux__)
            int32_t getTimer();
#endif
            //DWORD getTime();
            //uint64_t getElapsed();

            void restart();					// Restart Timer
            void pause();					// Pause Timer
            void stop();					// Stop Timer
            void start();					// Start and Resume Timer

			TIMER_STATE getState();
			void setState(TIMER_STATE _state);

			DWORD getElapsed();

        protected:
            void destroy();
			void init();
			void _stop(TIMER_STATE _state);
			void _stop_from_callback(TIMER_STATE _state);
			void check_event_end();

			void hasTimerTick();

        protected:
#if defined(_WIN32)
            HANDLE m_hTimerQueue;			// Timer Queue Pai
			HANDLE m_hTimer;				// Timer
			HANDLE m_hEvent;				// Event for delete timer of queue
#elif defined(__linux__)
            QueueTimer* m_hTimerQueue;
            int32_t m_hTimer;
            Event* m_hEvent;
#endif

			DWORD m_time_fix;				// Time in milliseconds not change
			DWORD m_time;					// Time in milliseconds

#if defined(_WIN32)
            LARGE_INTEGER m_elapsed;
			LARGE_INTEGER m_start;
			LARGE_INTEGER m_frequency;
#elif defined(__linux__)
            timespec m_elapsed;
			timespec m_start;
			timespec m_frequency;
#endif

            uint32_t m_state;			// PAUSED, RUNNING AND STOP
            uint32_t m_tipo;			// NORMAL, PERIODIC

            std::vector< DWORD > m_table_interval;

			timer_param *m_arg;

#if defined(_WIN32)
            CRITICAL_SECTION m_cs;
			CONDITION_VARIABLE m_cv;
#elif defined(__linux__)
            pthread_mutex_t m_cs;
            pthread_cond_t m_cv;
#endif
    };
}

#endif