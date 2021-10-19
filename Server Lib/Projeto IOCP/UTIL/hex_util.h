// Arquivo hex_util.h
// Criado em 10/06/2017 por Acrisio
// Definição da classe hex

#pragma once
#ifndef STDA_HEX_UTIL_H
#define STDA_HEX_UTIL_H

#include <sstream>

namespace stdA {
    class hex_util {
        public:
            static void ShowHex(unsigned char* buffer, size_t size);
            static std::string BufferToHexString(unsigned char* buffer, size_t size);
            static std::string StringToHexString(std::string s);
			static std::string ltoaToHex(int32_t l);
			static std::string lltoaToHex(int64_t ll);
    };
}

#endif