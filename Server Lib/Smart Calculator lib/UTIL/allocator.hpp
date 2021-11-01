// Arquivo allocator.hpp
// Criado em 21/11/2020 as 10:38 por Acrisio
// Definição e Implementação de alocadores

#pragma once
#ifndef _STDA_ALLOCATOR_HPP
#define _STDA_ALLOCATOT_HPP

#include <memory>

namespace stdA {

	template<typename _Type> _Type allocType(size_t _chunk) {
		return reinterpret_cast< _Type >( malloc(_chunk) );
	};
}

#endif // !_STDA_ALLOCATOR_HPP
