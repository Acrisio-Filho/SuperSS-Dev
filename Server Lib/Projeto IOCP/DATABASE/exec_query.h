// Arquivo exec_query.h
// Criado em 11/08/2017 por Acrisio
// Definição da classe exec_query

#pragma once
#ifndef _STDA_EXEC_QUERY_H
#define _STDA_EXEC_QUERY_H

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include "../UTIL/event.hpp"
#include <pthread.h>
#endif

#include <string>
#include "response.h"

namespace stdA {
    class exec_query {
		public:
			enum QUERY_TYPE : unsigned char {
				_QUERY,
				_PROCEDURE,
				_INSERT,
				_UPDATE,
				_DELETE
            };


        public:
            exec_query();
			exec_query(std::string _query, unsigned char _tipo = _QUERY);
			exec_query(std::wstring _query, unsigned char _tipo = _QUERY);
			exec_query(std::string _name_proc, std::string _params_proc, unsigned char _tipo = _PROCEDURE);
			exec_query(std::wstring _name_proc, std::wstring _params_proc, unsigned char _tipo = _PROCEDURE);
            ~exec_query();

			std::string getQuery();
			std::wstring& getWQuery();
			std::string getParams();
			std::wstring& getWParams();
			
			response *getRes();

			void setQuery(std::string _query);
			void setQuery(std::wstring _query);
			void setParam(std::string _params);
			void setParam(std::wstring _params);
			void setRes(response* _res);

			void waitEvent(DWORD milliseconds = INFINITE);
			void setEvent();
			void resetEvent();
			void pulseEvent();

			unsigned char getType();

			static void enter();
			static void release();
			static int getSpinCount();
        
        protected:
            std::wstring m_query;		// Aqui pode ser nome procedure, quanto query completa
			std::wstring m_params;
            response* m_res;

		protected:
			unsigned char m_type;

#if defined(_WIN32)
			static LONG volatile m_spin_count;
#elif defined(__linux__)
			static int32_t volatile m_spin_count;
#endif

#if defined(_WIN32)
            HANDLE m_hEvent;
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			Event *m_hEvent;
			pthread_mutex_t m_cs;
#endif
    };
}

#endif