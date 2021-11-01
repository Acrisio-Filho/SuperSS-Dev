// Arquivo func_arr.h
// Criado em 04/06/2017 por Acrisio
// Definição da class func_arr

#pragma once
#ifndef STDA_PACKET_FUNC_ARR_H
#define STDA_PACKET_FUNC_ARR_H

#include "../TYPE/stdAType.h"

#if defined(__linux__)
#include "WinPort.h"
#endif

#define MAX_CALL_FUNC_ARR 10000 // Era 500, era 1000

namespace stdA {
	class func_arr {
		public: // Estrutura func_arr_ex
			struct func_arr_ex {
				call_func cf;
				void* Param;
				int execCmd(void* _arg);/* {
					return cf(Param, _arg);
				};*/
			};

		public:
			func_arr();
			~func_arr();

			void addPacketCall(unsigned __int16 _tipo, call_func _func, void* _Param);

			func_arr_ex* getPacketCall(unsigned __int16 _tipo);

		protected:
			func_arr_ex m_func[MAX_CALL_FUNC_ARR];	// pacotes de recv
	};
}

#endif