// Arquivo epoll.cpp
// Criado em 05/03/2021 as 08:46 por Acrisio
// Implementa��o da classe iocp linux

#include "epoll.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <signal.h>

#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

#include <string>

using namespace stdA;

constexpr uint32_t MAX_EPOLL_EVENTS_WAIT = 1l;

// Event data type to exit epoll event
constexpr char EXIT_EPOLL_EVENT_TYPE[] = "FLAG_EXIT_EPOLL_THREAD";

iocp::iocp(int32_t _flag) : m_epollfd(-1l), m_pipefds{-1l} {
	
	if ((m_epollfd = epoll_create1(_flag)) == -1l)
		throw exception("[iocp::iocp][Error] ao criar o epoll fd.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 1, errno));

	if (pipe(m_pipefds) == -1)
		throw exception("[iocp::iocp][Error] ao criar o pipe de shutdown.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 20, errno));

	int flags = fcntl(m_pipefds[0], F_GETFL, 0);
	
	if (fcntl(m_pipefds[0], F_SETFL, flags | O_NONBLOCK) == -1)
		throw exception("[iocp::iocp][Error] ao set write pipe nonblocking.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 21, errno));

	epoll_event ep_ev;
	ep_ev.data.fd = m_pipefds[0]; // read
	ep_ev.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, ep_ev.data.fd, &ep_ev) == -1)
		throw exception("[iocp::iocp][Error] nao conseguiu adicionar o pipe read fd para epoll interest list. m_epollfd=" 
			+ std::to_string(m_epollfd) + ", pipe read fd=" + std::to_string(ep_ev.data.fd), STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 4, errno));
}

iocp::~iocp() {

	if (m_epollfd != -1l)
		close(m_epollfd);

	if (m_pipefds[0] != -1)
		close(m_pipefds[0]); // read
	
	if (m_pipefds[1] != -1)
		close(m_pipefds[1]); // write
}

void iocp::postStatus() {

	if (m_pipefds[1] == -1)
		throw exception("[iocp::postStatus][Error] invalid write pipe fd.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 22, 0));
	
	char ch = 'x';

	if (write(m_pipefds[1], &ch, 1) == -1)
		throw exception("[iocp::postStatus][Error] ao escrever no pipe.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 23, errno));
}

int32_t iocp::getStatus(epoll_event* _events, uint32_t _max_events, int32_t _timeout) {
	
	int32_t ret = -1l;

	if ((ret = epoll_wait(m_epollfd, _events, _max_events, _timeout)) == -1l)
		throw exception("[iocp::getStatus][Error] ao pegar um evento em epoll_wait.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 3, errno));

	return ret;
}

bool iocp::associaDeviceToPort(uint32_t _fd, epoll_event& _event) {

	if (-1l == epoll_ctl(m_epollfd, EPOLL_CTL_ADD, /*_event.data.fd*/_fd, &_event))
		throw exception("[iocp::associaDeviceToPort][Error] nao conseguiu adicionar o fd para epoll interest list. m_epollfd=" 
			+ std::to_string(m_epollfd) + ", _fd=" + std::to_string(_fd) + ", _event.data.fd=" + std::to_string(_event.data.fd), STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 4, errno));

	return true;
}

bool iocp::deleteFD(uint32_t _fd, epoll_event& _event) {

	if (-1l == epoll_ctl(m_epollfd, EPOLL_CTL_DEL, /*_event.data.fd*/_fd, &_event))
		throw exception("[iocp::deleteFD][Error] nao conseguiu deletar o fd do epoll interest list. m_epollfd=" 
			+ std::to_string(m_epollfd) + ", _fd=" + std::to_string(_fd) + ", _event.data.fd=" + std::to_string(_event.data.fd), STDA_MAKE_ERROR(STDA_ERROR_TYPE::IOCP, 5, errno));

	return true;
}

bool iocp::isExitEpollEvent(epoll_event& _event) {

	if ((_event.events & (EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP)) && _event.data.fd == m_pipefds[0]/*read*/) {

		// clear pipe
		if (m_pipefds[0] != -1 && (_event.events & EPOLLIN) == EPOLLIN) {

			char ch;
			int result = 0;

			do {
				result = read(m_pipefds[0], &ch, 1);
			} while (result > 0);

			// Post again, to always actives signal to exit
			postStatus();
		}

		return true;
	}

	return false;
}
