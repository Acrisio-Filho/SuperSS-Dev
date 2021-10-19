// Arquivo iocp.h
// Criado em 01/03/2017 por Acrisio
// Definição da classe iocp

#pragma once
#ifndef _STDA_IOCP_H
#define _STDA_IOCP_H

#include <Windows.h>

namespace stdA {
	class iocp {
	public:
		explicit iocp(size_t max_num_thread_same_time = 16);
		~iocp();

		void postStatus(ULONG_PTR completionKey, DWORD numBytes, OVERLAPPED *pOverlapped);

		void getStatus(PULONG_PTR completionKey, PDWORD numBytes, LPOVERLAPPED *pOverlapped, DWORD timeout_mili = INFINITE);

		bool associaDeviceToPort(ULONG_PTR completionKey, HANDLE device);
	
	protected:
		HANDLE m_iocp;
	};
}

#endif