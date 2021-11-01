// Arquivo crypt.h
// Criado em 23/09/2017 por Acrisio
// Definição da classe crypt

#pragma once
#ifndef _STDA_CRYPT_H
#define _STDS_CRYPT_H

#include <cstdint>

#if defined(__linux__)
    #include <stddef.h>
#endif

#define Keys _Keys

namespace stdA {
    class crypt {
        public:
            crypt();
            ~crypt();

        unsigned char init_key(unsigned char _key_high, unsigned char _key_low);

        void decrypt(unsigned char* _cipher, size_t _size, unsigned char* _plain);

        void encrypt(unsigned char* _plain, size_t _size, unsigned char* cipher);

        bool check_key(unsigned char _key);

        protected:
            uint32_t m_key[2];
    };
}

#endif