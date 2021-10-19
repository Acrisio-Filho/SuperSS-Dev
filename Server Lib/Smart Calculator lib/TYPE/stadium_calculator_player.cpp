// Arquivo stadium_calculator_player.cpp
// Criado em 21/11/2020 as 11:55 por Acrisio
// Implement���o da classe StadiumCalculatorPlayer

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "stadium_calculator_player.hpp"
#include "../UTIL/allocator.hpp"
#include "../UTIL/string_convert.hpp"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"
#include "../UTIL/logger.hpp"
#include "../SOCKET/pool.hpp"

#include "../UTIL/type_util.hpp"

#include <algorithm>

#if defined(_WIN32)
#include <random> // PI
#elif defined(__linux__)
#include <numbers>
#endif

#include "../../Projeto IOCP/UTIL/hex_util.h"

#define INIT_ARG_COMMAND(_arg) if ((_arg) == nullptr || (_arg) == "") \
									return; \
\
								auto v_args = split((_arg), " "); \

#define CHECK_INPUT_VALUE if (v_args.empty()) \
								return; \
\
							if (isNaN(v_args[0])) { \
\
								sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "invalid value", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
								return; \
							} \

#define CHECK_VALUE_IS_NUMBER(_value) if (isNaN((_value))) { \
\
										sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "invalid value", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
										return; \
									} \

#define CHECK_VALUE_COND(_condition) if ((_condition)) { \
\
										sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "invalid value", eTYPE_RESPONSE::PLAYER_RESPONSE); \
\
										return; \
									} \

#define SET_VALUE_INPUT_FROM_STRING(_value) (_value) = (float)std::atof(v_args[0].c_str());

#define INIT_RESPONSE(_this) StadiumCalculatorPlayer& stadium = reinterpret_cast< StadiumCalculatorPlayer& > ((_this));

using namespace stdA;

StadiumCalculatorPlayer::StadiumCalculatorPlayer(uint32_t _id) 
	: IStadiumCalculatorPlayer(_id), CalculatorPlayer(), m_calculator()
	, m_shot_name(), m_calculator_list()
{

	clear();

	initPacketResponse();
}

StadiumCalculatorPlayer::~StadiumCalculatorPlayer() {

	clear();
}

std::string StadiumCalculatorPlayer::cInfoToString() {
	return R"1(Stadium Calculator - Version 1.0.0 Alpha - Developer: Acrisio SuperSS Dev
Comandos:
* ping - Mostra a latencia.
* myinfo - (Ex: myinfo | myinfo input) {
	resolution: Mostra os dados de resolucao (Width x Height);
	auto_fit: Mostra o valor do auto_fit configurado (ON ou OFF);
	dfav: Mostra o desvio favorito;
	shot: Mostra os dados de (Tacada) club, shot e power shot;
	input: Mostra os dados de insercao.
}.
* list - (Ex: list shot) - (type) {
	calc: Mostra todas as calculadoras e o seu indice para usar com o comando "open";
	shot: - (Ex: list shot | list shot dunk) {
		bs:	Mostra todas as tacadas de back spin disponiveis e seu indice para usar com o comando "shot";
		dunk: Mostra todas as tacadas de dunk disponiveis e seu indice para usar com o comando "shot";
		toma: Mostra todas as tacadas de toma disponiveis e seu indice para usar com o comando "shot";
		spike: Mostra todas as tacadas de spike disponiveis e seu indice para usar com o comando "shot";
		cobra: Mostra todas as tacadas de cobra disponiveis e seu indice para usar com o comando "shot";
		pangya: Mostra todas as tacada de only pangya disponiveis e seu indice para usar com o comando "shot".
	};
	dfav: Mostra os tipos de retorno de desvio favorito disponiveis e seu indice para usar com o comando "dfav";
	macro: Mostra os macros disponiveis para usar com o comando "m".
}.
* shot - indice da tacada;
* open - Abre uma calculadora. Ex: open 1;
* calc - Faz os calculo com os dados inseridos;
* lastr - Mostra o ultimo resultado retornado do comando "calc";
* resolution - Altera a resolucao. Ex: 640 480 (Width Height);
* auto_fit - Altera o auto_fit. Ex: (1 ou ON, 0 ou OFF);
* dfav - Altera o tipo do desvio favorito. (indice) Ex: 0, 1 or etc N(number);
* m - Executa um macro nome(argumentos), Ex: hello(SuperSS, Dev);
* expr - Faz o calculo de um expressao matematica. Ex: 2 + 2 + sin(2.5);
* mycella - Faz o calculo de mycella por angulo. Ex: mycella 120 240 12, (Degree Shot, Degree Align, Slope breaks).

Obs: Para ver os comandos de insercao de dados use o comando myinfo.
)1";
}

std::string StadiumCalculatorPlayer::cToString() {
	return ("Shot"
		"\n(Ex: shot 2)    - Shot[" + std::to_string((unsigned short)m_shot_index) + "]: " + getShotName(m_shot_index) +
		"\nInputs"
		"\n(Ex: d 224.4)   - Distance: " + toFixed(m_distance, 2) +
		"\n(Ex: h -3.41)   - Height: " + toFixed(m_height, 2) +
		"\n(Ex: w 5)       - Wind: " + toFixed(m_wind, 0) +
		"\n(Ex: a 360)     - Degree: " + toFixed(m_degree, 2) +
		"\n(Ex: g 100)     - Ground: " + toFixed(m_ground, 0) +
		"\n(Ex: b -2.1)    - Slope Break: " + toFixed(m_slope_break, 2) +
		"\n(Ex: n 2.1)     - Green Slope: " + toFixed(m_green_slope, 2));
}

std::string StadiumCalculatorPlayer::cShotToString() {
	return ("Shot\n(Ex: shot 2)    - Shot[" + std::to_string((unsigned short)m_shot_index) + "]: " + getShotName(m_shot_index));
}

std::string StadiumCalculatorPlayer::cInputToString() {
	return ("Inputs\n(Ex: d 224.4)   - Distance: " + toFixed(m_distance, 2) +
		"\n(Ex: h -3.41)   - Height: " + toFixed(m_height, 2) +
		"\n(Ex: w 5)       - Wind: " + toFixed(m_wind, 0) +
		"\n(Ex: a 360)     - Degree: " + toFixed(m_degree, 2) +
		"\n(Ex: g 100)     - Ground: " + toFixed(m_ground, 0) +
		"\n(Ex: b -2.1)    - Slope Break: " + toFixed(m_slope_break, 2) +
		"\n(Ex: n 2.1)     - Green Slope: " + toFixed(m_green_slope, 2));
}

std::string StadiumCalculatorPlayer::cLastResultToString() {
	return ("Last result\nDeviation: " + toFixed(m_last_result.desvio, 2) +
		"\nPercent: " + toFixed(m_last_result.percent, 1) +
		"\nCaliper: " + CHECK_CALIPER_RESULT(m_last_result.caliper) +
		"\nHWI: " + toFixed(m_last_result.hwi, 2) +
		"\nSpin: " + toFixed(m_last_result.spin, 2));
}

std::string StadiumCalculatorPlayer::cResolutionToString() {
	return ("Resolution: " + std::to_string(m_resolution.width) + "x" + std::to_string(m_resolution.height));
}

std::string StadiumCalculatorPlayer::cAutoFitToString() {
	return ("Auto Fit: " + std::string(m_auto_fit ? "ON" : "OFF"));
}

std::string StadiumCalculatorPlayer::cDesvioFavoritoToString() {
	
	std::string ret = "UNKNOWN";

	if (m_desvio_fav >= (eTYPE_DESVIO_FAVORITO::END))
		return ret;

	ret = "[" + std::to_string((uint32_t)m_desvio_fav) + "] - ";
	ret += getTypeDesvioFavoritoName(m_desvio_fav);

	return ret;
}

std::string StadiumCalculatorPlayer::cListDesvioFavoritoToString() {
	
	std::string ret = "";

	for (auto i = 0; i < (uint32_t)eTYPE_DESVIO_FAVORITO::END; i++) {

		if (i != 0u)
			ret += "\n";

		ret += "[" + std::to_string((uint32_t)i) + "] - ";
		ret += getTypeDesvioFavoritoName(eTYPE_DESVIO_FAVORITO(i));
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListCalculatorToString() {
	
	std::string ret = "";

	if (m_calculator_list.empty())
		return "Not have calculators";

	for (auto i = 0u; i < m_calculator_list.size(); ++i) {

		if (i != 0u)
			ret += "\n";

		ret += "[" + std::to_string(i) + "] - ";
		ret += m_calculator_list[i];
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty() || std::count_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return !_el.m_club.empty();
	}) == 0u)
		return "Not have shots.";

	std::string ret = "";

	bool past_first = false;

	for (auto& el_shot : m_calculator.m_shot) {

		if (el_shot.m_club.empty())
			continue;

		if (past_first)
			ret += "\n";
		else
			past_first = true;

		ret += el_shot.m_name;

		for (auto& el_club : el_shot.m_club) {

			ret += "\n\t";
			ret += "[" + std::to_string(el_club.m_index) + "] - ";
			ret += el_club.m_name;
		}
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotBackSpinToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty())
		return "Not have shots.";

	std::string ret = "";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return _el.m_id == eTYPE_SHOT_ID::BACK_SPIN;
	});

	if (it_shot == m_calculator.m_shot.end() || it_shot->m_club.empty())
		return "Not have shots \"Back Spin\"";

	ret += it_shot->m_name;

	for (auto& el_club : it_shot->m_club) {

		ret += "\n\t";
		ret += "[" + std::to_string(el_club.m_index) + "] - ";
		ret += el_club.m_name;
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotDunkToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty())
		return "Not have shots.";

	std::string ret = "";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return _el.m_id == eTYPE_SHOT_ID::DUNK;
	});

	if (it_shot == m_calculator.m_shot.end() || it_shot->m_club.empty())
		return "Not have shots \"Dunk\"";

	ret += it_shot->m_name;

	for (auto& el_club : it_shot->m_club) {

		ret += "\n\t";
		ret += "[" + std::to_string(el_club.m_index) + "] - ";
		ret += el_club.m_name;
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotTomahawkToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty())
		return "Not have shots.";

	std::string ret = "";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return _el.m_id == eTYPE_SHOT_ID::TOMAHAWK;
	});

	if (it_shot == m_calculator.m_shot.end() || it_shot->m_club.empty())
		return "Not have shots \"Tomahawk\"";

	ret += it_shot->m_name;

	for (auto& el_club : it_shot->m_club) {

		ret += "\n\t";
		ret += "[" + std::to_string(el_club.m_index) + "] - ";
		ret += el_club.m_name;
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotSpikeToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty())
		return "Not have shots.";

	std::string ret = "";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return _el.m_id == eTYPE_SHOT_ID::SPIKE;
	});

	if (it_shot == m_calculator.m_shot.end() || it_shot->m_club.empty())
		return "Not have shots \"SPIKE\"";

	ret += it_shot->m_name;

	for (auto& el_club : it_shot->m_club) {

		ret += "\n\t";
		ret += "[" + std::to_string(el_club.m_index) + "] - ";
		ret += el_club.m_name;
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotCobraToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty())
		return "Not have shots.";

	std::string ret = "";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return _el.m_id == eTYPE_SHOT_ID::COBRA;
	});

	if (it_shot == m_calculator.m_shot.end() || it_shot->m_club.empty())
		return "Not have shots \"Cobra\"";

	ret += it_shot->m_name;

	for (auto& el_club : it_shot->m_club) {

		ret += "\n\t";
		ret += "[" + std::to_string(el_club.m_index) + "] - ";
		ret += el_club.m_name;
	}

	return ret;
}

std::string StadiumCalculatorPlayer::cListShotOnlyPangyaToString() {
	
	if (m_calculator.m_id == ~0u)
		return "Not have a Calculator opened";

	if (m_calculator.m_shot.empty())
		return "Not have shots.";

	std::string ret = "";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return _el.m_id == eTYPE_SHOT_ID::ONLY_PANGYA;
	});

	if (it_shot == m_calculator.m_shot.end() || it_shot->m_club.empty())
		return "Not have shots \"Only Pangya\"";

	ret += it_shot->m_name;

	for (auto& el_club : it_shot->m_club) {

		ret += "\n\t";
		ret += "[" + std::to_string(el_club.m_index) + "] - ";
		ret += el_club.m_name;
	}

	return ret;
}

void StadiumCalculatorPlayer::clear() {

	// Base
	m_last_result.clear();
	m_resolution.clear();
	m_desvio_fav = eTYPE_DESVIO_FAVORITO::YARD;
	m_auto_fit = false;

	// Stadium Calculator
	m_shot_index = 0u;

	m_distance = 0.f;
	m_height = 0.f;
	m_wind = 0.f;
	m_degree = 0.f;
	m_ground = 100.f;
	m_slope_break = 0.f;
	m_green_slope = 0.f;

	// Calcualtor Opened
	m_calculator.clear();

	// const shot name
	if (!m_shot_name.empty()) {
		m_shot_name.clear();
		m_shot_name.shrink_to_fit();
	}

	// Calculator List
	if (!m_calculator_list.empty()) {
		m_calculator_list.clear();
		m_calculator_list.shrink_to_fit();
	}
}

float StadiumCalculatorPlayer::getShotPower(uint32_t _index) {
	
	if (m_calculator.m_id == ~0u)
		return 230.f;

	if (m_calculator.m_shot.empty() || std::count_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return !_el.m_club.empty();
	}) == 0u)
		return 230.f;

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [_index](stShot& _el_shot) {

		if (_el_shot.m_club.empty())
			return false;

		auto it_club = std::find_if(_el_shot.m_club.begin(), _el_shot.m_club.end(), [_index](stClub& _el_club) {
			return _el_club.m_index == _index;
		});

		return it_club != _el_shot.m_club.end();
	});

	if (it_shot == m_calculator.m_shot.end())
		return 230.f;

	auto it_club = std::find_if(it_shot->m_club.begin(), it_shot->m_club.end(), [_index](stClub& _el_club) {
		return _el_club.m_index == _index;
	});

	if (it_club == it_shot->m_club.end())
		return 230.f;

	return it_club->m_power;
}

bool StadiumCalculatorPlayer::isValidShot(uint32_t _index) {

	if (m_calculator.m_id == ~0u)
		return false;

	if (m_calculator.m_shot.empty() || std::count_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return !_el.m_club.empty();
	}) == 0u)
		return false;

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [_index](stShot& _el_shot) {

		if (_el_shot.m_club.empty())
			return false;

		auto it_club = std::find_if(_el_shot.m_club.begin(), _el_shot.m_club.end(), [_index](stClub& _el_club) {
			return _el_club.m_index == _index;
		});

		return it_club != _el_shot.m_club.end();
	});

	if (it_shot == m_calculator.m_shot.end())
		return false;

	auto it_club = std::find_if(it_shot->m_club.begin(), it_shot->m_club.end(), [_index](stClub& _el_club) {
		return _el_club.m_index == _index;
	});

	if (it_club == it_shot->m_club.end())
		return false;

	return true;
}

void StadiumCalculatorPlayer::initPacketResponse() {

	if (!m_response_packet.empty())
		m_response_packet.clear();

	m_response_packet.insert({
		{
			ePACKET::COMMAND_STADIUM_PING,
			StadiumCalculatorPlayer::responseCommandString
		},
		{
			ePACKET::COMMAND_STADIUM_LIST_MACRO,
			StadiumCalculatorPlayer::responseCommandString
		},
		{
			ePACKET::COMMAND_STADIUM_LIST_CALC,
			StadiumCalculatorPlayer::responseListCalculator
		},
		{
			ePACKET::COMMAND_STADIUM_OPEN_CALC,
			StadiumCalculatorPlayer::responseOpenCalculator
		},
		{
			ePACKET::COMMAND_STADIUM_EXPRESSION,
			StadiumCalculatorPlayer::responseCommandString
		},
		{
			ePACKET::COMMAND_STADIUM_CALC,
			StadiumCalculatorPlayer::responseCalcule
		},
		{
			ePACKET::COMMAND_STADIUM_MACRO,
			StadiumCalculatorPlayer::responseMacro
		}
	});
}

void StadiumCalculatorPlayer::responseCommandString(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		std::string reply;
		_packet >> reply;

		sLogger::getInstance().Response(stadium.m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::responseListCalculator(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		// !@ Teste
		//sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Log] Packet: " + hex_util::BufferToHexString(_packet.m_data.data(), _packet.m_data.size()), eTYPE_LOG::TL_LOG);

		unsigned char opt = 0u;

		_packet >> opt;

		// Error
		if (opt == 1u) {

			std::string reply;
			_packet >> reply;

			sLogger::getInstance().Response(stadium.m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		unsigned short count = 0u;

		_packet >> count;

		if (count > 0u) {

			stadium.m_calculator_list.clear();

			std::string name;

			for (auto i = 0u; i < count; ++i) {

				_packet >> name;

				// !@ Teste
				//sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][Log][" + std::to_string(i) 
				//		+ "] Packet: " + hex_util::BufferToHexString(_packet.m_data.data(), _packet.m_data.size()), eTYPE_LOG::TL_LOG);

				stadium.m_calculator_list.push_back(name);
			}
		}

		sLogger::getInstance().Response(stadium.m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, stadium.cListCalculatorToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::responseOpenCalculator(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		unsigned char opt = 0u;

		_packet >> opt;

		// Error
		if (opt == 1u) {

			std::string reply;
			_packet >> reply;

			sLogger::getInstance().Response(stadium.m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		uint32_t id =(uint32_t)~0u;
		std::string name;

		_packet >> id;
		_packet >> name;

		unsigned short count_shot = 0u;

		_packet >> count_shot;

		if (count_shot == 0u)
			return;

		// Clear current opened calculator
		stadium.m_calculator.clear();

		stadium.m_calculator.m_id = id;
		stadium.m_calculator.m_name = name;

		stShot shot;
		stClub club;

		unsigned short count_club = 0u;

		for (auto i = 0u; i < count_shot; ++i) {

			shot.clear();

			_packet >> shot.m_id;
			_packet >> shot.m_name;

			_packet >> count_club;

			if (count_club == 0u) {
				
				stadium.m_calculator.m_shot.push_back(shot);

				continue;
			}

			for (auto j = 0u; j < count_club; ++j) {

				club.clear();

				_packet >> club.m_index;
				_packet >> club.m_power;
				_packet >> club.m_name;

				shot.m_club.push_back(club);
			}

			stadium.m_calculator.m_shot.push_back(shot);
		}

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::responseCalcule(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		std::string reply;
		unsigned char opt = 0u;
		_packet >> opt;

		stadium.m_last_result.clear();

		if (opt == 1u/*Error*/ || opt == 2u/*String*/)
			_packet >> reply;
		else if (opt == 0u) {

			stResult result;

			_packet.read(&result, sizeof(stResult));

			stadium.m_last_result = result;

			reply = toFixed(result.percent, 1) + "%, " + CHECK_CALIPER_RESULT(result.caliper) + "y, " + toFixed(desvioByScala(result.desvio, stadium), 2) + getDesvioLegendByType(stadium.m_desvio_fav);

			if (isMoreResult(stadium.m_desvio_fav))
				reply += ", " + toFixed(restOfDesvioByScala(result.desvio, stadium), 2) + getDesvioMoreLegendByType(stadium.m_desvio_fav);

			if (result.spin != 0.f)
				reply += " " + toFixed(result.spin, 0) + "s";

			//reply += " - \"" + stadium.cDesvioFavoritoToString() + "\"";
		}

		sLogger::getInstance().Response(stadium.m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::responseMacro(ICalculatorPlayer& _this, Packet< ePACKET >& _packet) {
	INIT_RESPONSE(_this);

	try {

		std::string reply;
		_packet >> reply;

		sLogger::getInstance().Response(stadium.m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

char* StadiumCalculatorPlayer::infoToString() {
	
	std::string info = cInfoToString();

	char* ret = allocType<char*>(info.length() + 1);

	copyStrToSzString(info, ret);

	return ret;
}

char* StadiumCalculatorPlayer::toString() {
	
	std::string s = cToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::lastResultToString() {
	
	std::string s = cLastResultToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::resolutionToString() {
	
	std::string s = cResolutionToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::autoFitToString() {
	
	std::string s = cAutoFitToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::desvioFavoritoToString() {
	
	std::string s = cDesvioFavoritoToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listDesvioFavoritoToString() {
	
	std::string s = cListDesvioFavoritoToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listCalculatorToString() {
	
	std::string s = cListCalculatorToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotToString() {
	
	std::string s = cListShotToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotBackSpinToString() {
	
	std::string s = cListShotBackSpinToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotDunkToString() {
	
	std::string s = cListShotDunkToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotTomahawkToString() {
	
	std::string s = cListShotTomahawkToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotSpikeToString() {
	
	std::string s = cListShotSpikeToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotCobraToString() {
	
	std::string s = cListShotCobraToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::listShotOnlyPangyaToString() {
	
	std::string s = cListShotOnlyPangyaToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::shotToString() {
	
	std::string s = cShotToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

char* StadiumCalculatorPlayer::inputToString() {
	
	std::string s = cInputToString();

	char* ret = allocType<char*>(s.length() + 1);

	copyStrToSzString(s, ret);

	return ret;
}

const char* StadiumCalculatorPlayer::getShotName(uint32_t _index) {

	if (m_calculator.m_id == ~0u)
		return "NOT HAVE A CALCULATOR OPNNED";

	if (m_calculator.m_shot.empty() || std::count_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [](stShot& _el) {
		return !_el.m_club.empty();
	}) == 0u)
		return "NOT HAVE SHOTS";

	auto it_shot = std::find_if(m_calculator.m_shot.begin(), m_calculator.m_shot.end(), [_index](stShot& _el_shot) {

		if (_el_shot.m_club.empty())
			return false;

		auto it_club = std::find_if(_el_shot.m_club.begin(), _el_shot.m_club.end(), [_index](stClub& _el_club) {
			return _el_club.m_index == _index;
		});

		return it_club != _el_shot.m_club.end();
	});

	if (it_shot == m_calculator.m_shot.end())
		return "UNKNOWN";

	m_shot_name = it_shot->m_name + " ";

	auto it_club = std::find_if(it_shot->m_club.begin(), it_shot->m_club.end(), [_index](stClub& _el_club) {
		return _el_club.m_index == _index;
	});

	if (it_club == it_shot->m_club.end())
		return "UNKNOWN";

	m_shot_name += it_club->m_name;

	return m_shot_name.c_str();
}

void StadiumCalculatorPlayer::freeResource(void* _resource) {

	if (_resource != nullptr)
		free(_resource);
}

void StadiumCalculatorPlayer::ping(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> pckt(ePACKET::COMMAND_STADIUM_PING);
		pckt << m_id;

		time_t now = 0u;
		std::time(&now);

		pckt << (uint32_t)now;

		sPool::getInstance().sendPacket(pckt);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::info(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cInfoToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::myInfo(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		std::string reply = "";

		if (v_args.size() > 0) {

			auto option = toLowerCase(v_args[0]);

			if (option.compare("shot") == 0)
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
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::list(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		if (v_args[0].compare("dfav") == 0)
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListDesvioFavoritoToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
		else if (v_args[0].compare("shot") == 0) {

			if (v_args.size() > 1) {

				if (v_args[1].compare("bs") == 0)
					sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotBackSpinToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
				else if (v_args[1].compare("dunk") == 0)
					sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotDunkToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
				else if (v_args[1].compare("toma") == 0)
					sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotTomahawkToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
				else if (v_args[1].compare("spike") == 0)
					sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotSpikeToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
				else if (v_args[1].compare("cobra") == 0)
					sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotCobraToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);
				else if (v_args[1].compare("pangya") == 0)
					sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotOnlyPangyaToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

			}else
				sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cListShotToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

		}else if (v_args[0].compare("calc") == 0) {
			
			Packet<ePACKET> pckt(ePACKET::COMMAND_STADIUM_LIST_CALC);

			pckt << m_id;

			sPool::getInstance().sendPacket(pckt);

		}else if (v_args[0].compare("macro") == 0) {

			Packet<ePACKET> pckt(ePACKET::COMMAND_STADIUM_LIST_MACRO);

			pckt << m_id;

			sPool::getInstance().sendPacket(pckt);
		}

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::calcule(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (m_calculator.m_id == ~0u || m_calculator.m_name.empty()) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "Not have a calculator opened", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		if (!isValidShot(m_shot_index)) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "Shot index invalid", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		Packet<ePACKET> p(ePACKET::COMMAND_STADIUM_CALC);

		p << m_id;
		
		p << m_calculator.m_name;

		p << m_shot_index;
		p << m_distance;
		p << m_height;
		p << m_wind;
		p << m_degree;
		p << m_ground;
		p << m_slope_break;
		p << m_green_slope;
		
		p << getShotPower(m_shot_index);

		sPool::getInstance().sendPacket(p);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::expression(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> pckt(ePACKET::COMMAND_STADIUM_EXPRESSION);

		pckt << m_id;

		pckt << (join(v_args, " "));

		sPool::getInstance().sendPacket(pckt);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::macro(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		Packet<ePACKET> p(ePACKET::COMMAND_STADIUM_MACRO);

		p << m_id;
		p << m_distance;
		p << m_height;
		p << m_wind;
		p << m_degree;
		p << m_ground;
		p << m_slope_break;
		p << (float)0.f; // Green Slope

		p << getShotPower(m_shot_index);

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

void StadiumCalculatorPlayer::last_result(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, cLastResultToString(), eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::resolution(const char* _args) {
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

void StadiumCalculatorPlayer::desvio_favorito(const char* _args) {
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

void StadiumCalculatorPlayer::auto_fit(const char* _args) {
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

void StadiumCalculatorPlayer::mycella_degree(const char* _args) {
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

			m_degree = degree_shot;
			m_slope_break = slope_real / getSlopeByResolution(m_resolution, m_auto_fit);
		}

		std::string reply = "Slope Real: " + toFixed(m_slope_break, 2);

		sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, reply, eTYPE_RESPONSE::PLAYER_RESPONSE);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::open(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		CHECK_VALUE_IS_NUMBER(v_args[0]);

		if (m_calculator_list.empty()) {
			
			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "Calculator list empty, execute command \"list calc\" to load calculators list.", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		uint32_t index = (uint32_t)(float)atof(v_args[0].c_str());

		if (index >= m_calculator_list.size()) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "Index invalid", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		Packet<ePACKET> pckt(ePACKET::COMMAND_STADIUM_OPEN_CALC);

		pckt << m_id;
		pckt << m_calculator_list[index];

		sPool::getInstance().sendPacket(pckt);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::shot(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		if (v_args.empty())
			return;

		CHECK_VALUE_IS_NUMBER(v_args[0]);

		unsigned char index = (unsigned char)(float)std::atof(v_args[0].c_str());

		if (!isValidShot(index)) {

			sLogger::getInstance().Response(m_id, eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM, "Invalid Shot index", eTYPE_RESPONSE::PLAYER_RESPONSE);

			return;
		}

		m_shot_index = index;

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::distance(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_distance);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::height(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_height);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::wind(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_wind);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::degree(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_degree);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::ground(const char* _args) {
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
	 
void StadiumCalculatorPlayer::slope_break(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_slope_break);

		// Update Qbra pela resolu��o e auto fit
		m_slope_break /= getSlopeByResolution(m_resolution, m_auto_fit);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}	 
	 
void StadiumCalculatorPlayer::green_slope(const char* _args) {
	INIT_ARG_COMMAND(_args);

	try {

		CHECK_INPUT_VALUE;

		SET_VALUE_INPUT_FROM_STRING(m_green_slope);

	}catch (exception& e) {
		
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

void StadiumCalculatorPlayer::translatePacket(void* _packet) {

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
