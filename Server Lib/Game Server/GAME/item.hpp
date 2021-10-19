// Arquivo item.hpp
// Criado em 24/02/2018 as 19:11 por Acrisio
// Definição da classe item

#pragma once
#ifndef _STDA_ITEM_HPP
#define _STDA_ITEM_HPP

#include <cstdint>

namespace stdA {
    class item {
        public:
            item();
            ~item();

		protected:
			uint32_t id;
			uint32_t _typeid;

			char name[64];
			char icon[41];

			uint32_t price;
			uint32_t desconto;

			unsigned short c[5];
#define STDA_ITEM_QNTD c[0]
#define STDA_ITEM_TIME c[5]
    };
}

#endif