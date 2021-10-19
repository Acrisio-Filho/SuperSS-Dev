// Arquivo event.hpp
// Criado em 12/03/2021 09:24 por Acrisio
// Definição da classe Event, Event manual reset Linux

#pragma once
#ifndef _STDA_EVENT_HPP
#define _STDA_EVENT_HPP

#include <cstdint>
#include <vector>

namespace stdA {

#ifndef INFINITE
    #define INFINITE (uint32_t)-1
#endif

#define WAIT_OBJECT_0   0x00000000
#define WAIT_TIMEOUT    0x00000102
#define WAIT_ABANDONED  0x00000080
#define WAIT_FAILED     0xFFFFFFFF

    class Event {
        public:
            Event(bool _manual_reset, uint64_t _init_state = 0ull);
            virtual ~Event();

            bool is_good();

            uint32_t wait(uint32_t _dwMilliseconds);
            bool set(uint64_t _state = 1ull);
            bool reset();
            bool pulse();

            static uint32_t waitMultipleEvent(uint32_t _count, std::vector< Event* > _events, bool _waitForAll, uint32_t _dwMilliseconds);

        protected:
            int32_t m_fd;
            bool m_manual_reset;

            bool m_good;
    };
}

#endif