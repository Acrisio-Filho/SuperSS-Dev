// Arquivo packet_func.cpp
// Criado em 05/12/2017 por Acrisio
// Implementação da classe packet_func_base

#if defined(_WIN32)
#include <Winsock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "packet_func.h"

using namespace stdA;

func_arr packet_func_base::funcs;
func_arr packet_func_base::funcs_sv;
func_arr packet_func_base::funcs_as;

packet_func_base::packet_func_base() {};
packet_func_base::~packet_func_base() {};