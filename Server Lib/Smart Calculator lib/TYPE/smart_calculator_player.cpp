// Arquivo smart_calculator_player.cpp
// Criado em 21/11/2020 as 09:11 por Acrisio
// Implementa��o da classe SmartCalculatorPlayer

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include <math.h>
#endif

#include "smart_calculator_player.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"
#include "../UTIL/allocator.hpp"
#include "../UTIL/string_convert.hpp"

#include "../UTIL/logger.hpp"
#include "../UTIL/type_util.hpp"

#include "../SOCKET/pool.hpp"

#include "../TYPE/common.hpp"

#include <cmath>

#if defined(_WIN32)
#include <random> // PI
#elif defined(__linux__)
#include <numbers>
#endif

const char* CLUB_INFO_ENUM[15]{
		{"1W"},
		{"2W"},
		{"3W"},
		{"2I"},
		{"3I"},
		{"4I"},
		{"5I"},
		{"6I"},
		{"7I"},
		{"8I"},
		{"9I"},
		{"PW"},
		{"SW"},
		{"PT1"},
		{"PT2"}
};

const char* POWER_SHOT_FACTORY_ENUM[4]{
	{"NO_POWER_SHOT"},
	{"ONE_POWER_SHOT"},
	{"TWO_POWER_SHOT"},
	{"ITEM_15_POWER_SHOT"}
};

const char* SHOT_TYPE_ENUM[4]{
	{"DUNK"},
	{"TOMAHAWK"},
	{"SPIKE"},
	{"COBRA"}
};

#define INIT_ARG_COMMAND(_arg) if ((_arg) == nullptr || (_arg) == "") \
									return; \
\
								auto v_args = split((_arg), " "); \

#define CHECK_INPUT_VALUE if (v_args.empty()) \
								return; \
\
							if (isNaN(v_args[0])) { \
\
								sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "invalid value", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
								return; \
							} \

#define CHECK_VALUE_IS_NUMBER(_value) if (isNaN((_value))) { \
\
										sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "invalid value", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
										return; \
									} \

#define CHECK_VALUE_COND(_condition) if ((_condition)) { \
\
										sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "invalid value", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
										return; \
									} \

#define SET_VALUE_INPUT_FROM_STRING(_value) (_value) = (float)std::atof(v_args[0].c_str());

#define CHECK_ALLOW_COMMAND if (!m_game_shot_value.gm) { \
\
								sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "you don't have permission to execute this command.", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
								return; \
							} \

#define INIT_RESPONSE(_this) SmartCalculatorPlayer& smart = reinterpret_cast< SmartCalculatorPlayer& > ((_this));

using namespace stdA;

// Implementa��o da classe SmartCalculatorPlayer
SmartCalculatorPlayer::SmartCalculatorPlayer(uint32_t _id) 
	: ISmartCalculatorPlayer(_id), CalculatorPlayer(), m_special_slope_flag(false), m_special_slope_break{ 0.f }
{

	clear();

	initPacketResponse();
}

SmartCalculatorPlayer::~SmartCalculatorPlayer() {

	clear();
}

std::string SmartCalculatorPlayer::cInfoToString() {
	return R"1(Smart Calculator - Version 1.0.0 Alpha - Developer: Acrisio SuperSS Dev
Comandos:
* ping - Mostra a latencia.
* myinfo - (Ex: myinfo | myinfo input) {
	resolution: Mostra os dados de resolucao (Width x Height);
	auto_fit: Mostra o valor do auto_fit configurado (ON ou OFF);
	dfav: Mostra o desvio favorito;
	stat: Mostra os dados de (Stats) power, ring, mascot, card e card_ps;
	shot: Mostra os dados de (Tacada) club, shot e power shot;
	input: Mostra os dados de insercao.
}.
* list - (Ex: list club) - (type) {
	club: Mostra os clubes disponiveis e o seu indice para usar com o comando "club";
	shot: Mostra as tacas disponiveis e o seu indice para usar com o comando "shot";
	ps: Mostra os power shot disponiveis e o seu indice para usar com o comando "ps";
	dfav: Mostra os tipos de retorno de desvio favorito disponiveis e seu indice para usar com o comando "dfav";
	macro: Mostra os macros disponiveis para usar com o comando "m".
}.
* calc - Faz os calculo com os dados inseridos;
* lastr - Mostra o ultimo resultado retornado do comando "calc";
* resolution - Altera a resolucao. Ex: 640 480 (Width Height);
* auto_fit - Altera o auto_fit. Ex: (1 ou ON, 0 ou OFF);
* dfav - Altera o tipo do desvio favorito. (indice) Ex: 0, 1 or etc N(number);
* m - Executa um macro nome(argumentos), Ex: hello(SuperSS, Dev);
* expr - Faz o calculo de um expressao matematica. Ex: 2 + 2 + sin(2.5);
* mycella - Faz o calculo de mycella por angulo. Ex: mycella 120 240 12, (Degree Shot, Degree Align, Slope breaks);
* aim - Mostra o angulo do ponto 0 do course para o buraco.

Obs: Para ver os comandos de insercao de dados use o comando myinfo.
)1";
}

std::string SmartCalculatorPlayer::cToString() {
	return ("Power\n(Ex: power 15)  - Power: " + std::to_string((unsigned short)m_power) +
		"\n\nPower extras"
		"\n(Ex: ring 2)    - Ring(Power): " + std::to_string((short)m_auxpart_pwr) +
		"\n(Ex: mascot 2)  - Mascot(Power): " + std::to_string((short)m_mascot_pwr) +
		"\n(Ex: card 4)    - Card(Power): " + std::to_string((short)m_card_pwr) +
		"\n(Ex: card_ps 2) - Card Power Shot(Power): " + std::to_string((short)m_ps_card_pwr) +
		"\nShot"
		"\n(Ex: club 1)    - Club[" + std::to_string((unsigned short)m_club_index) + "]: " + getClubName(m_club_index) +
		"\n(Ex: shot 2)    - Shot[" + std::to_string((unsigned short)m_shot_index) + "]: " + getShotName(m_shot_index) +
		"\n(Ex: ps 2)      - Power Shot[" + std::to_string((unsigned short)m_power_shot_index) + "]: " + getPowerShotName(m_power_shot_index) +
		"\nInputs"
		"\n(Ex: d 224.4)   - Distance: " + toFixed(m_distance, 2) +
		"\n(Ex: h -3.41)   - Height: " + toFixed(m_height, 2) +
		"\n(Ex: w 5)       - Wind: " + toFixed(m_wind, 0) +
		"\n(Ex: a 360)     - Degree: " + toFixed(m_degree, 2) +
		"\n(Ex: g 100)     - Ground: " + toFixed(m_ground, 0) +
		"\n(Ex: s 9)       - Spin: " + toFixed(m_spin, 2) +
		"\n(Ex: c 30)      - Curve: " + toFixed(m_curve, 2) +
		"\n(Ex: b -2.1)    - Slope Break: " + toFixed(m_slope_break, 2) +
		"\n(Ex: make_slope 1 3) - Special Slope: " + toFixed(m_special_slope_break[0], 2) + ", " + toFixed(m_special_slope_break[1], 2) + ", " + toFixed(m_special_slope_break[2], 2));
}

std::string SmartCalculatorPlayer::cStatsToString() {
	return ("Power\n(Ex: power 15)  - Power: " + std::to_string((unsigned short)m_power) +
		"\n\nPower extras"
		"\n(Ex: ring 2)    - Ring(Power): " + std::to_string((short)m_auxpart_pwr) +
		"\n(Ex: mascot 2)  - Mascot(Power): " + std::to_string((short)m_mascot_pwr) +
		"\n(Ex: card 4)    - Card(Power): " + std::to_string((short)m_card_pwr) +
		"\n(Ex: card_ps 2) - Card Power Shot(Power): " + std::to_string((short)m_ps_card_pwr));
}

std::string SmartCalculatorPlayer::cShotToString() {
	return ("Shot\n(Ex: club 1)    - Club[" + std::to_string((unsigned short)m_club_index) + "]: " + getClubName(m_club_index) +
		"\n(Ex: shot 2)    - Shot[" + std::to_string((unsigned short)m_shot_index) + "]: " + getShotName(m_shot_index) +
		"\n(Ex: ps 2)      - Power Shot[" + std::to_string((unsigned short)m_power_shot_index) + "]: " + getPowerShotName(m_power_shot_index));
}

std::string SmartCalculatorPlayer::cInputToString() {
	return ("Inputs\n(Ex: d 224.4)   - Distance: " + toFixed(m_distance, 2) +
		"\n(Ex: h -3.41)   - Height: " + toFixed(m_height, 2) +
		"\n(Ex: w 5)       - Wind: " + toFixed(m_wind, 0) +
		"\n(Ex: a 360)     - Degree: " + toFixed(m_degree, 2) +
		"\n(Ex: g 100)     - Ground: " + toFixed(m_ground, 0) +
		"\n(Ex: s 9)       - Spin: " + toFixed(m_spin, 2) +
		"\n(Ex: c 30)      - Curve: " + toFixed(m_curve, 2) +
		"\n(Ex: b -2.1)    - Slope Break: " + toFixed(m_slope_break, 2) +
		"\n(Ex: make_slope 1 3) - Special Slope: " + toFixed(m_special_slope_break[0], 2) + ", " + toFixed(m_special_slope_break[1], 2) + ", " + toFixed(m_special_slope_break[2], 2));
}

std::string SmartCalculatorPlayer::cLastResultToString() {
	return ("Last result\nDeviation: " + toFixed(m_last_result.desvio, 2) +
		"\nPercent: " + toFixed(m_last_result.percent, 1) +
		"\nCaliper: " + CHECK_CALIPER_RESULT(m_last_result.caliper));
}

std::string SmartCalculatorPlayer::cResolutionToString() {
	return ("Resolution: " + std::to_string(m_resolution.width) + "x" + std::to_string(m_resolution.height));
}

std::string SmartCalculatorPlayer::cAutoFitToString() {
	return ("Auto Fit: " + std::string(m_auto_fit ? "ON" : "OFF"));
}

std::string SmartCalculatorPlayer::cDesvioFavoritoToString() {
	
	std::string ret = "UNKNOWN";

	if (m_desvio_fav >= (eTYPE_DESVIO_FAVORITO::END))
		return ret;

	ret = "[" + std::to_string((uint32_t)m_desvio_fav) + "] - ";
	ret += getTypeDesvioFavoritoName(m_desvio_fav);

	return ret;
}

std::string SmartCalculatorPlayer::cListDesvioFavoritoToString() {
	
	std::string ret = "";

	for (auto i = 0; i < (uint32_t)eTYPE_DESVIO_FAVORITO::END; i++) {

		if (i != 0u)
			ret += "\n";

		ret += "[" + std::to_string((uint32_t)i) + "] - ";
		ret += getTypeDesvioFavoritoName(eTYPE_DESVIO_FAVORITO(i));
	}

	return ret;
}

std::string SmartCalculatorPlayer::cListClubToString() {
	
	std::string ret = "";

	bool past_first = false;

	for (auto i = 0u; i < (sizeof(CLUB_INFO_ENUM) / sizeof(CLUB_INFO_ENUM[0])); i++) {

		if (strstr(CLUB_INFO_ENUM[i], "PT") != nullptr)
			continue;

		if (past_first)
			ret += "\n";
		else
			past_first = true;

		ret += "[" + std::to_string(i) + "] - ";
		ret += getClubName(i);
	}

	return ret;
}

std::string SmartCalculatorPlayer::cListShotToString() {
	
	std::string ret = "";

	for (auto i = 0u; i < (sizeof(SHOT_TYPE_ENUM) / sizeof(SHOT_TYPE_ENUM[0])); i++) {

		if (i != 0u)
			ret += "\n";

		ret += "[" + std::to_string(i) + "] - ";
		ret += getShotName(i);
	}

	return ret;
}

std::string SmartCalculatorPlayer::cListPowerShotToString() {
	
	std::string ret = "";

	for (auto i = 0u; i < (sizeof(POWER_SHOT_FACTORY_ENUM) / sizeof(POWER_SHOT_FACTORY_ENUM[0])); i++) {

		if (i != 0u)
			ret += "\n";

		ret += "[" + std::to_string(i) + "] - ";
		ret += getPowerShotName(i);
	}

	return ret;
}

void SmartCalculatorPlayer::clear() {

	// Base
	m_last_result.clear();
	m_resolution.clear();
	m_desvio_fav = eTYPE_DESVIO_FAVORITO::YARD;
	m_auto_fit = false;

	// Smart Calculator
	m_power = 0u;

	// Pode ser negativo esses
	m_auxpart_pwr = 0;
	m_mascot_pwr = 0;
	m_card_pwr = 0;
	m_ps_card_pwr = 0;

	m_club_index = 0u;
	m_shot_index = 0u;
	m_power_shot_index = 0u;

	m_distance = 0.f;
	m_height = 0.f;
	m_wind = 0.f;
	m_degree = 0.f;
	m_ground = 100.f;
	m_spin = 0.f;
	m_curve = 0.f;
	m_slope_break = 0.f;

	// Quebra especial que manda 3 valores, Qbra x, �ngulo, Qbra Z
	m_special_slope_flag = false;
	memset(m_special_slope_break, 0, sizeof m_special_slope_break);

	m_game_shot_value.clear();
}

uint32_t SmartCalculatorPlayer::getClubTypeLength() {
	
	size_t length = 0u;

	length = std::count_if(CLUB_INFO_ENUM, CLUB_INFO_ENUM + (sizeof(CLUB_INFO_ENUM) / sizeof(CLUB_INFO_ENUM[0])), [](const char* _el) {
		return strstr(_el, "PT") == nullptr;
	});

	return (uint32_t)length;
}

void SmartCalculatorPlayer::initPacketResponse() {

	if (!m_response_packet.empty())
		m_response_packet.clear();

	m_response_packet.insert({
		{
			ePACKET::COMMAND_SMART_PING,
			SmartCalculatorPlayer::responseCommandString
		},
		{
			ePACKET::COMMAND_SMART_LIST_MACRO,
			SmartCalculatorPlayer::responseCommandString
		},
		{
			ePACKET::COMMAND_SMART_EXPRESSION,
			SmartCalculatorPlayer::responseCommandString
		},
		{
			ePACKET::COMMAND_SMART_CALC,
			SmartCalculatorPlayer::responseCalcule
		},
		{
			ePACKET::COMMAND_SMART_MACRO,
			SmartCalculatorPlayer::responseMacro
		}
	});
}

void SmartCalculatorPlayer::responseCommandString(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		std::string reply;
		_packet >> reply;

		sLogger::getInstance().Response(smart.m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::responseCalcule(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		std::string reply;
		unsigned char opt = 0u;
		_packet >> opt;

		smart.m_last_result.clear();

		if (opt == 1u/*Error*/ || opt == 2u/*String*/)
			_packet >> reply;
		else if (opt == 0u) {

			stResult result;

			_packet.read(&result, sizeof(stResult));

			smart.m_last_result = result;

			reply = toFixed(result.percent, 1) + "%, " + CHECK_CALIPER_RESULT(result.caliper) + "y, " + toFixed(desvioByScala(result.desvio, smart), 2) + getDesvioLegendByType(smart.m_desvio_fav);

			if (isMoreResult(smart.m_desvio_fav))
				reply += ", " + toFixed(restOfDesvioByScala(result.desvio, smart), 2) + getDesvioMoreLegendByType(smart.m_desvio_fav);

			//reply += " - \"" + smart.cDesvioFavoritoToString() + "\"";
		}

		sLogger::getInstance().Response(smart.m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::responseMacro(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		std::string reply;
		_packet >> reply;

		sLogger::getInstance().Response(smart.m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

char* SmartCalculatorPlayer::infoToString() {
	
	std::string info = cInfoToString();

	char* ret = allocType<char*>(info.length() + 1);

	copyStrToSzString(info, ret);

	return ret;
}

char* SmartCalculatorPlayer::toString() {

	std::string s = cToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::lastResultToString() {
	
	std::string s = cLastResultToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::resolutionToString() {
	
	std::string s = cResolutionToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::autoFitToString() {
	
	std::string s = cAutoFitToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::desvioFavoritoToString() {
	
	std::string s = cDesvioFavoritoToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::listDesvioFavoritoToString() {
	
	std::string s = cListDesvioFavoritoToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::listClubToString() {
	
	std::string s = cListClubToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::listShotToString() {
	
	std::string s = cListShotToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::listPowerShotToString() {
	
	std::string s = cListPowerShotToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::statsToString() {

	std::string s = cStatsToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::shotToString() {

	std::string s = cShotToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* SmartCalculatorPlayer::inputToString() {

	std::string s = cInputToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

const char* SmartCalculatorPlayer::getClubName(uint32_t _index) {

	uint32_t len = (sizeof(CLUB_INFO_ENUM) / sizeof(CLUB_INFO_ENUM[0]));

	if ((int)_index < 0l || _index >= len)
		return "UNKNOWN";

	return CLUB_INFO_ENUM[_index];
}

const char* SmartCalculatorPlayer::getShotName(uint32_t _index) {

	uint32_t len = (sizeof(SHOT_TYPE_ENUM) / sizeof(SHOT_TYPE_ENUM[0]));

	if ((int)_index < 0l || _index >= len)
		return "UNKNOWN";

	return SHOT_TYPE_ENUM[_index];
}

const char* SmartCalculatorPlayer::getPowerShotName(uint32_t _index) {

	uint32_t len = (sizeof(POWER_SHOT_FACTORY_ENUM) / sizeof(POWER_SHOT_FACTORY_ENUM[0]));

	if ((int)_index < 0l || _index >= len)
		return "UNKNOWN";

	return POWER_SHOT_FACTORY_ENUM[_index];
}

void SmartCalculatorPlayer::freeResource(void* _resource) {

	if (_resource != nullptr)
		free(_resource);
}

void SmartCalculatorPlayer::setGameShotValue(stGameShotValue& _gsv) {

	try {

		m_game_shot_value = _gsv;

		m_wind = m_game_shot_value.wind;
		m_distance = m_game_shot_value.distance;
		m_degree = m_game_shot_value.degree;

		m_power = m_game_shot_value.power_slot;

		m_auxpart_pwr = m_game_shot_value.auxpart_pwr;
		m_mascot_pwr = m_game_shot_value.mascot_pwr;
		m_card_pwr = m_game_shot_value.card_pwr;
		m_ps_card_pwr = m_game_shot_value.ps_card_pwr;

		// Só deixa o ground 100% se não tiver chuvendo
		if (m_game_shot_value.ground && !m_game_shot_value.rain)
			m_ground = 100.f;

		if (m_game_shot_value.safety) {
			
			// Só deixa o ground 100% se não tiver chovendo
			if (!m_game_shot_value.rain)
				m_ground = 100.f;

			m_special_slope_flag = false;

			m_slope_break = 0.f;
			memset(m_special_slope_break, 0, sizeof m_special_slope_break);
		}

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::ping(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> pckt(ePACKET::COMMAND_SMART_PING);
		pckt << m_id;

		time_t now = 0u;
		std::time(&now);

		pckt << (uint32_t)now;

		sPool::getInstance().sendPacket(pckt);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::info(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, cInfoToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::myInfo(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		std::string reply = "";

		if (v_args.size() > 0) {

			auto option = toLowerCase(v_args[0]);

			if (option.compare("stat") == 0)
				reply = cStatsToString();
			else if (option.compare("shot") == 0)
				reply = cShotToString();
			else if (option.compare("input") == 0)
				reply = cInputToString();
			else if (option.compare("lastR") == 0)
				reply = cLastResultToString();
			else if (option.compare("resolution") == 0)
				reply = cResolutionToString();
			else if (option.compare("auto_fit") == 0)
				reply = cAutoFitToString();
			else if (option.compare("dfav") == 0)
				reply = cDesvioFavoritoToString();

		}else
			reply = cToString();

		if (!reply.empty())
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::list(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		if (v_args[0].compare("dfav") == 0)
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, cListDesvioFavoritoToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
		else if (v_args[0].compare("club") == 0)
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, cListClubToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
		else if (v_args[0].compare("shot") == 0)
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, cListShotToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
		else if (v_args[0].compare("ps") == 0)
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, cListPowerShotToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
		else if (v_args[0].compare("macro") == 0) {

			Packet<ePACKET> pckt(ePACKET::COMMAND_SMART_LIST_MACRO);

			pckt << m_id;

			sPool::getInstance().sendPacket(pckt);
		}

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::calcule(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> p(ePACKET::COMMAND_SMART_CALC);

		p << m_id;
		p << m_power;
		p << m_auxpart_pwr;
		p << m_mascot_pwr;
		p << m_card_pwr;
		p << m_ps_card_pwr;
		p << m_club_index;
		p << m_shot_index;
		p << m_power_shot_index;
		p << m_distance;
		p << m_height;
		p << m_wind;
		p << m_degree;
		p << m_ground;
		p << m_spin;
		p << m_curve;

		// Option Quebra
		p << (unsigned char)(m_special_slope_flag ? 1u : 0u);

		if (!m_special_slope_flag)
			p << m_slope_break;
		else
			p.write(m_special_slope_break, sizeof m_special_slope_break);

		sPool::getInstance().sendPacket(p);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::expression(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> pckt(ePACKET::COMMAND_SMART_EXPRESSION);

		pckt << m_id;

		pckt << (join(v_args, " "));

		sPool::getInstance().sendPacket(pckt);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::macro(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> p(ePACKET::COMMAND_SMART_MACRO);

		p << m_id;
		p << m_distance;
		p << m_height;
		p << m_wind;
		p << m_degree;
		p << m_ground;

		// Option Quebra
		if (!m_special_slope_flag)
			p << m_slope_break;
		else {

			// Calcula slope
			float rad = (float)(m_special_slope_break[1] * 
#if defined(_WIN32)
				std::_Pi
#elif defined(__linux__)
				std::numbers::pi 
#endif
				/ 180.f);

			float slope = (float)(/*Nega*/-((cosf(rad) * m_special_slope_break[0]) - (-sinf(rad) * m_special_slope_break[2])));

			p << slope;
		}

		p << (float)0.f; // Green Slope

		p << m_game_shot_value.power_range;

		// Result
		stResult copy = m_last_result;

		copy.percent *= 0.01f; // divede por 100, que o macro usar

		p.write(&copy, sizeof(stResult));

		p << join(v_args, " ");

		sPool::getInstance().sendPacket(p);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::last_result(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, cLastResultToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::resolution(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.size() < 2)
			return;

		CHECK_VALUE_IS_NUMBER(v_args[0]);
		CHECK_VALUE_IS_NUMBER(v_args[1]);

		uint32_t width = (uint32_t)(float)std::atof(v_args[0].c_str());
		uint32_t height = (uint32_t)(float)std::atof(v_args[1].c_str());

		if (width < BASE_RESOLUTION_WIDTH)
			width = BASE_RESOLUTION_WIDTH;

		if (height < BASE_RESOLUTION_HEIGHT)
			height = BASE_RESOLUTION_HEIGHT;

		m_resolution.width = width;
		m_resolution.height = height;

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::desvio_favorito(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.size() < 1)
			return;

		uint32_t index = (uint32_t)(float)std::atof(v_args[0].c_str());

		CHECK_VALUE_COND(index >= (uint32_t)eTYPE_DESVIO_FAVORITO::END);

		m_desvio_fav = eTYPE_DESVIO_FAVORITO(index);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::auto_fit(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.size() < 1)
			return;

		if (isNaN(v_args[0])) {

#if defined(_WIN32)
			if (_stricmp(v_args[0].c_str(), "ON") == 0)
#elif defined(__linux__)
			if (strcasecmp(v_args[0].c_str(), "ON") == 0)
#endif
				m_auto_fit = true;
#if defined(_WIN32)
			else if (_stricmp(v_args[0].c_str(), "OFF") == 0)
#elif defined(__linux__)
			else if (strcasecmp(v_args[0].c_str(), "OFF") == 0)
#endif
				m_auto_fit = false;

		}else {

			uint32_t auto_fit = (uint32_t)(float)std::atof(v_args[0].c_str());

			m_auto_fit = (auto_fit == 1u) ? true : false;
		}

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::mycella_degree(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		if (v_args.size() < 3)
			return;

		CHECK_VALUE_IS_NUMBER(v_args[0]);
		CHECK_VALUE_IS_NUMBER(v_args[1]);
		CHECK_VALUE_IS_NUMBER(v_args[2]);

		float degree_shot = (float)std::atof(v_args[0].c_str());
		float degree_align = (float)std::atof(v_args[1].c_str());
		float slope_breaks = (float)std::atof(v_args[2].c_str());

		if (slope_breaks == 0.f) {

			// S� altera o �ngulo se for GM
			if (m_game_shot_value.gm)
				m_degree = degree_shot;

			m_slope_break = slope_breaks / getSlopeByResolution(m_resolution, m_auto_fit);
		
		}else {

			// Lambda degree to radianos
			constexpr auto degreeToRad = [](float _degree) -> float {
				return float(
#if defined(_WIN32)
					std::_Pi
#elif defined(__linux__)
					std::numbers::pi 
#endif
					/ 180.f * _degree);
			};

			float slope_real = fabsf(cosf(fabsf(degreeToRad(degree_shot - degree_align)))) * slope_breaks;

			// S� altera o �ngulo se for GM
			if (m_game_shot_value.gm)
				m_degree = degree_shot;

			m_slope_break = slope_real / getSlopeByResolution(m_resolution, m_auto_fit);
		}

		// Zera slope special
		m_special_slope_flag = false;
		memset(m_special_slope_break, 0, sizeof m_special_slope_break);

		// Resposta
		std::string reply = "Slope Real: " + toFixed(m_slope_break, 2);

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::club(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		// Verifica se � o nome do club
		if (isNaN(v_args[0])) {

			for (auto i = 0u; i < (sizeof(CLUB_INFO_ENUM) / sizeof(CLUB_INFO_ENUM[0])); i++) {

#if defined(_WIN32)
				if (_stricmp(CLUB_INFO_ENUM[i], v_args[0].c_str()) == 0) {
#elif defined(__linux__)
				if (strcasecmp(CLUB_INFO_ENUM[i], v_args[0].c_str()) == 0) {
#endif

					m_club_index = (unsigned char)i;

					break;
				}
			}

			return;
		}

		// Index do club
		unsigned char index = (unsigned char)(int)(float)std::atof(v_args[0].c_str());

		if (index >= getClubTypeLength()) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "Invalid Club Index", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		m_club_index = index;

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::shot(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		// Verifica se � o nome da tacada(shot)
		if (isNaN(v_args[0])) {

			for (auto i = 0u; i < (sizeof(SHOT_TYPE_ENUM) / sizeof(SHOT_TYPE_ENUM[0])); i++) {
				
#if defined(_WIN32)
				if (_stricmp(SHOT_TYPE_ENUM[i], v_args[0].c_str()) == 0) {
#elif defined(__linux__)
				if (strcasecmp(SHOT_TYPE_ENUM[i], v_args[0].c_str()) == 0) {
#endif
					
					m_shot_index = (unsigned char)i;

					break;
				}
			}

			return;
		}

		// Index da tacada(shot)
		unsigned char index = (unsigned char)(int)(float)std::atof(v_args[0].c_str());

		if (index >= (sizeof(SHOT_TYPE_ENUM) / sizeof(SHOT_TYPE_ENUM[0]))) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "Invalid Shot Index", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		m_shot_index = index;

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::power_shot(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		unsigned char index = (unsigned char)(int)(float)std::atof(v_args[0].c_str());

		if (index >= (sizeof(POWER_SHOT_FACTORY_ENUM) / sizeof(POWER_SHOT_FACTORY_ENUM[0]))) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, "Invalid Power Shot Index", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		m_power_shot_index = index;

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::power(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		m_power = (unsigned char)(int)(float)std::atof(v_args[0].c_str());

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::ring(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		m_auxpart_pwr = (char)(int)(float)std::atof(v_args[0].c_str());

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::mascot(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		m_mascot_pwr = (char)(int)(float)std::atof(v_args[0].c_str());

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::card(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		m_card_pwr = (char)(int)(float)std::atof(v_args[0].c_str());

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::card_power_shot(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		m_ps_card_pwr = (char)(int)(float)std::atof(v_args[0].c_str());

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::distance(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_distance);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::height(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_height);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::wind(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_wind);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::degree(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// Verifica se � GM para usar esse comando
		CHECK_ALLOW_COMMAND;

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_degree);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::ground(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_ground);

		if (m_ground < 50.f || m_ground > 100.f)
			m_ground = 100.f;

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::spin(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_spin);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::curve(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_curve);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::slope_break(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_slope_break);

		// Update Qbra pela resolu��o e auto fit
		m_slope_break /= getSlopeByResolution(m_resolution, m_auto_fit);

		m_special_slope_flag = false;
		memset(m_special_slope_break, 0, sizeof m_special_slope_break);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::make_slope_break(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		if (v_args.size() == 2) { // Qbra x, Qbra z
			
			CHECK_VALUE_IS_NUMBER(v_args[0]);
			CHECK_VALUE_IS_NUMBER(v_args[1]);

			auto scala = getSlopeByResolution(m_resolution, m_auto_fit);

			m_special_slope_break[0] = ((float)std::atof(v_args[0].c_str())) / scala * -1/*Inverte a slope*/;
			m_special_slope_break[2] = ((float)std::atof(v_args[1].c_str())) / scala * -1/*Inverte a slope*/;

			m_special_slope_break[1] = float(m_game_shot_value.mira_rad * 180 / 
#if defined(_WIN32)
				std::_Pi
#elif defined(__linux__)
				std::numbers::pi 
#endif
			); // Degree

			m_special_slope_flag = true;

			m_slope_break = 0.f;

		}else if (v_args.size() == 3) { // Qbra x, �ngulo, Qbra Z

			CHECK_VALUE_IS_NUMBER(v_args[0]);
			CHECK_VALUE_IS_NUMBER(v_args[1]);
			CHECK_VALUE_IS_NUMBER(v_args[2]);

			auto scala = getSlopeByResolution(m_resolution, m_auto_fit);

			m_special_slope_break[0] = ((float)std::atof(v_args[0].c_str())) / scala * -1/*Inverte a slope*/;
			m_special_slope_break[1] = (float)std::atof(v_args[1].c_str());
			m_special_slope_break[2] = ((float)std::atof(v_args[2].c_str())) / scala * -1/*Inverte a slope*/;

			m_special_slope_flag = true;

			m_slope_break = 0.f;
		}

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::aim_degree(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		// �ngulo mira
		float aim_degree = float(m_game_shot_value.mira_rad * 180.f / 
#if defined(_WIN32)
			std::_Pi
#elif defined(__linux__)
			std::numbers::pi 
#endif
		);

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::SMART_CALCULATOR, std::to_string(aim_degree), eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void SmartCalculatorPlayer::translatePacket(void* _packet) {

	if (_packet == nullptr) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Error] invalid _packet pointer.", eTYPE_LOG::TL_ERROR);

		return;
	}

	try {

		Packet< ePACKET >& pckt = *reinterpret_cast< Packet< ePACKET >* >(_packet);

		ePACKET sub_pckt;

		pckt >> sub_pckt;

		execResponsePacket(*this, sub_pckt, pckt);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}
