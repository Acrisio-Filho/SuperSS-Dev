// Arquivo reader_ini.hpp
// Criado em 26/03/2018 as 22:23 por Acrisio
// Definição da classe ReaderIni

#pragma once
#ifndef _STDA_READER_INI_HPP
#define _STDA_READER_INI_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <cstdint>
#endif

#include <string>

namespace stdA {
    class ReaderIni {
        public:
            enum STATE : unsigned char {
                UNINITIALIZED,
                INITIALIZED,
            };

        public:
            ReaderIni(std::string _file_name);
            ~ReaderIni();

			void init();

            std::string readString(std::string _section, std::string _key);
            int readInt(std::string _section, std::string _key);   
			int64_t readBigInt(std::string _section, std::string _key);

        protected:
            std::string m_file_name;
			std::string m_full_file_name;
            STATE m_state;
    };
}

#endif