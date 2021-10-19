// Arquivo calculator_player.cpp
// Criado em 23/11/2020 as 15:15 por Acrisio
// Implementa��o da classe CalculatorPlayer

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "calculator_player.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../UTIL/logger.hpp"

using namespace stdA;

CalculatorPlayer::CalculatorPlayer() : m_response_packet() {
}

CalculatorPlayer::~CalculatorPlayer() {

	if (!m_response_packet.empty())
		m_response_packet.clear();
}

void CalculatorPlayer::execResponsePacket(ICalculatorPlayer& _this, ePACKET _sub_packet, Packet< ePACKET >& _packet) {

	try {

		auto it = m_response_packet.find(_sub_packet);

		if (it != m_response_packet.end())
			it->second(_this, _packet);
		else
			sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][WARNING] Player[UID=" + std::to_string(_this.m_id) 
					+ "] Packet[ID=" + std::to_string((unsigned short)_sub_packet) + "] unknown.", eTYPE_LOG::TL_WARNING);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}
