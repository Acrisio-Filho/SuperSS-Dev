// Arquivo mssql.h
// Criado em 27/01/2018 as 23:08 por Acrisio
// Definição da classe mssql

#pragma once
#ifndef _STDA_MSSQL_H
#define _STDA_MSSQL_H

#include "database.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include <sql.h>
#include <sqlext.h>

#include "response.h"

// define SHOW_SUCCESS_INGO_MSG para mostrar as msg que o mssql server retorno mesmo com sucesso
//#define SHOW_SUCCESS_INFO_MSG

namespace stdA {
    class mssql : public database {
        public:
			mssql();
			mssql(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port);
			mssql(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port);
            virtual ~mssql();

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
				ctx_db(uint32_t _ul = 0u) {
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