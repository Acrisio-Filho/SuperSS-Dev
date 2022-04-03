// Arquivo bot_gm_event.cpp
// Criado em 03/11/2020 as 20:06 por Acrisio
// Implementa��o da classe BotGMEvent

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "bot_gm_event.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_bot_gm_event_info.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../UTIL/lottery.hpp"
#include "../../Projeto IOCP/UTIL/random_gen.hpp"

#include <algorithm>

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
								_smp::message_pool::getInstance().push(new message("[BotGMEvent::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[BotGMEvent::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

constexpr uint32_t MAX_REWARD_PER_ROUND = 3u;

using namespace stdA;

BotGMEvent::BotGMEvent() : m_rt(), m_rewards(), m_load(false), m_st{ 0u } {

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

BotGMEvent::~BotGMEvent() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void BotGMEvent::clear() {

	TRY_CHECK;

	if (!m_rt.empty()) {
		m_rt.clear();
		m_rt.shrink_to_fit();
	}

	if (!m_rewards.empty()) {
		m_rewards.clear();
		m_rewards.shrink_to_fit();
	}

	m_load = false;

	LEAVE_CHECK;
	CATCH_CHECK("clear");

	m_load = false;

	END_CHECK;
}

void BotGMEvent::load() {

	if (isLoad())
		clear();

	initialize();
}

bool BotGMEvent::isLoad() {
	
	bool isload = false;

	TRY_CHECK;

	isload = (m_load);

	LEAVE_CHECK;
	CATCH_CHECK("isLoad");

	isload = false;

	END_CHECK;

	return isload;
}

void BotGMEvent::initialize() {

	TRY_CHECK;

		CmdBotGMEventInfo cmd_bgei(true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_bgei, nullptr, nullptr);

		cmd_bgei.waitEvent();

		if (cmd_bgei.getException().getCodeError() != 0)
			throw cmd_bgei.getException();

		m_rt = cmd_bgei.getTimeInfo();
		m_rewards = cmd_bgei.getRewardInfo();

		// Log
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[BotGMEvent::initialize][Log] Carregou " + std::to_string(m_rt.size()) + " times.", CL_FILE_LOG_AND_CONSOLE));

		for (auto& el : m_rt)
			_smp::message_pool::getInstance().push(new message("[BotGMEvent::initialize][Log] Time[Start=" + _formatTime(el.m_start) + ", End=" + _formatTime(el.m_end) + ", Channel_id=" + std::to_string((unsigned short)el.m_channel_id) +"].", CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("[BotGMEvent::initialize][Log] Bot GM Event System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
/*#else
		_smp::message_pool::getInstance().push(new message("[BotGMEvent::initialize][Log] Carregou " + std::to_string(m_rt.size()) + " times.", CL_ONLY_FILE_LOG));

		for (auto& el : m_rt)
			_smp::message_pool::getInstance().push(new message("[BotGMEvent::initialize][Log] Time[Start=" + _formatTime(el.m_start) + ", End=" + _formatTime(el.m_end) + ", Channel_id=" + std::to_string((unsigned short)el.m_channel_id) + "].", CL_ONLY_FILE_LOG));

		_smp::message_pool::getInstance().push(new message("[BotGMEvent::initialize][Log] Bot GM Event System carregado com sucesso!", CL_ONLY_FILE_LOG));
#endif // _DEBUG*/

		// Carregou com sucesso
		m_load = true;

	LEAVE_CHECK;
	CATCH_CHECK("initialize");

		// Relan�a para o server tomar as provid�ncias
		throw;

	END_CHECK;
}

bool BotGMEvent::checkTimeToMakeRoom() {
	
	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[BotGMEvent::checkTimeToMakeRoom][Error] Bot GM Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	bool is_time = false;

	TRY_CHECK;

	GetLocalTime(&m_st);

	auto it = std::find_if(m_rt.begin(), m_rt.end(), [&](auto& _el) {
		return _el.isBetweenTime(m_st);
	});

	is_time = (it != m_rt.end());

	LEAVE_CHECK;
	CATCH_CHECK("checkTimeToMakeRoom");
	END_CHECK;

	return is_time;
}

bool BotGMEvent::messageSended() {
	
	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[BotGMEvent::messageSended][Error] Bot GM Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	bool is_sended = false;

	TRY_CHECK

		GetLocalTime(&m_st);

		auto it = std::find_if(m_rt.begin(), m_rt.end(), [&](auto& _el) {
			return _el.isBetweenTime(m_st);
		});

		is_sended = (it != m_rt.end() && it->m_sended_message);

	LEAVE_CHECK
	CATCH_CHECK("messageSended")
	END_CHECK

	return is_sended;
}

void BotGMEvent::setSendedMessage() {

	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[BotGMEvent::setSendedMessage][Error] Bot GM Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	TRY_CHECK

		GetLocalTime(&m_st);

		// Zera todas os intervalos que n�o est� na hora, e o intervalo que est� na hora seta ele
		std::for_each(m_rt.begin(), m_rt.end(), [&](auto& _el) {
			
			if (_el.isBetweenTime(m_st))
				_el.m_sended_message = true;
			else
				_el.m_sended_message = false;
		});

	LEAVE_CHECK
	CATCH_CHECK("setSendedMessage")
	END_CHECK
}

stRangeTime* BotGMEvent::getInterval() {
	
	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[BotGMEvent::getInterval][Error] Bot GM Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	stRangeTime *rt = nullptr;

	TRY_CHECK

		GetLocalTime(&m_st);

		auto it = std::find_if(m_rt.begin(), m_rt.end(), [&](auto& _el) {
			return _el.isBetweenTime(m_st);
		});

		if (it != m_rt.end())
			rt = &(*it);

	LEAVE_CHECK
	CATCH_CHECK("getInterval")

		rt = nullptr;

	END_CHECK

	return rt;
}

std::vector< stReward > BotGMEvent::calculeReward() {
	
	std::vector< stReward > v_reward;

	TRY_CHECK;

	// No m�ximo 3 pr�mios
	uint32_t num_r = (uint32_t)sRandomGen::getInstance().rIbeMt19937_64_rdeviceRange(1, MAX_REWARD_PER_ROUND);

	Lottery lottery(std::clock());
	Lottery::LotteryCtx* ctx = nullptr;

	for (auto& el : m_rewards)
		lottery.push(el.rate, (size_t)&el);

	bool remove_to_roleta = num_r < lottery.getCountItem();

	// Not loop infinite
	num_r = num_r > lottery.getCountItem() ? lottery.getCountItem() : num_r;

	while (num_r > 0) {

		if ((ctx = lottery.spinRoleta(remove_to_roleta)) == nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[BotGMEvent::calculeReward][Error][WARNING] nao conseguiu sortear um reward na lottery.", CL_FILE_LOG_AND_CONSOLE));

			// Continua
			continue;
		}

		v_reward.push_back(*(stReward*)ctx->value);

		// decrease num_r(reward)
		num_r--;
	}

	LEAVE_CHECK;
	CATCH_CHECK("calculeReward");
	END_CHECK;

	return v_reward;
}


