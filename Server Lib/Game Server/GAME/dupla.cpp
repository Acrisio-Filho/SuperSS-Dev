// Arquivo dupla.cpp
// Criado em 29/12/2019 as 11:44 por Acrisio
// Implementa��o da classe Dupla

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "dupla.hpp"

using namespace stdA;

Dupla::Dupla(unsigned char _numero, player *_p1, player *_p2)
	: numero(_numero), p{ _p1, _p2 }, pang_win{ 0ull }, pang{ 0ull }, state{ eSTATE::IN_GAME }, dados{ 0u } {
}

unsigned short Dupla::sumScoreP1() {

	unsigned short sum = 0l;

	for (auto i = 0u; i < (sizeof(dados[0/*P1*/]) / sizeof(Dados)); ++i)
		sum += dados[0/*P1*/][i].score;

	return sum;
}

unsigned short Dupla::sumScoreP2() {

	unsigned short sum = 0l;

	for (auto i = 0u; i < (sizeof(dados[1/*P2*/]) / sizeof(Dados)); ++i)
		sum += dados[1/*P2*/][i].score;

	return sum;
}