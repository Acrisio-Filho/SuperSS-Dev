// Arquivo database.h
// Criado em 27/01/2018 as 23:10 por Acrisio
// Definição da classe database

#pragma once
#ifndef _STDA_DATABASE_H
#define _STDA_DATABASE_H

#include <string>
#include "response.h"

#define DB_ESCAPE_KEYWORD_A "-//"
#define DB_ESCAPE_KEYWORD_W L"-//"

#define DB_MAKE_ESCAPE_KEYWORD_A(_keyword) DB_ESCAPE_KEYWORD_A _keyword DB_ESCAPE_KEYWORD_A
#define DB_MAKE_ESCAPE_KEYWORD_W(_keyword) DB_ESCAPE_KEYWORD_W _keyword DB_ESCAPE_KEYWORD_W

namespace stdA {
    class database {
        public:
			database(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port);
			database(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port);
            virtual ~database();

            virtual void init() = 0;

            bool is_valid();
            bool is_connected();

            virtual void connect() = 0;
			virtual void connect(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port) = 0;
			virtual void connect(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port) = 0;
            virtual void reconnect() = 0;
            virtual void disconnect() = 0;

			virtual response* ExecQuery(std::wstring _query) = 0;
			virtual response* ExecQuery(std::string _query) = 0;
			virtual response* ExecProc(std::wstring _proc_name, std::wstring _proc_params) = 0;
			virtual response* ExecProc(std::string _proc_name, std::string _proc_params) = 0;

            virtual std::string makeText(std::string _value) = 0;
            virtual std::wstring makeText(std::wstring _value) = 0;

			virtual std::string makeEscapeKeyword(std::string _value) = 0;
			virtual std::wstring makeEscapeKeyword(std::wstring _value) = 0;

			std::string parseEscapeKeyword(std::string _value);
			std::wstring parseEscapeKeyword(std::wstring _value);

		private:
			bool members_empty();

        protected:
            bool m_state;
            bool m_connected;

            std::wstring m_db_address;
            std::wstring m_db_name;
            std::wstring m_user_name;
            std::wstring m_user_pass;
            unsigned short m_db_port;
    };
}

#endif