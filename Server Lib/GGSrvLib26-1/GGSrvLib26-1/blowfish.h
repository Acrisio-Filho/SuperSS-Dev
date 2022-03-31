// Arquivo blowfish.h
// Criado em 25/03/2022 as 22:51 por Acrisio
// Definição do algoritimo de encriptação blowfish
// Cópia com pequenas modificações do código fonte em c do Bruce Schneier
// fonte: https://www.schneier.com/academic/blowfish/

#pragma once
#ifndef _BLOWFISH_H
#define _BLOWFISH_H

#define MAXKEYBYTES 56          /* 448 bits */
// #define little_endian 1              /* Eg: Intel */
#define big_endian 1            /* Eg: Motorola */

#define N				16
#define KEYBYTES		8

#if defined(_WIN32)
// Pack padr�o
#pragma pack(push, ggsrv)
#pragma pack( )
#endif

#include <cstdint>

struct _BLOWFISH_CTX {
	uint32_t P[N + 2];
	uint32_t S[4][256];
};

#if defined(_WIN32)
// Pop Pack padr�o
#pragma pack(pop, ggsrv)
#endif

uint32_t F(_BLOWFISH_CTX* _ctx, uint32_t x);

void Blowfish_Init(_BLOWFISH_CTX* _ctx, char _key[], int _keybytes);

void Blowfish_Encrypt(_BLOWFISH_CTX* _ctx, uint32_t *_xl, uint32_t *_xr);

void Blowfish_Decrypt(_BLOWFISH_CTX* _ctx, uint32_t *_xl, uint32_t *_xr);

#endif // !_BLOWFISH_H
