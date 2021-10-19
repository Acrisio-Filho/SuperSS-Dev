// Arquivo dupla.hpp
// Criado em 29/12/2019 as 11:43 por Acrisio
// Definição da classe Dupla

#pragma once
#ifndef _STDA_DUPLA_HPP
#define _STDA_DUPLA_HPP

#include "../TYPE/guild_type.hpp"
#include "../SESSION/player.hpp"

namespace stdA {
	struct Dupla {
	public:
		enum eSTATE : unsigned char {
			IN_GAME,
			OUT_GAME,
			OVER_TIME,	// Tempo acabou antes de player acabar o jogo
		};

	public:
		Dupla(unsigned char _numero, player *_p1, player *_p2);

		unsigned short sumScoreP1();		// Soma o score do player 1
		unsigned short sumScoreP2();		// Soma o score do player 2

		unsigned char numero;				// Número da dupla
		player *p[2];						// Players: [0] e [1]
		unsigned char hole[2];				// Número do hole que o player está no jogo: [0] e [1]
		uint32_t pang_win[2];			// Pang: [0] e [1]
		uint64_t pang[2];			// Total de pangs ganho no jogo: [0] e [1]
		eSTATE state[2];					// Estado no jogo: [0] e [1]
		Dados dados[2][18];					// Dados dos holes, 18 holes: [0] e [1]
	};
}

#endif // !_STDA_DUPLA_HPP
