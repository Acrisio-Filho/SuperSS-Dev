// Arquivo random_gen.hpp
// Criado em 15/08/2019 as 07:43 por Acrisio
// Definição a classe RandomGen

#pragma once
#ifndef _STDA_RANDOM_GEN_HPP
#define _STDA_RANDOM_GEN_HPP

#if defined(__linux__)
#include "WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../TYPE/singleton.h"

#include <random>

namespace stdA {
    class RandomGen {
        public:
            RandomGen();
            virtual ~RandomGen();

			bool isGood();
			
		private:
			// Random device
			uint64_t _rDevice();

			// Random indenpendent_bits_engine mt19937_64 chrono
			uint64_t _rIbeMt19937_64_chrono();

			// Random indenpendent_bits_engine mt19937_64 random device
			uint64_t _rIbeMt19937_64_rdevice();

			// Random Device sort of range
			uint64_t _rDeviceRange(uint64_t _min, uint64_t _max);

			// Random indenpendent_bits_engine mt19937_64 chrono
			uint64_t _rIbeMt19937_64_chronoRange(uint64_t _min, uint64_t _max);

			// Random indenpendent_bits_engine mt19937_64 random device
			uint64_t _rIbeMt19937_64_rdeviceRange(uint64_t _min, uint64_t _max);

		public:
			// Random device
			uint64_t rDevice();

			// Random indenpendent_bits_engine mt19937_64 chrono
			uint64_t rIbeMt19937_64_chrono();

			// Random indenpendent_bits_engine mt19937_64 random device
			uint64_t rIbeMt19937_64_rdevice();

			// Random Device sort of range
			uint64_t rDeviceRange(uint64_t _min, uint64_t _max);

			// Random indenpendent_bits_engine mt19937_64 chrono
			uint64_t rIbeMt19937_64_chronoRange(uint64_t _min, uint64_t _max);

			// Random indenpendent_bits_engine mt19937_64 random device
			uint64_t rIbeMt19937_64_rdeviceRange(uint64_t _min, uint64_t _max);

		private:
			bool init();
			void destroy();

        private:
			std::random_device *m_rd;
			std::independent_bits_engine< std::mt19937_64, 64, std::uint_fast64_t > *m_ibe_mt19937_64;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif

			bool m_state;
    };

	typedef Singleton< RandomGen > sRandomGen;
}

#endif // !_STDA_RANDOM_GEN_HPP