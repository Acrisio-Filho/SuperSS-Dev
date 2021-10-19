// Arquivo queue_timer.hpp
// Criado em 11/03/2021 as 17:50 por Acrisio
// Definição da classe QueueTimer para simular o ThreadPool QueueTimer do Windows no Linux

#pragma once
#ifndef _STDA_QUEUE_TIMER_HPP
#define _STDA_QUEUE_TIMER_HPP

#include <cstdint>
#include <unistd.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <pthread.h>
#include <signal.h>

#include <vector>
#include "../THREAD POOL/thread.h"
#include "../TYPE/list_fifo.h"
#include "../UTIL/event.hpp"

#define BEGIN_THREAD_SETUP(_type_class) void* result = (void*)0; \
						   try { \
                                auto sig_ign_st = (struct sigaction){SIG_IGN}; \
						   		sigaction(SIGPIPE, &sig_ign_st, nullptr); \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) { \

#define END_THREAD_SETUP(name_thread)	  } \
							}catch (exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (std::exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (...) { \
								_smp::message_pool::getInstance().push(new message(std::string((name_thread)) + " -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE)); \
							} \
							_smp::message_pool::getInstance().push(new message("Saindo do trabalho->" + std::string((name_thread)))); \
						return result; \

#ifndef ERROR_IO_PENDING
    #define ERROR_IO_PENDING (0x3E5 * -1)
#endif

namespace stdA {

    #define WT_EXECUTEDEFAULT               0x00000000
    #define WT_EXECUTEINIOTHREAD            0x00000001
    #define WT_EXECUTEONLYONCE              0x00000008
    #define WT_EXECUTELONGFUNCTION          0x00000010
    #define WT_EXECUTEINTIMERTHREAD         0x00000020
    #define WT_EXECUTEINPERSISTENTTHREAD    0x00000080
    #define WT_TRANSFER_IMPERSONATION       0x00000100

    typedef void (*WaitOrTimerCallback)(void* lpParameter, int TimerOrWaitFired);

    struct timer_epoll {
        int32_t fd;
        itimerspec time_val;
        void* lpParameter;
        WaitOrTimerCallback callback;
        uint32_t flags;
        Event* event_delete;
        uint8_t running_callback;   // Já está executando
        uint8_t deleting;   // Está deletando
        uint8_t toDelete;   // Marcou para deletar
    } /*__attribute__ ((__packed__))*/;

    struct msg_time {
        timer_epoll* te;
    };

    constexpr uint32_t MAX_THREAD_POOL = 500u;
    constexpr uint32_t MIN_THREAD_POOL = 5u;
    constexpr uint32_t NUM_MSG_IN_WAIT_TO_MAKE_MORE_THREAD = 15u;

    enum eSET_EXEC : uint32_t {
        SE_OK,
        SE_UNKNOWN_ERROR,
        SE_INVALID_PARAMETER,
        SE_ESTA_DELETANDO,
        SE_ALREADY_EXECUTING,
        SE_DELETE_TIMER_NOW,
        SE_NOT_IN_EXECTION,
    };

    enum eSET_DEL : uint32_t {
        SD_OK,
        SD_UNKNOWN_ERROR,
        SD_INVALID_PARAMETER,
        SD_ESTA_EXECUTANDO,
    };

    class QueueTimer {
        public:
            QueueTimer();
            virtual ~QueueTimer();

            bool is_good();

            uint32_t destroy();

            uint32_t createTimer(int32_t* _fd, WaitOrTimerCallback _callback, void* _lpParameter, uint32_t _dueTime, uint32_t _period, uint32_t _flags);
            uint32_t changeTimer(int32_t _fd, uint32_t _dueTime, uint32_t _period);
            uint32_t deleteTimer(int32_t _fd, Event* _event);

        protected:
            void downAllThread();
            void down_thread_timer();
            void down_threads_io_worker();

            void clear_timers();

        protected:
            static void* _timer_thread(void* lpParameter);
            static void* _io_worker(void* lpParameter);

        public:
            // static global
            static QueueTimer* CreateTimerQueue();
            static uint32_t CreateTimerQueueTimer(int32_t* _timerfd, QueueTimer* _queue_timer, WaitOrTimerCallback _callback, void* _lpParameter, uint32_t _dueTime, uint32_t _period, uint32_t _flags);
            static uint32_t DeleteTimerQueueTimer(QueueTimer* _queue_timer, int32_t _timerfd, Event* _event);
            static uint32_t ChangeTimerQueueTimer(QueueTimer* _queue_timer, int32_t _timerfd, uint32_t _dueTime, uint32_t _period);
            static uint32_t DeleteTimerQueueEx(QueueTimer* _queue_timer, Event* _event);

        protected:
            void* timer_thread();
            void* io_worker();

            void deleteTimer(timer_epoll* _timer);
            void deleteTimer(std::vector< timer_epoll* >::iterator _it);

            uint32_t setExecuteTimerExpires(timer_epoll* _timer);
            uint32_t clearExecuteTimerExpires(timer_epoll* _timer);
            uint32_t setDeleteTimer(timer_epoll* _timer, Event* _event);
            uint32_t setDeleteTimer(std::vector< timer_epoll* >::iterator _it, Event* _event);
            bool checkMarkToDelete(timer_epoll* _timer);

        protected:
            list_fifo_asyc< msg_time > m_msgs;

            volatile uint32_t m_down_io_worker;
            volatile uint32_t m_down_timer;

        protected:
            int32_t m_epollfd;
            std::vector< timer_epoll* > m_timers;
            
            thread *m_timer_thread;
            std::vector< thread* > m_thread_pool;

            pthread_mutex_t m_cs;

            bool m_good;
    };
}

#endif