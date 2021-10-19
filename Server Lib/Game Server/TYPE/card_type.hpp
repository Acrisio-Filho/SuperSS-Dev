// Arquivo card_type.hpp
// Criado em 01/07/2018 as 00:33 por Acrisio
// Definição das struct e tipo de suporte para a classe CardSystem

#pragma once
#ifndef _STDA_CARD_TYPE_HPP
#define _STDA_CARD_TYPE_HPP

#include <vector>

#include <memory.h>

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	enum CARD_TYPE : unsigned {
		T_NORMAL,
		T_RARE,
		T_SUPER_RARE,
		T_SECRET,
	};

	struct Card {
		void clear() { memset(this, 0, sizeof(Card)); };
		uint32_t _typeid;
		uint32_t prob;		// Probabilidade
		CARD_TYPE tipo;			// tipo, Normal, Rare, Super Rare, Secreto
	};

	struct CardPack {
		CardPack(uint32_t _ul = 0u) {
			clear();
		};
		CardPack(uint32_t __typeid, uint32_t _num, unsigned char _volume)
			: _typeid(__typeid), num(_num), volume(_volume) {
		};
		~CardPack() {};
		void clear() {
			if (!card.empty()) {
				card.clear();
				card.shrink_to_fit();
			}

			_typeid = 0u;
			num = 0u;
			volume = 0u;
		};
		struct Rate {
			void clear() { memset(this, 0, sizeof(Rate)); };
			unsigned short value[4];	// Normal, Rare, Super Rare, Secret
		};
		uint32_t _typeid;
		uint32_t num;			// Número de card(s) que esse pack dá
		unsigned char volume;		// Volume do Card Pack, Vol 1, 2, 3, 4, 5 etc
		Rate rate;					// Rate, N, R, SR, SC
		std::vector< Card > card;	// Cards
	};

	struct LoloCardCompose {
		void clear() { memset(this, 0, sizeof(LoloCardCompose)); };
		uint64_t pang;
		uint32_t _typeid[3];
	};

	struct LoloCardComposeEx : public LoloCardCompose {
		LoloCardComposeEx(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(LoloCardComposeEx)); };
		unsigned char tipo;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_CARD_TYPE_HPP
