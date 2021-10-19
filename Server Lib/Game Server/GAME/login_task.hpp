// Arquivo login_task.hpp
// Criado em 01/05/2018 as 17:26 por Acrisio
// Definição da classe LoginTask

#pragma once
#ifndef _STDA_LOGIN_TASK_HPP
#define _STDA_LOGIN_TASK_HPP

#include "../SESSION/player.hpp"

namespace stdA {
    class LoginTask {
        public:
            LoginTask(player& _session, KeysOfLogin& _kol, player_info& _pi, ClientVersion& _cv, void* _arg);
            ~LoginTask();

            void exec();

			player& getSession();
			KeysOfLogin& getKeysOfLofin();
			player_info& getInfo();
			ClientVersion& getClientVersion();

			void finishSessionInvalid();
			void sendFailLogin();
			void sendCompleteData();
			void sendReply(uint32_t _msg_id);

			uint32_t getCount();

			uint32_t incremenetCount();

			bool isFinished();

        protected:
            player& m_session;
			KeysOfLogin m_kol;
			player_info m_pi;
			ClientVersion m_cv;
			void* m_gs;

			uint32_t m_count;

			bool m_finish;
    };
}

#endif // !_STDA_LOGIN_TASK_HPP
