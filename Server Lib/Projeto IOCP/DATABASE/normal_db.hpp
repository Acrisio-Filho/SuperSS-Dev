// Arquivo normal_db.hpp
// Criado em 01/05/2018 as 10:17 por Acrisio
// Definição da classe NormalDB

#pragma once
#ifndef _STDA_NORMAL_DB_HPP
#define _STDA_NORMAL_DB_HPP

#include "../PANGYA_DB/pangya_db.h"
#include "../THREAD POOL/thread.h"
#include "../TYPE/list_async.h"

#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"

#include "../TYPE/stdAType.h"

#include "database.h"

#include "../UTIL/message_pool.h"

namespace stdA {
    typedef void(*callback_response)(uint32_t _msg_id, pangya_db& _db, void* _arg);

    class NormalDB {
        public:
            struct msg_t {
				msg_t() : id(0), _pangya_db(nullptr), func(nullptr), arg(nullptr) {
				};
				msg_t(uint32_t _id, pangya_db *__pangya_db, callback_response _callback_response, void *_arg)
					: id(_id), _pangya_db(__pangya_db), func(_callback_response), arg(_arg) {
				};
				~msg_t() {	// Destrutor

					// Libera memória allocada no pangya_db, se for para desalocar ela mesmo aqui
					if (_pangya_db != nullptr && !_pangya_db->isWaitable())
						delete _pangya_db;

					_pangya_db = nullptr;
				};
                void clear() { memset(this, 0, sizeof(msg_t)); };
                void execFunc() {
                    if (func == nullptr)
                        return;

					try {
						if (_pangya_db == nullptr)
							throw exception("_pangya_db is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 1, 0));

						func(id, *_pangya_db, arg);

					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("[NormalDB::mgs_t::execFunc][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
					}
                };
                void execQuery(database& _db) {
                    if (!_db.is_connected() && !_db.is_valid())
                        throw exception("[NormalDB::msg_t::execQuery][Error] argument database _db is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 5, 0));

                    if (_pangya_db == nullptr)
                        throw exception("[NormalDB::mgs_t::execQuery][Error] _pangya_db is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 1, 0));

                    _pangya_db->exec(_db);
                };
				void wakeupWaiter() {
					if (_pangya_db == nullptr)
						throw exception("[NormalDB::mgs_t::wakeupWaiter][Error] _pangya_db is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 1, 0));

					_pangya_db->wakeupWaiter();

					// Quando é Waitable, quem chamou não destroí a msg, quem libera a memóra da msg é o db manager.
					// Aqui coloca o _pangya_db como nullptr, por que o waitable é uma referência que libera a memória do _pangya_db e quem criou a msg
					// Quando vai liberar a memória no destrutor da msg ele verifica o _pangya_db, mas como o Waitable, quem criou que libera a memória ele pode pegar um _pangya_db com acesso de memório inválida.
					// Colocando nullptr ele vai pular isso e não vai da accesso de memória inválida.
					_pangya_db = nullptr;
				};
				bool isWaitable() {
					if (_pangya_db == nullptr)
						throw exception("[NormalDB::mgs_t::isWaitable][Error] _pangya_db is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::NORMAL_DB, 1, 0));

					return _pangya_db->isWaitable();
				};
            protected:
				uint32_t id;		// ID da msg
                pangya_db* _pangya_db;
                callback_response func;
                void* arg;
            };

            enum TT_DB : unsigned {
                TT_NORMAL_EXEC_QUERY,
                TT_NORMAL_RESPONSE,
            };

        public:
            NormalDB();
            ~NormalDB();

            void init();
            void sendCloseAndWait();

#if defined(_WIN32)
            static DWORD WINAPI CALLBACK ThreadFunc(LPVOID lpParameter);
            static DWORD WINAPI CALLBACK ThreadFunc2(LPVOID lpParameter);
#elif defined(__linux__)
            static void* ThreadFunc(LPVOID lpParameter);
            static void* ThreadFunc2(LPVOID lpParameter);
#endif

            int add(msg_t* _msg);
			int add(uint32_t _id, pangya_db *_pangya_db, callback_response _callback_response, void* _arg);

			void checkIsDeadAndRevive();

        protected:
            void close();

        protected:
#if defined(_WIN32)
            DWORD runExecQuery();
            DWORD runResponse();
#elif defined(__linux__)
            void* runExecQuery();
            void* runResponse();
#endif

        protected:
            bool loadIni();

        protected:
            thread *m_pExec;
            thread *m_pResponse;

            ctx_db m_ctx_db;

            bool m_state;

			uint32_t volatile m_continue_exec;
			uint32_t volatile m_continue_response;

        // Member of Class not instance
        protected:
            list_async< msg_t* > m_unExec;
            list_async< msg_t* > m_unResponse;
    };
}

#endif // !_STDA_NORMAL_DB_HPP
