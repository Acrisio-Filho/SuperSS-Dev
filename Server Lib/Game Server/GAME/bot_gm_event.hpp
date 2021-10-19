// Arquivo bot_gm_event.hpp
// Criado em 03/11/2020 as 20:03 por Acrisio
// Defini��o da classe BotGMEvent

#pragma once
#ifndef _STDA_BOT_GM_EVENT_HPP
#define _STDA_BOT_GM_EVENT_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include "../../Projeto IOCP/TYPE/singleton.h"
#include "../TYPE/bot_gm_event_type.hpp"
#include <vector>

namespace stdA {

	class BotGMEvent {

		public:
			BotGMEvent();
			virtual ~BotGMEvent();

			void load();

			bool isLoad();

			bool checkTimeToMakeRoom();

			// Verifica se a mensagem do intervalo de tempo atual j� foi enviada 
			bool messageSended();

			// Zera todas os intervalos que n�o est� na hora, e o intervalo que est� na hora seta ele
			void setSendedMessage();

			stRangeTime* getInterval();

			std::vector< stReward > calculeReward();

		protected:
			void initialize();

			void clear();

		private:
			std::vector< stRangeTime > m_rt;		// Times to make room event
			std::vector< stReward > m_rewards;

			bool m_load;

			SYSTEMTIME m_st;							// Usando para n�o ficar criando direto na fun��o de check

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< BotGMEvent > sBotGMEvent;
}

#endif // !_STDA_BOT_GM_EVENT_HPP
