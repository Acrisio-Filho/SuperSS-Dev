// Arquivo calculator_player.hpp
// Criado em 23/11/2020 as 15:08 por Acrisio
// Definição da classe CalculatorPlayer

#pragma once
#ifndef _STDA_CALCULATOR_PLAYER_HPP
#define _STDA_CALCULATOR_PLAYER_HPP

#include "../../Projeto IOCP/TYPE/smart_calculator_player_base.hpp"
#include "common.hpp"
#include <map>

namespace stdA {

	typedef void(*FNTRANSLATEPACKET)(ICalculatorPlayer& _this, Packet< ePACKET >& _packet);

#define CHECK_CALIPER_RESULT(_caliper) (_caliper < 0.f ? std::string("NULL") : toFixed(_caliper, 1))

	class CalculatorPlayer{

		public:
			CalculatorPlayer();
			virtual ~CalculatorPlayer();

			virtual void execResponsePacket(ICalculatorPlayer& _this, ePACKET _sub_packet, Packet< ePACKET >& _packet);

		protected:
			virtual void initPacketResponse() = 0;

		protected:
			std::map< ePACKET, FNTRANSLATEPACKET > m_response_packet;
	};
}

#endif // !_STDA_CALCULATOR_PLAYER_HPP
