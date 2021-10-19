// Arquivo hole.cpp
// Criado em 12/08/2018 as 11:37 por Acrisio
// Implementa��o da classe Hole

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "hole.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include "cube_coin_system.hpp"

#include <algorithm>

using namespace stdA;

Hole::Hole(unsigned char _course, unsigned short _numero, unsigned char _pin, eMODO _modo, unsigned char _hole_repeat, unsigned char _weather, unsigned char _wind, unsigned short _degree, uCubeCoinFlag _cube_coin)
	: m_course(_course & 0x7F), m_numero(_numero), m_pin(_pin), m_modo(_modo), m_hole_repeat(_hole_repeat), m_weather(_weather), m_wind(_wind, _degree), m_cube_coin(_cube_coin), m_par{0}, m_cube(), m_good(false) {

	if (sIff::getInstance().findCourse((iff::COURSE << 26) | (m_course & 0x7F)) == nullptr) {
		_smp::message_pool::getInstance().push(new message("[Hole::Hole][Error] course[" + std::to_string((unsigned short)m_course) + "] desconhecido. Bug", CL_FILE_LOG_AND_CONSOLE));
		
		return;
	}

	if (m_numero < 1 || m_numero > 18) {
		_smp::message_pool::getInstance().push(new message("[Hole::init][Error] numero do hole[" + std::to_string(m_numero) + "] nao esta em um intervalo permitido. Bug", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	init_from_IFF_STRUCT();

	// n�mero aleat�rio, para o id do hole(ACHO)
	float rand_f = (float)((((int)sRandomGen::getInstance().rIbeMt19937_64_chrono()) * 2.f) * sRandomGen::getInstance().rIbeMt19937_64_chrono());
	
	// Gerar n�meros grandes
#if defined(_WIN32)
	memcpy_s(&m_id, 4, &rand_f, 4);
#elif defined(__linux__)
	memcpy(&m_id, &rand_f, 4);
#endif

	// Se estiver ativado, inicializa o Coin Cube do Hole
	if (m_cube_coin.stFlag.enable && (m_cube_coin.stFlag.enable_cube || m_cube_coin.stFlag.enable_coin))
		init_cube_coin();

	m_good = true;
}

Hole::~Hole() {

	if (!m_cube.empty()) {
		m_cube.clear();
		m_cube.shrink_to_fit();
	}

	m_good = false;
}

void Hole::init(stXZLocation& _tee, stXZLocation& _pin) {

	Location tee = { _tee.x, 0.f, _tee.z, 0.f };
	Location pin = { _pin.x, 0.f, _pin.z, 0.f };

	init(tee, pin);
}

void Hole::init(Location& _tee, Location& _pin) {
	
	if (!isGood())
		throw exception("[Hole::init][Error] hole nao esta incializado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::HOLE, 1, 0));

	m_tee_location = _tee;
	m_pin_location = _pin;
}

bool Hole::isGood() {
	return m_good;
}

uint32_t Hole::getId() {
	return m_id;
}

unsigned short Hole::getNumero() {
	return m_numero;
}

unsigned char Hole::getTipo() {
	return m_tipo;
}

stHoleWind& Hole::getWind() {
	return m_wind;
}

stHolePar& Hole::getPar() {
	return m_par;
}

unsigned char Hole::getPin() {
	return m_pin;
}

unsigned char Hole::getWeather() {
	return m_weather;
}

unsigned Hole::getCourse() {
	return m_course;
}

uCubeCoinFlag& Hole::getCubeCoin() {
	return m_cube_coin;
}

Hole::eMODO Hole::getModo() {
	return m_modo;
}

unsigned char Hole::getHoleRepeat() {
	return m_hole_repeat;
}

Location& Hole::getPinLocation() {
	return m_pin_location;
}

Location& Hole::getTeeLocation() {
	return m_tee_location;
}

std::vector< CubeEx >& Hole::getCubes() {
	return m_cube;
}

void Hole::setWeather(unsigned char _weather) {
	m_weather = _weather;
}

void Hole::setWind(unsigned char _wind, unsigned short _degree) {

	m_wind.wind = _wind;
	m_wind.degree.setDegree(_degree);
}

void Hole::setWind(stHoleWind& _wind) {
	m_wind = _wind;
}

CubeEx* Hole::findCubeCoin(uint32_t _id) {
	
	auto it = std::find_if(m_cube.begin(), m_cube.end(), [&](auto& el) {
		return el.id == _id;
	});

	return (it != m_cube.end()) 
#if defined(_WIN32)
		? it._Ptr 
#elif defined(__linux__)
		? &(*it)
#endif 
		: nullptr;
}

void Hole::init_cube_coin() {

	// Cube ativo ou n�o
	bool cube = false;

	// Modo hole repeat, tem que pegar o n�mero certo do hole
	unsigned char numero = (unsigned char)m_numero;

	if (m_modo == M_REPEAT)
		numero = m_hole_repeat;

	// Cube Coin Manager
	if (!sCubeCoinSystem::getInstance().isLoad())
		sCubeCoinSystem::getInstance().load();

	auto course = sCubeCoinSystem::getInstance().findCourse((iff::COURSE << 26) | (m_course & 0x7F));

	if (course == nullptr)
		throw exception("[Hole::init_cube_coin][Error] course\"" + std::to_string((unsigned short)(m_course & 0x7F)) + "\" nao existe no Cube Coin System. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::HOLE, 20, 0));

	// Isso s� desativa os cube, se o course e hole tiver coin � para colocar elas s� n�o o cube se ele estiver desativado
	if (course->isActived()) {

		if (m_course == RoomInfo::eCOURSE::WIZ_CITY)	// Aqui s� tem cube nos holes 3 12 14 18
			cube = (numero == 3 || numero == 12 || numero == 14 || numero == 18) && (m_modo != M_REPEAT || m_numero % 3 == 0); // Modo Hole Repeat s� de 3 em 3 holes que tem cube, mesmo em Wiz City
		else
			cube = m_cube_coin.stFlag.enable_cube == 1u;
	}

	auto hole = course->findHole(numero);

	if (hole == nullptr)
		throw exception("[Hole::init_cube_coin][Error] numero do hole[NUMERO=" + std::to_string(m_numero) + "] is valid. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::HOLE, 21, 0));

	// Wiz City usa a fun��o dela e o resto usa outra fun��o generica
	auto all_coin_cube = (m_course == RoomInfo::eCOURSE::WIZ_CITY) ? hole->getAllCoinCubeWizCity(cube) : hole->getAllCoinCube(cube);
		
	m_cube.insert(m_cube.begin(), all_coin_cube.begin(), all_coin_cube.end());
}

void Hole::init_from_IFF_STRUCT() {

	auto course = sIff::getInstance().findCourse((iff::COURSE << 26) | (m_course & 0x7F));

	if (course == nullptr)
		throw exception("[Hole::init_from_IFF_STRUCT][Error] course[" + std::to_string((unsigned short)m_course & 0x7F) + "] desconhecido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::HOLE, 2, 0));

	auto numero = m_numero;

	if (m_modo == M_REPEAT)
		numero = m_hole_repeat;

	if (numero < 1 || numero > 18)
		throw exception("[Hole::init_from_IFF_STRUCT][Error] numero do hole[" + std::to_string(m_numero) + "] nao esta em um intervalo permitido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::HOLE, 3, 0));

	// !!!!@@@@@@------------===
	// Os Valores do Par dos Holes Mysthic Ruins est�o errados no IFF STRUCT, 
	// eles colocaram os valores do Abbot Mine, tenho que trocar depois isso
	if ((course->_typeid & 0xFF) == RoomInfo::CHRONICLE_1_CHAOS) {
		m_par.par = 4/*Depois eu colocou no iff dele e ajeitar o Mystic Ruins tbm*/;

		m_par.range_score[0] = -2;
		m_par.range_score[1] = 5;

		m_par.total_shot = m_par.par + m_par.range_score[1];
	}else {
		m_par.par = course->par_score_hole.par_hole[numero - 1];

		m_par.range_score[0] = course->par_score_hole.min_score_hole[numero - 1];
		m_par.range_score[1] = course->par_score_hole.max_score_hole[numero - 1];

		m_par.total_shot = m_par.par + m_par.range_score[1];
	}
}
