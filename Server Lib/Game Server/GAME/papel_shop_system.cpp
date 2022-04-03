// Arquivo papel_shop_system.cpp
// Criado em 09/07/2018 as 18:43 por Acrisio
// Implementa��o da classe PapelShopSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "papel_shop_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../UTIL/lottery.hpp"

#include <algorithm>
#include <ctime>

#include "../../Projeto IOCP/UTIL/iff.h"
#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include "../PANGYA_DB/cmd_papel_shop_config.hpp"
#include "../PANGYA_DB/cmd_papel_shop_coupon.hpp"
#include "../PANGYA_DB/cmd_papel_shop_item.hpp"

#include "../PANGYA_DB/cmd_update_papel_shop_info.hpp"
#include "../PANGYA_DB/cmd_update_papel_shop_config.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../Game Server/game_server.h"

using namespace stdA;

// Papel Shop com redução do limite de vezes que poder jogar no dia
// 1 ON, 0 OFF
#define _PS_COM_REDUCE_LIMIT 0

#define CHECK_SESSION(_method) { \
	if (!_session.getState() || !_session.isConnected() || _session.isQuit()) \
		throw exception("[PapelShopSystem::" + std::string((_method)) + "][Error] player is not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 2, 0)); \
}

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
								_smp::message_pool::getInstance().push(new message("[PapelShopSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[PapelShopSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

//std::vector< ctx_papel_shop_item > PapelShopSystem::m_ctx_psi;
//std::map< uint32_t, ctx_papel_shop_coupon > PapelShopSystem::m_ctx_psc;
//ctx_papel_shop PapelShopSystem::m_ctx_ps{0};
//bool PapelShopSystem::m_load = false;

PapelShopSystem::PapelShopSystem() : m_load(false), m_ctx_ps{ 0 }, m_ctx_psc{}, m_ctx_psi{} {

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

PapelShopSystem::~PapelShopSystem() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void PapelShopSystem::initialize() {

	TRY_CHECK;

	// Load Config
	CmdPapelShopConfig cmd_psc(true); // Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_psc, nullptr, nullptr);

	cmd_psc.waitEvent();

	if (cmd_psc.getException().getCodeError() != 0)
		throw cmd_psc.getException();

	m_ctx_ps = cmd_psc.getInfo();

	// Laod Coupon(s)
	CmdPapelShopCoupon cmd_psCoupon(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_psCoupon, nullptr, nullptr);

	cmd_psCoupon.waitEvent();

	if (cmd_psCoupon.getException().getCodeError() != 0)
		throw cmd_psCoupon.getException();

	m_ctx_psc = cmd_psCoupon.getInfo();

	// Load Item(s)
	CmdPapelShopItem cmd_psi(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_psi, nullptr, nullptr);

	cmd_psi.waitEvent();

	if (cmd_psi.getException().getCodeError() != 0)
		throw cmd_psi.getException();

	m_ctx_psi = cmd_psi.getInfo();

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PapelShopSystem::initialize][Log] Papel Shop System Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[PapelShopSystem::initialize][Log] Papel Shop System Carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif

	// Carregado com sucesso!
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relança para o server toma as providências
	throw;

	END_CHECK;
}

void PapelShopSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_ctx_ps = { 0 };

	if (!m_ctx_psi.empty()) {
		m_ctx_psi.clear();
		m_ctx_psi.shrink_to_fit();
	}

	if (!m_ctx_psc.empty())
		m_ctx_psc.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

bool PapelShopSystem::checkUpdate() {

	if (!isLoad())
		throw exception("[PapelShopSystem::checkUpdate][Error] O Papel Shop System nao foi carregado ainda, carregue ele primeiro antes de tentar verificar se pode atualizar o dia", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 3, 0));
	
	SYSTEMTIME local{ 0 };

	GetLocalTime(&local);

	bool check = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// update
	check = (m_ctx_ps.update_date.wYear < local.wYear || m_ctx_ps.update_date.wMonth < local.wMonth || m_ctx_ps.update_date.wDay < local.wDay);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return check;
}

bool PapelShopSystem::checkUpdate(SYSTEMTIME& _st) {

	// Verifica se é vazio, se for retorna true por que é diferente
	if (isEmpty(_st))
		return true;

	bool check = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	check = (m_ctx_ps.update_date.wYear != _st.wYear || m_ctx_ps.update_date.wMonth != _st.wMonth || m_ctx_ps.update_date.wDay != _st.wDay);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return check;
}

void PapelShopSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool PapelShopSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = (m_load && !m_ctx_psi.empty() && !m_ctx_psc.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

bool PapelShopSystem::isLimittedPerDay() {

	bool limited_per_day = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	limited_per_day = m_ctx_ps.limitted_per_day;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return limited_per_day;
}

void PapelShopSystem::init_player_papel_shop_info(player& _session) {
	CHECK_SESSION("init_player_papel_shop_info");

	TRY_CHECK;

	if (!isLimittedPerDay()/*Not Limitted Per Day*/)
		_session.m_pi.mi.papel_shop = { -1 };
	else {

		// Limitted Per Day
		if (checkUpdate(_session.m_pi.mi.papel_shop_last_update)) {

			// Update Papel Shop Last Day Update of Player
			updateDiaPlayer(_session);

		}
	}

	LEAVE_CHECK;

	CATCH_CHECK("init_player_papel_shop_info");
	END_CHECK;
}

void PapelShopSystem::updateDia() {

	if (!isLoad())
		throw exception("[PapelShopSystem::updateDia][Error] O Papel Shop System nao foi carregado ainda, carregue ele primeiro antes de tentar atualizar o dia", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 3, 0));

	TRY_CHECK;

	if (checkUpdate()) {
		
		// Update Dia do Papel Shop
		GetLocalTime(&m_ctx_ps.update_date);

		CmdUpdatePapelShopConfig cmd_upsc(m_ctx_ps, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_upsc, PapelShopSystem::SQLDBResponse, nullptr);

		cmd_upsc.waitEvent();

		if (cmd_upsc.getException().getCodeError() != 0) {

			LEAVE_CHECK;
			
			_smp::message_pool::getInstance().push(new message("[PapelShopSystem::updateDia][ErrorSystem] " + cmd_upsc.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			return;	// Error Sai da função
		}

		// Update Aqui por que outro sistema atualizou primeiro no banco de dados
		if (cmd_upsc.isUpdated())
			_smp::message_pool::getInstance().push(new message("[PapelShopSystem::updateDia][Log] Atualizou Papel Shop Config[" + cmd_upsc.getInfo().toString() 
					+ "] com sucesso", CL_FILE_LOG_AND_CONSOLE));
		else {
			
			m_ctx_ps = cmd_upsc.getInfo();

			_smp::message_pool::getInstance().push(new message("[PapelShopSystem::updateDia][Log] Atualizou Papel Shop Config["
					+ cmd_upsc.getInfo().toString() + "] com os dados do DB, mas quem atualizou no DB foi outro sistema.", CL_FILE_LOG_AND_CONSOLE));
		}
	}

	LEAVE_CHECK;

	CATCH_CHECK("updateDia");
	END_CHECK;
}

void PapelShopSystem::updateDiaPlayer(player& _session) {
	CHECK_SESSION("updateDaiPlayer");

	TRY_CHECK;

	// Update Time
	GetLocalTime(&_session.m_pi.mi.papel_shop_last_update);

#if _PS_COM_REDUCE_LIMIT == 1
	// Atualiza o contador de quantas vez o player pode jogar no dia
	_session.m_pi.mi.papel_shop.remain_count = _session.m_pi.mi.papel_shop.limit_count;

	// Só aumenta o limite, se o player não jogou todo o seu limite
	if (_session.m_pi.mi.papel_shop.current_count < _session.m_pi.mi.papel_shop.limit_count) {

		if (_session.m_pi.mi.papel_shop.limit_count < 50)
			_session.m_pi.mi.papel_shop.limit_count = 50;
		else if (_session.m_pi.mi.papel_shop.limit_count < 100)
			_session.m_pi.mi.papel_shop.limit_count = 100;
	}
#else // Sem limit
	_session.m_pi.mi.papel_shop.remain_count = _session.m_pi.mi.papel_shop.limit_count = 100;
#endif

	// Reseta o Atual Contador do player
	_session.m_pi.mi.papel_shop.current_count = 0;

	// Log
	_smp::message_pool::getInstance().push(new message("[PapelShopSystem::updateDiaPlayer][Log] player[UID=" + std::to_string(_session.m_pi.uid)
			+ "] atualizou Papel Shop Limit[REMAIN=" + std::to_string(_session.m_pi.mi.papel_shop.remain_count)
			+ ", CURRENT=" + std::to_string(_session.m_pi.mi.papel_shop.current_count)
			+ ", LIMIT=" + std::to_string(_session.m_pi.mi.papel_shop.limit_count) + ", LAST_UPDATE=" 
			+ _formatDate(_session.m_pi.mi.papel_shop_last_update) + "]", CL_FILE_LOG_AND_CONSOLE));

	// UPDATE ON DB
	snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdatePapelShopInfo(_session.m_pi.uid, _session.m_pi.mi.papel_shop, _session.m_pi.mi.papel_shop_last_update), PapelShopSystem::SQLDBResponse, nullptr);

	LEAVE_CHECK;

	CATCH_CHECK("updateDiaPlayer");
	END_CHECK;
}

void PapelShopSystem::updatePlayerCount(player& _session) {
	CHECK_SESSION("updatePlayerCount");

	TRY_CHECK;

	if (isLimittedPerDay()) {

		_session.m_pi.mi.papel_shop.current_count++;

#if _PS_COM_REDUCE_LIMIT == 1
		if (--_session.m_pi.mi.papel_shop.remain_count == 0) {	// Zerou tira 50 do limite do player

			// Não diminui o limite deixa ele em 100 por dia mesmo
			if (_session.m_pi.mi.papel_shop.limit_count > 50)
				_session.m_pi.mi.papel_shop.limit_count = 50;
			if (_session.m_pi.mi.papel_shop.limit_count > 30)
				_session.m_pi.mi.papel_shop.limit_count = 30;
			else
				_session.m_pi.mi.papel_shop.limit_count = 30;
		}
#else // Sem limit
		--_session.m_pi.mi.papel_shop.remain_count;
#endif // _PS_COM_LIMIT

		// UPDATE ON DB
		snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdatePapelShopInfo(_session.m_pi.uid, _session.m_pi.mi.papel_shop, _session.m_pi.mi.papel_shop_last_update), PapelShopSystem::SQLDBResponse, nullptr);
	}

	LEAVE_CHECK;

	CATCH_CHECK("updatePlayerCount");
	END_CHECK;
}

void PapelShopSystem::updateConfig(ctx_papel_shop& _ps) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_ctx_ps = _ps;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

uint64_t PapelShopSystem::getPriceNormal() {

	uint64_t normal = 0ull;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	normal = m_ctx_ps.price_normal;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return normal;
}

uint64_t PapelShopSystem::getPriceBig() {

	uint64_t big = 0ull;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	big = m_ctx_ps.price_big;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return big;
}

WarehouseItemEx* PapelShopSystem::hasCoupon(player& _session) {
	CHECK_SESSION("hasCoupon");

	WarehouseItemEx *pWi = nullptr;

	TRY_CHECK;

	for (auto& el : m_ctx_psc) {
		
		if (el.second.active && (pWi = _session.m_pi.findWarehouseItemByTypeid(el.second._typeid)) != nullptr) {

			LEAVE_CHECK;

			return pWi;
		}
	}

	LEAVE_CHECK;

	CATCH_CHECK("hasCoupon");
	END_CHECK;

	return nullptr;
}

std::vector< ctx_papel_shop_ball > PapelShopSystem::dropBalls(player& _session) {
	CHECK_SESSION("dropBalls");

	if (!isLoad())
		throw exception("[PapelShopSystem::dropBalls][Error] O Papel Shop System nao foi carregado ainda, carregue ele primeiro antes de tentar dropar umas Balls", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 3, 0));

	std::vector< ctx_papel_shop_ball > v_ball;
	ctx_papel_shop_ball ctx_b{ 0 };

	TRY_CHECK;

	Lottery lottery((uint64_t)&m_ctx_psi);

	// Pega o Rate do Game Server
	auto rate_cookie_server = sgs::gs::getInstance().getInfo().rate.papel_shop_cookie_item / 100.f;
	auto rate_rare_server = sgs::gs::getInstance().getInfo().rate.papel_shop_rare_item / 100.f;

	for (auto& el : m_ctx_psi)
		if (el.active && (el.numero == -1 || el.numero == m_ctx_ps.numero))
			lottery.push((uint32_t)(el.probabilidade * (el.tipo == PST_COOKIE ? rate_cookie_server : (el.tipo == PST_RARE ? rate_rare_server : /*(NORMAL)*/1.f))), (size_t)&el);

	// Número de balls
	unsigned char num_ball = PAPEL_SHOP_MIN_BALL + (sRandomGen::getInstance().rIbeMt19937_64_chrono() % (PAPEL_SHOP_MAX_BALL - PAPEL_SHOP_MIN_BALL + 1));

	Lottery::LotteryCtx *lc = nullptr;
	
	do {

		CHECK_SESSION("dropBalls");

		// Sortea um valor
		lc = lottery.spinRoleta();

		if (lc == nullptr)
			throw exception("[PapelShopSystem::dropsBalls][Error] nao conseguiu sortear Bola. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 1, 0));

		ctx_b.clear();

		auto ctx_psi = (ctx_papel_shop_item*)lc->value;

		// Player já tem o item, e nao pode ter duplicate, sortea um novo para ele
		if ((!sIff::getInstance().IsCanOverlapped(ctx_psi->_typeid) || sIff::getInstance().getItemGroupIdentify(ctx_psi->_typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(ctx_psi->_typeid))
			continue;

		// Color
		ctx_b.color = PAPEL_SHOP_BALL_COLOR(sRandomGen::getInstance().rIbeMt19937_64_chrono() % (PAPEL_SHOP_BALL_COLOR::PSBC_RED + 1));

		// Raro Item Sempre é a qntd minima
		if (ctx_psi->tipo == PST_RARE)
			ctx_b.qntd = PAPEL_SHOP_ITEM_MIN_QNTD;
		else
			ctx_b.qntd = PAPEL_SHOP_ITEM_MIN_QNTD + (sRandomGen::getInstance().rIbeMt19937_64_chrono() % (PAPEL_SHOP_ITEM_MAX_QNTD - PAPEL_SHOP_ITEM_MIN_QNTD + 1));

		// Item
		ctx_b.ctx_psi = *ctx_psi;

		// Add Ball ao vector de bolas dropadas
		v_ball.push_back(ctx_b);

		// Decrementa o num_ball, que uma bola já foi dropada
		num_ball--;

	} while (num_ball > 0);

	LEAVE_CHECK;

	CATCH_CHECK("dropBalls");
	END_CHECK;
	
	return v_ball;
}

std::vector< ctx_papel_shop_ball > PapelShopSystem::dropBigBall(player& _session) {
	CHECK_SESSION("dropBigBall");

	if (!isLoad())
		throw exception("[PapelShopSystem::dropBigBall][Error] O Papel Shop System nao foi carregado ainda, carregue ele primeiro antes de tentar dropar uma Big Ball", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 3, 0));

	std::vector< ctx_papel_shop_ball > v_ball;
	ctx_papel_shop_ball ctx_b{ 0 };

	TRY_CHECK;

	Lottery lottery((uint64_t)&m_ctx_psi);

	// Pega o Rate do Game Server
	auto rate_cookie_server = sgs::gs::getInstance().getInfo().rate.papel_shop_cookie_item / 100.f;
	auto rate_rare_server = sgs::gs::getInstance().getInfo().rate.papel_shop_rare_item / 100.f;

	for (auto& el : m_ctx_psi)
		if (el.active && (el.numero == -1 || el.numero == m_ctx_ps.numero))
			lottery.push((uint32_t)(el.probabilidade * (el.tipo == PST_COOKIE ? rate_cookie_server : (el.tipo == PST_RARE ? rate_rare_server : 1.f))), (size_t)&el);

	// Número de balls
	unsigned char num_ball = PAPEL_SHOP_BIG_BALL;

	Lottery::LotteryCtx *lc = nullptr;

	do {

		CHECK_SESSION("dropBigBall");

		// Sortea um valor
		lc = lottery.spinRoleta();

		if (lc == nullptr)
			throw exception("[PapelShopSystem::dropsBalls][Error] nao conseguiu sortear Bola. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PAPEL_SHOP_SYSTEM, 1, 0));

		ctx_b.clear();

		auto ctx_psi = (ctx_papel_shop_item*)lc->value;

		// Player já tem o item, e nao pode ter duplicate, sortea um novo para ele
		if ((!sIff::getInstance().IsCanOverlapped(ctx_psi->_typeid) || sIff::getInstance().getItemGroupIdentify(ctx_psi->_typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(ctx_psi->_typeid))
			continue;

		// Color
		ctx_b.color = PAPEL_SHOP_BALL_COLOR(sRandomGen::getInstance().rIbeMt19937_64_chrono() % (PAPEL_SHOP_BALL_COLOR::PSBC_RED + 1));

		// Raro Item Sempre é a qntd minima
		if (ctx_psi->tipo == PST_RARE)
			ctx_b.qntd = PAPEL_SHOP_ITEM_MIN_QNTD;
		else
			ctx_b.qntd = PAPEL_SHOP_ITEM_MIN_QNTD + (sRandomGen::getInstance().rIbeMt19937_64_chrono() % (PAPEL_SHOP_ITEM_MAX_QNTD - PAPEL_SHOP_ITEM_MIN_QNTD + 1));

		// Item
		ctx_b.ctx_psi = *ctx_psi;

		// Add Ball ao vector de bolas dropadas
		v_ball.push_back(ctx_b);

		// Decrementa o num_ball, que uma bola já foi dropada
		num_ball--;

	} while (num_ball > 0);

	LEAVE_CHECK;

	CATCH_CHECK("dropBigBall");
	END_CHECK;

	return v_ball;
}

void PapelShopSystem::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

#ifdef _DEBUG
	// Classe estatica não pode passar o ponteiro dela, por ser estática, então passa nullptr
	if (_arg == nullptr)
		// Static class
		_smp::message_pool::getInstance().push(new message("[PapelShopSystem::SQLDBResponse][WARNING] _arg is nullptr", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[PapelShopSystem::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	switch (_msg_id) {
	case 1: // Update Papel Shop Config
	{
		auto cmd_upsc = reinterpret_cast< CmdUpdatePapelShopConfig* >(&_pangya_db);

		if (cmd_upsc->isUpdated())
			_smp::message_pool::getInstance().push(new message("[PapelShopSystem::SQLDBResponse][Log] Atualizou Papel Shop Config[" + cmd_upsc->getInfo().toString() + "] com sucesso", CL_FILE_LOG_AND_CONSOLE));
		else {

			// Não conseguiu atualizar papel shop config, por que outro sistema atualizou primeiro, atualiza o papel shop config com os dados retornados
			sPapelShopSystem::getInstance().updateConfig(cmd_upsc->getInfo());

			_smp::message_pool::getInstance().push(new message("[PapelShopSystem::SQLDBResposne][Log] Atualizou Papel Shop Config[" 
					+ cmd_upsc->getInfo().toString() + "] com os dados do DB, mas quem atualizou no DB foi outro sistema.", CL_FILE_LOG_AND_CONSOLE));
		}

		break;
	}
	case 2:	// Update Papel Shop Player Info
	{
		// Aqui não usa por que é um update
		break;
	}
	case 3:	// Update Papel Shop Last Day Update do player
	{
		// Aqui não usa por que é um update
		break;
	}
	case 0:
	default:
		break;
	}
}
