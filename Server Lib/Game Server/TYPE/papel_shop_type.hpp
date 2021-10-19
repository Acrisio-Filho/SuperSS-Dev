// Arquivo papel_shop_type.hpp
// Criado em 09/07/2018 as 18:10 por Acrisio
// Definição de tipo usados na classe PapelShopSystem

#pragma once
#ifndef _STDA_PAPEL_SHOP_TYPE_HPP
#define _STDA_PAPEL_SHOP_TYPE_HPP

#include <memory>
#include "../../Projeto IOCP/UTIL/util_time.h"

#include <memory.h>

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	enum PAPEL_SHOP_TYPE : unsigned char {
		PST_COMMUN,
		PST_COOKIE,
		PST_RARE
	};

	enum PAPEL_SHOP_BALL_COLOR : unsigned char {
		PSBC_BLUE,
		PSBC_GREEN,
		PSBC_RED,
	};

	struct ctx_papel_shop {
		void clear() { memset(this, 0, sizeof(ctx_papel_shop)); };
		std::string toString() {
			return "NUMERO=" + std::to_string(numero) + ", PRICE_NORMAL=" + std::to_string(price_normal) 
					+ ", PRICE_BIG=" + std::to_string(price_big) + ", LIMITTED_PER_DAY=" + std::to_string((unsigned short)limitted_per_day) 
					+ ", UPDATE_DATE=" + _formatDate(update_date);
		};
		uint32_t numero;				// Atual Número do Papel Shop	
		uint64_t price_normal;	// Preço do Jogo Normal
		uint64_t price_big;		// Preço do Jogo Big
		unsigned char limitted_per_day : 1; // Limitado por dia, tem uma quantidade que pode jogar	// 0 ou 1
		SYSTEMTIME update_date;				// Date de atualização do dia do papel shop
	};

	struct ctx_papel_shop_item {
		void clear() { memset(this, 0, sizeof(ctx_papel_shop_item)); };
		uint32_t _typeid;
		uint32_t probabilidade;
		int32_t numero;					// Número que o papel shop já está
		PAPEL_SHOP_TYPE tipo;
		unsigned char active : 1;		// Active 0 ou 1
	};

	struct ctx_papel_shop_ball {
		ctx_papel_shop_ball(uint32_t _ul = 0u) {
			clear();
		};
		void clear() { memset(this, 0, sizeof(ctx_papel_shop_ball)); };
		PAPEL_SHOP_BALL_COLOR color;
		ctx_papel_shop_item ctx_psi;
		uint32_t qntd;					// Qntd do item que foi sorteado
		void* item;							// stItem, para depois que add no banco de dados, retornar o id, precisa quando envia o pacote de resposta de jogar o papel shop
	};

	struct ctx_papel_shop_coupon {
		void clear() { memset(this, 0, sizeof(ctx_papel_shop_coupon)); }
		uint32_t _typeid;
		unsigned char active : 1;	// 0 ou 1
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_PAPEL_SHOP_TYPE_HPP
