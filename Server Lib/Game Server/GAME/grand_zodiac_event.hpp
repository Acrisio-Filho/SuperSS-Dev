// Arquivo grand_zodiac_event.hpp
// Criado em 26/06/2020 as 13:09 por Acrisio
// Defini��o da classe GrandZodiacEvent

#pragma once
#ifndef _STDA_GRAND_ZODIAC_EVENT_HPP
#define _STDA_GRAND_ZODIAC_EVENT_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../../Projeto IOCP/TYPE/singleton.h"
#include "../TYPE/grand_zodiac_type.hpp"

#include <vector>

namespace stdA {
	class GrandZodiacEvent {

		public:
			GrandZodiacEvent();
			virtual ~GrandZodiacEvent();

			/*static */void load();

			/*static */bool isLoad();

			/*static */bool checkTimeToMakeRoom();

			// Verifica se a mensagem do intervalo de tempo atual j� foi enviada 
			/*static*/bool messageSended();

			// Zera todas os intervalos que n�o est� na hora, e o intervalo que est� na hora seta ele
			/*static*/void setSendedMessage();

			/*static*/range_time* getInterval();

		protected:
			/*static */void initialize();

			/*static */void clear();

		private:
			/*static */std::vector< range_time > m_rt;	// Times to make room event

			/*static */bool m_load;

			SYSTEMTIME m_st;							// Usando para n�o ficar criando direto na fun��o de check
			
#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< GrandZodiacEvent > sGrandZodiacEvent;
}

#endif // !_STDA_GRAND_ZODIAC_EVENT_HPP
