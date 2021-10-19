// Arquivo rank_registry.cpp
// Criado em 16/06/2020 as 14:50 por Acrisio
// Implementa��o da classe RankRegistry

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "rank_registry.hpp"

using namespace stdA;

RankRegistry::RankRegistry() 
	: m_uid(0u), m_current_position(0u), m_last_position(0u), m_value(0) {
}

RankRegistry::RankRegistry(uint32_t _uid, uint32_t _current_position, uint32_t _last_position, int32_t _valuen)
	: m_uid(_uid), m_current_position(_current_position), m_last_position(_last_position) {
}

RankRegistry::~RankRegistry() {
	clear();
}

void RankRegistry::clear() {

	m_uid = 0u;
	m_current_position = 0u;
	m_last_position = 0u;
	m_value = 0l;
}

void RankRegistry::toPacket(packet& _packet) {

	_packet.addUint32(m_uid);
	_packet.addUint32(m_current_position);
	_packet.addUint32(m_last_position);
	_packet.addInt32(m_value);
}

void RankRegistry::toCompactPacket(packet& _packet) {

	// Aqui se o rank passar de 10 mil zera, por que o pangya s� conta o top 10 mil
	// Mas no banco de dados ele conta normal, que no toPacket completo ele manda o rank mesmo se for maior que 10 mil
	// P�gina ele s� envia 5 mil, ele s� mostra as p�ginas de 5 mil ranks (Registros)

	// Vou fazer assim.
	// Se o rank atual passou de 10 mil, zera os 2
	// Se o rank atual for menor ou igual a 10 mil, envia o lest position mesmo se ele for maior que 10 mil

	if (CHECK_LIMIT_RANK_POSITION_COMPACT_PACKET(m_current_position))
		_packet.addZeroByte(8u);	// Envia Zeros no Current Position e Last Position, por que o current position � maior que 10 mil
	else {
		_packet.addUint32(m_current_position);
		_packet.addUint32(m_last_position);
	}

	_packet.addInt32(m_value);
}

// Get
uint32_t& RankRegistry::getUID() {
	return m_uid;
}

uint32_t& RankRegistry::getCurrentPosition() {
	return m_current_position;
}

uint32_t& RankRegistry::getLastPosition() {
	return m_last_position;
}

int32_t& RankRegistry::getValue() {
	return m_value;
}

// Set
void RankRegistry::setUID(uint32_t _uid) {
	m_uid = _uid;
}

void RankRegistry::setCurrentPosition(uint32_t _current_position) {
	m_current_position = _current_position;
}

void RankRegistry::setLastPosition(uint32_t _last_position) {
	m_last_position = _last_position;
}

void RankRegistry::setValue(int32_t _value) {
	m_value = _value;
}
