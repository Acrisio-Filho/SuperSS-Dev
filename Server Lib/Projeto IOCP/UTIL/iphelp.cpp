// Arquivo iphelp.cpp
// Criado em 21/10/2017 por Acrisio
// Implementa��o da classe ip_help

#include <Windows.h>
#include "iphelp.h"
#include <string>

#pragma comment(lib, "IPHLPAPI.lib")

using namespace stdA;

std::vector< IP_ADAPTER_INFO > ip_help::getVectorAdaptersInfo() {
	PIP_ADAPTER_INFO pAdapters = NULL, pap = NULL;
	ULONG nSize = 0;

	std::vector< IP_ADAPTER_INFO > v_adapters;

	if (::GetAdaptersInfo(pAdapters, &nSize) == ERROR_BUFFER_OVERFLOW) {
		pAdapters = (PIP_ADAPTER_INFO)malloc(nSize);

		if (GetAdaptersInfo(pAdapters, &nSize) == NO_ERROR) {
			pap = pAdapters;

			while (pap) {
				v_adapters.push_back(*pap);

				pap = pap->Next;
			}
		}

		free(pAdapters);
	}

	return v_adapters;
};

std::string ip_help::toPad(const std::string _s, size_t _num, char _ch_pad) {
	std::string s = _s;

	if (_num > s.size())
		s.insert(s.begin(), _num - s.size(), _ch_pad);

	return s;
};

std::string ip_help::makeMACString(BYTE *_buff, ULONG _size) {
	std::string s;
	BYTE buff[3];

	for (size_t i = 0; i < _size; ++i) {
		_ltoa_s((int)_buff[i], (char*)buff, sizeof(buff), 16);

		s += toPad((char*)buff, 2, '0');

		if (i < (_size - 1))
			s += ":";
	}

	return s;
};
