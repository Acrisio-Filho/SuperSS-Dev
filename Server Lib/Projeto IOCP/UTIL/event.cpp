// Arquivo event.cpp
// Criado em 12/03/2021 as 09:45 por Acrisio
// Implementação da classe Event

#include "event.hpp"

#include <unistd.h>
#include <sys/poll.h>
#include <sys/eventfd.h>
#include <errno.h>

#include <stdexcept>

using namespace stdA;

Event::Event(bool _manual_reset, uint64_t _init_state)
    : m_fd(-1), m_manual_reset(_manual_reset), m_good(false) {

    // Error
    if ((m_fd = eventfd(_init_state, EFD_NONBLOCK)) == -1)
        return;

    // Success
    m_good = true;
}

Event::~Event() {

    if (m_fd != -1)
        close(m_fd);
}

bool Event::is_good() {
    return m_good && m_fd != -1;
}

uint32_t Event::wait(uint32_t _dwMilliseconds) {

    if (!is_good())
        return WAIT_FAILED;

    pollfd pev{ 0u };
    pev.fd = m_fd;
    pev.events = POLLIN;
    
    int32_t ready = poll(&pev, 1, (int32_t)_dwMilliseconds);

    if (ready == -1)
        return WAIT_FAILED;

    if (ready == 0) // TIMEOUT
        return WAIT_TIMEOUT;

    if (pev.revents & POLLIN) {

        if (!m_manual_reset)
            reset();
    
    }else // POLLERR | POLLHUP
        return WAIT_FAILED;

    return WAIT_OBJECT_0;
}

bool Event::reset() {

    if (!is_good()) // Event FAILED
        return false;

    uint64_t buff;

    int32_t count = read(m_fd, &buff, sizeof(uint64_t));

    if (count <= 0 && errno != EAGAIN)
        return false;
    else if (count > 0 && count != (int32_t)sizeof(uint64_t))
        return false;

    return true;
}

bool Event::set(uint64_t _state) {

    if (!is_good())
        return false;

    if ((int64_t)_state <= 0)
        _state = 1ull;

    int32_t count = write(m_fd, &_state, sizeof(uint64_t));

    if (count <= 0 || count != (int32_t)sizeof(uint64_t))
        return false;

    return true;
}

bool Event::pulse() {

    if (!is_good())
        return false;

    // No Windows API essa função está deprecated, só tem para suporte de versões antigas do Windows
    // Então só vou setar o evento
    return set();
};

uint32_t Event::waitMultipleEvent(uint32_t _count, std::vector< Event* > _events, bool _waitForAll, uint32_t _dwMilliseconds) {

    if (_events.empty())
        return WAIT_FAILED;

    uint32_t dwWait = WAIT_FAILED;

    pollfd *pevs = new pollfd[_count];
    uint32_t completed = 0u;
    timespec ts;

    if (_waitForAll)
        _dwMilliseconds = INFINITE; // Espera por todos

    try {

        for (auto i = 0u; i < _count; ++i) {
            
            if (_events[i] == nullptr || !_events[i]->is_good())
                throw std::invalid_argument("invalid event");

            pevs[i].fd = _events[i]->m_fd;
            pevs[i].revents = 0;
            pevs[i].events = POLLIN;
        }

        int32_t ready = -1;
        
        do {
            
            ready = poll(pevs, _count, _dwMilliseconds);

            if (ready == -1)
                throw "Fail in call poll()";

            if (ready == 0) {

                dwWait = WAIT_TIMEOUT;

                break;
            }

            completed += ready;

            for (auto i = 0u; i < _count; ++i) {

                if (pevs[i].revents != 0) {

                    if (pevs[i].revents & POLLIN) {

                        if (!_waitForAll) {

                            if (!_events[i]->m_manual_reset)
                                _events[i]->reset();

                            dwWait = WAIT_OBJECT_0 + i;

                            break;
                        
                        }else {

                            pevs[i].fd = -1;
                        }

                    }else // POLLERR || POLLHUP
                        throw "Fail in poll event";
                }
            }

        } while (_waitForAll && completed < _count);

        if (dwWait != WAIT_TIMEOUT && _waitForAll) {

            dwWait = WAIT_OBJECT_0 + (_count - 1);

            for (auto i = 0u; i < _count; ++i) {

                if (!_events[i]->m_manual_reset)
                    _events[i]->reset();
            }
        }

    }catch (...) {

        // Fail, exception c+(acho que o linux pega também, ou é signal)
        dwWait = WAIT_FAILED;
    }

    if (pevs != nullptr)
        delete[] pevs;

    return dwWait;
}