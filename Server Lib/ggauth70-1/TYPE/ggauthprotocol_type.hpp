// Arquivo ggauthprotocol_type.hpp
// Criado em 11/10/2021 as 14:06 por Acrisio
// Definição dos tipos usados no GameGuard Auth Protocolo

#pragma once
#ifndef _GGAUTHPROTOCOL_TYPE_HPP
#define _GGAUTHPROTOCOL_TYPE_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"

typedef ushort				WORD, USHORT;
typedef unsigned char		UCHAR, BYTE;
typedef int32_t				INT32;

typedef uintptr_t			UINT_PTR;
#endif

struct GG_VERSION {
	DWORD dwGGver;
	WORD wYear;
	WORD wMonth;
	WORD wDay;
	WORD wNum;
};

struct GG_AUTH_DATA {
	// Define Union Types
public:
	union uIndex {
		DWORD dwIndex;
		USHORT usIndex[2];
		UCHAR ucIndex[4];
	};

	// Members
public:
	uIndex dwIndex;
	DWORD dwValue1;
	uIndex dwValue2;
	uIndex dwValue3;
};

// Ainda vou declara ela
#define GG_AUTH_PROTOCOL void*

class CCSAuth2 {
public:
	GG_AUTH_DATA		m_AuthQuery;
	GG_AUTH_DATA		m_AuthAnswer;
	DWORD				m_dwUniqValue1;
	DWORD				m_dwUniqValue2;
	CCSAuth2	       *m_pNext;
	bool				m_bAuth;
	GG_AUTH_PROTOCOL	m_pProtocol;
	UINT32				m_bPrtcRef;
	DWORD				m_dwUserFlag;
	GG_VERSION			m_GGVer;
	GG_AUTH_DATA		m_AuthQueryTmp;
	bool				m_bAllowOldVersion;
	int					m_nSequenceNum;
	DWORD				m_dwServerKey;
	DWORD				m_dwLastValue4;
	BYTE				m_byLastLoop1;
	BYTE				m_byLastLoop2;
	DWORD				m_dwLoop1AuthArray[0x100];
	DWORD				m_dwLoop2AuthArray[0x100];
};

#endif // !_GGAUTHPROTOCOL_TYPE_HPP