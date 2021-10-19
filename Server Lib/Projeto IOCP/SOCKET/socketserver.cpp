// Arquivo socketserver.cpp
// Criado em 01/05/2017 por Acrisio
// Implementação da classe socketserver

#include "socketserver.h"
#include "../UTIL/message_pool.h"

using namespace stdA;

socketserver::socketserver()
#ifdef _WIN32 
	: wsa() 
#endif
{

}

socketserver::~socketserver() {

}

SOCKET socketserver::getsocklistener() {
	SOCKET m_listensock;

#if defined(_WIN32)
	m_listensock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#elif defined(__linux__)
	m_listensock.fd = ::socket(AF_INET, SOCK_STREAM, 0);

	// set reuse addr
	int reuse_addr = 1; // true;

	if (setsockopt(m_listensock.fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1)
		_smp::message_pool::getInstance().push(new message("[socketserver::getsocklistener][Error] fail to set reuse addr to socket[FD=" 
			+ std::to_string(m_listensock.fd) + "]", CL_FILE_LOG_AND_CONSOLE));

	clock_gettime(CLOCK_MONOTONIC_RAW, &m_listensock.connect_time);
#endif

	return m_listensock;
}
