// Arquivo player_info.hpp
// Criado em 11/03/2018 as 14:04 por Acrisio
// Definição da classe PlayerInfo

#pragma once
#ifndef _STDA_PLAYER_INFO_HPP
#define _STDA_PLAYER_INFO_HPP

#include "pangya_login_st.h"

namespace stdA {
    class PlayerInfo : public player_info {
        public:
            PlayerInfo();
            ~PlayerInfo();

            void clear();

        public:
            unsigned char m_state;
            unsigned char m_place;
			uint32_t m_server_uid;		// Server UID em que eles está conectado
    };
}

#endif