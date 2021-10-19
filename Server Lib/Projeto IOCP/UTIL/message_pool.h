// Arquivo message_pool.h
// Criado em 21/05/2017 por Acrisio
// Definição da classe message_pool

#pragma once
#ifndef _STDA_MESSAGE_POOL_H
#define _STDA_MESSAGE_POOL_H

#include "message.h"
#include <queue>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <pthread.h>
#endif

#include "../TYPE/list_fifo_console.h"
#include "../TYPE/singleton.h"

namespace stdA {
    class message_pool {
        public:
            message_pool();
            ~message_pool();

            void init();
            void destroy();

            void console_log();

			void push(message *m);
			void push_front(message *m);
			void push_back(message *m);

			message* getMessage();
            message* getFirstMessage();
            message* getLastMessage();

			message* peekMessage();
            message* peekFirstMessage();
            message* peekLastMessage();

        private:
            std::deque< message* > m_messages;

#if defined(_WIN32)
            CRITICAL_SECTION cs, cs_console;
			CONDITION_VARIABLE cv;
#elif defined(__linux__)
            pthread_mutex_t cs, cs_console;
            pthread_cond_t cv;
#endif
    };

	/*class _smp {
		public:
			static list_fifo_console_asyc< message > message_pool;
	};*/

	namespace _smp {
		typedef Singleton< list_fifo_console_asyc< message > > message_pool;
	}
}

#endif