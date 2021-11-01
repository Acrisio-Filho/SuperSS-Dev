// Arquivo socket.cpp
// Criado em 14/11/2020 as 08:29 por Acrisio
// Implementa��o da classe Socket

#if defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <netinet/tcp.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#endif

#include "socket.hpp"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/exception.h"

#include <iostream>

using namespace stdA;

#if defined(_WIN32)
#pragma comment(lib, "ws2_32.lib")
#endif

Socket::Socket() 
	: 
#if defined(_WIN32)
	m_socket(INVALID_SOCKET), 
#elif defined(__linux__)
	m_socket{ INVALID_SOCKET, {0u} }, 
#endif
	m_sockaddr{ 0u } {
}

Socket::~Socket() {

#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET) {
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET) {
#endif
		try {
			abortiveClose();
		}catch (...) {}
	}
}

SOCKET Socket::detatch() {
	SOCKET ret_sock = m_socket;

#if defined(_WIN32)
	m_socket = INVALID_SOCKET;
#elif defined(__linux__)
	m_socket.fd = INVALID_SOCKET;
	m_socket.connect_time = {0u};
#endif

	return ret_sock;
}

void Socket::attach(SOCKET socket) {
	abortiveClose();

	m_socket = socket;
}

void Socket::close() {


	try {
		shutdown(SD_BOTH);
	}catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

#if defined(_WIN32)
		m_socket = INVALID_SOCKET;
#elif defined(__linux__)
		m_socket.fd = INVALID_SOCKET;
		m_socket.connect_time = {0u};
#endif
	}

#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET && 0 != ::closesocket(m_socket))
		throw exception("[Socket::close][Error] ao fechar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 2, WSAGetLastError()));

	m_socket = INVALID_SOCKET;
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET && 0 != ::closesocket(m_socket.fd))
		throw exception("[Socket::close][Error] ao fechar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 2, errno));

	m_socket.fd = INVALID_SOCKET;
	m_socket.connect_time = {0u};
#endif
}

void Socket::abortiveClose() {

#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET) {
		LINGER li;

		li.l_onoff = 1;
		li.l_linger = 0;

		if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof li))
			throw exception("[Socket::abortiveClose][Error] ao setsockopt.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 3, WSAGetLastError()));

		close();
	}
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET) {
		linger li;

		li.l_onoff = 1;
		li.l_linger = 0;

		if (SOCKET_ERROR == setsockopt(m_socket.fd, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof li))
			throw exception("[Socket::abortiveClose][Error] ao setsockopt.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 3, errno));

		close();
	}
#endif
}

void Socket::shutdown(int como) {
#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET && 0 != ::shutdown(m_socket, como))
		throw exception("[Socket::shutdown][Error] ao desligar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 4, WSAGetLastError()));
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET && 0 != ::shutdown(m_socket.fd, como))
		throw exception("[Socket::shutdown][Error] ao desligar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 4, errno));
#endif
}

void Socket::listen(int max_listen_same_time) {
#if defined(_WIN32)
	if (SOCKET_ERROR == ::listen(m_socket, max_listen_same_time))
		throw exception("[Socket::shutdown][Error] ao colocar o socket para escutar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 5, WSAGetLastError()));
#elif defined(__linux__)
	if (SOCKET_ERROR == ::listen(m_socket.fd, max_listen_same_time))
		throw exception("[Socket::shutdown][Error] ao colocar o socket para escutar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 5, errno));
#endif
}

void Socket::bind(const SOCKADDR_IN& address) {
#if defined(_WIN32)
	if (SOCKET_ERROR == ::bind(m_socket, reinterpret_cast<struct sockaddr*>(const_cast<SOCKADDR_IN*>(&address)), sizeof SOCKADDR_IN))
		throw exception("[Socket::bind][Error] ao binder o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 6, WSAGetLastError()));
#elif defined(__linux__)
	if (SOCKET_ERROR == ::bind(m_socket.fd, reinterpret_cast<struct sockaddr*>(const_cast<SOCKADDR_IN*>(&address)), sizeof(SOCKADDR_IN)))
		throw exception("[Socket::bind][Error] ao binder o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 6, errno));
#endif
}

void Socket::connect(std::string host, size_t port) {

	if (host.empty())
		throw exception("[Socket::connect][Error] host have empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 7, 0));
	
	sockaddr_in sa = { 0 };

	if (isalpha(host.c_str()[0])) {
		addrinfo hinst = { 0 }, *pAddrInfo = nullptr;

		char name[1024];

		hinst.ai_family = AF_INET;
		hinst.ai_protocol = IPPROTO_TCP;
		hinst.ai_socktype = SOCK_STREAM;

#if defined(_WIN32)
		if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hinst, &pAddrInfo) != 0)
			throw exception("[Socket::connect][Error] Nao conseguiu resolver o nome de host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 10, WSAGetLastError()));
#elif defined(__linux__)
		int error = 0;

		if ((error = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hinst, &pAddrInfo)) != 0)
			throw exception("[Socket::connect][Error] Nao conseguiu resolver o nome de host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 10, (error == EAI_SYSTEM ? errno : error)));
#endif

		if (pAddrInfo == nullptr)
			throw exception("[Socket::connect][Error] Nao conseguiu pegar o host pelo nome", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 11, 0));

		//for (addrinfo *ptr = pAddrInfo; ptr != nullptr; ptr = ptr->ai_next);

		inet_ntop(AF_INET, &((sockaddr_in*)pAddrInfo->ai_addr)->sin_addr, name, 1024);

		if (pAddrInfo != nullptr)
			freeaddrinfo(pAddrInfo);

		if (!inet_pton(AF_INET, name, &sa.sin_addr.s_addr)) {

#if defined(_WIN32)
			auto last_error = WSAGetLastError();
#elif defined(__linux__)
			auto last_error = errno;
#endif
			
			throw exception("[Socket::connect][Error] ao converter o ip host string doted for binary mode.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 8, last_error));
		}

	}
	else if (!inet_pton(AF_INET, host.c_str(), &sa.sin_addr.s_addr)) {

#if defined(_WIN32)
		auto last_error = WSAGetLastError();
#elif defined(__linux__)
		auto last_error = errno;
#endif

		throw exception("[Socket::connect][Error] ao converter o ip host string doted for binary mode.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 8, last_error));
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons((u_short)port);
	
	connect(&sa);
}

void Socket::connect(const SOCKADDR_IN* _s_addr) {
#if defined(_WIN32)
	if (SOCKET_ERROR == ::connect(m_socket, (const SOCKADDR*)_s_addr, sizeof SOCKADDR))
		throw exception("[Socket::connect][Error] ao connectar com o host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 9, WSAGetLastError()));
#elif defined(__linux__)
	if (SOCKET_ERROR == ::connect(m_socket.fd, (const sockaddr*)_s_addr, sizeof(sockaddr))) {

		// epoll, poll and select, nonblocking socket
		if (errno != EAGAIN && errno != EINPROGRESS)
			throw exception("[Socket::connect][Error] ao connectar com o host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 9, errno));
	}
#endif
}

bool Socket::isConnected() {
	bool ret = false;

	try {

		// getConnectTime pode lan�ar exception
		ret = (getConnectTime() >= 0);
	
	}catch (exception& e) {
		UNREFERENCED_PARAMETER(e);

		// Relan�a
		throw;
	}

	return ret;
}

int Socket::read(unsigned char* _buff, size_t _len) {
#if defined(_WIN32)
	return ::recv(m_socket, (char*)_buff, (int)_len, 0);
#elif defined(__linux__)
	return ::recv(m_socket.fd, (char*)_buff, (int)_len, 0);
#endif
}

int Socket::getConnectTime() {
	
#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET) {
		int seconds;
		int size_seconds = sizeof(seconds);

		if (0 == ::getsockopt(m_socket, SOL_SOCKET, SO_CONNECT_TIME, (char*)&seconds, &size_seconds))
			return seconds;
		else
			throw exception("[Socket::getConnectTime] erro ao pegar optsock SO_CONNECT_TIME.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 50, WSAGetLastError()));
	}
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET) {

		if (m_socket.connect_time.tv_nsec == 0 && m_socket.connect_time.tv_sec == 0)
			return -1;

		int rt_flg = fcntl(m_socket.fd, F_GETFL);

		if (rt_flg == -1)
			return (errno == EAGAIN | errno == EBADF | errno == EACCES) ? -1 : -2; // -1 socket is not connected, -2 Erro ao pegar status flag do socket

		int error = 0;
		socklen_t len = sizeof (error);
		int retval = getsockopt (m_socket.fd, SOL_SOCKET, SO_ERROR, &error, &len);

		if (retval != 0) {
			/* there was a problem getting the error code */
			return -1;
		}

		if (error != 0) {
			/* socket has a non zero error status */
			return -1;
		}

		timespec ts;

		clock_gettime(CLOCK_SOCKET_TO_CHECK, &ts);

		return (int)(((uint64_t)(ts.tv_sec * 1000000000 + ts.tv_nsec) - (uint64_t)(m_socket.connect_time.tv_sec * 1000000000 + m_socket.connect_time.tv_nsec)) / 1000000000);
	}
#endif

	return -1;
}
