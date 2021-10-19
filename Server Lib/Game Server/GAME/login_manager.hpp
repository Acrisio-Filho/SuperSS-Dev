// Arquivo login_manager.hpp
// Criado em 01/05/2018 as 17:28 por Acrisio
// Definição da classe LoginManager

#pragma once
#ifndef _STDA_LOGIN_MANAGER_HPP
#define _STDA_LOGIN_MANAGER_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "login_task.hpp"
#include <vector>

#include "../SESSION/player.hpp"

#include "../../Projeto IOCP/THREAD POOL/thread.h"

namespace stdA {
    class LoginManager {
        public:
            LoginManager();
            ~LoginManager();

            LoginTask* createTask(player& _session, KeysOfLogin& _kol, player_info& _pi, ClientVersion& _cv, void* _gs);
			void deleteTask(LoginTask* _task);

            static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

			static bool canSameIDLogin();
			static const std::string& getClientVersionSideServer();

#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _checkTaskFinish(LPVOID _lpParameter);
#elif defined(__linux__)
			static void* _checkTaskFinish(LPVOID _lpParameter);
#endif

		protected:
			void loadIni();

			void clear();
			size_t getSize();

        protected:
            std::vector< LoginTask* > v_task;

			thread *m_pThread;

#if defined(_WIN32)
			DWORD checkTaskFinish();
#elif defined(__linux__)
			void* checkTaskFinish();
#endif

		protected:
			static bool m_same_id_login;
			static std::string m_client_version;

#if defined(_WIN32)
			LONG volatile m_check_task_finish_shutdown;
#elif defined(__linux__)
			uint32_t volatile m_check_task_finish_shutdown;
#endif

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
    };
}

#endif //!_STDA_LOGIN_MANAGER_HPP
