// Arquivo unit_gg_interface.hpp
// Criado em 02/02/2021 as 23:50 por Acrisio
// Definição da Interface Unit Game Guard Auth Server

#pragma once
#ifndef _STDA_UNIT_GG_INTERFACE_HPP
#define _STDA_UNIT_GG_INTERFACE_HPP

#include "../CSAuth/GGAuth.hpp"

namespace stdA {

    class IUnitGGAuthServer {
        public:
            virtual void ggAuthCmdGetQuery(uint32_t _error, uint32_t _socket_id, GG_AUTH_DATA& _data) = 0;
			virtual void ggAuthCmdCheckAnswer(uint32_t _error, uint32_t _socket_id) = 0;
    };
}

#endif // !_STDA_UNIT_GG_INTERFACE_HPP