// Arquivo packet.h
// Criado em 05/11/2017 por Acrisio
// Definição da classe packet com o novo modo de alocar memoria dinamica

#pragma once
#ifndef _STDA_PACKET__H
#define _STDA_PACKET__H

#include <cstdint>

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include <memory>
#include <string>

#include <memory.h>

#define CHUNCK_ALLOC 512u//16384u

#define MAX_BUFFER_PACKET 10000ul

namespace stdA {
	class session;

#if defined(__linux__)
#pragma pack(1)
#endif

    struct packet_head {
        packet_head() : low_key(0), size(0) {};
        unsigned char low_key;
        unsigned short size;
    };
    
    struct packet_head_client : public packet_head {
        packet_head_client() : packet_head(), seq(0) {};
        unsigned char seq; // Packet Sequência
    };

#if defined(__linux__)
#pragma pack()
#endif

    struct conversionByte {
        #define CB_BASE_256			10
        #define CB_BASE_255			20
        #define CB_SEQ_NORMAL		1
        #define CB_SEQ_INVERTIDA	2
        #define CB_PARAM_DEFAULT	0

        conversionByte() : unionConvertido() {};
        inline conversionByte(const uint32_t _dwConvertido, unsigned char _flag = CB_PARAM_DEFAULT) : unionConvertido(_dwConvertido), m_flag(_flag) {
            if (m_flag != CB_PARAM_DEFAULT)
                invert();
        };
    
        inline conversionByte(const unsigned char* _ucpConvertido, unsigned char _flag = CB_PARAM_DEFAULT) : unionConvertido(0), m_flag(_flag) {
            if (_ucpConvertido != nullptr) {
                unionConvertido.dwConvertido = *(uint32_t*)_ucpConvertido;
                
                if (m_flag != CB_PARAM_DEFAULT)
                    invert();
            }
        };
    
        union U {
            U() : dwConvertido(0) {};
            explicit U(const uint32_t _dwConvertido) : dwConvertido(_dwConvertido) {};
    
            uint32_t dwConvertido;
    
            struct {
                unsigned char a;
                unsigned char b;
                unsigned char c;
                unsigned char d;
            }stConvertido;
        }unionConvertido;
    
    private:
    
        inline void invert() {
            if (m_flag & CB_BASE_255) {
                unionConvertido.dwConvertido = getNumberIS();
                unionConvertido.dwConvertido = getNumberBase256();
            }else {
                unionConvertido.dwConvertido = getNumberBase255();
                unionConvertido.dwConvertido = getNumberIS();
            }
        }
    
        unsigned char m_flag;	// 0x1 - Sequência invertida, 0x10 - Base256
    
    public: 
        inline uint32_t getNumberNS() { // Normal Sequência
            return unionConvertido.dwConvertido;
        };
    
        inline uint32_t getNumberIS() { // Inverse Sequência
            uint32_t ulNumber = 0;
    
            ulNumber = unionConvertido.stConvertido.d;
            ulNumber |= unionConvertido.stConvertido.c << 8;
            ulNumber |= unionConvertido.stConvertido.b << 16;
            ulNumber |= unionConvertido.stConvertido.a << 24;
    
            return ulNumber;
        };

    private:
        uint32_t ulNumber_temp;
    
    public:
        inline unsigned char* getLPUCNS() {
            ulNumber_temp = getNumberNS();
    
            return (unsigned char*)&ulNumber_temp;
        };
    
        inline unsigned char* getLPUCIS() {
            ulNumber_temp = getNumberIS();
    
            return (unsigned char*)&ulNumber_temp;
        };
    
        inline uint32_t getNumberBase256() {
            return getNumberNS() * 255 / 256 + 1;
        };
    
        inline uint32_t getNumberBase255() {
            return ((unionConvertido.dwConvertido / 255) << 8) | unionConvertido.dwConvertido % 255;
        };
    
        inline uint32_t getISNumberBase256() {
            return (uint32_t)(unionConvertido.dwConvertido * 255ll / 256 + 1);
        };
    
        inline uint32_t getISNumberBase255() {
            return ((getNumberIS() / 255) << 8) | getNumberIS() % 255;
        };
    
        inline int putNumberBuffer(unsigned char* buffer) {
            if (buffer == nullptr)
                return -1;
    
            size_t sz_union = sizeof(unionConvertido);

#if defined(_WIN32)
            memcpy_s(buffer, sz_union + 1, &unionConvertido, sz_union);
#elif defined(__linux__)
            memcpy(buffer, &unionConvertido, sz_union);
#endif
    
            return (int)sz_union;
        };
    };

    typedef unsigned char* LPUCHAR;
    
    struct offset_index {
        void clear() { memset(this, 0, sizeof(offset_index)); };
		void reset_read() { m_index_r = 0; };
		void reset_write() { m_index_w = 0; m_size = 0; };
		void reset() {
			reset_read();
			reset_write();
		};
        LPUCHAR m_buf;
        size_t m_index_r;
		size_t m_index_w;
        size_t m_size;
		size_t m_size_alloced;
    };

    class packet {
        public:
            packet();
			explicit packet(size_t _size/*, session* _session*/);
			//explicit packet(session* _session);
            explicit packet(/*session* _session, */unsigned short _tipo);
            ~packet();

            void init_maked();
			void init_maked(size_t _size);
            void init_plain(/*session* _session, */unsigned short _tipo);

			void reset();

            void add_maked(const void* _buf, size_t _size);
            void add_plain(const void* _buf, size_t _size);

            void read_maked(void* _buf, size_t _size);
            void read_plain(void* _buf, size_t _size);

            void unMake(unsigned char _key);
            void unMakeFull(unsigned char _key);
            void make(unsigned char _key);
            void makeFull(unsigned char _key);

            void unMakeRaw();
            void makeRaw();

			// Utiliza no Rank Server
			void unMakeRawClient();

            WSABUF getPlainBuf();
            WSABUF getMakedBuf();

			size_t getSizePlain();
			size_t getSizeMaked();

            // Methods gets
            unsigned short getTipo();
			//session* getSession();

			size_t getSize();
			unsigned char* getBuffer();

            // Methods Plain
            void addBuffer(const void* buffer, size_t size);
            void addZeroByte(size_t size);
            void addQWord(const void* qword);
            void addDWord(const void* dword);
            void addWord(const void* word);
            void addByte(const void* byte);
    
			// signed
            void addInt64(const int64_t int64);
            void addInt32(const int32_t int32);
            void addInt16(const int16_t int16);
            void addInt8(const int8_t int8);
            void addFloat(const float _float);
            void addDouble(const double _double);

			// unsigned
			void addUint64(const uint64_t uint64);
			void addUint32(const uint32_t uint32);
			void addUint16(const uint16_t uint16);
			void addUint8(const uint8_t uint8);

			// unsigned
			uint64_t readUint64();
			uint32_t readUint32();
			uint16_t readUint16();
			uint8_t  readUint8();
    
			void addString(const std::string string);
			void addString(const std::wstring string);
    
            void readBuffer(void* buffer, size_t size);
            void readQWord(void* qword);
            void readDWord(void* dword);
            void readWord(void* word);
            void readByte(void* byte);
    
			// signed
            int64_t readInt64();
            int32_t readInt32();
            int16_t readInt16();
            int8_t readInt8();
            float readFloat();
            double readDouble();
    
            std::string readString();
			std::wstring readWString();

        protected:
            void add(offset_index& _index, const void* _buf, size_t _size);
            void read(offset_index& _index, void* _buf, size_t _size);

            void alloc(offset_index& _index, size_t _size);

        private:
            void destroy();
        
        protected:
            unsigned short m_tipo;
			//session *m_session;
            offset_index m_plain;
            offset_index m_maked;
    };
}

#endif