// Arquivo rank_character.cpp
// Criado em 16/06/2020 as 18:32 por Acrisio
// Implementa��o da classe RankCharacter

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "rank_character.hpp"

using namespace stdA;

RankCharacter::RankCharacter() : m_uid(0u), m_id{ 0u }, m_nickname{ 0u }, m_level(0u), m_ci{ 0u }, m_term_s5_type(0u), m_class_type(0u) {
}

RankCharacter::RankCharacter(uint32_t _uid, std::string _id, std::string _nickname, unsigned short _level, CharacterInfo _ci, unsigned char _term_s5_type, unsigned char _class_type)
	: m_uid(_uid), m_level(_level), m_ci(_ci), m_id{ 0u }, m_nickname{ 0u }, m_term_s5_type(_term_s5_type), m_class_type(_class_type) {

	if (!_id.empty())
#if defined(_WIN32)
		memcpy_s(m_id, sizeof(m_id), _id.c_str(), (_id.length() > sizeof(m_id) ? sizeof(m_id) : _id.length()));
#elif defined(__linux__)
		memcpy(m_id, _id.c_str(), (_id.length() > sizeof(m_id) ? sizeof(m_id) : _id.length()));
#endif

	if (!_nickname.empty())
#if defined(_WIN32)
		memcpy_s(m_nickname, sizeof(m_nickname), _nickname.c_str(), (_nickname.length() > sizeof(m_nickname) ? sizeof(m_nickname) : _nickname.length()));
#elif defined(__linux__)
		memcpy(m_nickname, _nickname.c_str(), (_nickname.length() > sizeof(m_nickname) ? sizeof(m_nickname) : _nickname.length()));
#endif
}

RankCharacter::~RankCharacter() {
	clear();
}

void RankCharacter::clear() {

	m_uid = 0u;
	m_level = 0u;
	m_term_s5_type = 0u;
	m_class_type = 0u;

	memset(m_id, 0, sizeof(m_id));
	memset(m_nickname, 0, sizeof(m_nickname));

	m_ci.clear();
}

void RankCharacter::playerInfoToPacket(packet& _packet) {

	_packet.addUint8((unsigned char)m_level);
	
	// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
	_packet.addUint8(m_term_s5_type);

	// Op��es descontinuadas no Fresh UP!, por�m ele ainda mant�m nos packet
	_packet.addUint8(m_class_type);

	_packet.addString(m_id);
	_packet.addString(m_nickname);
}

void RankCharacter::playerFullInfoPacket(packet& _packet) {

	_packet.addUint32(m_uid);
	_packet.addBuffer(m_id, sizeof(m_id));
	_packet.addBuffer(m_nickname, sizeof(m_nickname));
	_packet.addUint16(m_level);
}

void RankCharacter::playerCharacterInfoToPacket(packet& _packet) {

	_packet.addBuffer(&m_ci, sizeof(CharacterInfo));
}

uint32_t& RankCharacter::getUID() {
	return m_uid;
}

const char* RankCharacter::getId() {
	return m_id;
}

const char* RankCharacter::getNickname() {
	return m_nickname;
}

unsigned short& RankCharacter::getLevel() {
	return m_level;
}

unsigned char& RankCharacter::getTermS5Type() {
	return m_term_s5_type;
}

unsigned char& RankCharacter::getClassType() {
	return m_class_type;
}

CharacterInfo& RankCharacter::getCharacterInfo() {
	return m_ci;
}

void RankCharacter::setUID(uint32_t _uid) {
	m_uid = _uid;
}

void RankCharacter::setId(std::string _id) {

	if (!_id.empty())
#if defined(_WIN32)
		memcpy_s(m_id, sizeof(m_id), _id.c_str(), (_id.length() > sizeof(m_id) ? sizeof(m_id) : _id.length()));
#elif defined(__linux__)
		memcpy(m_id, _id.c_str(), (_id.length() > sizeof(m_id) ? sizeof(m_id) : _id.length()));
#endif
	else
		memset(m_id, 0, sizeof(m_id));
}

void RankCharacter::setNickname(std::string _nickname) {

	if (!_nickname.empty())
#if defined(_WIN32)
		memcpy_s(m_nickname, sizeof(m_nickname), _nickname.c_str(), (_nickname.length() > sizeof(m_nickname) ? sizeof(m_nickname) : _nickname.length()));
#elif defined(__linux__)
		memcpy(m_nickname, _nickname.c_str(), (_nickname.length() > sizeof(m_nickname) ? sizeof(m_nickname) : _nickname.length()));
#endif
	else
		memset(m_nickname, 0, sizeof(m_nickname));
}

void RankCharacter::setLevel(unsigned short _level) {
	m_level = _level;
}

void RankCharacter::setTermS5Type(unsigned char _term_s5_type) {
	m_term_s5_type = _term_s5_type;
}

void RankCharacter::setClassType(unsigned char _class_type) {
	m_class_type = _class_type;
}

void RankCharacter::setCharacterInfo(CharacterInfo _ci) {
	m_ci = _ci;
}
