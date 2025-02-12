// Arquivo memorial_system.cpp
// Criado em 21/07/2018 as 18:58 por Acrisio
// Implementa��o da classe MemorialSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "memorial_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/iff.h"
#include "../UTIL/lottery.hpp"

#include <string>

#include <algorithm>

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_memorial_normal_item_info.hpp"
#include "../PANGYA_DB/cmd_memorial_level_info.hpp"

#include "../Game Server/game_server.h"

#define CHECK_SESSION(_method) { \
	if (!_session.getState() || !_session.isConnected() || _session.isQuit()) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 1, 0)); \
} \

#define CHECK_SESSION_AND_COIN(_method) { \
	CHECK_SESSION((_method)); \
	\
	if (!isLoad()) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] Memorial System not loadded, please call load method first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 2, 0)); \
	\
	if (_ctx_c._typeid == 0) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] coin _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 3, 0)); \
	\
	if (_ctx_c.item.empty()) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] coin is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 4, 0)); \
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
								_smp::message_pool::getInstance().push(new message("[MemorialSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[MemorialSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

using namespace stdA;

//std::map< uint32_t, ctx_coin > MemorialSystem::m_coin;
//std::map< uint32_t, ctx_memorial_level > MemorialSystem::m_level;
//std::map< uint32_t, ctx_coin_set_item > MemorialSystem::m_consolo_premio;
//bool MemorialSystem::m_load = false;

MemorialSystem::MemorialSystem() : m_load(false), m_coin{}, m_level{}, m_consolo_premio{} {

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

MemorialSystem::~MemorialSystem() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void MemorialSystem::initialize() {

	TRY_CHECK;

	// Carrega as Coin e os Itens
	auto coins = sIff::getInstance().getMemorialShopCoinItem();
	auto rares = sIff::getInstance().getMemorialShopRareItem();

	ctx_coin c{ 0 };
	ctx_coin_item_ex ci{ 0 };

	for (auto& el : coins) {
		c.clear();

		c.tipo = MEMORIAL_COIN_TYPE(el.second.type);
		c._typeid = el.second._typeid;
		c.probabilidade = el.second.probability;

		for (auto& el2 : rares) {

			if (!el.second.gacha_range.empty() && !el.second.gacha_range.isBetweenGacha(el2.gacha.number))
				continue;
			
			if (el.second.emptyFilter()) {
				ci.clear();

				ci.tipo = el2.rare_type;
				ci._typeid = el2._typeid;
				ci.probabilidade = el2.probability;
				ci.gacha_number = el2.gacha.number;
				ci.qntd = 1;

				c.item.push_back(ci);
			}else {
				for (auto i = 0u; i < (sizeof(el2.filter) / sizeof(el2.filter[0])); ++i) {
					
					if (el.second.hasFilter(el2.filter[i])) {
						ci.clear();

						ci.tipo = el2.rare_type;
						ci._typeid = el2._typeid;
						ci.probabilidade = el2.probability;
						ci.gacha_number = el2.gacha.number;
						ci.qntd = 1;

						c.item.push_back(ci);

						break;	// Sai do Loop de Filters
					}
				}	// Fim do loop de Filters
			}
		}	// Fim do loop de Rare Item

		// Add Coin ao Map
		auto it = m_coin.find(c._typeid);

		if (it == m_coin.end())	// Add Coin ao map
			m_coin[c._typeid] = c;
		else
			_smp::message_pool::getInstance().push(new message("[MemorialSystem::initialize][WARNING] ja tem essa coin[TYPEID=" + std::to_string(c._typeid) + "] no map.", CL_FILE_LOG_AND_CONSOLE));

	}	// Fim do loop de Coin Item

	// Add os Itens Padr�es, para quando n�o ganha o rare item
	CmdMemorialNormalItemInfo cmd_mnii(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_mnii, nullptr, nullptr);

	cmd_mnii.waitEvent();

	if (cmd_mnii.getException().getCodeError() != 0)
		throw cmd_mnii.getException();

	m_consolo_premio = cmd_mnii.getInfo();

	// Levels
	CmdMemorialLevelInfo cmd_mli(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_mli, nullptr, nullptr);

	cmd_mli.waitEvent();

	if (cmd_mli.getException().getCodeError() != 0)
		throw cmd_mli.getException();

	m_level = cmd_mli.getInfo();

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[MemorialSystem::initialize][Log] Memorial System Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[MemorialSystem::initialize][Log] Memorial System Carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif // _DEBUG

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void MemorialSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_coin.empty())
		m_coin.clear();

	if (!m_level.empty())
		m_level.clear();

	if (!m_consolo_premio.empty())
		m_consolo_premio.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

uint32_t MemorialSystem::calculeMemorialLevel(uint32_t _achievement_pontos) {
	
	if (_achievement_pontos == 0)
		return 0u;	// Level 0

	auto level = ((_achievement_pontos - 1) / 300);

	return (level > MEMORIAL_LEVEL_MAX ? MEMORIAL_LEVEL_MAX : level);
}

bool MemorialSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// + 1 no MEMORIAL_LEVEL_MAX por que � do 0 a 24, da 25 Levels
	isLoad = (m_load && !m_coin.empty() && (!m_level.empty() && m_level.size() == (MEMORIAL_LEVEL_MAX + 1)) && !m_consolo_premio.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

void MemorialSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

ctx_coin* MemorialSystem::findCoin(uint32_t _typeid) {

	TRY_CHECK;

	auto it = m_coin.find(_typeid);

	if (it != m_coin.end()) {

		LEAVE_CHECK;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findCoin");
	END_CHECK;

	return nullptr;
}

std::vector< ctx_coin_item_ex > MemorialSystem::drawCoin(player& _session, ctx_coin& _ctx_c) {
	CHECK_SESSION_AND_COIN("drawCoin");

	std::vector< ctx_coin_item_ex > v_item;

	TRY_CHECK;
	
	Lottery lottery((uint64_t)&_ctx_c);

	ctx_coin_item_ex *ci = nullptr;
	ctx_coin_set_item *csi = nullptr;

	// Calcula Memorial Level Pelos Achievement Pontos
	uint32_t level = calculeMemorialLevel(_session.m_pi.mgr_achievement.getPontos());

	// Initialize Rare Item e add � roleta
	for (auto& el : _ctx_c.item) {
		switch (_ctx_c.tipo) {
		case MCT_NORMAL:
			if (el.gacha_number < 0 || (uint32_t)el.gacha_number <= m_level[level].gacha_number)
				lottery.push(el.probabilidade, (size_t)&el);
			break;
		case MCT_PREMIUM:
			if (el.gacha_number < 0 || (uint32_t)el.gacha_number <= m_level[MEMORIAL_LEVEL_MAX].gacha_number)     //Teste Memorial
				lottery.push(el.probabilidade/* + _ctx_c.probabilidade*/, (size_t)&el);
			break;
		case MCT_SPECIAL:	// Special n�o tem limite de level, ele pega todos
				lottery.push(el.probabilidade/* + _ctx_c.probabilidade*/, (size_t)&el);
			break;
		default:
			throw exception("[MemorialSystem::drawCoin][Error] Memorial Coin[TYPE=" + std::to_string(_ctx_c.tipo) + "] desconhecido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 6, 0));
		}
	}

	// Init Common Itens
	auto limit_prob = lottery.getLimitProbilidade();

	auto count_item = std::count_if(m_consolo_premio.begin(), m_consolo_premio.end(), [&](auto& el) {
		return el.second.tipo == (_ctx_c.tipo == MCT_PREMIUM ? 1 : 0);
	});

	auto rate_memorial = (float)(sgs::gs::getInstance().getInfo().rate.memorial_shop) / 100.f;

	// Rate A+ da Coin
	if (_ctx_c.probabilidade > 0)
		rate_memorial += (float)(_ctx_c.probabilidade * 4 / 100.f);	// 100 * 4 / 100 4 / 4 50% coin premium 

	limit_prob = (uint64_t)(limit_prob * (4 / rate_memorial));	// Padr�o 75% do limite de probabilidade consolo, 25% normal

	if (count_item > 0)
		count_item = (uint32_t)limit_prob / count_item;

	// Add Common Itens � roleta
	for (auto& el : m_consolo_premio)
		if (el.second.tipo == (_ctx_c.tipo == MCT_PREMIUM ? 1 : 0))
			lottery.push((uint32_t)count_item/*probabilidade*/, (size_t)&el.second);

	Lottery::LotteryCtx* lc = nullptr;
	uint32_t count = 1;	// Qntd de pr�mios sorteados

	do {

		CHECK_SESSION("drawCoin");

		lc = lottery.spinRoleta(true);	// Remove os Item que j� foi sorteado

		if (lc == nullptr)
			throw exception("[MemorialSystem::drawCoin][Error] nao conseguiu sortear um item. erro na hora de rodar a roleta", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 5, 0));

		if (lc->value == 0u)
			throw exception("[MemorialSystem::drawCoin][Error] nao conseguiu sortear um item. lc->value is invalid(0).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 5, 1));

		// Tempor�rio Coin Item

		// Verifica se � SetItem ou Item
		auto test = *(int32_t*)lc->value;

		if (test == -100) {	// SetItem
			csi = (ctx_coin_set_item*)lc->value;

			for (auto& el : csi->item) {
				// Contianua que o player j� tem esse item, e n�o pode ter duplicatas dele 
				if ((!sIff::getInstance().IsCanOverlapped(el._typeid) || sIff::getInstance().getItemGroupIdentify(el._typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(el._typeid))
					continue;

				v_item.push_back(el);
			}
		}else {	// Item
			ci = (ctx_coin_item_ex*)lc->value;

			// Contianua que o player j� tem esse item, e n�o pode ter duplicatas dele
			if ((!sIff::getInstance().IsCanOverlapped(ci->_typeid) || sIff::getInstance().getItemGroupIdentify(ci->_typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(ci->_typeid))
				continue;

			v_item.push_back(*ci);
		}

		// Decrementa o count, que 1 item voi sorteado
		--count;

	} while (count > 0);

	LEAVE_CHECK;

	CATCH_CHECK("drawCoin");
	END_CHECK;

	return v_item;
}
