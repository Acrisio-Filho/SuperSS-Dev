// Arquivo box_system.cpp
// Criado em 15/07/2018 as 21:48 por Acrisio
// Implementa��o da classe BoxSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "box_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PANGYA_DB/cmd_box_info.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include <algorithm>

#include "../UTIL/lottery.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

#define CHECK_SESSION(_method) { \
	if (!_session.getState() || !_session.isConnected() || _session.isQuit()) \
		throw exception("[BoxSystem::" + std::string((_method)) + "][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 1, 0)); \
} \

#define CHECK_SESSION_AND_BOX(_method) { \
	CHECK_SESSION((_method)); \
	\
	if (!isLoad()) \
		throw exception("[BoxSystem::" + std::string((_method)) + "][Error] Box System not loadded, please call load method first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 2, 0)); \
	\
	if (_ctx_b._typeid == 0) \
		throw exception("[BoxSystem::" + std::string((_method)) + "][Error] box _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 3, 0)); \
	\
	if (_ctx_b.item.empty()) \
		throw exception("[BoxSystem::" + std::string((_method)) + "][Error] box is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 4, 0)); \
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
								_smp::message_pool::getInstance().push(new message("[BoxSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[BoxSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

using namespace stdA;

//std::map< uint32_t, ctx_box > BoxSystem::m_box;
//bool BoxSystem::m_load = false;

BoxSystem::BoxSystem() : m_load(false), m_box{} {

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

BoxSystem::~BoxSystem() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void BoxSystem::initialize() {

	TRY_CHECK;

	// Carrega as box do banco de dados
	CmdBoxInfo cmd_bi(true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_bi, nullptr, nullptr);

	cmd_bi.waitEvent();

	if (cmd_bi.getException().getCodeError() != 0)
		throw cmd_bi.getException();

	m_box = cmd_bi.getInfo();

//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[BoxSystem::initialize][Log] Box System Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
//#else
	//_smp::message_pool::getInstance().push(new message("[BoxSystem::initialize][Log] Box System Carregado com sucesso!", CL_ONLY_FILE_LOG));
//#endif // _DEBUG

	// Carregou com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void BoxSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_box.empty())
		m_box.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void BoxSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool BoxSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = (m_load && !m_box.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

ctx_box* BoxSystem::findBox(uint32_t _typeid) {

	if (!isLoad())
		throw exception("[BoxSystem::findBox][Error] Box System not loadded, please call load method first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 2, 0));

	TRY_CHECK;

	auto it = std::find_if(m_box.begin(), m_box.end(), [&](auto& el) {
		return el.second._typeid == _typeid;
	});

	if (it != m_box.end()) {

		LEAVE_CHECK;

		return &it->second;
	}

	LEAVE_CHECK;

	CATCH_CHECK("findBox");
	END_CHECK;

	return nullptr;
}

ctx_box_item* BoxSystem::drawBox(player& _session, ctx_box& _ctx_b) {
	CHECK_SESSION_AND_BOX("drawBox");

	ctx_box_item *bi = nullptr;

	TRY_CHECK;
	
	Lottery lottery((uint64_t)&_ctx_b);

	for (auto& el : _ctx_b.item) {

		if (el.active) {

			// Verifica qual o tipo da box, se for 100% raro, ent�o s� coloca os raros se j� pegou todos os raros colocar os Lucky reward
			if (_ctx_b.tipo != BOX_TYPE::ALL_RARE_OR_LUCKY_REWARD || el.raridade != BOX_TYPE_RARETY::R_NORMAL) {
				
				// S� pode add os itens que o player n�o tem ou pode ter duplicada
				if (/*Nega*/!(!el.duplicar && (!sIff::getInstance().IsCanOverlapped(el._typeid) || sIff::getInstance().getItemGroupIdentify(el._typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(el._typeid)))
					lottery.push(el.probabilidade, (size_t)&el);

			}

		}
	}

	// Player j� tem todos os raros da o Lucky reward para ele
	if (_ctx_b.tipo == BOX_TYPE::ALL_RARE_OR_LUCKY_REWARD && lottery.getCountItem() == 0) {

		for (auto& el : _ctx_b.item) {

			// Add na roleta os itens normais para o player ganhar que ele j� ganhou todos os raros
			if (el.active && el.raridade == BOX_TYPE_RARETY::R_NORMAL) {

				// S� pode add os itens que o player n�o tem ou pode ter duplicada
				if (/*Nega*/!(!el.duplicar && (!sIff::getInstance().IsCanOverlapped(el._typeid) || sIff::getInstance().getItemGroupIdentify(el._typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(el._typeid)))
					lottery.push(el.probabilidade, (size_t)&el);

			}
		}
	}

	// Verifica se o tem algum item para o player ganhar raro ou Lucky reward
	if (lottery.getCountItem() == 0)
		throw exception("[BoxSystem::drawBox][Error] player ja tem todos os itens e a Box[Typeid=" + std::to_string(_ctx_b._typeid) 
				+ ", ID=" + std::to_string(_ctx_b.id) + "] nao tem o Lucky reward ou ele nao pode ter duplicata.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 8, 0));

	Lottery::LotteryCtx *lc = nullptr;
	uint32_t count = 1;	// 1 Item

	do {
		
		CHECK_SESSION("drawBox");

		lc = lottery.spinRoleta(true);	// Remove o item sorteado para n�o sortear ele novamente

		if (lc == nullptr)
			throw exception("[BoxSystem::drawBox][Error] nao conseguiu sortear um item, erro na hora de rodar a roleta", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 6, 0));

		// Tempor�rio Box Item
		bi = (ctx_box_item*)lc->value;

		// Contianua que o player j� tem esse item, e n�o pode ter duplicatas dele
		// Colocar l� em cima
		/*if (!bi->duplicar && (!sIff::getInstance().IsCanOverlapped(bi->_typeid) || sIff::getInstance().getItemGroupIdentify(bi->_typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(bi->_typeid))
			continue;*/

		// Decrementa o count, que 1 item voi sorteado
		--count;

	} while (count > 0);

	LEAVE_CHECK;

	CATCH_CHECK("drawBox");
	END_CHECK;

	return bi;
}