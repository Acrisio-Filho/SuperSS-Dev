// Arquivo pangya_db.h
// Criado em 25/12/2017 por Acrisio
// Definição da classe pangya_db

#pragma once
#ifndef _STDA_PANGYA_DB_H
#define _STDA_PANGYA_DB_H

#include "../DATABASE/exec_query.h"
#include "../TYPE/list_fifo.h"
#include "../TYPE/list_async.h"
#include "../UTIL/string_util.hpp"
#include <string>

#include "../DATABASE/database.h"

#if defined(__linux__)
#include "../UTIL/event.hpp"
#endif

#ifndef IFNULL
#define IFNULL(_func, _data) ((_data == nullptr) ? 0 : _func((_data))) 
#endif

namespace stdA {
    class pangya_db {
        public:
            pangya_db(bool _waitable);
            virtual ~pangya_db();

            virtual void exec(database& _db);

			virtual exception& getException();

			virtual void waitEvent();
			virtual void wakeupWaiter();
			virtual bool isWaitable();

			friend class NormalDB;

        protected: // Metôdos
			virtual response* _insert(database& _db, std::string _query);
			virtual response* _insert(database& _db, std::wstring _query);
			virtual response* _update(database& _db, std::string _query);
			virtual response* _update(database& _db, std::wstring _query);
			virtual response* _delete(database& _db, std::string _query);
			virtual response* _delete(database& _db, std::wstring _query);
			virtual response* consulta(database& _db, std::string _query);
			virtual response* consulta(database& _db, std::wstring _query);
			virtual response* procedure(database& _db, std::string _name, std::string _params);
			virtual response* procedure(database& _db, std::wstring _name, std::wstring _params);

            virtual void postAndWaitResponseQuery(exec_query& _query);

			virtual void clear_result(result_set*& _rs);
			virtual void clear_response(response* _res);

            virtual void checkColumnNumber(uint32_t _number_cols1, uint32_t _number_cols2);
			virtual void checkResponse(response* r, std::string _exception_msg);
			virtual void checkResponse(response* r, std::wstring _exception_msg);

        protected:
            virtual void lineResult(result_set::ctx_res* _result, uint32_t _index_result) = 0;
            virtual response* prepareConsulta(database& _db) = 0;

			// get Class name
			virtual std::string _getName() = 0;
			virtual std::wstring _wgetName() = 0;

		protected:
			static bool is_valid_c_string(char* _ptr_c_string);

		protected:
			exception m_exception;
			bool m_waitable;
#if defined(_WIN32)
			HANDLE hEvent;
#elif defined(__linux__)
			Event *hEvent;
#endif

        public: // Metôdos estaticos
            static list_fifo_asyc< exec_query > m_query_pool;
            static list_async< exec_query* > m_cache_query;

            static bool compare(exec_query* _query1, exec_query* _query2);
    };
}

#endif