// Arquivo timer.cpp
// Criado em 01/01/2018 por Acrisio
// Implementação da classe timer

#if defined(_WIN32)
#pragma pack(1)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "timer.h"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"
#include "../UTIL/message_pool.h"

#define LIMIT_COUNT_TRY 3	// 3x é o limite de vezes que ele vai tentar execitar a função se der outro erro diferente de ERROR_IO_PENDING

using namespace stdA;

void CALLBACK timer::callback(LPVOID lpParameter, BOOLEAN TimerOrWaitFired) {
	timer *_timer = reinterpret_cast< timer* >(lpParameter);

	if (!TimerOrWaitFired)
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[timer::callback][WARNING] TimerOrWaitFired is FALSE, were TRUE.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[timer::callback][WARNING] TimerOrWaitFired is FALSE, were TRUE.", CL_ONLY_FILE_LOG));
#endif
	
	_timer->hasTimerTick();
};

timer::timer(
#if defined(_WIN32)
	HANDLE _timerQueue, 
#elif defined(__linux__)
	QueueTimer* _timerQueue,
#endif
	DWORD _time, timer_param *_arg, uint32_t _tipo)
	: m_hTimerQueue(_timerQueue), m_time_fix(_time), m_arg(_arg), m_tipo(_tipo), 
		m_hTimer(INVALID_HANDLE_VALUE), 
#if defined(_WIN32)
		m_hEvent(INVALID_HANDLE_VALUE), 
#elif defined(__linux__)
		m_hEvent(nullptr), 
#endif
		m_elapsed{ 0 }, m_start{ 0 }, m_frequency{ 0 },
		m_state(STOPPED), m_table_interval() {

#if defined(_WIN32)
	if ((m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[timer::timer][Error] Ao criar evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 7, GetLastError()));

	InitializeCriticalSection(&m_cs);
	InitializeConditionVariable(&m_cv);
#elif defined(__linux__)
	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("[timer::timer][Error] Ao criar evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 7, errno));
	}

	pthread_mutexattr_t __mattr;
	pthread_mutexattr_init(&__mattr);
	pthread_mutexattr_settype(&__mattr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&m_cs, &__mattr);

	pthread_mutexattr_destroy(&__mattr);

	pthread_cond_init(&m_cv, nullptr);
#endif

	start();
}

timer::timer(
#if defined(_WIN32)
	HANDLE _timerQueue, 
#elif defined(__linux__)
	QueueTimer* _timerQueue,
#endif
	DWORD _time, timer_param *_arg, std::vector<DWORD> _table_interval, uint32_t _tipo)
	: m_hTimerQueue(_timerQueue), m_time_fix(_time), m_arg(_arg), m_table_interval(_table_interval), m_tipo(_tipo),
		m_hTimer(INVALID_HANDLE_VALUE), 
#if defined(_WIN32)
		m_hEvent(INVALID_HANDLE_VALUE), 
#elif defined(__linux__)
		m_hEvent(nullptr), 
#endif
		m_elapsed{ 0 }, m_start{ 0 }, m_frequency{ 0 },
		m_state(STOPPED) {

	if (m_table_interval.empty() && (m_tipo == PERIODIC || m_tipo == PERIODIC_INFINITE))
		throw exception("[timer::timer][Error] m_table_interval is empty with flag PERIODIC", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 9, 0));

#if defined(_WIN32)
	if ((m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[timer::timer][Error] Ao criar evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 7, GetLastError()));

	InitializeCriticalSection(&m_cs);
	InitializeConditionVariable(&m_cv);
#elif defined(__linux__)
	m_hEvent = new Event(true, 0u);

	if (!m_hEvent->is_good()) {

		delete m_hEvent;

		m_hEvent = nullptr;

		throw exception("[timer::timer][Error] Ao criar evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 7, errno));
	}

	pthread_mutexattr_t __mattr;
	pthread_mutexattr_init(&__mattr);
	pthread_mutexattr_settype(&__mattr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&m_cs, &__mattr);

	pthread_mutexattr_destroy(&__mattr);

	pthread_cond_init(&m_cv, nullptr);
#endif

	start();
};

timer::~timer() {

	try {

		destroy();

		// Espera pelo fim do timer
#if defined(_WIN32)
		if (m_hTimer != nullptr && m_hTimer != INVALID_HANDLE_VALUE)
#elif defined(__linux__)
		if (m_hTimer != INVALID_HANDLE_VALUE)
#endif
			check_event_end();

	}catch (exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_ONLY_FILE_LOG));
#endif
	}

#if defined(_WIN32)
	if (m_hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent);

	m_hEvent = INVALID_HANDLE_VALUE;

	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)

	if (m_hEvent != nullptr)
		delete m_hEvent;

	m_hEvent = nullptr;

	pthread_mutex_destroy(&m_cs);
	pthread_cond_destroy(&m_cv);
#endif

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[timer::~timer][Log] Destruiu objeto de timer com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

};

#if defined(_WIN32)
HANDLE timer::getTimer() 
#elif defined(__linux__)
int32_t timer::getTimer()
#endif
{
	return m_hTimer;
};

void timer::restart() {

	if (getState() != STOPPED && getState() != STOPPING && getState() != STOP)
		stop();

	// Espera pelo fim do timer
	if (getState() == STOPPING || getState() == PAUSING)
		check_event_end();

	start();
};

void timer::pause() {

	if (getState() == PAUSED)
		throw exception("[timer::pause][Error] Timer already is paused, start it before of pause.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 5, 0));

	_stop(PAUSE);

	if (getState() == PAUSING)
		check_event_end();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[timer::pause][Log] Timer parado com sucesso", CL_FILE_LOG_AND_CONSOLE));
#endif
};

void timer::stop() {

	if (getState() == STOPPED)
		throw exception("[timer::stop][Error] Timer already is stopped, start it before of stop.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 4, 0));

	_stop(STOP);

	if (getState() == STOPPING)
		check_event_end();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[timer::stop][Log] Timer parado com sucesso", CL_FILE_LOG_AND_CONSOLE));
#endif
};

void timer::start() {

	if (getState() == RUNNING)
		throw exception("[tmer::start][Error] Timer already is running, stop it before of init", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 2, 1));

	// Espera pelo fim do timer
	if (getState() == STOPPING || getState() == PAUSING)
		check_event_end();

	if (getState() == STOPPED)
		m_time = m_time_fix;
	
	init();

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[time::start][Log] Timer[Time=" + std::to_string(m_time) + ", Time_FIX=" + std::to_string(m_time_fix) + "] iniciado com sucesso", CL_FILE_LOG_AND_CONSOLE));
#endif
};

timer::TIMER_STATE timer::getState() {
	
	auto ret = STOPPED;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	ret = (TIMER_STATE)m_state;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ret;
};

void timer::setState(TIMER_STATE _state) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_state = _state;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

DWORD timer::getElapsed() {

#if defined(_WIN32)
	QueryPerformanceCounter(&m_elapsed);

	return (DWORD)((m_elapsed.QuadPart - m_start.QuadPart) * 1000/*milliseconds*/ / m_frequency.QuadPart);
#elif defined(__linux__)
	clock_gettime(CLOCK_TICK_TO_TIMER, &m_elapsed);

	return (uint32_t)DIFF_TICK(m_elapsed, m_start, m_frequency);
#endif
};

void timer::destroy() {

	if (
#if defined(_WIN32)
		m_hTimer != nullptr && m_hTimer != INVALID_HANDLE_VALUE 
#elif defined(__linux__)
		m_hTimer != INVALID_HANDLE_VALUE
#endif
			&& (getState() == STOPPING || getState() == PAUSING))
		check_event_end();

	// Para Timer
	_stop(STOP);

	if (!m_table_interval.empty()) {
		m_table_interval.clear();
		m_table_interval.shrink_to_fit();
	}

	if (m_arg != nullptr)
		delete m_arg;

	m_arg = nullptr;

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[timer::destroy][Log] Timer[Time=" + std::to_string(m_time) + ", Time_FIX=" + std::to_string(m_time_fix) + "] foi destruido com sucesso", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[timer::destroy][Log] Timer[Time=" + std::to_string(m_time) + ", Time_FIX=" + std::to_string(m_time_fix) + "] foi destruido com sucesso", CL_ONLY_FILE_LOG));
#endif
};

void timer::init() {

#if defined(_WIN32)
	if (!QueryPerformanceFrequency(&m_frequency))
		throw exception("[timer::init][Error] Not get QueryPerformanceFrequency", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 6, GetLastError()));
#elif defined(__linux__)
	if (clock_getres(CLOCK_TICK_TO_TIMER, &m_frequency) == -1)
		throw exception("[timer::init][Error] Not get QueryPerformanceFrequency", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 6, errno));
#endif

	if (m_arg == nullptr)
		throw exception("[timer::init][Error] Timer_param *m_arg is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 1, 0));

	if (getState() == RUNNING)
		throw exception("[timer::init][Error] Timer already is running, stop before init it.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 2, 0));

	// Espera pelo fim do timer
	if (getState() == STOPPING || getState() == PAUSING)
		check_event_end();

	if (m_hTimerQueue == nullptr)
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[timer::init][Log] Creating TimerQueueTimer in default queue.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[timer::init][Log] Creating TimerQueueTimer in default queue.", CL_ONLY_FILE_LOG));
#endif

	DWORD periodic = 0;

	if (getState() == PAUSED)
#if defined(_WIN32)
		if ((int)(m_time -= (DWORD)((m_elapsed.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart)) < 0)
#elif defined(__linux__)
		if ((int)(m_time -= (DWORD)DIFF_TICK(m_elapsed, m_start, m_frequency)) < 0)
#endif
			m_time = m_time_fix;
	
	// Ainda tenho que implementar direito a tabela de periodo de tempos
	if (!m_table_interval.empty()) {
		
		if (m_tipo == PERIODIC)
			periodic = m_table_interval[0];
		else if (m_tipo == PERIODIC_INFINITE)
			periodic = m_time;
	}

	// Resata o Event de espera de fim do callback do timer
#if defined(_WIN32)
	ResetEvent(m_hEvent);

	setState(STANDBY);	// Esperando a thread callback ser inicializada

	if (!CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue,
			(WAITORTIMERCALLBACK)timer::callback, this,
			0/*Chama o callback em 1 milliseconds, para inicializar o timer Callback*/,
			(m_tipo == PERIODIC) ? periodic : m_time/*Agora é o tempo que ele vai ficar chamando o callback em tempos e tempos*/,
			WT_EXECUTEINTIMERTHREAD))
		throw exception("[timer::init][Error] Nao conseguiu criar TimerQueueTimer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 3, GetLastError()));
#elif defined(__linux__)
	if (m_hEvent != nullptr)
		m_hEvent->reset();

	setState(STANDBY);	// Esperando a thread callback ser inicializada

	if (!QueueTimer::CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue,
			(WaitOrTimerCallback)timer::callback, this,
			0/*Chama o callback em 1 milliseconds, para inicializar o timer Callback*/,
			(m_tipo == PERIODIC) ? periodic : m_time/*Agora é o tempo que ele vai ficar chamando o callback em tempos e tempos*/,
			WT_EXECUTEINTIMERTHREAD))
		throw exception("[timer::init][Error] Nao conseguiu criar TimerQueueTimer", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 3, errno));
#endif
};

void timer::_stop(TIMER_STATE _state) {

	if (getState() == STOPPING || getState() == PAUSING) {

		check_event_end();

		// Já tinha requisitado para parar o tempo, aguardou ele terminar, agora só retorna.
		// não precisa chamar a função que deleta(pause, stop, cancel) o timer de novo
		return;
	}

#if defined(_WIN32)
	QueryPerformanceCounter(&m_elapsed);
#elif defined(__linux__)
	clock_gettime(CLOCK_TICK_TO_TIMER, &m_elapsed);
#endif

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Para não tentar excluír antes depois que entrou na critical secção e tentar excluír de novo
		if (getState() != _state + 1 && getState() != _state + 2) {

			setState((TIMER_STATE)(_state + 1));	// Executando o request

			BOOL error = 0;
			DWORD lastError = 0u;
			uint32_t count = 0u;

#if defined(_WIN32)
			do {

				if (m_hTimer != nullptr && m_hTimer != INVALID_HANDLE_VALUE && (error = DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, m_hEvent)) == 0 && (lastError = GetLastError()) != ERROR_IO_PENDING)
					_smp::message_pool::getInstance().push(new message("[timer::_stop][Error] Ao deletar timer. Error Code: " + STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 12, lastError), CL_FILE_LOG_AND_CONSOLE));
				
			} while (error == 0/*Error in DeleteTimerQueueTimer*/ && lastError != ERROR_IO_PENDING && count++ < LIMIT_COUNT_TRY);	// Tenta De novo

			// Ele está esperando o callback do timer terminar
			if (lastError == ERROR_IO_PENDING) {
			
				// Libera para a função Callback poder limpar os dados dela antes de verifica o final dela
				LeaveCriticalSection(&m_cs);

#elif defined(__linux__)
			do {

				if (m_hTimer != INVALID_HANDLE_VALUE && (error = QueueTimer::DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, m_hEvent)) <= 0 && error != ERROR_IO_PENDING)
					_smp::message_pool::getInstance().push(new message("[timer::_stop][Error] Ao deletar timer. Error Code: " + STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 12, error), CL_FILE_LOG_AND_CONSOLE));

			} while (error <= 0/*Error in DeleteTimerQueueTimer*/ && error != ERROR_IO_PENDING && count++ < LIMIT_COUNT_TRY);	// Tenta De novo

			// Ele está esperando o callback do timer terminar
			if (error == ERROR_IO_PENDING) {
			
				// Libera para a função Callback poder limpar os dados dela antes de verifica o final dela
				pthread_mutex_unlock(&m_cs);

#endif

				check_event_end();

				return;
			}else {

				m_hTimer = INVALID_HANDLE_VALUE;

				setState((TIMER_STATE)(_state + 2));	// Executado o Request	(Ação concluída)
			}
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[timer::_stop][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

};

void timer::_stop_from_callback(TIMER_STATE _state) {

#if defined(_WIN32)
	QueryPerformanceCounter(&m_elapsed);
#elif defined(__linux__)
	clock_gettime(CLOCK_TICK_TO_TIMER, &m_elapsed);
#endif

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Para não tentar excluír antes depois que entrou na critical secção e tentar excluír de novo
		if (getState() != _state + 1 && getState() != _state + 2) {

			setState((TIMER_STATE)(_state + 1));	// Executando o request

			BOOL error = 0;
			DWORD lastError = 0u;
			uint32_t count = 0u;

#if defined(_WIN32)
			do {

				if (m_hTimer != nullptr && m_hTimer != INVALID_HANDLE_VALUE && (error = DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, m_hEvent)) == 0 && (lastError = GetLastError()) != ERROR_IO_PENDING)
					_smp::message_pool::getInstance().push(new message("[timer::_stop][Error] Ao deletar timer. Error Code: " + STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 12, lastError), CL_FILE_LOG_AND_CONSOLE));

			} while (error == 0/*Error in DeleteTimerQueueTimer*/ && lastError != ERROR_IO_PENDING && count++ < LIMIT_COUNT_TRY);	// Tenta De novo
		}

		LeaveCriticalSection(&m_cs);

#elif defined(__linux__)
			do {

				if (m_hTimer != INVALID_HANDLE_VALUE && (error = QueueTimer::DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, m_hEvent)) <= 0 && error != ERROR_IO_PENDING)
					_smp::message_pool::getInstance().push(new message("[timer::_stop][Error] Ao deletar timer. Error Code: " + STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 12, error), CL_FILE_LOG_AND_CONSOLE));

			} while (error <= 0/*Error in DeleteTimerQueueTimer*/ && error != ERROR_IO_PENDING && count++ < LIMIT_COUNT_TRY);	// Tenta De novo
		}

		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[timer::_stop_from_callback][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void timer::check_event_end() {

#if defined(_WIN32)
	if (m_hTimer == nullptr || m_hTimer == INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	if (m_hTimer == INVALID_HANDLE_VALUE)
#endif
		throw exception("[timer::check_event_end][Error] O timer handle is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 11, 0));

	if (getState() == PAUSED || getState() == STOPPED)
		throw exception("[timer::check_event_end][Error] O timer ja esta parado, o evento ja foi sinalizado", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 10, 0));

	// Teste
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[timer_check_event_end][Log] Vai entrar no Wait", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

#if defined(_WIN32)
	if (WaitForSingleObject(m_hEvent, INFINITE) != WAIT_OBJECT_0)
		throw exception("[timer::check_event_end][Error] Ao esperar evento.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 8, GetLastError()));
#elif defined(__linux__)
	if (m_hEvent != nullptr && m_hEvent->wait(INFINITE) != WAIT_OBJECT_0)
		throw exception("[timer::check_event_end][Error] Ao esperar evento.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER, 8, errno)); // talvez o errno tenha outro erro(lixo)
#endif

	// Teste
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[timer_check_event_end][Log] Saiu do Wait", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	m_hTimer = INVALID_HANDLE_VALUE;

	// Update TIMER STATE to ENDDED ACTION REQUEST
	setState((TIMER_STATE)(getState() + 1));
};

void timer::hasTimerTick() {

	// CallBack Timer Initialized
	if (getState() == STANDBY) {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[timer::hasTimerTick][Log] Timer Callback has initialized with succes!, Now Timer is running", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		// Troca estado do timer
		setState(RUNNING);

#if defined(_WIN32)
		QueryPerformanceCounter(&m_start);
#elif defined(__linux__)
		clock_gettime(CLOCK_TICK_TO_TIMER, &m_start);
#endif

	}else {	// Timer já está  inicializado

#if defined(_WIN32)
		QueryPerformanceCounter(&m_elapsed);

		auto d = (DWORD)((m_elapsed.QuadPart - m_start.QuadPart) * 1000/*milliseconds*/ / m_frequency.QuadPart);
#elif defined(__linux__)
		clock_gettime(CLOCK_TICK_TO_TIMER, &m_elapsed);

		auto d = (DWORD)DIFF_TICK(m_elapsed, m_start, m_frequency);
#endif

		//// Coloca o Job no pool para tratar ele
		//m_arg->_job_pl.push(m_arg->_job.newInstanceJob());

		// Para o Tempo Antes de Colocar o Job no Job Pool [Para evitar de dar deadlock]
		if (m_tipo == NORMAL) {
			
			if ((d + 10) >= m_time) {

				_stop_from_callback(STOP);	// No Check End Be Cause, this is Callback function, if check get deadlock

				// Coloca o Job no pool para tratar ele
				m_arg->_job_pl.push(m_arg->_job.newInstanceJob());

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[timer::hasTimerTick][Log] Timer::time: " + std::to_string(m_time) + "\tTimer::time_elapsed: " + std::to_string(d), CL_FILE_LOG_AND_CONSOLE));
#endif
			}
		
		}else if (m_tipo == PERIODIC) {
			
			if ((d + 10) >= m_time)
				_stop_from_callback(STOP);	// No Check End Be Cause, this is Callback function, if check get deadlock

			// Coloca o Job no pool para tratar ele
			m_arg->_job_pl.push(m_arg->_job.newInstanceJob());

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[timer::hasTimerTick][Log] Timer::time: " + std::to_string(m_time) + "\tTimer::time_elapsed: " + std::to_string(d), CL_FILE_LOG_AND_CONSOLE));
#endif

		}else if (m_tipo == PERIODIC_INFINITE) {

			//if ((d + 10) >= m_time) {
				//_stop_from_callback(STOP);	// No Check End Be Cause, this is Callback function, if check get deadlock

			// Coloca o Job no pool para tratar ele
			m_arg->_job_pl.push(m_arg->_job.newInstanceJob());

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[timer::hasTimerTick][Log] Timer::time: " + std::to_string(m_time) + "\tTimer::time_elapsed: " + std::to_string(d), CL_FILE_LOG_AND_CONSOLE));
#endif
		}
	}
};
