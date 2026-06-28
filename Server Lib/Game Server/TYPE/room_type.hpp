// Arquivo room_type.hpp
// Criado em 29/08/2023 as 07:39 por Acrisio
// Definição dos tipos usados na classe room e seus derivados

#pragma once
#ifndef _STDA_ROOM_TYPE_HPP
#define _STDA_ROOM_TYPE_HPP

namespace stdA {
	// Times room
	constexpr uint32_t k40SecMilli = 40u * 1000u;
	constexpr uint32_t k60SecMilli = 60u * 1000u;
	constexpr uint32_t k120SecMilli = 120u * 1000u;
	constexpr uint32_t k300SecMilli = 300u * 1000u;
	constexpr uint32_t k7MinMilli = 7u * 60000u;
	constexpr uint32_t k10MinMilli = 10u * 60000u;
	constexpr uint32_t k15MinMilli = 15u * 60000u;
	constexpr uint32_t k20MinMilli = 20u * 60000u;
	constexpr uint32_t k25MinMilli = 25u * 60000u;
	constexpr uint32_t k30MinMilli = 30u * 60000u;
	constexpr uint32_t k35MinMilli = 35u * 60000u;
	constexpr uint32_t k40MinMilli = 40u * 60000u;
	constexpr uint32_t k45MinMilli = 45u * 60000u;
	constexpr uint32_t k50MinMilli = 50u * 60000u;
	constexpr uint32_t k55MinMilli = 55u * 60000u;

	enum eROOM_CLASS_TYPE : uint8_t {
		RCT_NORMAL,
		RCT_GRAND_PRIX,
		RCT_GRAND_ZODIAC_EVENT,
		RCT_BOT_GM_EVENT,
		RCT_MASTER_CUP_EVENT
	};
}

#endif // !_STDA_ROOM_TYPE_HPP
