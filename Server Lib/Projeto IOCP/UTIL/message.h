// Arquivo message.h
// Criado em 21/05/2017 por Acrisio
// Definição da classe message

#pragma once
#ifndef _STDA_MESSAGE_H
#define _STDA_MESSAGE_H

#include <string>

namespace stdA {
	enum {
		CL_ONLY_CONSOLE,
		CL_FILE_TIME_LOG_AND_CONSOLE,
		CL_FILE_LOG_AND_CONSOLE,
		CL_ONLY_FILE_LOG,
		CL_ONLY_FILE_TIME_LOG,
		CL_ONLY_FILE_LOG_IO_DATA,
		CL_FILE_LOG_IO_DATA_AND_CONSOLE,
		CL_ONLY_FILE_LOG_TEST,
		CL_FILE_LOG_TEST_AND_CONSOLE,
	};

    class message {
        public:
            message();
            message(std::string s, size_t _tipo = 0);
			message(std::wstring s, size_t _tipo = 0);
            ~message();

            void append(std::string s);
            void set(std::string s);

            std::string get();
			size_t getTipo();

        private:
            std::string m_message;
			size_t m_tipo;
    };
}

#endif