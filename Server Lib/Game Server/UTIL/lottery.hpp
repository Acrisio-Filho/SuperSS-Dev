// Arquivo lottery.hpp
// Criado em 24/06/2018 as 11:53 por Acrisio
// Definição da classe Lottery

#pragma once
#ifndef _STDA_LOTTERY_HPP
#define _STDA_LOTTERY_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include <vector>
#include <map>

#include <memory.h>

namespace stdA {
	class Lottery {
		public:
			struct LotteryCtx {
				void clear() { memset(this, 0, sizeof(LotteryCtx)); };
				uint32_t prob;	// Probabilidade
				size_t value;
				uint64_t offset[2];	// 0 start, 1 end
				unsigned char active : 1, : 0;	// 0 ou 1 ativo
			};

		public:
			Lottery(uint64_t _value_rand);
			~Lottery();
			
			void clear();	// Clear Ctx

			void push(LotteryCtx& _lc);
			void push(uint32_t _prob, size_t _value);

			uint64_t getLimitProbilidade();

			// Retorna a quantidade de itens que tem para sortear
			uint32_t getCountItem();

			// Deleta o Item Sorteado, para não sair ele de novo, se for passado true
			LotteryCtx* spinRoleta(bool _remove_item_draw = false);

		protected:
			void initialize(uint64_t _value_rand);

			void fill_roleta();
			void clear_roleta();

			void remove_draw_item(LotteryCtx* _lc);

			void shuffle_values_rand();

		private:
			std::map< uint64_t, LotteryCtx* > m_roleta;

			std::vector< LotteryCtx > m_ctx;
			std::vector< uint64_t > m_rand_values;

			uint64_t m_prob_limit;
	};
}

#endif // !_STDA_LOTTERY_HPP
