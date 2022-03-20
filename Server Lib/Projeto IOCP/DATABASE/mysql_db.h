// Arquivo mysql_db.h
// Criado em 23/07/2017 por Acrisio
// Definição da classe mysql_db

#pragma once
#ifndef _STDA_MYSQL_DB_H
#define _STDA_MYSQL_DB_H

#include <cstdint>

#if INTPTR_MAX == INT64_MAX
	#define PATH_MYSQL_INCLUDE "C:\\Program Files\\MySQL\\MySQL Connector C 6.1\\include\\mysql.h"
	#define PATH_MYSQL_LIB "C:\\Program Files\\MySQL\\MySQL Connector C 6.1\\lib\\libmysql.lib"
#elif INTPTR_MAX == INT32_MAX
	#define PATH_MYSQL_INCLUDE "C:\\Program Files (x86)\\MySQL\\MySQL Connector C 6.1\\include\\mysql.h"
	#define PATH_MYSQL_LIB "C:\\Program Files (x86)\\MySQL\\MySQL Connector C 6.1\\lib\\libmysql.lib"
#else
	#error Unknown pointer size or missing size macros!
#endif

#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#if defined(_WIN32)
#include PATH_MYSQL_INCLUDE
#elif defined(__linux__)
#include <mysql/mysql.h>
#endif

#include "response.h"
#include "database.h"

namespace stdA {
    class mysql_db : public database {
        public:
            mysql_db(unsigned short _db_port = 3306);
			mysql_db(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port = 3306);
			mysql_db(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port = 3306);
            ~mysql_db();

            virtual void init() override;
			virtual void destroy();

            virtual void connect() override;
			virtual void connect(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port = 3306) override;
			virtual void connect(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port = 3306) override;
            virtual void reconnect() override;
            virtual void disconnect() override;

			virtual response* ExecQuery(std::string _query) override;
			virtual response* ExecQuery(std::wstring _query) override;
			//response* try_exec_query(std::string query);
			virtual response* ExecProc(std::string _proc_name, std::string _proc_params) override;
			virtual response* ExecProc(std::wstring _proc_name, std::wstring _proc_params) override;

			virtual std::string makeText(std::string _value) override;
			virtual std::wstring makeText(std::wstring _value) override;

			virtual std::string makeEscapeKeyword(std::string _value) override;
			virtual std::wstring makeEscapeKeyword(std::wstring _value) override;

        protected:
            MYSQL *m_mysql;

		/*private:
			CRITICAL_SECTION m_cs;

/*#ifdef _DEBUG
			// CRITICAL_SECTION DEBUG TEM 4 Bytes a+
			char cs_debug[4];
#endif*/
    };
}

#endif