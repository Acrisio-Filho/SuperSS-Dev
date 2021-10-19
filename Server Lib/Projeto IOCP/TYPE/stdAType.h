// Arquivo stdAType.h
// Criado em 04/06/2017 por Acrisio
// Definição dos tipos utilizado no projeto

#pragma once
#ifndef STDA_STDATYPE_H
#define STDA_STDATYPE_H

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <sys/epoll.h>
#endif

#define INVALID_CHANNEL (unsigned char)(~0)
#define INVALID_INDEX (size_t)(~0)

#define INVALID_LOBBY INVALID_CHANNEL

#define VECTOR_FIND_ITEM(vector, member, _operator, value) std::find_if((vector).begin(), (vector).end(), [&](auto& _element) { return _element.member _operator (value); })
#define VECTOR_FIND_PTR_ITEM(vector, member, _operator, value) std::find_if((vector).begin(), (vector).end(), [&](auto& _element) { return _element->member _operator (value); })

#include <algorithm>
#include <cstring>

namespace stdA {
	class player;
	class packet;
	class Buffer;

	struct ctx_db {
		char engine[255];
		char ip[255];
		char name[255];
		char user[255];
		char pass[255];
		unsigned short port;
	};

	struct myOver
#if defined(_WIN32)
		: OVERLAPPED
#elif defined(__linux__)
		: epoll_event
#endif
	{
		void clear() {
			memset(this, 0, sizeof(myOver));
		};
        DWORD tipo;
		Buffer* buffer;
    } /*__attribute__ ((__packed__))*/;

	struct ParamDispatch {
		player& _session;
		packet *_packet;
	};

	typedef int(*call_func)(void *_arg1, void *_arg2);
}

#endif