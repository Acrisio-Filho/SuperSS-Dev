// Arquivo player_info.hpp
// Criado em 02/02/2021 as 17:04 por Acrisio
// Definição da classe PlayerInfo

#pragma once
#ifndef _STDA_PLAYER_INFO_HPP
#define _STDA_PLAYER_INFO_HPP

#include "pangya_gg_auth_st.hpp"

namespace stdA {

	class PlayerInfo : public player_info {
		public:
			PlayerInfo();
			virtual ~PlayerInfo();

			void clear();

		public:
			unsigned char m_state;
	};
}

#endif // !_STDA_PLAYER_INFO_HPP
