// Arquivo player_info.hpp
// Criado em 02/12/2018 as 13:08 por Acrisio
// Defini��o da classe player_info

#pragma once
#ifndef _STDA_PLAYER_INFO_HPP
#define _STDA_PLAYER_INFO_HPP

#include "pangya_auth_st.h"

namespace stdA {
	class PlayerInfo : public player_info {
		public:
			PlayerInfo();
			virtual ~PlayerInfo();

			void clear();

		public:
			unsigned char m_state;
			unsigned char m_place;
			uint32_t m_server_uid;		// Server UID em que eles est� conectado
	};
}

#endif // !_STDA_PLAYER_INFO_HPP
