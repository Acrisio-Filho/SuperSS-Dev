// Arquivo extra_power.hpp
// Criado em 07/10/202 as 13:59 por Acrisio
// Defini��o da classe ExtraPower

#pragma once
#ifndef _STDA_EXTRA_POWER_HPP
#define _STDA_EXTRA_POWER_HPP

#include "../TYPE/extra_power_interface.hpp"

#include <cstdint>

namespace stdA {

	class ExtraPower : public IExtraPower {

		public:
			struct stPowerQntd {
				public:
					int32_t m_auxpart;
					int32_t m_mascot;
					int32_t m_card;

				public:
					int32_t total() {
						return m_auxpart + m_mascot + m_card;
					};
			};

		public:
			ExtraPower();
			ExtraPower(stPowerQntd _drive, stPowerQntd _power_shot);
			virtual ~ExtraPower();

			stPowerQntd& getPowerDrive();
			stPowerQntd& getPowerShot();

			void setPowerDrive(stPowerQntd _drive);
			void setPowerShot(stPowerQntd _power_shot);

		public:
			virtual float getTotal(unsigned char _psf) override;

		private:
			// Drive normal
			stPowerQntd m_drive;

			// Power Shot
			stPowerQntd m_power_shot;
	};
}

#endif // !_STDA_EXTRA_POWER_HPP
