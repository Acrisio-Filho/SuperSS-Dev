// Arquivo normal_manager_db.hpp
// Criado em 01/05/2018 as 12:45 por Acrisio
// Definição da classe NormalManagerDB

#pragma once
#ifndef _STDA_NORMAL_MANAGER_DB_HPP
#define _STDA_NORMAL_MANAGER_DB_HPP

#include "normal_db.hpp"
#include <vector>

#include "../SOCKET/session.h"
#include "../THREAD POOL/thread.h"

#include "../TYPE/singleton.h"

#define NUM_DB_THREAD   26ul

namespace stdA {
    class NormalManagerDB {
		public:
			struct DownEvent {
				public:
#if defined(_WIN32)
					typedef LONG _MY_LONG;
#elif defined(__linux__)
					typedef int32_t _MY_LONG;
#endif
				public:
					DownEvent();

					void set(_MY_LONG _value = 0);
					bool isLive();

				protected:
					_MY_LONG volatile m_continue;
			};

        public:
            NormalManagerDB();
            ~NormalManagerDB();

            void create(uint32_t _db_instance_num = NUM_DB_THREAD);

			void init();
			void destroy();

			void checkIsDeadAndRevive();

			int add(NormalDB::msg_t* _msg);
			int add(uint32_t _id, pangya_db *_pangya_db, callback_response _callback_response, void* _arg);

			void freeAllWaiting(std::string _msg);

        // Methods que o normal db faz para o server e o channel
        public:
            void insertLoginLog(session& _session, NormalDB::msg_t* _msg);

		protected:
#if defined(_WIN32)
			static DWORD CALLBACK _Revive(LPVOID lpParameter);

			DWORD Revive();
#elif defined(__linux__)
			static void* _Revive(void* lpParameter);

			void* Revive();
#endif

		private:
			inline void checkDBInstanceNumAndFix();

        protected:
            std::vector< NormalDB* > m_dbs;

            bool m_state;

			uint32_t m_db_instance_num;

            thread *m_pRevive;
			DownEvent m_continue_revive;
    };

	namespace snmdb {
		typedef Singleton< NormalManagerDB > NormalManagerDB;
	};
}

#endif // !_STDA_NORMAL_MANAGER_DB_HPP
