// Arquivo treasure_hunter_system.cpp
// Criado em 06/06/2018 as 18::22 por Acrisio
// Implementa��o da classe TreasureHunterSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "treasure_hunter_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PANGYA_DB/cmd_treasure_hunter_info.hpp"
#include "../PANGYA_DB/cmd_treasure_hunter_item.hpp"
#include "../PANGYA_DB/cmd_update_treasure_hunter_course_point.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../UTIL/lottery.hpp"

#include "../Game Server/game_server.h"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

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
								_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

using namespace stdA;

/*TreasureHunterInfo TreasureHunterSystem::m_thi[];
std::vector< TreasureHunterItem > TreasureHunterSystem::m_thItem;
bool TreasureHunterSystem::m_load;
std::time_t TreasureHunterSystem::m_time = 0ull;
uint32_t TreasureHunterSystem::m_rate = 100u;*/

#define TREASURE_HUNTER_TIME_UPDATE (30 * 60)		// 30 minutos
#define TREASURE_HUNTER_LIMIT_POINT_COURSE 1000		// 1000 limite de pontos do course
#define TREASURE_HUNTER_INCREASE_POINT	50			// 50 pontos que soma a cada 10 minutos para todos course pontos
#define TREASURE_HUNTER_BOX_PER_POINT	100			// 100 pontos por uma box

TreasureHunterSystem::TreasureHunterSystem() : m_thi{}, m_thItem(), m_load(false), m_time(0ull) {

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

TreasureHunterSystem::~TreasureHunterSystem() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void TreasureHunterSystem::initialize() {

	TRY_CHECK;

	CmdTreasureHunterInfo cmd_thi(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_thi, nullptr, nullptr);

	cmd_thi.waitEvent();

	if (cmd_thi.getException().getCodeError() != 0)
		throw cmd_thi.getException();

	if (cmd_thi.getInfo().empty())
		throw exception("[TreasureHunterSystem::initialize][Error] nao conseguiu pegar os treasure hunter point dos course na data base.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TREASURE_HUNTER_SYSTEM, 1, 0));

	auto& v_thi = cmd_thi.getInfo();

	for (std::vector< TreasureHunterInfo >::iterator i = v_thi.begin(); i != v_thi.end(); i++)
#if defined(_WIN32)
		m_thi[i->course] = *i._Ptr;
#elif defined(__linux__)
		m_thi[i->course] = *i;
#endif

	// Item Treasure Hunter
	CmdTreasureHunterItem cmd_thItem(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_thItem, nullptr, nullptr);

	cmd_thItem.waitEvent();

	if (cmd_thItem.getException().getCodeError() != 0)
		throw cmd_thItem.getException();

	if (cmd_thItem.getInfo().empty())
		throw exception("[TreasureHunterSystem::initialize][Error] nao tem item do Treasure Hunter no banco de dados.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TREASURE_HUNTER_SYSTEM, 1, 1));

	m_thItem = cmd_thItem.getInfo();

	// Init Time
	m_time = std::time(nullptr);

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::initialize][Log] Treasure Hunter System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::initialize][Log] Treasure Hunter System carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif // _DEBUG

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void TreasureHunterSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	memset(m_thi, 0, sizeof(m_thi));

	// Limpa a lista de itens do Treasure Hunter System
	// n�o faz o shrink_to_fit por que pode preencher ela novamente
	if (!m_thItem.empty())
		m_thItem.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

float TreasureHunterSystem::getCourseRate(unsigned char _course) {

	float rate_course = 0.f;

	TRY_CHECK;

	// Prote��o contra o Random Map, que usa o negatico do 'char'
	auto course = findCourse(_course & 0x7F);

	auto point = (course != nullptr ? course->point : 0u);

	if (point > 0)
		rate_course = (float)point / TREASURE_HUNTER_LIMIT_POINT_COURSE; // 1000.f;

	LEAVE_CHECK;

	CATCH_CHECK("getCourseRate");
	END_CHECK;

	return rate_course;
}

void TreasureHunterSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool TreasureHunterSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = (m_load && !m_thItem.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

TreasureHunterInfo* TreasureHunterSystem::getAllCoursePoint() {

	TreasureHunterInfo *thi = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	thi = m_thi;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return thi;
}

TreasureHunterInfo* TreasureHunterSystem::findCourse(unsigned char _course) {

	// Prote��o contra o Random Map, que usa o negatico do 'char'
	if ((_course & 0x7F) >= MS_NUM_MAPS)
		throw exception("[TreasureHunterSystem::findCourse][Error] _course is invalid[VALUE=" 
				+ std::to_string((unsigned short)_course & 0x7F) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TREASURE_HUNTER_SYSTEM, 2, 0));
	
	TreasureHunterInfo *thi = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// Prote��o contra o Random Map, que usa o negatico do 'char'
	thi = &m_thi[_course & 0x7F];

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return thi;
}

uint32_t TreasureHunterSystem::calcPointNormal(uint32_t _tacada, unsigned char _par_hole) {

	if (_tacada == 1)	// Hole In One(HIO)
		return 100;

	uint32_t point = 0u;

	switch ((unsigned char)(_tacada - _par_hole)) {
	case (unsigned char)-3:	// Albatross
		point = 100;
		break;
	case (unsigned char)-2:	// Eagle
		point = 50;
		break;
	case (unsigned char)-1:	// Birdie
		point = 30;
		break;
	case 0:		// Par
		point = 15;
		break;
	case 1:		// Bogey
		point = 10;
	case 2:		// Double Bogey
		point = 7;
	case 3:		// Triple Bogey
		point = 4;
	case 4:		// +4
		point = 1;
		break;
	}

	return point;
}

uint32_t TreasureHunterSystem::calcPointSSC(uint32_t _tacada, unsigned char _par_hole) {
	
	if (_tacada == 1)	// Hole In One(HIO)
		return 30;

	uint32_t point = 0u;

	switch ((unsigned char)(_tacada - _par_hole)) {
	case (unsigned char)-3:	// Albatross
		point = 1;
		break;
	case (unsigned char)-2:	// Eagle
		point = 4;
		break;
	case (unsigned char)-1:	// Birdie
		point = 7;
		break;
	case 0:		// Par
		point = 10;
		break;
	case 1:		// Bogey
		point = 15;
		break;
	case 2:		// Double Bogey
		point = 30;
		break;
	case 3:		// Triple Bogey
		point = 50;
		break;
	case 4:		// +4
		point = 100;
		break;
	}

	return point;
}

std::vector< TreasureHunterItem > TreasureHunterSystem::drawItem(uint32_t _point, unsigned char _course) {

	std::vector< TreasureHunterItem > v_item;

	TRY_CHECK;

	float rate_course = getCourseRate(_course);
	
	float rate = _point * sgs::gs::getInstance().getInfo().rate.treasure * rate_course / 100.f;

	uint32_t box = (uint32_t)(rate / TREASURE_HUNTER_BOX_PER_POINT);

	// Pelo menos uma box sortea
	if (box == 0)
		box = 1u;

	Lottery lottery((uint64_t)_point);

	for (auto& el : m_thItem)
		lottery.push(el.probabilidade, (size_t)&el);

	TreasureHunterItem thi{ 0 }, *pThi = nullptr;

	Lottery::LotteryCtx *ctx = nullptr;

	for (auto i = 0u; i < box; /*Incrementa no final do loop for*/) {

		// Sorteia item
		if ((ctx = lottery.spinRoleta()) == nullptr) {
			_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::drawItem][WARNING] nao conseguiu sortear o item. Bug", CL_FILE_LOG_AND_CONSOLE));
			continue;
		}

		pThi = (TreasureHunterItem*)ctx->value;

		// Verifica se o item existe no IFF_STRUCT do server, para n�o da erro mais tarde
		if (sIff::getInstance().findCommomItem(pThi->_typeid) == nullptr) {
			_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::drawItem][WARNING] nao conseguiu encontrar o Item[TYPEID="
					+ std::to_string(pThi->_typeid) + "] no IFF_STRUCT do server. Bug", CL_FILE_LOG_AND_CONSOLE));
			continue;
		}

		// Sorteia quantidade
		if (pThi->qntd > 1 && pThi->_typeid != PANG_POUCH_TYPEID/*PANG POUCH*/)
			pThi->qntd = 1 + (sRandomGen::getInstance().rIbeMt19937_64_chrono() % pThi->qntd);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::Sorteia Item][Log] sorteou Item[TYPEID=" + std::to_string(pThi->_typeid) + ", QNTD=" 
				+ std::to_string(pThi->qntd) + ", PROBABILIDADE=" + std::to_string(pThi->probabilidade) + ", FLAG=" + std::to_string((unsigned short)pThi->flag) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		v_item.push_back(*pThi);

		// Incrementa o index
		i++;
	}

	LEAVE_CHECK;

	CATCH_CHECK("drawItem");
	END_CHECK;

	return v_item;
}

std::vector< TreasureHunterItem > TreasureHunterSystem::drawApproachBox(uint32_t _num_box, unsigned char _course) {
	
	std::vector< TreasureHunterItem > v_item;
	TreasureHunterItem thi{ 0 }, *pThi = nullptr;

	if (_num_box == 0)
		return v_item;

	TRY_CHECK;

	float rate_course = getCourseRate(_course);

	uint32_t box = (uint32_t)(_num_box * sgs::gs::getInstance().getInfo().rate.treasure * rate_course / 100.f);

	// _num box � maior que zero, box n�o pode ser 0, tem que ser pelo menos 1
	if (box == 0u)
		box = 1u;

	Lottery lottery((uint64_t)box);

	for (auto& el : m_thItem)
		lottery.push(el.probabilidade, (size_t)&el);

	Lottery::LotteryCtx *ctx = nullptr;

	for (auto i = 0u; i < box; /*Incrementa no final do loop for*/) {

		// Sorteia item
		if ((ctx = lottery.spinRoleta()) == nullptr) {
			_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::drawApproachBox][WARNING] nao conseguiu sortear o item. Bug", CL_FILE_LOG_AND_CONSOLE));
			continue;
		}

		pThi = (TreasureHunterItem*)ctx->value;

		// Verifica se o item existe no IFF_STRUCT do server, para n�o da erro mais tarde
		if (sIff::getInstance().findCommomItem(pThi->_typeid) == nullptr) {
			_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::drawApprochBox][WARNING] nao conseguiu encontrar o Item[TYPEID=" 
					+ std::to_string(pThi->_typeid) + "] no IFF_STRUCT do server. Bug", CL_FILE_LOG_AND_CONSOLE));
			continue;
		}

		// Sorteia quantidade
		if (pThi->qntd > 1 && pThi->_typeid != PANG_POUCH_TYPEID/*PANG POUCH*/)
			pThi->qntd = 1 + (sRandomGen::getInstance().rIbeMt19937_64_chrono() % pThi->qntd);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::Sorteia Approach Box][Log] sorteou Item[TYPEID=" + std::to_string(pThi->_typeid) + ", QNTD="
				+ std::to_string(pThi->qntd) + ", PROBABILIDADE=" + std::to_string(pThi->probabilidade) + ", FLAG=" + std::to_string((unsigned short)pThi->flag) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		v_item.push_back(*pThi);

		// Incrementa o index
		i++;
	}

	LEAVE_CHECK;

	CATCH_CHECK("drawApproachBox");
	END_CHECK;

	return v_item;
}

bool TreasureHunterSystem::checkUpdateTimePointCourse() {

	TRY_CHECK;

	// Adiciona +100 pontos para o course de 30 e 30 minutos o TEMPO certo est� no TREASURE_HUNTER_TIME_UPDATE
	if ((std::time(nullptr) - m_time) >= TREASURE_HUNTER_TIME_UPDATE) {

		for (auto i = 0; i < MS_NUM_MAPS; i++)
			if (m_thi[i].point < TREASURE_HUNTER_LIMIT_POINT_COURSE)
				updateCoursePoint(m_thi[i], TREASURE_HUNTER_INCREASE_POINT);

		_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem][Log] Atualizou Pontos dos course.", CL_FILE_LOG_AND_CONSOLE));

		m_time = std::time(nullptr);

		LEAVE_CHECK;

		return true;
	}

	LEAVE_CHECK;

	CATCH_CHECK("checkUpdateTimePointCourse");
	END_CHECK;

	return false;
}

void TreasureHunterSystem::updateCoursePoint(TreasureHunterInfo& _thi, int32_t _point) {
	
	TRY_CHECK;

	if (_point < 0)	// Decrease
		_thi.point = (_thi.point + _point < 0) ? 0u : _thi.point + _point;
	else	// Increase
		_thi.point = (_thi.point + _point > TREASURE_HUNTER_LIMIT_POINT_COURSE) ? TREASURE_HUNTER_LIMIT_POINT_COURSE : _thi.point + _point;

	snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdateTreasureHunterCoursePoint(_thi), TreasureHunterSystem::SQLDBResponse, nullptr);

	LEAVE_CHECK;

	CATCH_CHECK("updateCoursePoint");
	END_CHECK;
}

void TreasureHunterSystem::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
#ifdef _DEBUG
		// Static class
		_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::SQLDBResponse][WARNING] _arg is nullptr com msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_channel = reinterpret_cast< TreasureHunterSystem* >(_arg);

	switch (_msg_id) {
	case 1:	// Update Treasure Hunter Course Point
	{
		auto cmd_uthcp = reinterpret_cast< CmdUpdateTreasureHunterCoursePoint* >(&_pangya_db);

		_smp::message_pool::getInstance().push(new message("[TreasureHunterSystem::SQLDBResponse][Log] Atualizou Treasure Hunter Course Point[COURSE=" 
				+ std::to_string((unsigned short)(cmd_uthcp->getInfo().course & 0x7F)) + ", POINT=" + std::to_string(cmd_uthcp->getInfo().point) + "]", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}

};
