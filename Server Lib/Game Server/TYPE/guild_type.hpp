// Arquivo guild_type.hpp
// Criado em 29/12/2019 as 11:41 por Acrisio
// Definição dos tipos usados no guild battle

#pragma once
#ifndef _STDA_GUILD_TYPE_HPP
#define _STDA_GUILD_TYPE_HPP

#include <memory.h>
#include <cstdint>

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

	// Dados dos holes no jogo
	struct Dados {
		unsigned short score;
		uint32_t tacada;
		unsigned char finish : 1;
	};

	// Guild Match register
	struct GuildMatch {
		void clear() {
			memset(this, 0, sizeof(GuildMatch));
		};
		uint32_t uid[2];		// Guild UID: [0] e [1]
		uint32_t point[2];		// Guild Point: [0] e [1]
		uint32_t pang[2];		// Guild Pang: [0] e [1]
	};

	// Guild Points
	struct GuildPoints {
		enum eGUILD_WIN : unsigned char {
			WIN,
			LOSE,
			DRAW,
		};
		void clear() {
			memset(this, 0, sizeof(GuildPoints));
		};
		uint32_t uid;
		uint64_t point;
		uint64_t pang;
		eGUILD_WIN win;
	};

	// Guild Member Points
	struct GuildMemberPoints {
		void clear() {
			memset(this, 0, sizeof(GuildMemberPoints));
		};
		uint32_t guild_uid;
		uint32_t member_uid;
		uint32_t point;
		uint32_t pang;
	};

#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_GUILD_TYPE_HPP
