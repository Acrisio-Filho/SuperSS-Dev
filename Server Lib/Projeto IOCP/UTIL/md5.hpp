// Arquivo md5.hpp
// Criado em 18/03/2019 as 23:35 por Acrisio
// Definição da classe de suporte do hash de MD5 usando API do WinCrypt

#pragma once
#ifndef _STDA_MD5_HPP
#define _STDA_MD5_HPP

#if defined(_WIN32)
#include <Windows.h>
#include <wincrypt.h>
#elif defined(__linux__)
// evp_digestinit(3) - Linux man page, para mais de um algoritimo
//#include <openssl/evp.h>

/***
#include <stdio.h>
#include <openssl/evp.h>

main(int argc, char *argv[])
{
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    char mess1[] = "Test Message\n";
    char mess2[] = "Hello World\n";
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;

    OpenSSL_add_all_digests();

    if(!argv[1]) {
        printf("Usage: mdtest digestname\n");
        exit(1);
    }

    md = EVP_get_digestbyname(argv[1]/*MD5*//*);

    if(!md) {
        printf("Unknown message digest %s\n", argv[1]);
        exit(1);
    }

    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    EVP_DigestUpdate(&mdctx, mess1, strlen(mess1));
    EVP_DigestUpdate(&mdctx, mess2, strlen(mess2));
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);

    printf("Digest is: ");
    for(i = 0; i < md_len; i++) printf("%02x", md_value[i]);
        printf("\n");
}
*/
#include <openssl/md5.h>
#endif

#include <string>

namespace stdA {

#define MD5LEN 16

    class md5 {
        public:
            md5();
            ~md5();

            static bool isInit();
            static void init();

            static void destroy();

            static void processData(unsigned char* _data, uint32_t _size);

            static std::string getHash();

        /*protected:
            const char *m_hexDigits = "0123456789abcdef";*/

        private:
            static bool m_is_init;
            static bool m_is_processed;

#if defined(_WIN32)
            static HCRYPTPROV m_hProv;
            static HCRYPTHASH m_hHash;
#elif defined(__linux__)
            static MD5_CTX m_hHash;
#endif
    };
}

#endif // !_STDA_MD5_HPP