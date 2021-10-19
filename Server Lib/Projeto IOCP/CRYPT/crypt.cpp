// Arquivo crypt.cpp
// Criado em 23/09/2017 por Acrisio
// Implementação da classe crypt

#include "crypt.h"
#include "../UTIL/exception.h"
#include <memory.h>
#include "../TYPE/stda_error.h"

#include "key_dictionary.h"

using namespace stdA;

crypt::crypt() {
    memset(m_key, 0, sizeof(m_key));
};

crypt::~crypt() {};

unsigned char crypt::init_key(unsigned char _key_high, unsigned char _key_low) {
    unsigned short pos_dic = (_key_high << 8) | _key_low;
   
    m_key[0] = Keys[pos_dic];
    m_key[1] = Keys[4096 + pos_dic];

    return (unsigned char)m_key[1];
};

void crypt::decrypt(unsigned char* _cipher, size_t _size, unsigned char* _plain) {
    if (_cipher == nullptr)
        throw exception("Error _cipher is nullptr, crypt::decrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 1, 0));
    
    if (_plain == nullptr)
        throw exception("Error _plain is nullptr, crypt::decrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 1, 0));

    size_t i = 0;

	if (_size > 0) {

		for (i = 0; i < (_size >= 4 ? 4 : _size); ++i)
			_plain[i] = _cipher[i] ^ ((unsigned char*)&m_key[0])[i];

		for (i = 4; i < _size; ++i)
			_plain[i] = _cipher[i] ^ _plain[i - 4];

		if (!check_key(*_plain))
			throw exception("Error nao conseguiu decrypt, crypt::decrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 3, 0));
	}else
		throw exception("Error _size is 0, crypt::decrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 4, 0));
};

void crypt::encrypt(unsigned char* _plain, size_t _size, unsigned char* _cipher) {
    if (_plain == nullptr)
        throw exception("Error _plain is nullptr, crypt::encrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 1, 0));
    
    if (_cipher == nullptr)
        throw exception("Error _cipher is nullptr, crypt::encrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 1, 0));

    size_t i = 0;

	if (_size > 0) {
		for (i = 0; i < (_size >= 4 ? 4 : _size); ++i)
			_cipher[i] = _plain[i] ^ ((unsigned char*)&m_key[0])[i];

		for (i = 4; i < _size; ++i)
			_cipher[i] = _plain[i] ^ _plain[i - 4];

		if (!check_key(*_cipher ^ (unsigned char)m_key[0]))
			throw exception("Error nao conseguiu encrypt, crypt::encrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 2, 0));
	}else
		throw exception("Error _size is 0, crypt::encrypt()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CRYPT, 4, 0));
};

bool crypt::check_key(unsigned char _key) {
    return (_key == (unsigned char)m_key[1]);
};
