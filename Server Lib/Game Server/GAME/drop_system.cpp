// Arquivo drop_system.cpp
// Criado em 07/09/218 as 13:39 por Acrisio
// Implementa��o da classe DropSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "drop_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../UTIL/lottery.hpp"

#include "../PANGYA_DB/cmd_drop_course_info.hpp"
#include "../PANGYA_DB/cmd_drop_course_config.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

using namespace stdA;

#define CHECK_SESSION(_method) { \
	if (!_session.getState() || !_session.isConnected() || _session.isQuit()) \
		throw exception("[DropSystem::" + std::string((_method)) + "][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 1, 0)); \
} \

#define CHECK_DROP(_method) { \
	if (!isLoad()) \
		throw exception("[DropSystem::" + std::string((_method)) + "][Error] Drop System not loadded, please call load method first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 2, 0)); \
	\
	if (_dc.course == 0x7F/*Random*/) \
		throw exception("[DropSystem::" + std::string((_method)) + "][Error] course is invalid(0x7F) Random value", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 3, 0)); \
	\
	if (_dc.v_item.empty()) \
		throw exception("[DropSystem::" + std::string((_method)) + "][Error] drop item vector is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 4, 0)); \
} \

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
								_smp::message_pool::getInstance().push(new message("[DropSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[DropSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

#define CHECK_SESSION_AND_DROP(_method) CHECK_SESSION((_method)) CHECK_DROP((_method))

#define ART_RAINBOW_MAGIC_HAT		0x1A0001BEul
#define ART_WICKED_BROOMSTICK		0x1A0001B4u	// +500 pangs no Ultimo Hole do game de 18H
#define ART_TEORITE_ORE				0x1A0001B6u	// +1000 pangs no Ultimo Hole do game de 18H
#define ART_REDNOSE_WIZBERRY		0x1A0001B8u	// +1500 pangs no Ultimo Hole do game de 18H
#define ART_MAGANI_FLOWER			0x1A0001BAu	// +3000 pangs no Ultimo Hole do game de 18H
#define ART_ROGER_K_STEERING_WHEEL	0x1A0001BCu	// de 500 a 501000 pangs no Ultimo Hole do game de 18H

#ifndef PANG_POUCH_TYPEID
#define PANG_POUCH_TYPEID			0x1A000010
#endif // !PANG_POUCH_TYPEID

#ifndef GRAND_PRIX_TICKET
#define GRAND_PRIX_TICKET			0x1A000264u
#endif

#define SSC_TICKET					0x1A0000F7u

//std::map< unsigned char, DropSystem::stDropCourse > DropSystem::m_course;
//DropSystem::stConfig DropSystem::m_config;
//bool DropSystem::m_load = false;

DropSystem::DropSystem() : m_load(false), m_config{}, m_course{} {

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

DropSystem::~DropSystem() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void DropSystem::initialize() {

	TRY_CHECK;

	CmdDropCourseConfig cmd_dcc(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_dcc, nullptr, nullptr);

	cmd_dcc.waitEvent();

	if (cmd_dcc.getException().getCodeError() != 0)
		throw cmd_dcc.getException();

	m_config = cmd_dcc.getConfig();

	CmdDropCourseInfo cmd_dci(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_dci, nullptr, nullptr);

	cmd_dci.waitEvent();

	if (cmd_dci.getException().getCodeError() != 0)
		throw cmd_dci.getException();

	m_course = cmd_dci.getInfo();

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[DropSystem::initialize][Log] Drop System Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[DropSystem::initialize][Log] Drop System Carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif // _DEBUG

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void DropSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_course.empty())
		m_course.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

DropSystem::stDropCourse* DropSystem::findCourse(unsigned char _course) {

	TRY_CHECK;

	// Prote��o contra o Random Map, que usa o negatico do 'char'
	auto it = m_course.find(_course & 0x7F);

	if (it != m_course.end()) {

		LEAVE_CHECK;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findCourse");
	END_CHECK;

	return nullptr;
}

void DropSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool DropSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = m_load;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

DropItem DropSystem::drawArtefactPang(stCourseInfo& _ci, uint32_t _num_players) {

	DropItem di{ 0 };

	TRY_CHECK;

	uint32_t pang = 0u;

	switch (_ci.artefact) {
	case ART_WICKED_BROOMSTICK:
		pang = 1;
		break;
	case ART_TEORITE_ORE:
		pang = 2;
		break;
	case ART_REDNOSE_WIZBERRY:
		pang = 3;
		break;
	case ART_MAGANI_FLOWER:
		pang = 6;
		break;
	case ART_ROGER_K_STEERING_WHEEL:
	{
		pang = (uint32_t)(_num_players < 25 ? (sRandomGen::getInstance().rIbeMt19937_64_chrono() % 6) + 1 : (sRandomGen::getInstance().rIbeMt19937_64_chrono() % 1002) + 1);
		
		break;
	}	// END CASE ART_ROGER_K_STEERING_WHEEL
	}	// END SWITCH

	if (pang != 0) {
		
		di._typeid = PANG_POUCH_TYPEID;
		di.course = _ci.course & 0x7F;
		di.numero_hole = _ci.hole;
		
		di.qntd = (short)pang;
		di.type = DropItem::eTYPE::QNTD_MULTIPLE_500;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[DropSystem::drawArtefactPang][Log] Dropou Item[TYPEID=" + std::to_string(di._typeid) + ", QNTD=" + std::to_string(di.qntd) + ", COURSE="
				+ std::to_string((unsigned short)_ci.course & 0x7F) + ", HOLE=" + std::to_string((unsigned short)_ci.hole) + ", TIPO=" + std::to_string(di.type) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
	}

	LEAVE_CHECK;

	CATCH_CHECK("drawArtectPang");
	END_CHECK;

	return di;
}

std::vector< DropItem > DropSystem::drawCourse(stDropCourse& _dc, stCourseInfo& _ci) {
	CHECK_DROP("drawCourse");

	std::vector< DropItem > v_item;

	TRY_CHECK;

	uint32_t qntd = 1;

	if (_ci.char_motion)
		qntd *= 2;

	if (_ci.artefact == ART_RAINBOW_MAGIC_HAT)
		qntd++;

	Lottery lottery((uint64_t)&_dc);
	Lottery::LotteryCtx *ctx = nullptr;

	DropSystem::stDropCourse::stDropItem *pDi = nullptr;
	DropItem di{ 0 };

	float rate = 1.f;

	if (_ci.rate_drop > 100)
		rate *= _ci.rate_drop / 100.f;

	if (_ci.angel_wings)
		rate *= 1.2f;	// 20%

	for (auto& el : _dc.v_item) {

		if ((el.tipo != DropSystem::stDropCourse::stDropItem::eTIPO::LAST_HOLE_PROBABILITY || _ci.seq_hole == _ci.qntd_hole)
			&& (el.tipo != DropSystem::stDropCourse::stDropItem::eTIPO::SEQUENCE_DROP || (_ci.seq_hole % el.probabilidade[DropSystem::stDropCourse::stDropItem::ePROB_TIPO::_6HOLES_SEQUENCE]) == 0)) {

			lottery.clear();

			switch (el.tipo) {
				case DropSystem::stDropCourse::stDropItem::eTIPO::ALL_PROBABILITY:
				case DropSystem::stDropCourse::stDropItem::eTIPO::SEQUENCE_DROP:
				default:
					// Aqui � item por item que sorteia, cada item tem sua chance no course
					lottery.push(el.probabilidade[DropSystem::stDropCourse::stDropItem::ePROB_TIPO::_3HOLES_ALL], (size_t)&el);
					break;
				case DropSystem::stDropCourse::stDropItem::eTIPO::LAST_HOLE_PROBABILITY:
				{
					// Holes do jogo
					if (_ci.qntd_hole == 3)
						lottery.push(el.probabilidade[DropSystem::stDropCourse::stDropItem::ePROB_TIPO::_3HOLES_ALL], (size_t)&el);
					else if (_ci.qntd_hole == 6)
						lottery.push(el.probabilidade[DropSystem::stDropCourse::stDropItem::ePROB_TIPO::_6HOLES_SEQUENCE], (size_t)&el);
					else if (_ci.qntd_hole == 9)
						lottery.push(el.probabilidade[DropSystem::stDropCourse::stDropItem::ePROB_TIPO::_9HOLES], (size_t)&el);
					else		// 18 holes
						lottery.push(el.probabilidade[DropSystem::stDropCourse::stDropItem::ePROB_TIPO::_18HOLES], (size_t)&el);

					break;
				}
			}

			// S� coloca outro pra sortear se a probabilidade for menor que 1000(100%)
			if ((lottery.getLimitProbilidade() * rate) < 1000u)
				lottery.push((uint32_t)(1000 - (lottery.getLimitProbilidade() * rate)), 0u);

			// Sorteia
			ctx = lottery.spinRoleta();

			if (ctx == nullptr)
				throw exception("[DropSystem::drawCourse][Error] nao conseguiu sortear um drop.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 7, 0));

			if (ctx->value != 0) {
			
				pDi = (DropSystem::stDropCourse::stDropItem*)ctx->value;

				di.clear();

				di._typeid = pDi->_typeid;
				di.qntd = (short)pDi->qntd;

				di.course = _ci.course & 0x7F;
				di.numero_hole = _ci.hole;

				di.type = DropItem::eTYPE::NORMAL_QNTD;

				for (auto i = 0u; i < qntd; ++i)
					v_item.push_back(di);

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[DropSystem::drawCourse][Log] Dropou Item[TYPEID=" + std::to_string(di._typeid) + ", QNTD=" + std::to_string(di.qntd) + ", COURSE="
						+ std::to_string((unsigned short)_ci.course & 0x7F) + ", HOLE=" + std::to_string((unsigned short)_ci.hole) + ", TIPO=" + std::to_string(di.type) + "] FLAG[CHAR_MOTION_ITEM="
						+ std::to_string(_ci.char_motion) + ", ART_RAINBOW_MAGIC_HAT=" + std::to_string((_ci.artefact == ART_RAINBOW_MAGIC_HAT) ? 1 : 0) + "]", CL_FILE_LOG_AND_CONSOLE));
	#endif // _DEBUG
			}

		}
	}

	LEAVE_CHECK;

	CATCH_CHECK("drawCourse");
	END_CHECK;

	return v_item;
}

DropItem DropSystem::drawManaArtefact(stCourseInfo& _ci) {

	DropItem di{ 0 };

	TRY_CHECK;

	auto item = sIff::getInstance().getItem();

	Lottery lottery((uint64_t)&_ci);

	std::for_each(item.begin(), item.end(), [&](auto& _el) {
		if (_el.second.tipo_item == 4/*Artefect Mana*/)
			lottery.push(200, (size_t)&_el.second);
	});

	auto limit = lottery.getLimitProbilidade();

	if (limit <= 0)
		throw exception("[DropSystem::drawManaArtefact][Error] nao achou nenhum mana artefact item no IFF_STRUCT. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 8, 0));

	float rate = (float)((m_config.rate_mana_artefact > 0) ? m_config.rate_mana_artefact / 100.f : 1.f);

	// Drop Item Rate of player and Angel Wing
	if (_ci.rate_drop > 100)
		rate *= _ci.rate_drop / 100.f;

	if (_ci.angel_wings)
		rate *= 1.2f;	// 20%
	// End Drop Item Rate of Player and Angel Wing

	limit = (uint64_t)(limit / rate);

	lottery.push((uint32_t)limit, 0);

	auto ctx = lottery.spinRoleta();

	if (ctx == nullptr)
		throw exception("[DropSystem::drawManaArtefact][Error] nao conseguiu sortear um drop.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 7, 0));

	if (ctx->value != 0) {
		auto pDi = (IFF::Item*)ctx->value;

		di._typeid = pDi->_typeid;
		di.qntd = 1;

		di.course = _ci.course & 0x7F;
		di.numero_hole = _ci.hole;

		di.type = DropItem::eTYPE::NORMAL_QNTD;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[DropSystem::drawManaArtefact][Log] Dropou Item[TYPEID=" + std::to_string(di._typeid) + ", QNTD=" + std::to_string(di.qntd) + ", COURSE="
				+ std::to_string((unsigned short)_ci.course & 0x7F) + ", HOLE=" + std::to_string((unsigned short)_ci.hole) + ", TIPO=" + std::to_string(di.type) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}

	LEAVE_CHECK;

	CATCH_CHECK("drawManaArtefact");
	END_CHECK;

	return di;
}

DropItem DropSystem::drawGrandPrixTicket(stCourseInfo& _ci, player& _session) {
	CHECK_SESSION("drawGrandPrixTicket");

	DropItem di{ 0 };

	TRY_CHECK;

	short qntd = 0u;

	auto pWi = _session.m_pi.findWarehouseItemByTypeid(GRAND_PRIX_TICKET);

	if (pWi == nullptr || (qntd = pWi->STDA_C_ITEM_QNTD) < LIMIT_GRAND_PRIX_TICKET) {

		if (_ci.qntd_hole == 18 || _ci.qntd_hole == 9) {	// 100%
			
			di._typeid = GRAND_PRIX_TICKET;
			di.course = _ci.course & 0x7F;
			di.numero_hole = _ci.hole;
			di.type = DropItem::eTYPE::NORMAL_QNTD;

			di.qntd = (qntd == 49 || _ci.qntd_hole == 9) ? 1 : 2;

		}else {	// 50%

			Lottery lottery((uint64_t)&_ci);

			lottery.push(200, GRAND_PRIX_TICKET);
			lottery.push(400, 0);

			auto ctx = lottery.spinRoleta();

			if (ctx == nullptr)
				throw exception("[DropSystem::drawGrandPrixTicket][Error] nao conseguiu sortear um drop.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 7, 0));

			if (ctx->value != 0) {

				di._typeid = (uint32_t)ctx->value;
				di.qntd = 1;

				di.course = _ci.course & 0x7F;
				di.numero_hole = _ci.hole;

				di.type = DropItem::eTYPE::NORMAL_QNTD;
			}
		}

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[DropSystem::drawGrandPrixTicket][Log] Dropou Item[TYPEID=" + std::to_string(di._typeid) + ", QNTD=" + std::to_string(di.qntd) + ", COURSE="
				+ std::to_string((unsigned short)_ci.course & 0x7F) + ", HOLE=" + std::to_string((unsigned short)_ci.hole) + ", TIPO=" + std::to_string(di.type) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
	}

	LEAVE_CHECK;

	CATCH_CHECK("drawGrandPrixTicket");
	END_CHECK;

	return di;
}

std::vector< DropItem > DropSystem::drawSSCTicket(stCourseInfo& _ci) {

	std::vector< DropItem > v_item;
	uint32_t qntd = 1;

	TRY_CHECK;

	if (_ci.char_motion)
		qntd *= 2;

	if (_ci.artefact == ART_RAINBOW_MAGIC_HAT)
		qntd++;

	Lottery lottery((uint64_t)&_ci);

	lottery.push(200, SSC_TICKET);

	auto limit = lottery.getLimitProbilidade();

	float rate = (float)((m_config.rate_SSC_ticket > 0) ? m_config.rate_SSC_ticket / 100.f : 1.f);

	// Drop Item Rate of player and Angel Wing
	if (_ci.rate_drop > 100)
		rate *= _ci.rate_drop / 100.f;

	if (_ci.angel_wings)
		rate *= 1.2f;	// 20%
	// End Drop Item Rate of Player and Angel Wing

	limit = (uint64_t)(limit / rate);

	lottery.push((uint32_t)limit, 0);
	lottery.push((uint32_t)limit, 0);

	auto ctx = lottery.spinRoleta();

	if (ctx == nullptr)
		throw exception("[DropSystem::drawSSCTicker][Error] nao conseguiu sortear um drop.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::DROP_SYSTEM, 7, 0));

	if (ctx->value != 0) {

		DropItem di{ 0 };

		di._typeid = (uint32_t)ctx->value;
		di.qntd = 1;

		di.course = _ci.course & 0x7F;
		di.numero_hole = _ci.hole;

		di.type = DropItem::eTYPE::NORMAL_QNTD;

		for (auto i = 0u; i < qntd; ++i)
			v_item.push_back(di);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[DropSystem::drawSSCTicker][Log] Dropou Item[TYPEID=" + std::to_string(di._typeid) + ", QNTD=" + std::to_string(di.qntd) + ", COURSE="
				+ std::to_string((unsigned short)_ci.course & 0x7F) + ", HOLE=" + std::to_string((unsigned short)_ci.hole) + ", TIPO=" + std::to_string(di.type) + "] FLAG[CHAR_MOTION_ITEM="
				+ std::to_string(_ci.char_motion) + ", ART_RAINBOW_MAGIC_HAT=" + std::to_string((_ci.artefact == ART_RAINBOW_MAGIC_HAT) ? 1 : 0) + "]", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	}

	LEAVE_CHECK;

	CATCH_CHECK("drawSSCTicket");
	END_CHECK;

	return v_item;
}
