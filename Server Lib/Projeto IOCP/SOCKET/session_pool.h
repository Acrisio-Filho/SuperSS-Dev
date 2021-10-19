// Arquivo session_pool.h
// Criado em 17/06/2017 por Acrisio
// Definição da classe session_pool

#pragma once
#ifndef STDA_SESSION_POOL_H
#define STDA_SESSION_POOL_H

#include "session.h"
#include <windows.h>
#include <vector>

namespace stdA {
	class threadpool;

    class session_pool {
        public:
			session_pool(size_t _TTL = 10000/*10sec*/);
            session_pool(threadpool* _threadpool, size_t _max_session, size_t _TTL = 10000/*10sex*/);
            ~session_pool();
			
			void init(threadpool* _threadpool, size_t _max_session, size_t _TTL = 10000/*10sex*/);

			static int64_t count;

        private:
			void destroy();
        
        public:
            session* addSession(SOCKET _sock, SOCKADDR_IN _addr, unsigned char _key);
            void deleteSession(session *s);

			void checkSessionLive();

			session* findSession(uint32_t uid, uint32_t oid = ~0);

			std::vector< session* > getChannelSessions(unsigned char _channel = ~0, unsigned char _lobby = ~0);

        private:
            void findNextSessionFree();

        private:
            session *m_sessions;

            size_t m_max_session;
            size_t m_session_free;
			size_t m_TTL;			// Time To Live

            CRITICAL_SECTION cs;
    };
}

#endif