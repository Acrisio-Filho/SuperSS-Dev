// Arquivo player_info.hpp
// Criado em 15/06/2020 as 15:07 por Acrisio
// Definição da classe PlayerInfo

#pragma once
#ifndef _STDA_PLAYER_INFO_HPP
#define _STDA_PLAYER_INFO_HPP

#include "pangya_rank_st.hpp"

namespace stdA {
	class PlayerInfo : public player_info {
		public:
			PlayerInfo();
			virtual ~PlayerInfo();

			virtual void clear();

		public:
			unsigned char m_state;

			// Dados que usa para consultar o rank
			search_dados_ex m_sd;			// Search dados
	};
}

#endif // !_STDA_PLAYER_INFO_HPP
