// Arquivo normal_manager_db.hpp
// Criado em 01/05/2018 as 12:45 por Acrisio
// Definição da classe NormalManagerDB

#pragma once
#ifndef _STDA_NORMAL_MANAGER_DB_HPP
#define _STDA_NORMAL_MANAGER_DB_HPP

#include "normal_db.hpp"
#include <vector>

#include "../SOCKET/session.h"

#define NUM_DB_THREAD   26ul

namespace stdA {
    class NormalManagerDB {
        public:
            NormalManagerDB();
            ~NormalManagerDB();

            static void create(uint32_t _db_instance_num = NUM_DB_THREAD);
			static void destroy();

			static void checkIsDeadAndRevive();

			static int add(NormalDB::msg_t* _msg);
			static int add(uint32_t _id, pangya_db *_pangya_db, callback_response _callback_response, void* _arg);

        // Methods que o normal db faz para o server e o channel
        public:
            static void insertLoginLog(session& _session, NormalDB::msg_t* _msg);

		private:
			inline static void checkDBInstanceNumAndFix();

        protected:
            static std::vector< NormalDB* > m_dbs;

            static bool m_state;

			static uint32_t m_db_instance_num;
    };
}

#endif // !_STDA_NORMAL_MANAGER_DB_HPP
