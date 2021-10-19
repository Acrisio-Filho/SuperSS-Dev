// Arquivo extra_power_interface.hpp
// Criado em 03/10/2020 as 15:21 por Acrisio
// Definição da classe Interface ExtraPower

#pragma once
#ifndef _STDA_EXTRA_POWER_INTERFACE_HPP
#define _STDA_EXTRA_POWER_INTERFACE_HPP

namespace stdA {

	class IExtraPower {

		public:
			virtual float getTotal(unsigned char _psf) = 0;
	};
}

#endif // !_STDA_EXTRA_POWER_INTERFACE_HPP
