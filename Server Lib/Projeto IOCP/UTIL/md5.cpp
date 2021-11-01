// Arquivo md5.cpp
// Criado em 18/03/2019 as 23:49 por Acrisio
// Implementação da classe de suporte do hash de MD5 usando API do WinCrypt

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "md5.hpp"
#include "exception.h"
#include "../TYPE/stda_error.h"
#include "message_pool.h"

#include "hex_util.h"

using namespace stdA;

bool md5::m_is_init = false;
bool md5::m_is_processed = false;

#if defined(_WIN32)
HCRYPTPROV md5::m_hProv = 0;
HCRYPTHASH md5::m_hHash = 0;
#elif defined(__linux__)
MD5_CTX md5::m_hHash = {0u};
#endif

md5::md5() {

    try {
        
        init();
    
    }catch (exception& e) {

        _smp::message_pool::getInstance().push(new message("[md5::md5][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

        m_is_init = false;
        m_is_processed = false;
    }
}

md5::~md5() {

    destroy();
}

bool md5::isInit() {
    return m_is_init;
}

void md5::init() {

    if (isInit())
        destroy();

#if defined(_WIN32)
    if (!CryptAcquireContext(&m_hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        throw exception("[md5::init][Error] nao conseguiu adiquirir o Crypt Context.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 1, GetLastError()));

    if (!CryptCreateHash(m_hProv, CALG_MD5, 0, 0, &m_hHash)) {

        if (m_hProv != 0)
            CryptReleaseContext(m_hProv, 0);

        throw exception("[md5::init][Error] nao conseguiu Criar o Crypt Hash MD5.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 2, GetLastError()));
    }
#elif defined(__linux__)
    if (!MD5_Init(&m_hHash))
        throw exception("[md5::init][Error] nao conseguiu Inicializar o MD5 Context.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 2, errno));
#endif

    m_is_init = true;
    m_is_processed = false;
}

void md5::destroy() {

#if defined(_WIN32)
    if (m_hProv != 0)
        CryptReleaseContext(m_hProv, 0);

    if (m_hHash != 0)
        CryptDestroyHash(m_hHash);

	m_hProv = 0;
	m_hHash = 0;
#endif

    m_is_init = false;
    m_is_processed = false;    
}

void md5::processData(unsigned char* _data, uint32_t _size) {

    if (!isInit())
        init();

#if defined(_WIN32)
    if (!CryptHashData(m_hHash, _data, _size, 0))
        throw exception("[md5::processData][Error] nao conseguiu processar os dados para o hash do Crypt MD5.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 3, GetLastError()));
#elif defined(__linux__)
    if (!MD5_Update(&m_hHash, _data, _size))
        throw exception("[md5::processData][Error] nao conseguiu processar os dados para o hash do Crypt MD5.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 3, errno));
#endif

    m_is_processed = true;
}

std::string md5::getHash() {

    if (!isInit())
        init();

    if (!m_is_processed)
        throw exception("[md5::getHash][Error] nao foi processado dados no hash, para poder pegar um hash de MD5.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 4, 0));

#if defined(_WIN32)
    unsigned char hash[MD5LEN];
    DWORD cbHash = MD5LEN;

    if (!CryptGetHashParam(m_hHash, HP_HASHVAL, hash, &cbHash, 0))
        throw exception("[md5::getHash][Error] nao conseguiu pegar o Hash Param do Crypt Hash MD5.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 5, GetLastError()));
#elif defined(__linux__)
    unsigned char hash[MD5_DIGEST_LENGTH];
    DWORD cbHash = MD5_DIGEST_LENGTH;

    if (!MD5_Final(hash, &m_hHash))
        throw exception("[md5::getHash][Error] nao conseguiu pegar o Hash Param do Crypt Hash MD5.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MD5, 5, errno));
#endif

    std::string sHash = "";

    for (auto i = 0u; i < cbHash; ++i)
        sHash += hex_util::ltoaToHex((uint32_t)hash[i]);

	// Destroy to reuse next time
	destroy();

    return sHash;
}