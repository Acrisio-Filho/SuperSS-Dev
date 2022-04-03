// Arquivo coin_cube_location_update_system.cpp
// Criado em 16/10/2020 as 20:46 por Acrisio
// Implementa��o da classe CoinCubeLocationUpdateSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "coin_cube_location_update_system.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/THREAD POOL/threadpool_base.hpp"

#include "../PANGYA_DB/cmd_coin_cube_location_info.hpp"
#include "../PANGYA_DB/cmd_update_coin_cube_location.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "cube_coin_system.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../UTIL/quad_tree3d.hpp"

#include "../UTIL/map.hpp"

#include <algorithm>

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

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
								_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

#define _VERSION_PACKET_CALCULE 0x12F // 0x12 � o pacote que da para fazer o calculo em todos os modos, 0x12F s� pode fazer o calculo no tournament(base)

#define RAIO_CUBE_COLISION 5 * SCALE_PANGYA // 5y

#define ALTURA_MIN_TO_CUBE_SPAWN 60.f // 60 (SCALE_PANGYA)

#define LIMIT_LOCATION_COIN_CUBE_PER_HOLE_PAR_3 30ul
#define LIMIT_LOCATION_COIN_CUBE_PER_HOLE_PAR_4 100ul
#define LIMIT_LOCATION_COIN_CUBE_PER_HOLE_PAR_5 150ul

using namespace stdA;

CoinCubeLocationUpdateSystem::CoinCubeLocationUpdateSystem() 
	: m_pedidos(), m_course_coin_cube(), m_course_coin_cube_current(), m_thread(nullptr), m_update_location_time(0u) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

#if defined(_WIN32)
	InterlockedExchange(&m_continue_translate, 1u);
#elif defined(__linux__)
	__atomic_store_n(&m_continue_translate, 1u, __ATOMIC_RELAXED);
#endif

	m_thread = new thread(TT_COIN_CUBE_LOCATION_TRANSLATE, CoinCubeLocationUpdateSystem::_translateOrder, (LPVOID)this);

	// Inicializa
	initialize();
}

CoinCubeLocationUpdateSystem::~CoinCubeLocationUpdateSystem() {

	clear();

	try {

#if defined(_WIN32)
		InterlockedExchange(&m_continue_translate, 0u);
#elif defined(__linux__)
		__atomic_store_n(&m_continue_translate, 0u, __ATOMIC_RELAXED);
#endif

		if (m_thread != nullptr) {

			m_thread->waitThreadFinish(INFINITE);

			delete m_thread;

			m_thread = nullptr;
		}
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::~CoinCubeLocationUpdateSystem][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

#if defined(_WIN32)
DWORD CoinCubeLocationUpdateSystem::_translateOrder(LPVOID lpParameter) {
#elif defined(__linux__)
void* CoinCubeLocationUpdateSystem::_translateOrder(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(CoinCubeLocationUpdateSystem);

	result = pTP->translateOrder();

	END_THREAD_SETUP("translateOrder()");
}

#if defined(_WIN32)
DWORD CoinCubeLocationUpdateSystem::translateOrder() {
#elif defined(__linux__)
void* CoinCubeLocationUpdateSystem::translateOrder() {
#endif
	
	try {

		CalculeCoinCubeUpdateOrder pedido;

		_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::translateOrder][Log] translateOrder iniciado com sucesso!"));
		
#if defined(_WIN32)
		while (InterlockedCompareExchange(&m_continue_translate, 1, 1)) {
#elif defined(__linux__)
		uint32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_continue_translate, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
			
			try {

				// Verifica se j� est� na hora de atualizar os spawn location da coin e do cube
				if (m_update_location_time == 0u)
					std::time(&m_update_location_time);
				else if ((m_update_location_time + (UPDATE_TIME_INTERVALE_HOUR * 60 * 60)) < std::time(nullptr))
					update_spwan_location();

				// Verifica se tem pedido para calcular tacada
				pedido = m_pedidos.getFirst(5 * 1000); // Espera 5 segundos por um pedido

				switch (pedido.type) {
				case CalculeCoinCubeUpdateOrder::eTYPE::COIN:
					// Calcule coin location
					calculeShotCoin(pedido);
					break;
				case CalculeCoinCubeUpdateOrder::eTYPE::CUBE:
					// Calcule cube location
					calculeShotCube(pedido);
					break;
				}

			}catch (exception& e) {

				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::LIST_ASYNC)
					throw;

				if (STDA_ERROR_DECODE(e.getCodeError()) != 2/*Time out*/)
					_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::translateOrder][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::translateOrder][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[CoinCubeLocationUpdateSystem::translateOrder][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "[CoinCubeLocationUpdateSystem::monitor][ErrorSystem] translateOrder() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de translateOrder()..."));
	
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
}

void CoinCubeLocationUpdateSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool CoinCubeLocationUpdateSystem::isLoad() {
	
	bool isLoad = false;

	TRY_CHECK;

	isLoad = (m_load);

	LEAVE_CHECK;
	CATCH_CHECK("isLoad");
	END_CHECK;

	return isLoad;
}

void CoinCubeLocationUpdateSystem::pushOrderToCalcule(CalculeCoinCubeUpdateOrder _cccuo) {

	m_pedidos.push(_cccuo);
}

#ifdef _DEBUG
void CoinCubeLocationUpdateSystem::forceUpdate() {
	update_spwan_location();
}
#endif // _DEBUG

void CoinCubeLocationUpdateSystem::initialize() {

	TRY_CHECK;

	CmdCoinCubeLocationInfo cmd_ccli(true); // Waiter

	unsigned char course_id = 0u;

	for (auto& el : sIff::getInstance().getCourse()) {

		course_id = (unsigned char)sIff::getInstance().getItemIdentify(el.second._typeid);

		cmd_ccli.setCourse(course_id);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_ccli, nullptr, nullptr);

		cmd_ccli.waitEvent();

		if (cmd_ccli.getException().getCodeError() != 0)
			throw cmd_ccli.getException();

		auto it = m_course_coin_cube_current.find(course_id);

		if (it == m_course_coin_cube_current.end())
			m_course_coin_cube_current.insert({ course_id, cmd_ccli.getInfo() });
		else {

			// Log
			_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::initialize][Log] ja tem o MAP_HOLE_COIN_CUBE no map<course> de course[ID=" 
					+ std::to_string((unsigned short)course_id) + "], substituindo.", CL_FILE_LOG_AND_CONSOLE));

			m_course_coin_cube_current[course_id] = cmd_ccli.getInfo();
		}
	}

	// Initialize time to update
	std::time(&m_update_location_time);

	// Log
	_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::initialize][Log] Cube Coin Location Update System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void CoinCubeLocationUpdateSystem::clear() {
	
	TRY_CHECK;

	if (!m_pedidos.empty())
		m_pedidos.clear();

	if (!m_course_coin_cube.empty())
		m_course_coin_cube.clear();

	if (!m_course_coin_cube_current.empty())
		m_course_coin_cube_current.clear();

	m_update_location_time = 0u;

	m_load = false;

	LEAVE_CHECK;
	CATCH_CHECK("clear");
	END_CHECK;
}

void CoinCubeLocationUpdateSystem::calculeShotCube(CalculeCoinCubeUpdateOrder& _cccuo) {

	// Wiz City n�o calcula
	if ((_cccuo.course & 0x7Fu) != RoomInfo::eCOURSE::WIZ_CITY) {

#if _VERSION_PACKET_CALCULE == 0x12F

		QuadTree3D qt;
		Ball3D ball;

		Vector3D wind(_cccuo.shot_data_for_cube.wind_influence.x, _cccuo.shot_data_for_cube.wind_influence.y, _cccuo.shot_data_for_cube.wind_influence.z);

		auto distance = (float)_cccuo.pin.diffXZ(*(Location*)&_cccuo.shot_data_for_cube.location) * DIVIDE_SCALE_PANGYA;

		uint32_t club_index = _cccuo.shot_data_for_cube.taco;

		if (club_index >= 3 && club_index < 13)
			club_index -= 2;
		else if (club_index == 13)
			club_index = 12;
		else if (club_index == 14)
			club_index = 11;
		else if (club_index > 14)
			club_index -= 2;

		Club3D club(sAllClubInfo3D::getInstance().m_clubs[club_index], calculeTypeDistance(distance));

		if (club.m_club_info.m_type == eCLUB_TYPE::PT)
			return;

		// Pelo pacote12F funciona s� em tournament base
		qt.init_shot(&ball, &club, &wind, _cccuo.shot_data_for_cube, distance);

		do {

			qt.ballProcess(STEP_TIME); // S� vai at� o max height

		} while (ball.m_num_max_height < 0 && ball.m_count < 5000);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::calculeShotCube][Log] Power(" + std::to_string(_cccuo.shot_data_for_cube.power_club)
				+ "y) Cube Location[X=" + std::to_string(ball.m_position.m_x) + ", Y=" + std::to_string(ball.m_position.m_y) + ", Z=" + std::to_string(ball.m_position.m_z) + "]", CL_FILE_LOG_TEST_AND_CONSOLE));
#endif // _DEBUG

#elif _VERSION_PACKET_CALCULE == 0x12

		QuadTree3D qt;
		Ball3D ball;

		auto distance = (float)_last_position.diffXZ(_pin) * DIVIDE_SCALE_PANGYA;

		Club3D club(sAllClubInfo3D::getInstance().m_clubs[_pgi.shot_data.club], calculeTypeDistance(distance));

		// N�o Calcula Putt
		if (club.m_club_info.m_type == eCLUB_TYPE::PT)
			return;

		auto deg_360 = (uint32_t)(_pgi.degree / 255.f * 360.f);

		Vector3D wind(_wind * (float)std::sin(deg_360 * PI / 180.f) * -1.f, 0.f, _wind * (float)std::cos(deg_360 * PI / 180.f));

		auto extra_power = _pi.getExtraPower();

		options3D options(
			_pgi.shot_data.special_shot,
			Vector3D(_last_position.x, _last_position.y, _last_position.z),
			extra_power,
			(_pgi.shot_sync.state_shot.shot.stShot.power_shot ? ePOWER_SHOT_FACTORY::ONE_POWER_SHOT
				: (_pgi.shot_sync.state_shot.shot.stShot.double_power_shot ? ePOWER_SHOT_FACTORY::TWO_POWER_SHOT : ePOWER_SHOT_FACTORY::NO_POWER_SHOT)),
			distance,
			(float)_pi.getSlotPower(),
			(_pgi.shot_data.bar_point[0] - 360.f) / 140.f,
			_pgi.shot_data.ball_effect[1],
			_pgi.shot_data.ball_effect[0],
			_pgi.shot_data.mira);

		// Pelo pacote 12, funciona em todos os modos
		qt.init_shot(&ball, &club, &wind, options);

		do {

			qt.ballProcess(STEP_TIME); // S� vai at� o max height

		} while (ball.m_num_max_height < 0 && ball.m_count < 5000);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::calculeShotCube][Log] Power(" + club.getRange(options.m_extra_power, options.m_power_slot, options.m_power_shot))
				+ "y) Cube Location[X=" + std::to_string(ball.m_position.m_x) + ", Y=" + std::to_string(ball.m_position.m_y) + ", Z=" + std::to_string(ball.m_position.m_z) + "]", CL_FILE_LOG_TEST_AND_CONSOLE));
#endif // _DEBUG

#endif // _VERSION_PACKET_CALCULE

		// Verifica se ele atingiu uma altura boa para o spinning cube spawnar
		if ((ball.m_position.m_y - _cccuo.last_location.y) < ALTURA_MIN_TO_CUBE_SPAWN) // Altura menor que 70 n�o coloca o spinning cube para os spawns poss�veis
			return; // Sai

		// Verifica se o cube pode ser um poss�vel spawn e se sim adiciona ele para a lista<map>
		checkAndAddCoinCube(_cccuo.course & 0x7Fu, _cccuo.hole, CubeEx(
			0u,
			Cube::eTYPE::CUBE,
			0u,
			Cube::eFLAG_LOCATION::AIR,
			ball.m_position.m_x,
			ball.m_position.m_y,
			ball.m_position.m_z,
			1ul
		));
	}

}

void CoinCubeLocationUpdateSystem::calculeShotCoin(CalculeCoinCubeUpdateOrder& _cccuo) {
	
	// Verifica se a coin pode ser um poss�vel spawn e se sim adiciona ela para a lista<map>
	checkAndAddCoinCube(_cccuo.course & 0x7Fu, _cccuo.hole, CubeEx(
		0u,
		Cube::eTYPE::COIN,
		0u,
		Cube::eFLAG_LOCATION::GROUND,
		_cccuo.last_location.x,
		_cccuo.last_location.y,
		_cccuo.last_location.z,
		1ul
	));
}

void CoinCubeLocationUpdateSystem::checkAndAddCoinCube(unsigned char _course_id, unsigned char _hole_number, CubeEx _cube) {

	TRY_CHECK;

	auto it_course = m_course_coin_cube.find(_course_id & 0x7Fu);

	if (it_course != m_course_coin_cube.end()) {

		auto it_hole = it_course->second.find(_hole_number);

		if (it_hole != it_course->second.end()) {

			// Verifica se o cube est� no raio de outro cube se n�o add o cube ou aumenta o rate do outro cube
			Location lc{ _cube.location.x, _cube.location.y, _cube.location.z };

			auto it = std::find_if(it_hole->second.begin(), it_hole->second.end(), [&](auto& _el) {
				return _el.tipo == _cube.tipo && std::abs(lc.diff(*(Location*)&_el.location)) <= RAIO_CUBE_COLISION;
			});

			if (it != it_hole->second.end())
				it->rate++;
			else
				it_hole->second.push_back(_cube);

		}else {

			auto ret = it_course->second.insert(std::make_pair(_hole_number, std::vector< CubeEx > { _cube }));

			if (!ret.second && ret.first == it_course->second.end())
				_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::checkAndAddCoinCube][WARNING] nao conseguiu adicionar o map de hole com o Cube/Coin[TYPE=" 
						+ std::to_string(_cube.tipo) + ", X=" + std::to_string(_cube.location.x) + ", Y=" + std::to_string(_cube.location.y) 
						+ ", Z=" + std::to_string(_cube.location.z) + "] no Course[ID=" + std::to_string((unsigned short)_course_id) 
						+ ", HOLE=" + std::to_string((unsigned short)_hole_number) + "]", CL_FILE_LOG_AND_CONSOLE));
		}
	
	}else {

		auto ret = m_course_coin_cube.insert(std::make_pair(
			(unsigned char)(_course_id & 0x7Fu), 
			std::map< unsigned char, std::vector< CubeEx > >{ { _hole_number, std::vector< CubeEx > { _cube } } }
		));

		if (!ret.second && ret.first == m_course_coin_cube.end())
			_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::checkAndAddCoinCube][WARNING] nao conseguiu adicionar o map de course com o Cube/Coin[TYPE="
					+ std::to_string(_cube.tipo) + ", X=" + std::to_string(_cube.location.x) + ", Y=" + std::to_string(_cube.location.y)
					+ ", Z=" + std::to_string(_cube.location.z) + "] no Course[ID=" + std::to_string((unsigned short)_course_id)
					+ ", HOLE=" + std::to_string((unsigned short)_hole_number) + "]", CL_FILE_LOG_AND_CONSOLE));
	}

	LEAVE_CHECK;
	CATCH_CHECK("checkAndAddCoinCube");
	END_CHECK;
}

void CoinCubeLocationUpdateSystem::update_spwan_location() {

	TRY_CHECK;

	// lambda
	auto sort_per_rate = [](const auto& _a, const auto& _b) {
		return _a.rate > _b.rate;
	};

	// CoinCubeUpdate para atualiza no banco de dados
	std::vector< CoinCubeUpdate > v_coin_cube_update;

	Map::stCtx *map_ctx = nullptr;
	Location lc{ 0u };

	uint32_t count_new_add_all = 0u;
	uint32_t count_new_add_hole = 0u;
	uint32_t limit = 0u;

	for (auto& el_course : m_course_coin_cube) {

		// Wiz City n�o atualiza nada
		if ((el_course.first & 0x7Fu) == RoomInfo::eCOURSE::WIZ_CITY)
			continue;

		auto it_c = m_course_coin_cube_current.find(el_course.first & 0x7Fu);

		if (it_c == m_course_coin_cube_current.end())
			continue;

		map_ctx = sMap::getInstance().getMap(el_course.first & 0x7Fu);

		if (map_ctx == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSyatem::update_spwan_location][WARNING] nao encontrou o Course[ID="
					+ std::to_string((unsigned short)el_course.first) + "] no singleton de Course(sMap).", CL_FILE_LOG_AND_CONSOLE));

			// Continua
			continue;
		}

		for (auto& el_hole : el_course.second) {

			if (el_hole.first < 1 && el_hole.first > 18) {

				// Log
				_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::update_spwan_location][WARNING] hole[NUMERO="
						+ std::to_string((unsigned short)el_hole.first) + "] invalido no Course[ID=" + std::to_string((unsigned short)el_course.first) + "]", CL_FILE_LOG_AND_CONSOLE));

				// Continua
				continue;
			}

			// Get limit per hole
			limit = getLimitCoinCubePerParHole((unsigned char)map_ctx->range_score.par[el_hole.first - 1]);

			// [upt] Count all add
			count_new_add_all += count_new_add_hole;

			// reset
			count_new_add_hole = 0u;

			auto it_h = it_c->second.find(el_hole.first);

			// Adiciona os cube/coin que conseguiu esse hole n�o tem nenhum cube/coin
			if (it_h == it_c->second.end()) {

				for (auto& el_cube : el_hole.second) {

					// Adiciona se tiver espa�o
					if (count_new_add_hole < limit) {

						// Add new Cube/Coin Location
						count_new_add_hole++;

						v_coin_cube_update.push_back(CoinCubeUpdate{
							CoinCubeUpdate::eTYPE::INSERT,
							(unsigned char)(el_course.first & 0x7Fu),
							el_hole.first,
							el_cube
							});
					}
				}

				// Continua
				continue;
			}

			// Sort cubes por rate
			std::sort(it_h->second.begin(), it_h->second.end(), sort_per_rate);
			std::sort(el_hole.second.begin(), el_hole.second.end(), sort_per_rate);
			std::sort(v_coin_cube_update.begin(), v_coin_cube_update.end(), [](CoinCubeUpdate& _rhs, CoinCubeUpdate& _lhs) {
				return _rhs.cube.rate < _lhs.cube.rate;
			});

			for (auto& el_cube : el_hole.second) {

				lc.x = el_cube.location.x;
				lc.y = el_cube.location.y;
				lc.z = el_cube.location.z;

				// Atualiza o rate da mesma location - no que j� est�o sendo atualizados
				auto it_upt_cc = std::find_if(v_coin_cube_update.begin(), v_coin_cube_update.end(), [&](CoinCubeUpdate& _el) {
					return (_el.course_id & 0x7Fu) == (el_course.first & 0x7Fu) && _el.hole_number == el_hole.first && _el.cube.tipo == el_cube.tipo
						&& std::abs(lc.diff(*(Location*)&_el.cube.location)) <= RAIO_CUBE_COLISION;
				});

				if (it_upt_cc != v_coin_cube_update.end()) {

					// Update
					it_upt_cc->cube.rate += el_cube.rate;

					// Continua
					continue;
				}

				// Atualiza o rate da mesma location
				auto it_upt = std::find_if(it_h->second.begin(), it_h->second.end(), [&](auto& _el) {
					return _el.tipo == el_cube.tipo && std::abs(lc.diff(*(Location*)&_el.location)) <= RAIO_CUBE_COLISION;
				});

				if (it_upt != it_h->second.end()) {

					// Update
					it_upt->rate += el_cube.rate;

					v_coin_cube_update.push_back(CoinCubeUpdate{
						CoinCubeUpdate::eTYPE::UPDATE,
						(unsigned char)(el_course.first & 0x7Fu),
						el_hole.first,
						*it_upt
						});

					// Continua
					continue;
				}

				// Adiciona se tiver espa�o
				if ((it_h->second.size() + count_new_add_hole) < limit) {

					// Add new Cube/Coin Location
					count_new_add_hole++;

					v_coin_cube_update.push_back(CoinCubeUpdate{
						CoinCubeUpdate::eTYPE::INSERT,
						(unsigned char)(el_course.first & 0x7Fu),
						el_hole.first,
						el_cube
						});

					// Continua
					continue;
				}

				// Verifica se o rate passou do outro que est� nos atualizados e substitui
				it_upt_cc = std::find_if(v_coin_cube_update.begin(), v_coin_cube_update.end(), [&](CoinCubeUpdate& _el) {
					return (_el.course_id & 0x7Fu) == (el_course.first & 0x7Fu) && _el.hole_number == el_hole.first && _el.cube.tipo == el_cube.tipo
						&& _el.cube.rate < el_cube.rate;
				});

				if (it_upt_cc != v_coin_cube_update.end()) {

					if (it_upt_cc->type == CoinCubeUpdate::eTYPE::UPDATE)
						el_cube.id = it_upt_cc->cube.id;

					it_upt_cc->cube = el_cube;

					// Continua;
					continue;
				}

				// Verifica se o rate passou do outro e substitui
				it_upt = std::find_if(it_h->second.begin(), it_h->second.end(), [&](auto& _el) {
					return _el.tipo == el_cube.tipo && _el.rate < el_cube.rate;
				});

				// Update
				if (it_upt != it_h->second.end()) {

					// Atualiza a location do Cube/Coin
					el_cube.id = it_upt->id;

					v_coin_cube_update.push_back(CoinCubeUpdate{
						CoinCubeUpdate::eTYPE::UPDATE,
						(unsigned char)(el_course.first & 0x7Fu),
						el_hole.first,
						el_cube
						});
				}
			}
		}
	}

	// Finish update count all
	if (count_new_add_hole > 0u)
		// [upt] Count all add
		count_new_add_all += count_new_add_hole;

	// reset
	count_new_add_hole = 0u;

	// Adiciona no baco de dados
	if (!v_coin_cube_update.empty()) {
		
		CmdUpdateCoinCubeLocation cmd_uccl(true); // Waiter;
		
		for (auto& el : v_coin_cube_update) {

			cmd_uccl.setInfo(el);

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_uccl, nullptr, nullptr);

			cmd_uccl.waitEvent();

			if (cmd_uccl.getException().getCodeError() != 0)
				_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::update_spawn_location][Error] " + cmd_uccl.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	// Atualizou o Spwan location da coin e do cube
	std::time(&m_update_location_time);

	// Atualiza os current e esse
	load();

	// Atualiza o sistema de cube e coin
	sCubeCoinSystem::getInstance().load();

	// Log
	_smp::message_pool::getInstance().push(new message("[CoinCubeLocationUpdateSystem::update_spawn_location][Log] Adicionou " 
			+ std::to_string(count_new_add_all) + " e Atualizou " + std::to_string(v_coin_cube_update.size() - count_new_add_all) + " spawn(s) location da coin e cube com sucesso.", CL_FILE_LOG_AND_CONSOLE));

	LEAVE_CHECK;
	CATCH_CHECK("update_spawn_location");
	END_CHECK;
}

uint32_t CoinCubeLocationUpdateSystem::getLimitCoinCubePerParHole(unsigned char _par_hole) {

	uint32_t limit = 0u;

	switch (_par_hole) {
	case 3:
		limit = LIMIT_LOCATION_COIN_CUBE_PER_HOLE_PAR_3;
		break;
	case 4:
		limit = LIMIT_LOCATION_COIN_CUBE_PER_HOLE_PAR_4;
		break;
	case 5:
		limit = LIMIT_LOCATION_COIN_CUBE_PER_HOLE_PAR_5;
		break;
	}

	return limit;
}
