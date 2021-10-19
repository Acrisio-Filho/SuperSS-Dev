// Arquivo pangya_login_st.h
// Criado em 23/07/2017 por Acrisio
// Definição das estruturas usadas no pangya login

#pragma once
#ifndef _STDA_PANGYA_LOGIN_ST_H
#define _STDA_PANGYA_LOGIN_ST_H

#include <memory>

#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	// Player info
	struct player_info {
		player_info() {
			clear();
		};
		void clear() {
			memset(this, 0, sizeof(player_info));
		};
		unsigned int  uid;
		unsigned int  m_cap;
		BlockFlag block_flag;
		unsigned short level;
		char id[22];
		char nickname[22];
		char pass[40];
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif