// Arquivo lobby.h
// Criado em 24/12/2017 por Acrisio
// Definição da classe lobby

#pragma once
#ifndef _STDA_LOBBY_H
#define _STDA_LOBBY_H

#include <vector>
#include "../../Projeto IOCP/SOCKET/session.h"
#include "room.h"

namespace stdA {
    class lobby {
        public:
            lobby();
            ~lobby();

        protected:
            std::vector< room* > v_rooms;
            std::vector< session* > v_sessions;
    };
}

#endif