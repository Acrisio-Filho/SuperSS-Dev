// Arquivo cube_coin_system.cpp
// Criado em 01/09/2018 as 18:00 por Acrisio
// Implementa��o da classe CubeCoinSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cube_coin_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../UTIL/lottery.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../TYPE/game_type.hpp"

#include "../UTIL/map.hpp"

#include "../PANGYA_DB/cmd_coin_cube_info.hpp"
#include "../PANGYA_DB/cmd_coin_cube_location_info.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

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
								_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

#if defined(_WIN32)
#define TRY_CHECK_CTX			 try { \
									EnterCriticalSection(&m_cs);
#elif defined(__linux__)
#define TRY_CHECK_CTX			 try { \
									pthread_mutex_lock(&m_cs);
#endif

#if defined(_WIN32)
#define LEAVE_CHECK_CTX				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
#define LEAVE_CHECK_CTX				pthread_mutex_unlock(&m_cs);
#endif

#if defined(_WIN32)
#define CATCH_CHECK_CTX(_method) }catch (exception& e) { \
									LeaveCriticalSection(&m_cs); \
									\
									_smp::message_pool::getInstance().push(new message("[CourseCtx::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK_CTX(_method) }catch (exception& e) { \
									pthread_mutex_unlock(&m_cs); \
									\
									_smp::message_pool::getInstance().push(new message("[CourseCtx::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK_CTX			} \

using namespace stdA;

//std::map< uint32_t, CubeCoinSystem::CourseCtx > CubeCoinSystem::m_course;
//bool CubeCoinSystem::m_load = false;

CubeCoinSystem::CubeCoinSystem() : m_load(false), m_course{} {

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

CubeCoinSystem::~CubeCoinSystem() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void CubeCoinSystem::initialize() {

	TRY_CHECK;

	CmdCoinCubeInfo cmd_cci(true); // Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_cci, nullptr, nullptr);

	cmd_cci.waitEvent();

	if (cmd_cci.getException().getCodeError() != 0u)
		throw cmd_cci.getException();

	for (auto& el : sIff::getInstance().getCourse()) {

		auto it_course_info = cmd_cci.getInfo().find((unsigned char)sIff::getInstance().getItemIdentify(el.second._typeid));

		m_course.insert(std::make_pair(el.second._typeid, CubeCoinSystem::CourseCtx(el.second._typeid, 
			it_course_info != cmd_cci.getInfo().end() 
			? it_course_info->second 
			: false)));
	}

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::initialize][Log] Cube Coin System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::initialize][Log] Cube Coin System carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif // _DEBUG

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void CubeCoinSystem::clear() {

	TRY_CHECK;

	if (!m_course.empty())
		m_course.clear();

	m_load = false;

	LEAVE_CHECK;
	CATCH_CHECK("clear");
	END_CHECK;
}

void CubeCoinSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool CubeCoinSystem::isLoad() {

	bool isLoad = false;

	TRY_CHECK;

	isLoad = (m_load && !m_course.empty());

	LEAVE_CHECK;
	CATCH_CHECK("isLoad");
	END_CHECK;

	return isLoad;
}

CubeCoinSystem::CourseCtx* CubeCoinSystem::findCourse(uint32_t _course_typeid) {
	
	if (_course_typeid == 0)
		throw exception("[CubeCoinSystem::findCourse][Error] _course_typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CUBE_COIN_SYSTEM, 1, 0));

	TRY_CHECK;

	auto it = m_course.find(_course_typeid);

	if (it != m_course.end()) {

		LEAVE_CHECK;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findCourse");
	END_CHECK;

	return nullptr;
}

CubeCoinSystem::CoinCubeInHole CubeCoinSystem::getAllCoinCubeInHoleWizCity(unsigned char _number_hole) {

	CoinCubeInHole cbih{ 0u };
	
	switch (_number_hole) {
		case 3:
		case 12:
			cbih.m_all_cube = 5u;
			cbih.m_all_coin_and_cube = 60u;
			break;
		case 14:
			cbih.m_all_cube = 2u;
			cbih.m_all_coin_and_cube = 48u;
			break;
		case 18:
			cbih.m_all_cube = 3u;
			cbih.m_all_coin_and_cube = 33u;
			break;
		default:
			cbih.m_all_cube = 0u;
			cbih.m_all_coin_and_cube = 20u; // Coin green edge
			break;
	}

	return cbih;
}

CubeCoinSystem::CoinCubeInHole CubeCoinSystem::getAllCoinCubeInHole(unsigned char _course_id, unsigned char _number_hole) {
		
	CoinCubeInHole cbih{ 0u };

	if ((char)_number_hole < 0 || _number_hole > 18u) {

		_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::CoinCubeInHole][WARNING] invalid number hole(" 
				+ std::to_string((unsigned short)_number_hole) + ")", CL_FILE_LOG_AND_CONSOLE));

		return cbih;
	}

	auto course = sMap::getInstance().getMap(_course_id);

	if (course == nullptr) {
		
		_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::CoinCubeInHole][WARNING] Course[=" + std::to_string(_course_id) 
				+ "] nao foi encontrado no singleton sMap.", CL_FILE_LOG_AND_CONSOLE));
		
		return cbih;
	}

	switch (course->range_score.par[_number_hole - 1]) {
	case 3:
		cbih.m_all_cube = 1u;
		cbih.m_all_coin_and_cube = 1u;
		break;
	case 4:
		cbih.m_all_cube = 1u;
		cbih.m_all_coin_and_cube = 5u;
		break;
	case 5:
		cbih.m_all_cube = 2u;
		cbih.m_all_coin_and_cube = 8u;
		break;
	}

	return cbih;
}

// Class CourseCtx
CubeCoinSystem::CourseCtx::CourseCtx(uint32_t _typeid, bool _active) : m_typeid(_typeid), m_active(_active), m_hole() {
	
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
	
	initialize();
}

CubeCoinSystem::CourseCtx::~CourseCtx() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void CubeCoinSystem::CourseCtx::initialize() {

	TRY_CHECK_CTX;

	auto course_id = sIff::getInstance().getItemIdentify(m_typeid);
	
	CubeCoinSystem::CoinCubeInHole cbih{ 0u };

	CmdCoinCubeLocationInfo cmd_ccli((unsigned char)course_id, true); // Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_ccli, nullptr, nullptr);

	cmd_ccli.waitEvent();

	if (cmd_ccli.getException().getCodeError() != 0u)
		throw cmd_ccli.getException();

	std::vector< CubeEx > v_cubes;

	// Carrega os Cube que tem no hole
	for (unsigned char i = 1u; i <= 18u; ++i) {

		if (course_id == RoomInfo::eCOURSE::WIZ_CITY)
			cbih = CubeCoinSystem::getAllCoinCubeInHoleWizCity(i);
		else
			cbih = CubeCoinSystem::getAllCoinCubeInHole((unsigned char)course_id, i);

		auto ret = m_hole.insert(std::make_pair(i, Hole(i, cbih.m_all_cube, cbih.m_all_coin_and_cube)));

		if (!ret.second && ret.first == m_hole.end()) {

			// Log
			_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::CourseCtx::initialize][WARNING] nao conseguiu adicionar o Hole[UID=" + std::to_string((unsigned short)i) 
					+ "] do Course[ID=" + std::to_string(course_id) + "] no map<>.", CL_FILE_LOG_AND_CONSOLE));

			continue;
		}

		// Pega todos as coin e cube do hole do course
		v_cubes = cmd_ccli.getAllCoinCubeHole(i);

		if (!v_cubes.empty())
			m_hole.at(i).v_cube.insert(m_hole.at(i).v_cube.end(), v_cubes.begin(), v_cubes.end());

#ifdef _DEBUG
		// Log de que gera as coordenadas para inserir no banco de dados
		/*for (auto itt = m_hole.at(i).v_cube.begin(); itt != m_hole.at(i).v_cube.end(); ++itt)
			_smp::message_pool::getInstance().push(new message("[CubeCoinSystem::CourseCtx::initialize][Log] INSERT INTO pangya.pangya_coin_cube([COURSE], [HOLE], [TIPO], [TIPO_LOCATION], [X], [Y], [Z]) VALUES("
					+ std::to_string(course_id) + ", " + std::to_string((unsigned short)i) + ", " + std::to_string(itt->tipo) + ", " + std::to_string(itt->flag_location) + ", "
					+ std::to_string(itt->location.x) + ", " + std::to_string(itt->location.y) + ", " + std::to_string(itt->location.z) + ")", CL_ONLY_FILE_LOG_TEST));*/
#endif // _DEBUG
	}

	LEAVE_CHECK_CTX;

	CATCH_CHECK_CTX("initialize");

	// Relan�a para o server ou o CubeCoinSystem tomar as provid�ncias
	throw;

	END_CHECK_CTX;
}

void CubeCoinSystem::CourseCtx::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_hole.empty())
		m_hole.clear();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

bool CubeCoinSystem::CourseCtx::isActived() {
	
	bool isActive = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif
	
	isActive = m_active;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isActive;
}

CubeCoinSystem::CourseCtx::Hole* CubeCoinSystem::CourseCtx::findHole(unsigned char _hole) {

	if (_hole == 0)
		throw exception("[CubeCoinSystem::CourseCtx::findHole][Error] _hole is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CUBE_COIN_SYSTEM, 2, 0));

	TRY_CHECK_CTX;

	auto it = m_hole.find(_hole);

	if (it != m_hole.end()) {

		LEAVE_CHECK_CTX;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK_CTX("findHole");
	END_CHECK_CTX;

	return nullptr;
}

std::vector< CubeEx > CubeCoinSystem::CourseCtx::Hole::getAllCoinCubeWizCity(bool _cube) {

	if (v_cube.empty())
		return v_cube;
	
	std::vector< CubeEx > cpy = v_cube, ret;

	std::shuffle(cpy.begin(), cpy.end(), std::independent_bits_engine< std::mt19937_64, 64, std::uint_fast64_t >(std::chrono::system_clock::now().time_since_epoch().count()));

	Lottery lottery((uint64_t)this);

	// Initialize the Roleta
	for (auto it = cpy.begin(); it != cpy.end(); ++it)
		if (it->tipo == Cube::COIN && it->flag_location == Cube::CARPET)
			lottery.push(100 * it->rate, (size_t)&(*it));

	// All coin edge green
	for (auto it = cpy.begin(); it != cpy.end(); ++it)
		if (it->flag_location == Cube::EDGE_GREEN)
			ret.push_back(*it);

	if (lottery.getCountItem() > 0u) {

		Lottery::LotteryCtx *ctx = nullptr;

		// Cube
		if (_cube) {

			uint32_t count_cube = number_of_cube;

			count_cube = lottery.getCountItem() < count_cube ? lottery.getCountItem() : count_cube;

			CubeEx *dice = nullptr;

			while (count_cube-- > 0u) {

				if ((ctx = lottery.spinRoleta(true)) != nullptr && (dice = (CubeEx*)ctx->value) != nullptr)
					ret.push_back(CubeEx(dice->id, Cube::eTYPE::CUBE, 0, dice->flag_location, dice->location.x, dice->location.y, dice->location.z, dice->rate));
			}
		}

		uint32_t rest_count = max_coin_and_cube - (uint32_t)ret.size();

		rest_count = lottery.getCountItem() < rest_count ? lottery.getCountItem() : rest_count;

		while (rest_count-- > 0u) {

			if ((ctx = lottery.spinRoleta(true)) != nullptr && ctx->value != 0u)
				ret.push_back(*(CubeEx*)ctx->value);
		}
	}

	return ret;
}

std::vector< CubeEx > CubeCoinSystem::CourseCtx::Hole::getAllCoinCube(bool _cube) {
	
	if (v_cube.empty())
		return v_cube;

	std::vector< CubeEx > cpy = v_cube, ret;

	std::shuffle(cpy.begin(), cpy.end(), std::independent_bits_engine< std::mt19937_64, 64, std::uint_fast64_t >(std::chrono::system_clock::now().time_since_epoch().count()));

	Lottery lottery((uint64_t)this);

	Lottery::LotteryCtx *ctx = nullptr;

	// All Cube Air
	if (_cube) {

		// Initialize the Roleta CUBE
		for (auto it = cpy.begin(); it != cpy.end(); ++it)
			if (it->tipo == Cube::CUBE && it->flag_location == Cube::AIR)
				lottery.push(100 * it->rate, (size_t)&(*it));

		if (lottery.getCountItem() > 0u) {

			uint32_t count_cube = number_of_cube;

			count_cube = lottery.getCountItem() < count_cube ? lottery.getCountItem() : count_cube;

			CubeEx *dice = nullptr;

			while (count_cube-- > 0u) {

				if ((ctx = lottery.spinRoleta(true)) != nullptr && (dice = (CubeEx*)ctx->value) != nullptr)
					ret.push_back(*(CubeEx*)ctx->value);
			}
		}
	}

	// Clear
	lottery.clear();

	// Initialize the Roleta COIN
	for (auto it = cpy.begin(); it != cpy.end(); ++it)
		if (it->tipo == Cube::COIN && it->flag_location == Cube::GROUND)
			lottery.push(100 * it->rate, (size_t)&(*it));

	if (lottery.getCountItem() > 0u) {

		uint32_t rest_count = max_coin_and_cube - (uint32_t)ret.size();

		rest_count = lottery.getCountItem() < rest_count ? lottery.getCountItem() : rest_count;

		while (rest_count-- > 0u) {

			if ((ctx = lottery.spinRoleta(true)) != nullptr && ctx->value != 0u)
				ret.push_back(*(CubeEx*)ctx->value);
		}
	}

	return ret;
}
