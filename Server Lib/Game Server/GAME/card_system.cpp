// Arquivo card_system.cpp
// Criado em 30/06/2018 as 18:06 por Acrisio
// Implementa��o da classe CardSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "card_system.hpp"

#include "../UTIL/lottery.hpp"

#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../PANGYA_DB/cmd_card_pack.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include <algorithm>

using namespace stdA;

#define FIND_MAP_CARD_PACK_BY_VOLUME(_map, _value) std::find_if((_map).begin(), (_map).end(), [&](auto& _el_map) { \
	return _el_map.second.volume == (_value); \
}); \

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
								_smp::message_pool::getInstance().push(new message("[CardSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[CardSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

//std::vector< Card > CardSystem::m_card;
//std::map< uint32_t, CardPack > CardSystem::m_card_pack;
//std::map< uint32_t, CardPack > CardSystem::m_box_card_pack;
//
//bool CardSystem::m_load = false;

CardSystem::CardSystem() : m_load(false) {

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

CardSystem::~CardSystem() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void CardSystem::initialize() {

	TRY_CHECK;

	// Load Card from IFF_STRUCT
	auto card = sIff::getInstance().getCard();

	for (auto& el : card) {
		switch (sIff::getInstance().getItemSubGroupIdentify22(el.second._typeid)) {
		case 0:	// Character
		case 1:	// Caddie
		case 2:	// Special
		case 5:	// NPC
			m_card.push_back({ el.second._typeid, 0, CARD_TYPE(el.second.tipo) });
			break;
		case 4:	// Box Card Pack
		{
			if ((m_box_card_pack.find(el.second._typeid)) == m_box_card_pack.end())
				m_box_card_pack[el.second._typeid] = { el.second._typeid, 3, (unsigned char)el.second.volume };
			break;
		}
		case 3:	// Card Pack
			// N�o usa esses, por que � card pack os dois
			break;
		default:
			throw exception("[CardSystem::initialize][Error] Card Group Type Is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 4, 0));
		}
	}

	// Load Card Pack Map
	CmdCardPack cmd_cp(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_cp, nullptr, nullptr);

	cmd_cp.waitEvent();

	if (cmd_cp.getException().getCodeError() != 0)
		throw cmd_cp.getException();

	m_card_pack = cmd_cp.getCardPack();

	// Load Box Card Pack
	for (auto& el : m_box_card_pack) {
		auto it = std::find_if(m_card_pack.begin(), m_card_pack.end(), [&](auto& el2) {
			return el2.second.volume == el.second.volume;
		});

		if (it != m_card_pack.end())
			el.second.card.assign(it->second.card.begin(), it->second.card.end());
	}

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[CardSystem::initialize][Log] Carregou os CardPack/Box e Card com sucesso.", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[CardSystem::initialize][Log] Carregou os CardPack/Box e Card com sucesso.", CL_ONLY_FILE_LOG));
//#endif
	
	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void CardSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_card.empty()) {
		m_card.clear();
		m_card.shrink_to_fit();
	}

	if (!m_card_pack.empty())
		m_card_pack.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void CardSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool CardSystem::isLoad() {

	bool isLoad = false;
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = (m_load && !m_card_pack.empty() && !m_box_card_pack.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

CardPack* CardSystem::findCardPack(uint32_t _typeid) {

	if (!isLoad())
		throw exception("[CardSystem::findCardPack][Error] Card System nao esta carregado, carregue ele primeiro antes de procurar um Card Pack.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 5, 0));

	if (_typeid == 0)
		throw exception("[CardSystem::findCardPack][Error] _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 1, 0));

	TRY_CHECK;

	auto it = m_card_pack.end();

	if ((it = m_card_pack.find(_typeid)) != m_card_pack.end()) {

		LEAVE_CHECK;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findCardPack");
	END_CHECK;

	return nullptr;
}

CardPack* CardSystem::findBoxCardPack(uint32_t _typeid) {

	if (!isLoad())
		throw exception("[CardSystem::findBoxCardPack][Error] Card System nao esta carregado, carregue ele primeiro antes de procurar um Box Card Pack.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 5, 0));

	if (_typeid == 0)
		throw exception("[CardSystem::findBoxCardPack][Error] _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 1, 0));

	TRY_CHECK;

	auto it = m_box_card_pack.end();

	if ((it = m_box_card_pack.find(_typeid)) != m_box_card_pack.end()) {
		
		LEAVE_CHECK;
		
		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findBoxCardPack");
	END_CHECK;

	return nullptr;
}

Card* CardSystem::findCard(uint32_t _typeid) {

	if (!isLoad())
		throw exception("[CardSystem::findCard][Error] Card System nao esta carregado, carregue ele primeiro antes de procurar um Card.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 5, 0));

	if (_typeid == 0)
		throw exception("[CardSystem::findCard][Error] _typeid is invalid(zeror)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 1, 0));

	TRY_CHECK;

	auto it = std::find_if(m_card.begin(), m_card.end(), [&](auto& el) {
		return el._typeid == _typeid;
	});

	if (it != m_card.end()) {
		
		LEAVE_CHECK;
		
#if defined(_WIN32)
		return it._Ptr;
#elif defined(__linux__)
		return &(*it);
#endif
	}

	LEAVE_CHECK;

	CATCH_CHECK("findCard");
	END_CHECK;

	return nullptr;
};

std::vector< Card > CardSystem::draws(CardPack& _cp) {

	if (_cp._typeid == 0 || _cp.num == 0 || _cp.card.empty())
		throw exception("[CardSystem::findCardPack][Error] CardPack[TYPEID=" + std::to_string(_cp._typeid) + ", NUM=" + std::to_string(_cp.num) +", card(s)=" 
				+ std::to_string(_cp.card.size()) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 1, 0));

	std::vector< Card > v_card;

	TRY_CHECK;

	Lottery lottery((uint64_t)&_cp);

	for (auto& el : _cp.card)
		lottery.push((uint32_t)(el.prob * (double)((el.tipo > CARD_TYPE::T_SECRET ? 1.f : _cp.rate.value[el.tipo] / 100.f))), (size_t)&el);

	for (auto i = 0u; i < _cp.num; ++i) {
		auto lc = lottery.spinRoleta();

		if (lc == nullptr)
			throw exception("[CardSystem::draws][ErrorSystem] nao conseguiu sortear um card", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 2, 0));

		if ((Card*)lc->value == nullptr)
			throw exception("[CardSystem::draws][ErrorSystem] valor retornado do sorteio eh invalido(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 3, 0));

		v_card.push_back(*(Card*)lc->value);
	}

	LEAVE_CHECK;
	CATCH_CHECK("draws");
	END_CHECK;

	return v_card;
}

Card CardSystem::drawsLoloCardCompose(LoloCardComposeEx& _lcc) {
	
	Card card{ 0 };

	TRY_CHECK;

	Lottery lottery((uint64_t)&_lcc);

	uint32_t prob = 0u;

	for (auto i = 0u; i < (sizeof(_lcc._typeid) / sizeof(_lcc._typeid[0])); ++i)
		prob += ((_lcc.tipo + 1) * 20);

	for (auto i = 1u; i <= 5u; ++i) {
		auto it = FIND_MAP_CARD_PACK_BY_VOLUME(m_card_pack, i);

		if (it != m_card_pack.end()) {
			for (auto& el : it->second.card)
				lottery.push((el.tipo > CARD_TYPE::T_NORMAL ? el.prob + prob : el.prob), (size_t)&el);
		}
	}

	auto lc = lottery.spinRoleta();

	if (lc == nullptr)
		throw exception("[CardSystem::drawsLoloCardCompose][ErrorSystem] nao conseguiu sortear um card", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 2, 0));

	if ((Card*)lc->value == nullptr)
		throw exception("[CardSystem::drawsLoloCardCompose][ErrorSystem] valor retornado do sorteio eh invalido(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CARD_SYSTEM, 3, 0));

	card = *(Card*)lc->value;

	LEAVE_CHECK;
	CATCH_CHECK("drawsLoloCardCompose");
	END_CHECK;

	return card;
}
