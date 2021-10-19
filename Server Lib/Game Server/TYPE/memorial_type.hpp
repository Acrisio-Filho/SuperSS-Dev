// Arquivo memorial_type.hpp
// Criado em 21/07/2018 as 18:54 por Acrisio
// Definição dos tipos utilizados pela classe MemorialSystem

#pragma once
#ifndef _STDA_MEMORIAL_TYPE_HPP
#define _STDA_MEMORIAL_TYPE_HPP

#include <vector>
#include <memory>

#include <memory.h>

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	enum MEMORIAL_COIN_TYPE : uint32_t {
		MCT_NORMAL,
		MCT_PREMIUM,
		MCT_SPECIAL,
	};

	struct ctx_coin_item {
		ctx_coin_item(uint32_t _ul = 0u) {
			clear();
		};
		ctx_coin_item(int32_t _tipo, uint32_t __typeid, uint32_t _qntd) 
			: tipo(_tipo), _typeid(__typeid), qntd(_qntd) {
		};
		void clear() { memset(this, 0, sizeof(ctx_coin_item)); };
		int32_t tipo;
		uint32_t _typeid;
		uint32_t qntd;
	};

	struct ctx_coin_item_ex : public ctx_coin_item {
		ctx_coin_item_ex(uint32_t _lc = 0u) {
			clear();
		};
		ctx_coin_item_ex(int32_t _tipo, uint32_t __typeid, uint32_t _qntd, uint32_t _probabilidade, int32_t _gachar_number) 
			: ctx_coin_item(_tipo, __typeid, _qntd), probabilidade(_probabilidade), gacha_number(_gachar_number) {
		};
		void clear() { memset(this, 0, sizeof(ctx_coin_item_ex)); };
		uint32_t probabilidade;
		int32_t gacha_number;
	};

	struct ctx_coin {
		ctx_coin(uint32_t _ul = 0u) {
			clear();
		};
		~ctx_coin() {};
		void clear() {

			tipo = MCT_NORMAL;
			_typeid = 0u;
			probabilidade = 100;

			if (!item.empty()) {
				item.clear();
				item.shrink_to_fit();
			}
		};
		MEMORIAL_COIN_TYPE tipo;
		uint32_t _typeid;
		uint32_t probabilidade;
		std::vector< ctx_coin_item_ex > item;
	};

	struct ctx_memorial_level {
		void clear() { memset(this, 0, sizeof(ctx_memorial_level)); };
		uint32_t level;		// Level
		uint32_t gacha_number;	// número máximo do gacha
	};

	struct ctx_coin_set_item {
		ctx_coin_set_item(uint32_t _ul = 0u) {
			clear();
		};
		~ctx_coin_set_item() {};
		void clear() {
			
			_typeid = 0u;
			tipo = 0u;
			flag = -100;

			if (!item.empty()) {
				item.clear();
				item.shrink_to_fit();
			}
		};
		int32_t flag;
		uint32_t _typeid;
		unsigned char tipo : 1, : 0;		// Tipo 0 e 1, 1 Premium e 0 todos os outros
		std::vector< ctx_coin_item_ex > item;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_MEMORIAL_TYPE_HPP
