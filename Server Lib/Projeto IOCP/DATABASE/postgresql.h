// Arquivo postgresql.h
// Criado em 16/07/2021 as 09:23 por Acrisio
// Definição da classe postgresql

#pragma once
#ifndef _STDA_POSTGRESQL_H
#define _STDA_POSTGRESQL_H

#include "database.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include <sql.h>
#include <sqlext.h>

#include "response.h"

// define SHOW_SUCCESS_INGO_MSG para mostrar as msg que o postgresql server retorno mesmo com sucesso
//#define SHOW_SUCCESS_INFO_MSG

namespace stdA {
    class postgresql : public database {
        public:
			postgresql();
			postgresql(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port);
			postgresql(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port);
            virtual ~postgresql();

            virtual void init() override;
            virtual void destroy();

			virtual bool hasGoneAway() override;

            virtual void connect() override;
			virtual void connect(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port) override;
			virtual void connect(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port) override;
            virtual void reconnect() override;
            virtual void disconnect() override;

			virtual response* ExecQuery(std::wstring _query) override;
			virtual response* ExecQuery(std::string _query) override;
			virtual response* ExecProc(std::wstring _proc_name, std::wstring _proc_params) override;
			virtual response* ExecProc(std::string _proc_name, std::string _proc_params) override;

            virtual std::string makeText(std::string _value) override;
            virtual std::wstring makeText(std::wstring _value) override;

			virtual std::string makeEscapeKeyword(std::string _value) override;
			virtual std::wstring makeEscapeKeyword(std::wstring _value) override;

			virtual void clear_stmt(std::wstring _query);

        protected:
            struct ctx_db {
				ctx_db(unsigned long _ul = 0ul) {
					clear();
				};
				void clear() {
					hEnv = SQL_NULL_HANDLE;
					hDbc = SQL_NULL_HANDLE;
					hStmt = SQL_NULL_HANDLE;
				};
                SQLHENV     hEnv;
                SQLHDBC     hDbc;
                SQLHSTMT    hStmt;
            };

            ctx_db m_ctx;

        private:
            static void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, ERROR_TYPE _error_type, RETCODE RetCode, std::wstring _query = L"");
    };
}

#endif