// Arquivo login_reward_system.cpp
// Criado em 27/10/2020 as 12:25 por Acrisio
// Implementa��o da classe LoginRewardSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "login_reward_system.hpp"

#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_add_login_reward_player.hpp"
#include "../PANGYA_DB/cmd_login_reward_info.hpp"
#include "../PANGYA_DB/cmd_login_reward_player_info.hpp"
#include "../PANGYA_DB/cmd_update_login_reward.hpp"
#include "../PANGYA_DB/cmd_update_login_reward_player.hpp"

#include "item_manager.h"

#include "mail_box_manager.hpp"

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
								_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

#define RETURNN_CHECK(_ret_value)	{ \
										LEAVE_CHECK; \
										return (_ret_value); \
									} \

#define RETURN_CHECK()				{ \
										LEAVE_CHECK; \
										return; \
									} \

#define CHECK_SESSION(method) if (!_session.getState()) \
									throw exception("[LoginRewardSystem::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_REWARD_SYSTEM, 1, 0)); \

using namespace stdA;

LoginRewardSystem::LoginRewardSystem() : m_events() {

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

LoginRewardSystem::~LoginRewardSystem() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void LoginRewardSystem::initialize() {

	TRY_CHECK;

	// Carrega a lista de eventos
	CmdLoginRewardInfo cmd_lri(true); // Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_lri, nullptr, nullptr);

	cmd_lri.waitEvent();

	if (cmd_lri.getException().getCodeError() != 0)
		throw cmd_lri.getException();

	m_events = cmd_lri.getInfo();

	for (auto& el_e : m_events) {

		// Verifica se j� venceu
		if (!isEmpty(el_e.end_date) && getLocalTimeDiff(el_e.end_date) > 0ll) {

			el_e.is_end = true;

			// Atualiza aqui no banco de dados o evento
			snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdateLoginReward(el_e.id, el_e.is_end), LoginRewardSystem::SQLDBResponse, this);
		}
	}

	std::string log = "";

	for (auto& el_e : m_events)
		log += "\nLogin Reward[" + el_e.toString() + "]";

	// Log
	_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::initialize][Log] Login Reward System Event - Carregou: " + log, CL_FILE_LOG_AND_CONSOLE));

	//#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::initialize][Log] Login Reward System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
	//#else
		//_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::initialize][Log] Login Reward System carregado com sucesso!", CL_ONLY_FILE_LOG));
	//#endif // _DEBUG

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void LoginRewardSystem::clear() {

	TRY_CHECK;

	if (!m_events.empty()) {
		m_events.clear();
		m_events.shrink_to_fit();
	}

	m_load = false;

	LEAVE_CHECK;
	CATCH_CHECK("clear");
	END_CHECK;
}

void LoginRewardSystem::load() {

	if (isLoad())
		clear();

	initialize();
}

bool LoginRewardSystem::isLoad() {
	
	bool isLoad = false;

	TRY_CHECK;

	isLoad = (m_load);

	LEAVE_CHECK;
	CATCH_CHECK("isLoad");
	END_CHECK;

	return isLoad;
}

void LoginRewardSystem::checkRewardLoginAndSend(player& _session) {
	CHECK_SESSION("checkRewardLoginAndSend");

	TRY_CHECK;

	// Carrega lista de player do evento
	CmdLoginRewardPlayerInfo cmd_lrpi(_session.m_pi.uid, true); // Waiter

	// Check All Event Enabled
	for (auto& el_e : m_events) {

		if (el_e.is_end)
			continue;

		// Se tiver data e passou dela, encerra esse evento
		if (!isEmpty(el_e.end_date) && getLocalTimeDiff(el_e.end_date) > 0ll) {

			el_e.is_end = true;

			// Atualiza aqui no banco de dados o evento
			snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdateLoginReward(el_e.id, el_e.is_end), LoginRewardSystem::SQLDBResponse, this);

			// Continua
			continue;
		}

		// Pega info do player no banco de dados
		cmd_lrpi.setId(el_e.id);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_lrpi, nullptr, nullptr);

		cmd_lrpi.waitEvent();

		if (cmd_lrpi.getException().getCodeError() != 0) {

			// Log
			_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::checkRewardLoginAndSend][Error][WARNING] " + cmd_lrpi.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			// Continua
			continue;
		}

		auto p = cmd_lrpi.getInfo();
		// ---- end get player info event

		if (p.id == 0ull && p.uid == 0u) { // N�o tem, cria um novo

			p = stPlayerState{ 0ull, _session.m_pi.uid, 1u, 0u, { 0u } };

			if (isEmpty(p.update_date))
				GetLocalTime(&p.update_date);

			// Add o player ao banco de dados aqui
			CmdAddLoginRewardPlayer cmd_alrp(el_e.id, p, true); // Waiter

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_alrp, nullptr, nullptr);

			cmd_alrp.waitEvent();

			// Error ao adicionar o player no banco de dados
			if (cmd_alrp.getException().getCodeError() != 0) {

				// Log
				_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::checkRewardLoginAndSend][Error][WARNING] " + cmd_alrp.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

				// Continua
				continue;
			}

			if (!cmd_alrp.isGood()) {

				// Log
				_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::checkRewardLoginAndSend][Error][WARINIG] nao conseguiu adicionar o Player[UID=" 
						+ std::to_string(_session.m_pi.uid) + "] no Login Reward[ID=" + std::to_string(el_e.id) + "] no banco de dados, nao retornou o id do player criado.", CL_FILE_LOG_AND_CONSOLE));

				// Continua
				continue;
			}

			// Pega o Id que foi gerado quando adicionou no banco de dados
			p = cmd_alrp.getPlayerState();

			// Log
			_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::checkRewardLoginAndSend][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
					+ "] Primeira participacao do player[" + p.toString() + "] no Login Reward Event[" + el_e.toString() + "]", CL_FILE_LOG_AND_CONSOLE));

		}else {

			// Continua o player j� terminou esse evento
			if (p.is_clear)
				continue;

			// Ainda n�o � outro dia
			if (getLocalDateDiff(p.update_date) <= 0ll)
				continue;
			
			p.count_days++; // Update count

			// Update time
			GetLocalTime(&p.update_date);

			// Atualiza aqui o state do player no banco de dados
			snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdateLoginRewardPlayer(p), LoginRewardSystem::SQLDBResponse, this);
		}

		// Verifica quantas vezes tem que logar para receber o pr�mio

		// Player n�o tem o n�mero de login's necess�rios
		if (p.count_days < el_e.days_to_gift)
			continue;

		p.count_seq++;

		if (el_e.type == stLoginReward::eTYPE::N_TIME) {

			if (p.count_seq < el_e.n_times_gift)
				p.count_days = 0u;
			else
				p.is_clear = true;

		}else if (el_e.type == stLoginReward::eTYPE::FOREVER)
			p.count_days = 0u;

		// Atualiza aqui o state do plauer no banco de dados
		snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdateLoginRewardPlayer(p), LoginRewardSystem::SQLDBResponse, this);

		// Log
		_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::checkRewardLoginAndSend][Log] Player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] ganhou item[" + el_e.item_reward.toString() + "] no Login Reward[" + el_e.toString() + "] com [DAYS=" + std::to_string(p.count_days) 
				+ ", SEQ=" + std::to_string(p.count_seq) + ", IS_CLEAR=" + std::string(p.is_clear ? "TRUE" : "FALSE") + "]", CL_FILE_LOG_AND_CONSOLE));

		// Send Gift
		sendGiftToPlayer(_session, el_e);
	}

	LEAVE_CHECK;
	CATCH_CHECK("checkRewardLoginAndSend");
	END_CHECK;
}

void LoginRewardSystem::updateLoginReward() {

	TRY_CHECK;

	for (auto& el_e : m_events) {

		if (el_e.is_end)
			continue;

		if (!isEmpty(el_e.end_date) && getLocalTimeDiff(el_e.end_date) > 0ll) {

			el_e.is_end = true;

			// Atualiza aqui no banco de dados o evento
			snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdateLoginReward(el_e.id, el_e.is_end), LoginRewardSystem::SQLDBResponse, this);
		}
	}

	LEAVE_CHECK;
	CATCH_CHECK("updateLoginReward");
	END_CHECK;
}

void LoginRewardSystem::sendGiftToPlayer(player& _session, stLoginReward& _lr) {
	CHECK_SESSION("sendGiftToPlayer");

	// Lambda[getItemName]
	auto getItemName = [](uint32_t _typeid) -> std::string {

		std::string ret = "";

		auto base = sIff::getInstance().findCommomItem(_typeid);

		if (base != nullptr)
			ret = std::string(base->name);

		return ret;
	};

	try {

		stItem item{ 0 };
		BuyItem bi{ 0 };

		// Limpa
		bi.clear();
		item.clear();

		// Initialize
		bi.id = -1;
		bi._typeid = _lr.item_reward._typeid;
		bi.qntd = _lr.item_reward.qntd;
		bi.time = (unsigned short)_lr.item_reward.qntd_time;

		item_manager::initItemFromBuyItem(_session.m_pi, item, bi, false, 0, 0, 1/*~nao Check Level*/);

		if (item._typeid == 0)
			_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::sendGiftToPlayer][Error][WARNING] tentou enviar o reward para o player[UID="
					+ std::to_string(_session.m_pi.uid) + "] o Item[" + _lr.item_reward.toString() + "], mas nao conseguiu inicializar o item. Bug", CL_FILE_LOG_AND_CONSOLE));

		auto msg = std::string("Login Reward System - \"" + _lr.getName() + "\": item[ " + getItemName(_lr.item_reward._typeid) + " ]");

		if (MailBoxManager::sendMessageWithItem(0, _session.m_pi.uid, msg, item) <= 0)
			_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::sendGiftToPlayer][Error][WARNING] tentou enviar reward para o player[UID="
					+ std::to_string(_session.m_pi.uid) + "] o Item[" + _lr.item_reward.toString() + "], mas nao conseguiu colocar o item no mail box dele. Bug", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::sendGiftToPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void LoginRewardSystem::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
#ifdef _DEBUG
		// Static class
		_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::SQLDBResponse][WARNING] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG
		return;
	}

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto gts = reinterpret_cast<LoginRewardSystem*>(_arg);

	switch (_msg_id) {
	case 1:	// Update Login Reward
	{

		auto cmd_ulr = reinterpret_cast< CmdUpdateLoginReward* >(&_pangya_db);

		// Log
		_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::SQLDBResponse][Log] Atualizaou Login Reward[ID=" 
				+ std::to_string(cmd_ulr->getId()) + ", IS_END=" + std::string(cmd_ulr->getIsEnd() ? "TRUE" : "FALSE") + "]", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 2: // Update Login Reward Player
	{

		auto cmd_ulrp = reinterpret_cast< CmdUpdateLoginRewardPlayer* >(&_pangya_db);

		// Log
		_smp::message_pool::getInstance().push(new message("[LoginRewardSystem::SQLDBResponse][Log] Atualizou o Player[" 
				+ cmd_ulrp->getPlayerState().toString() + "] do Login Reward.", CL_FILE_LOG_AND_CONSOLE));

		break;
	}
	case 0:
	default:
		break;
	}
};
