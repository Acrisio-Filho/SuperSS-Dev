// Arquivo comet_refill_type.hpp
// Criado em 08/07/2018 as 22:08 por Acrisio
// Definições de tipos para a classe CometRefillSystem

#pragma once
#ifndef _STDA_COMET_REFILL_TYPE_HPP
#define _STDA_COMET_REFILL_TYPE_HPP

#include <memory.h>

#include <cstdint>

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	// Contexto de Comet Refill
	struct ctx_comet_refill {
		void clear() { memset(this, 0, sizeof(ctx_comet_refill)); };
		uint32_t _typeid;
		struct QntdRange {
			bool isValid() { return (min > 0 && max > 0); };
			unsigned short min;
			unsigned short max;
		};
		QntdRange qntd_range;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_COMET_REFILL_TYPE_HPP
