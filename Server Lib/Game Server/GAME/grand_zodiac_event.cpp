// Arquivo grand_zodiac_event.cpp
// Criado em 26/06/2020 as 13:13 por Acrisio
// Implementa��o da classe GrandZodiacEvent

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "grand_zodiac_event.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_grand_zodiac_event_info.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

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
								_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

/*
	TRY_CHECK
	LEAVE_CHECK
	CATCH_CHECK()
	END_CHECK
*/

using namespace stdA;

GrandZodiacEvent::GrandZodiacEvent() : m_rt(), m_load(false), m_st{ 0 } {

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

GrandZodiacEvent::~GrandZodiacEvent() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void GrandZodiacEvent::clear() {

	TRY_CHECK

		if (!m_rt.empty()) {
			m_rt.clear();
			m_rt.shrink_to_fit();
		}

		m_load = false;

	LEAVE_CHECK
	CATCH_CHECK("clear")

		m_load = false;

	END_CHECK
}

void GrandZodiacEvent::load() {

	if (isLoad())
		clear();

	initialize();
}

bool GrandZodiacEvent::isLoad() {

	bool isload = false;

	TRY_CHECK

		isload = (m_load && !m_rt.empty());

	LEAVE_CHECK
	CATCH_CHECK("isLoad")

		isload = false;

	END_CHECK

	return isload;
}

bool GrandZodiacEvent::checkTimeToMakeRoom() {

	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::checkTimeToMakeRoom][Error] Grand Zodiac Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	bool is_time = false;

	TRY_CHECK

		GetLocalTime(&m_st);

		auto it = std::find_if(m_rt.begin(), m_rt.end(), [&](auto& _el) {
			return _el.isBetweenTime(m_st);
		});

		is_time = (it != m_rt.end());

	LEAVE_CHECK
	CATCH_CHECK("checkTimeToMakeRoom")
	END_CHECK

	return is_time;
}

bool GrandZodiacEvent::messageSended() {
	
	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::messageSended][Error] Grand Zodiac Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

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

void GrandZodiacEvent::setSendedMessage() {

	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::setSendedMessage][Error] Grand Zodiac Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

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

range_time* GrandZodiacEvent::getInterval() {
	
	if (!isLoad()) {

		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::getInterval][Error] Grand Zodiac Event not have initialized, please call init function first.", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	range_time *rt = nullptr;

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

void GrandZodiacEvent::initialize() {

	TRY_CHECK;

		CmdGrandZodiacEventInfo cmd_gzei(true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_gzei, nullptr, nullptr);

		cmd_gzei.waitEvent();

		if (cmd_gzei.getException().getCodeError() != 0)
			throw cmd_gzei.getException();

		m_rt = cmd_gzei.getInfo();

		// Log
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::initialize][Log] Carregou " + std::to_string(m_rt.size()) + " times.", CL_FILE_LOG_AND_CONSOLE));

		for (auto& el : m_rt)
			_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::initialize][Log] Time[Start=" + _formatTime(el.m_start) + ", End=" + _formatTime(el.m_end) + ", Type=" + std::to_string((unsigned short)el.m_type) +"].", CL_FILE_LOG_AND_CONSOLE));

		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::initialize][Log] Grand Zodiac Event System carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
/*#else
		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::initialize][Log] Carregou " + std::to_string(m_rt.size()) + " times.", CL_ONLY_FILE_LOG));

		for (auto& el : m_rt)
			_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::initialize][Log] Time[Start=" + _formatTime(el.m_start) + ", End=" + _formatTime(el.m_end) + ", Type=" + std::to_string((unsigned short)el.m_type) + "].", CL_ONLY_FILE_LOG));

		_smp::message_pool::getInstance().push(new message("[GrandZodiacEvent::initialize][Log] Grand Zodiac Event System carregado com sucesso!", CL_ONLY_FILE_LOG));
#endif // _DEBUG*/

		// Carregou com sucesso
		m_load = true;

	LEAVE_CHECK
	CATCH_CHECK("initialize")

		// Relan�a para o server tomar as provid�ncias
		throw;

	END_CHECK
}
