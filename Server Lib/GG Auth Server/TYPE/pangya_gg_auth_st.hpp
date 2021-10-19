// Arquivo pangya_gg_auth_st.hpp
// Criado em 02/02/2021 as 17:07 por Acrisio
// Definição das estruturas usadas no Game Guard Auth Server

#pragma once
#ifndef _STDA_PANGYA_GG_AUTH_ST_HPP
#define _STDA_PANGYA_GG_AUTH_ST_HPP

#include <memory>

#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {

	// Player Info
	struct player_info {
		player_info() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(player_info));
		};
		unsigned long uid;
		unsigned long tipo;
		char id[22];
		char nickname[22];
		unsigned long auth_key;
	};
}

#endif // !_STDA_PANGYA_GG_AUTH_ST_HPP
