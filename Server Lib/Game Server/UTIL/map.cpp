// Arquivo map.cpp
// Criado em 21/09/2018 as 21:00 por Acrisio
// Implementa��o da classe Map

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "map.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"
#include "../TYPE/pangya_game_st.h"

#include "../../Projeto IOCP/UTIL/message_pool.h"

#if defined(_WIN32)
#define TRY_CHECK			 try { \
								EnterCriticalSection(&m_cs);
#elif defined(__linux__)
#define TRY_CHECK			 try { \
								pthread_mutex_lock(&m_cs);
#endif

#if defined(_WIN32)
#define LEAVE_CHECK				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
#define LEAVE_CHECK				pthread_mutex_unlock(&m_cs);
#endif

#if defined(_WIN32)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								LeaveCriticalSection(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[Map::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[Map::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

using namespace stdA;

//bool Map::m_load = false;
//std::map< unsigned char, Map::stCtx > Map::m_map;

Map::Map() : m_load(false), m_map{} {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Inicializa
	initialize();
}

Map::~Map() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void Map::initialize() {

	TRY_CHECK;

	if (!sIff::getInstance().isLoad())
		sIff::getInstance().load();

	auto& course = sIff::getInstance().getCourse();

	stCtx ctx{ 0 };

	for (const auto& el : course) {
		ctx.clear();

		ctx.name = el.second.name;
#if defined(_WIN32)
		memcpy_s(&ctx.range_score, sizeof(ctx.range_score), &el.second.par_score_hole, sizeof(el.second.par_score_hole));
#elif defined(__linux__)
		memcpy(&ctx.range_score, &el.second.par_score_hole, sizeof(el.second.par_score_hole));
#endif
		ctx.star = 1.f + (el.second.star.uStarMask.star_mask.star_num / 10.f);

		switch (el.second._typeid & 0xFF) {
		case RoomInfo::eCOURSE::BLUE_LAGOON:
			ctx.clear_bonus = 20u;
			break;
		case RoomInfo::eCOURSE::BLUE_WATER:
			ctx.clear_bonus = 50u;
			break;
		case RoomInfo::eCOURSE::BLUE_MOON:
			ctx.clear_bonus = 50u;
			break;
		case RoomInfo::eCOURSE::SEPIA_WIND:
			ctx.clear_bonus = 55u;
			break;
		case RoomInfo::eCOURSE::PINK_WIND:
			ctx.clear_bonus = 20u;
			break;
		case RoomInfo::eCOURSE::WIND_HILL:
			ctx.clear_bonus = 80u;
			break;
		case RoomInfo::eCOURSE::WIZ_WIZ:
			ctx.clear_bonus = 65u;
			break;
		case RoomInfo::eCOURSE::WHITE_WIZ:
			ctx.clear_bonus = 55u;
			break;
		case RoomInfo::eCOURSE::WEST_WIZ:
			ctx.clear_bonus = 24u;
			break;
		case RoomInfo::eCOURSE::WIZ_CITY:
			ctx.clear_bonus = 40u;
			break;
		case RoomInfo::eCOURSE::DEEP_INFERNO:
			ctx.clear_bonus = 80u;
			break;
		case RoomInfo::eCOURSE::ICE_SPA:
			ctx.clear_bonus = 20u;
			break;
		case RoomInfo::eCOURSE::ICE_CANNON:
			ctx.clear_bonus = 40u;
			break;
		case RoomInfo::eCOURSE::ICE_INFERNO:
			ctx.clear_bonus = 70u;
			break;
		case RoomInfo::eCOURSE::SILVIA_CANNON:
			ctx.clear_bonus = 70u;
			break;
		case RoomInfo::eCOURSE::SHINNING_SAND:
			ctx.clear_bonus = 40u;
			break;
		case RoomInfo::eCOURSE::EASTERN_VALLEY:
			ctx.clear_bonus = 40u;
			break;
		case RoomInfo::eCOURSE::LOST_SEAWAY:
			ctx.clear_bonus = 20u;
			break;
		case RoomInfo::eCOURSE::GRAND_ZODIAC:
			ctx.clear_bonus = 0u;
			break;
		case RoomInfo::eCOURSE::CHRONICLE_1_CHAOS:
			ctx.clear_bonus = 360u;
			break;
		case RoomInfo::eCOURSE::ABBOT_MINE:
			ctx.clear_bonus = 40u;
			break;
		case RoomInfo::eCOURSE::MYSTIC_RUINS:
			ctx.clear_bonus = 40u;
			break;
		}	// End Switch

		m_map[(unsigned char)(el.second._typeid & 0xFF)] = ctx;
	}

	// Log
//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[Map::initialize][Log] Map Dados Estaticos carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[Map::initialize][Log] Map Dados Estaticos carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif // _DEBUG

	// Map Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void Map::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_map.empty())
		m_map.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

bool Map::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = (m_load && !m_map.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

void Map::load() {

	if (isLoad())
		clear();

	initialize();
}

Map::stCtx* Map::getMap(unsigned char _course) {

	TRY_CHECK;

	auto it = m_map.end();

	// Prote��o contra o Random Map, que usa o negatico do 'char'
	if ((it = m_map.find(_course & 0x7F)) != m_map.end()) {
		
		LEAVE_CHECK;
		
		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("getMap");
	END_CHECK;

	return nullptr;
}

uint32_t Map::calculeClearVS(stCtx& _ctx, uint32_t _num_player, uint32_t _qntd_hole) {
	return _ctx.clear_bonus * _qntd_hole * (_num_player - 1);
}

uint32_t Map::calculeClearMatch(stCtx& _ctx, uint32_t _qntd_hole) {
	return _ctx.clear_bonus * _qntd_hole;
}

uint32_t Map::calculeClear30s(stCtx& _ctx, uint32_t _qntd_hole) {
	
	// Protection Division per zero (0)
	if (_ctx.clear_bonus == 0 || _qntd_hole == 0)
		return 0;

	return (_ctx.clear_bonus * _qntd_hole) / 2;
}

uint32_t Map::calculeClearSSC(stCtx& _ctx) {
	return _ctx.clear_bonus;
}
