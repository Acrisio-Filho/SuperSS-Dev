// Arquivo box_type.hpp
// Criado em 15/07/2018 as 21:31 por Acrisio
// Definição dos tipos usado pela classe BoxSystem

#pragma once
#ifndef _STDA_BOX_TYPE_HPP
#define _STDA_BOX_TYPE_HPP

#include <memory>
#include <vector>

#include <memory.h>

namespace stdA {

	enum BOX_TYPE_RARETY : unsigned char {
		R_NORMAL,
		R_RARE,
		R_SUPER_RARE,
	};

	enum BOX_TYPE_OPEN : unsigned char {
		O_SEND_MAIL,
		O_SEND_MYROOM,
	};

	enum BOX_TYPE : unsigned char {
		NORMAL,
		ALL_RARE_OR_LUCKY_REWARD,
	};

	struct ctx_box_item {
		void clear() { memset(this, 0, sizeof(ctx_box_item)); };
		uint32_t _typeid;
		int32_t numero;
		int32_t qntd;
		uint32_t probabilidade;
		BOX_TYPE_RARETY raridade;
		unsigned char duplicar : 1, : 0;	// 0 ou 1, 1 pode duplicar item
		unsigned char active : 1, : 0;		// 0 ou 1
	};

	struct ctx_box {
		ctx_box(uint32_t _ul = 0u) {
			clear();
		};
		~ctx_box() {};
		void clear() { memset(this, 0, sizeof(ctx_box)); };
		BOX_TYPE_OPEN tipo_open;
		BOX_TYPE tipo;
		int32_t numero;
		int32_t id;
		uint32_t _typeid;
		uint32_t opened_typeid;			// Typeid da Box aberta se tiver
		char msg[80];							// Msg da box
		//unsigned char active : 1, : 0;			// 0 ou 1, melhor só ter essa opção no DB mesmo
		std::vector< ctx_box_item > item;		// Todos os itens da box
	};
}

#endif // !_STDA_BOX_TYPE_HPP
