// Arquivo game_guard_type.hpp
// Criado em 31/03/2020 as 19:23 por Acrisio
// Definição dos tipos usados pelo game guard

#pragma once
#ifndef _STDA_GAME_GUARD_TYPE_HPP

#include "../CSAuth/GGAuth.hpp"

namespace stdA {

	constexpr time_t AUTH_PERIOD_SEC = 60;
	constexpr time_t AUTH_PERIOD_VARIATION_SEC = 60;

	struct PlayerGameGuard {
		CCSAuth2 m_csa;
		time_t m_auth_time;
		bool m_auth_reply;
	};
}

#endif // !_STDA_GAME_GUARD_TYPE_HPP
