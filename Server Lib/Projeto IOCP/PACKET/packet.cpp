// Arquivo packet.cpp
// Criado em 05/11/2017 por Acrisio
// Implementação da classe packet

// Tem que ter o pack aqui se não da erro na hora da allocação do HEAP
#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <memory.h>
#endif

#include "packet.h"

#include <ctime>

#include "../UTIL/exception.h"
#include "../COMPRESS/compress.h"
#include "../CRYPT/crypt.h"
#include "../SOCKET/session.h"

#include "../TYPE/stda_error.h"

#include "../UTIL/string_util.hpp"	

using namespace stdA;

packet::packet() {
    m_plain.clear();
    m_maked.clear();

	m_tipo = ~0;
};

//packet::packet(session* _session) : m_session(_session) {
//	m_plain.clear();
//	m_maked.clear();
//
//	m_tipo = ~0;
//};

packet::packet(size_t _size/*, session* _session) : m_session(_session*/) {
	m_plain.clear();
	m_maked.clear();

	m_tipo = ~0;

	m_maked.m_size = _size;

	alloc(m_maked, _size);
};

packet::packet(/*session* _session, */unsigned short _tipo) : /*m_session(_session),*/ m_tipo(_tipo) {
    m_plain.clear();
    m_maked.clear();

    add_plain(&m_tipo, sizeof(unsigned short));
};

packet::~packet() {
    destroy();
};

void packet::init_maked() {
    read_plain(&m_tipo, sizeof(unsigned short));
};

void packet::init_maked(size_t _size) {
	m_maked.m_size = _size;

	alloc(m_maked, _size);
};

void packet::init_plain(/*session* _session, */unsigned short _tipo) {
    m_tipo = _tipo;
	//m_session = _session;

	m_plain.reset();
	m_maked.reset();

    add_plain(&m_tipo, sizeof(unsigned short));
};

void packet::reset() {
	m_plain.reset();
	m_maked.reset();
};

void packet::destroy() {
	if (m_plain.m_buf != nullptr)
        delete[] m_plain.m_buf;

	if (m_maked.m_buf != nullptr)
        delete[] m_maked.m_buf;
};

void packet::add_maked(const void* _buf, size_t _size) {
    if (_buf == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::add_plain()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    alloc(m_maked, _size);
    add(m_maked, _buf, _size);
};

void packet::add_plain(const void* _buf, size_t _size) {
    if (_buf == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::add_plain()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    alloc(m_plain, _size);
    add(m_plain, _buf, _size);
};

void packet::read_maked(void* _buf, size_t _size) {
    if (_buf == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::read_maked()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    if (m_maked.m_index_w < (_size + m_maked.m_index_r))
        throw exception("Error not enough for read em packet::read_maked()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 3, 0));
    
    read(m_maked, _buf, _size);
};

void packet::read_plain(void* _buf, size_t _size) {
    if (_buf == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::read_plain()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    if (m_plain.m_index_w < (_size + m_plain.m_index_r))
        throw exception("Error not enough for read em packet::read_plain()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 3, 0));

    read(m_plain, _buf, _size);
};

void packet::add(offset_index& _index, const void* _buf, size_t _size) {
#if defined(_WIN32)
    memcpy_s(_index.m_buf + _index.m_index_w, _index.m_size_alloced - _index.m_index_w, _buf, _size);
#elif defined(__linux__)
	memcpy(_index.m_buf + _index.m_index_w, _buf, _size);
#endif
    _index.m_index_w += _size;
};

void packet::read(offset_index& _index, void* _buf, size_t _size) {
    memcpy(_buf, _index.m_buf + _index.m_index_r, _size);
    _index.m_index_r += _size;
};

void packet::alloc(offset_index& _index, size_t _size) {
	if (_size > (_index.m_size_alloced - _index.m_index_w)) {
		size_t ant = _index.m_size_alloced;

		if ((int)_size < 0)
			throw exception("Negative size. packet::alloc()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

		_index.m_size_alloced += ((_size - (_index.m_size_alloced - _index.m_index_w)) / CHUNCK_ALLOC + 1) * CHUNCK_ALLOC;
				//((_size > CHUNCK_ALLOC) ? 
				//(((_size / CHUNCK_ALLOC) + 1) * CHUNCK_ALLOC) : 
				//(_index.m_size_alloced + CHUNCK_ALLOC));

		try {
			if (_index.m_buf != nullptr) {
				LPUCHAR tmp = new UCHAR[_index.m_size_alloced];

#if defined(_WIN32)
				memcpy_s(tmp, _index.m_size_alloced, _index.m_buf, _index.m_index_w);
#elif defined(__linux__)
				memcpy(tmp, _index.m_buf, _index.m_index_w);
#endif

				delete[] _index.m_buf;

				_index.m_buf = tmp;
			}else
				_index.m_buf = new UCHAR[_index.m_size_alloced];
		}catch (std::exception& e) {
			if (_index.m_buf != nullptr)
				delete _index.m_buf;

			throw exception("Error ao allocar memoria. size_ant: " + std::to_string(ant) + "\r\nsize_alloc: " + std::to_string(_index.m_size_alloced) + "\r\nsize_request: " + std::to_string(_size) + ". " + std::string(e.what()) + ". packet::alloc()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 1, 2));
		}

        if (_index.m_buf == nullptr)
            throw exception("Error ao alocar memoria para o buffer em packet::alloc()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 1, 0));
    }
};

unsigned short packet::getTipo() {
    return m_tipo;
};

//session* packet::getSession() {
//	return m_session;
//};

size_t packet::getSize() {
	return m_plain.m_index_w;
};

unsigned char* packet::getBuffer() {
	return m_plain.m_buf;
};

void packet::unMake(unsigned char _key) {
    size_t index = 0;

	packet_head_client phc;

	if (m_maked.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::unMake()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

    memcpy(&phc, &m_maked.m_buf[index], sizeof(packet_head_client));	index += sizeof(packet_head_client);

    if (phc.size > m_maked.m_index_w)
        throw exception("Erro: Unknown Packet. packet::unMake()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 11, 0));

    crypt _crypt;
    _crypt.init_key(_key, phc.low_key);

    unsigned char *decrypt = new unsigned char[phc.size];

    try {
        _crypt.decrypt(&m_maked.m_buf[index], phc.size, decrypt);
    }catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

        if (decrypt != nullptr)
            delete[] decrypt;

        throw;
    }

	// Reset Plain
	m_plain.reset();

    add_plain(&decrypt[1], phc.size - 1);

    delete[] decrypt;
};

void packet::unMakeFull(unsigned char _key) {

    size_t index = 0, decompress_size = 0, size_decompress_out = 0;
    packet_head ph;

	if (m_maked.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::unMakeFull()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

    memcpy(&ph, &m_maked.m_buf[index], sizeof(packet_head));	index += sizeof(packet_head);

    if (ph.size > m_maked.m_index_w)
        throw exception("Erro: Unknown Packet. packet::unMakeFull()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 11, 0));

    if (ph.low_key == 0 && m_maked.m_buf[index] == 0) {
        
		unMakeRaw();

		readWord(&m_tipo);

		m_plain.reset_read();	// Reseta para funcionar depois normal
        
        switch (m_tipo) {
        case 0:
			// Pacote Raw Login
			if (ph.size == 0x0B && (m_maked.m_buf[index + 4] == 0 && m_maked.m_buf[index + 5] == 0 && m_maked.m_buf[index + 6] == 0))
				return;
			else
				m_plain.reset_write();
            break;
        case 0x2E:		// Pacote Raw MSN
        case 0x3F:		// Pacote Raw Game
        case 0x1388:	// Pacote Raw Rank
            return;
		default:
			m_plain.reset_write();
        }
    }

    crypt *_crypt = nullptr;
    compress *_compress = nullptr;

	_crypt = new crypt();

    _crypt->init_key(_key, ph.low_key);

    unsigned char *decrypt = new unsigned char[ph.size];

    try {
        _crypt->decrypt(&m_maked.m_buf[index], ph.size, decrypt);
    }catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

		// Clean
		if (decrypt != nullptr) {

			delete[] decrypt;

			decrypt = nullptr;
		}

		if (_crypt != nullptr) {

			delete _crypt;

			_crypt = nullptr;
		}

        throw;
    }

    index = 1;	// Depois de decryptado não usa a chave que fica

    conversionByte cb(decrypt + index, CB_BASE_255);	index += sizeof(conversionByte::unionConvertido);

    decompress_size = cb.getNumberNS();

    unsigned char* decompress = new unsigned char[cb.getNumberBase255()];

	_compress = new compress();

    try {
        _compress->decompress_data(decrypt + index, ph.size - index, decompress, &size_decompress_out, decompress_size);
    }catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

		// Clean
		if (decrypt != nullptr) {

			delete[] decrypt;

			decrypt = nullptr;
		}

		if (_crypt != nullptr) {

			delete _crypt;

			_crypt = nullptr;
		}

		if (decompress != nullptr) {

			delete[] decompress;

			decompress = nullptr;
		}

		if (_compress != nullptr) {

			delete _compress;

			_compress = nullptr;
		}

        throw;
    }

    // Libera memória alocada para o decrypt
	if (decrypt != nullptr) {

		delete[] decrypt;

		decrypt = nullptr;
	}

	// Reset Plain
	m_plain.reset();

    add_plain(decompress, decompress_size);

    // Libera memória alocada para decompress
	if (decompress != nullptr) {

		delete[] decompress;

		decompress = nullptr;
	}

	if (_crypt != nullptr) {

		delete _crypt;

		_crypt = nullptr;
	}

	if (_compress != nullptr) {

		delete _compress;

		_compress = nullptr;
	}
};

void packet::make(unsigned char _key) {

    if (m_plain.m_index_w <= 0)
        throw exception("Buffer is zero, not size for a packet. packet::make()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 6, 0));

	if (m_plain.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::make()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

    crypt _crypt;
    packet_head_client phc;

    unsigned char* tmp = nullptr;

    std::srand((uint32_t)std::time(nullptr) * 7 * (uint32_t)m_plain.m_index_w);

    phc.size = (unsigned short)m_plain.m_index_w + 1/*key sec*/;
    phc.low_key = std::rand() & 255;
    phc.seq = 0;

    tmp = new unsigned char[phc.size];

	alloc(m_plain, m_plain.m_index_w + 1);

    memmove(&m_plain.m_buf[1], &m_plain.m_buf[0], m_plain.m_index_w);

	m_plain.m_buf[0] = _crypt.init_key(_key, phc.low_key);

    try {
        _crypt.encrypt(&m_plain.m_buf[0], phc.size, tmp);
    }catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

        if (tmp != nullptr)
            delete []tmp;

        throw;
    }

	// Maked Reset
	m_maked.reset();

    add_maked(&phc, sizeof(packet_head_client));
    add_maked(tmp, phc.size);

    if (tmp != nullptr)
        delete[] tmp;
};

void packet::makeFull(unsigned char _key) {

    if (m_plain.m_index_w <= 0)
        throw exception("Buffer is zero, not size for a packet. packet::makeFull()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 6, 0));

	if (m_plain.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::makeFull()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

    compress *_compress = nullptr;
    crypt *_crypt = nullptr;
    packet_head ph;

    conversionByte cb((const uint32_t)m_plain.m_index_w, CB_BASE_256);

    unsigned char* tmp = new unsigned char[cb.getNumberIS() + 5 + 5];
    size_t compress_out = 0;

	_compress = new compress();

    try {
        _compress->compress_data(m_plain.m_buf, m_plain.m_index_w, &tmp[5], &compress_out);
    }catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

		// Clean
		if (tmp != nullptr) {

			delete[] tmp;

			tmp = nullptr;
		}

		if (_compress != nullptr) {

			delete _compress;

			_compress = nullptr;
		}

        throw;
    }

    // Make Packet Head
    ph.size = (unsigned short)compress_out + 5/*key low and size raw decompressed*/;

    std::srand((uint32_t)std::time(nullptr) * 7 * ph.size);

    ph.low_key = std::rand() & 255;
    // Fim Make Packet Head

	_crypt = new crypt();

    tmp[0] = _crypt->init_key(_key, ph.low_key);

    cb.putNumberBuffer(&tmp[1]);

	// Maked Reset
	m_maked.reset();

	add_maked(&ph, sizeof(packet_head));
	alloc(m_maked, ph.size);	// add size para escrita

    try {
        _crypt->encrypt(tmp, ph.size, &m_maked.m_buf[m_maked.m_index_w]);
    }catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

		// Clean
		if (tmp != nullptr) {

			delete[] tmp;

			tmp = nullptr;
		}

		if (_compress != nullptr) {

			delete _compress;

			_compress = nullptr;
		}

		if (_crypt != nullptr) {

			delete _crypt;

			_crypt = nullptr;
		}

        throw;
    }

	//add_maked(tmp, ph.size);
	m_maked.m_index_w += ph.size;

	// Clean
	if (tmp != nullptr) {

		delete[] tmp;

		tmp = nullptr;
	}

	if (_compress != nullptr) {

		delete _compress;

		_compress = nullptr;
	}

	if (_crypt != nullptr) {

		delete _crypt;

		_crypt = nullptr;
	}
};

void packet::unMakeRaw() {
    size_t index = 0;
    packet_head ph;

	if (m_maked.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::unMakeRaw()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

    memcpy(&ph, &m_maked.m_buf[index], sizeof(packet_head));	index += sizeof(packet_head);

    if (ph.size > m_maked.m_index_w)
        throw exception("Erro: Unknown Packet. packet::unMakeRaw()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 11, 0));
    
    index++;	// Tira o byte com o valor zero que diz que é um pacote Raw

	// Plain Reset
	m_plain.reset();

    add_plain(&m_maked.m_buf[index], ph.size - 1);
};

void packet::makeRaw() {
	size_t index = 0;
	packet_head ph;

	if (m_plain.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::makeRaw()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

	ph.low_key = 0; // low part of key random - 0 nesse pacote porque ele é o primiero que passa a chave
	ph.size = (unsigned short)(m_plain.m_index_w + 1);

	// Maked Reset
	m_maked.reset();

	add_maked(&ph, sizeof(packet_head));

	int key = 0;	// byte com valor 0 para dizer que é um pacote raw
	add_maked(&key, 1);

	add_maked(m_plain.m_buf, m_plain.m_index_w);
};

void packet::unMakeRawClient() {

	size_t index = 0;
	packet_head_client phc;

	if (m_maked.m_buf == nullptr)
		throw exception("Error buf is nullptr em packet::unMakeRawClient()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 15, 0));

	memcpy(&phc, &m_maked.m_buf[index], sizeof(packet_head_client));	index += sizeof(packet_head_client);

	if (phc.size > m_maked.m_index_w)
		throw exception("Erro: Unknown Packet. packet::unMakeRawClient()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 11, 0));

	index++;	// Tira o byte com o valor zero que diz que é um pacote Raw

	// Plain Reset
	m_plain.reset();

	add_plain(&m_maked.m_buf[index], phc.size - 1);
}

WSABUF packet::getPlainBuf() {
    return { (uint32_t)m_plain.m_index_w, (char*)m_plain.m_buf };
};

WSABUF packet::getMakedBuf() {
    return { (uint32_t)m_maked.m_index_w, (char*)m_maked.m_buf };
};

size_t packet::getSizePlain() {
	return m_plain.m_size;
};

size_t packet::getSizeMaked() {
	return m_maked.m_size;
};

void packet::addBuffer(const void* buffer, size_t size) {
    if (buffer == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::addBuffer()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    add_plain(buffer, size);
};

void packet::addZeroByte(size_t size) {
    alloc(m_plain, size);
    memset(m_plain.m_buf + m_plain.m_index_w, 0, size);
	m_plain.m_index_w += size;
};

void packet::addQWord(const void* qword) {
    if (qword == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::addQWord()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    add_plain(qword, 8);
};

void packet::addDWord(const void* dword) {
    if (dword == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::addDWord()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    add_plain(dword, 4);
};

void packet::addWord(const void* word) {
    if (word == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::addWord()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    add_plain(word, 2);
};

void packet::addByte(const void* byte) {
    if (byte == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::addByte()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    add_plain(byte, 1);
};

void packet::addInt64(const int64_t int64) {
    add_plain(&int64, sizeof(int64_t));
};

void packet::addInt32(const int32_t int32) {
    add_plain(&int32, sizeof(int32_t));
};

void packet::addInt16(const int16_t int16) {
    add_plain(&int16, sizeof(int16_t));
};

void packet::addInt8(const int8_t int8) {
    add_plain(&int8, sizeof(int8_t));
};

void packet::addFloat(const float _float) {
    add_plain(&_float, sizeof(float));
};

void packet::addDouble(const double _double) {
    add_plain(&_double, sizeof(double));
};

void packet::addUint64(const uint64_t uint64) {
	add_plain(&uint64, sizeof(uint64_t));
};

void packet::addUint32(const uint32_t uint32) {
	add_plain(&uint32, sizeof(uint32_t));
};

void packet::addUint16(const uint16_t uint16) {
	add_plain(&uint16, sizeof(uint16_t));
};

void packet::addUint8(const uint8_t uint8) {
	add_plain(&uint8, sizeof(uint8_t));
};

uint64_t packet::readUint64() {
	uint64_t _u64;

	read_plain(&_u64, sizeof(uint64_t));

	return _u64;
};

uint32_t packet::readUint32() {
	uint32_t _u32;

	read_plain(&_u32, sizeof(uint32_t));

	return _u32;
};

uint16_t packet::readUint16() {
	uint16_t _u16;

	read_plain(&_u16, sizeof(uint16_t));

	return _u16;
};

uint8_t packet::readUint8() {
	uint8_t _u8;

	read_plain(&_u8, sizeof(uint8_t));

	return _u8;
};

void packet::addString(const std::string string) {
    addInt16((short)string.size());
    add_plain(string.c_str(), string.size());
};

void packet::addString(const std::wstring string) {
	addString(WcToMb(string));
};

void packet::readBuffer(void* buffer, size_t size) {
    if (buffer == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::readBuffer()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    read_plain(buffer, size);
};

void packet::readQWord(void* qword) {
    if (qword == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::readQword()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    read_plain(qword, 8);
};

void packet::readDWord(void* dword) {
    if (dword == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::readDWord()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    read_plain(dword, 4);
};

void packet::readWord(void* word) {
    if (word == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::readWord()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    read_plain(word, 2);
};

void packet::readByte(void* byte) {
    if (byte == nullptr)
        throw exception("Error arguments invalid, _buf is nullptr em packet::readByte()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET, 2, 0));

    read_plain(byte, 1);
};

int64_t packet::readInt64() {
    int64_t _64;

    read_plain(&_64, sizeof(int64_t));

    return _64;
};

int32_t packet::readInt32() {
	int32_t _32;
    
    read_plain(&_32, sizeof(int32_t));

    return _32;
};

int16_t packet::readInt16() {
    int16_t _16;

    read_plain(&_16, sizeof(int16_t));

    return _16;
};

int8_t packet::readInt8() {
    int8_t _8;

    read_plain(&_8, sizeof(int8_t));

    return _8;
};

float packet::readFloat() {
    float _float;

    read_plain(&_float, 4);

    return _float;
};

double packet::readDouble() {
    double _double;

    read_plain(&_double, 8);

    return _double;
};

std::string packet::readString() {
    int16_t len = readInt16();

	if (len <= 0)
		return std::string("");	// String Vazia

	char* szString = new char[len + 1];

	read_plain(szString, len);

	szString[len] = '\0';

	std::string s(szString);

	delete[] szString;

	return s;
};

std::wstring packet::readWString() {
	int16_t len = readInt16();

	if (len <= 0)
		return std::wstring(L"");		// String Vazia

	char* szString = new char[len + 1];

	read_plain(szString, len);

	szString[len] = '\0';

	std::wstring s = MbToWc(szString);

	delete[] szString;

	return s;
};
