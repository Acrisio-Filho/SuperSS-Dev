// Arquivo random_gen.cpp
// Criado em 15/08/2019 as 07:45 por Acrisio
// Implementação da classe RandomGen

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "WinPort.h"
#endif

#include "random_gen.hpp"
#include "exception.h"
#include "message_pool.h"

#include <chrono>

using namespace stdA;

RandomGen::RandomGen() : m_rd(nullptr), m_ibe_mt19937_64(nullptr), m_state(false) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

RandomGen::~RandomGen() {

	destroy();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

bool RandomGen::init() {

	if (isGood())
		return false;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_rd == nullptr)
			m_rd = new std::random_device();

		if (m_ibe_mt19937_64 == nullptr)
			m_ibe_mt19937_64 = new std::independent_bits_engine< std::mt19937_64, 64, std::uint_fast64_t >();

		// Inicializou com sucesso
		m_state = true;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

		// Falha ao inicializar
		m_state = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::init][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

		// Falha ao inicializar
		m_state = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::init][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return m_state;
}

void RandomGen::destroy() {

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_rd != nullptr)
			delete m_rd;

		m_rd = nullptr;

		if (m_ibe_mt19937_64 != nullptr)
			delete m_ibe_mt19937_64;

		m_ibe_mt19937_64 = nullptr;

		// Destruiu os dados com sucesso
		m_state = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
	
	}catch (exception& e) {

		// Destruiu os dados com sucesso
		m_state = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::destroy][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

		// Destruiu os dados com sucesso
		m_state = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::destroy][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}
}

bool RandomGen::isGood() {

	bool state = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	state = (m_state && m_rd != nullptr && m_ibe_mt19937_64 != nullptr);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return state;
}

uint64_t RandomGen::_rDevice() {

	if (!isGood() && !init())
		throw exception("[RandomGen::_rDevice][Error] Nao conseguiu inicializar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANDOM_GEN, 1, 0));

	uint64_t dice = 0ull;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_rd != nullptr)
			dice = (*m_rd)();

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

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rDevice][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rDevice][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return dice;
}

uint64_t RandomGen::_rIbeMt19937_64_chrono() {
	
	if (!isGood() && !init())
		throw exception("[RandomGen::_rIbeMt19937_64_chrono][Error] Nao conseguiu inicializar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANDOM_GEN, 1, 0));

	uint64_t dice = 0ull;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_ibe_mt19937_64 != nullptr) {
			
			// Initialize seed
			m_ibe_mt19937_64->seed(std::chrono::system_clock::now().time_since_epoch().count());

			dice = (*m_ibe_mt19937_64)();
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

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_chrono][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_chrono][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return dice;
}

uint64_t RandomGen::_rIbeMt19937_64_rdevice() {
	
	if (!isGood() && !init())
		throw exception("[RandomGen::_rIbeMt19937_64_rdevice][Error] Nao conseguiu inicializar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANDOM_GEN, 1, 0));

	uint64_t dice = 0ull;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_rd != nullptr && m_ibe_mt19937_64 != nullptr) {
			
			// Initialize seed
			m_ibe_mt19937_64->seed((*m_rd)());

			dice = (*m_ibe_mt19937_64)();
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

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_rdevice][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_rdevice][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return dice;
}

uint64_t RandomGen::_rDeviceRange(uint64_t _min, uint64_t _max) {

	if (!isGood() && !init())
		throw exception("[RandomGen::_rDeviceRange][Error] Nao conseguiu inicializar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANDOM_GEN, 1, 0));

	uint64_t dice = 0ull;

	try {

		std::uniform_int_distribution< uint64_t > distribution(_min, _max);

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_rd != nullptr)
			dice = distribution(*m_rd);

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

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rDeviceRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rDeviceRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return dice;
}

uint64_t RandomGen::_rIbeMt19937_64_chronoRange(uint64_t _min, uint64_t _max) {
	
	if (!isGood() && !init())
		throw exception("[RandomGen::_rIbeMt19937_64_chronoRange][Error] Nao conseguiu inicializar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANDOM_GEN, 1, 0));

	uint64_t dice = 0ull;

	try {

		std::uniform_int_distribution< uint64_t > distribution(_min, _max);

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_ibe_mt19937_64 != nullptr) {

			// Initialize seed
			m_ibe_mt19937_64->seed(std::chrono::system_clock::now().time_since_epoch().count());

			dice = distribution(*m_ibe_mt19937_64);
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

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_chronoRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_chronoRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return dice;
}

uint64_t RandomGen::_rIbeMt19937_64_rdeviceRange(uint64_t _min, uint64_t _max) {
	
	if (!isGood() && !init())
		throw exception("[RandomGen::_rIbeMt19937_64_rdeviceRange][Error] Nao conseguiu inicializar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::RANDOM_GEN, 1, 0));

	uint64_t dice = 0ull;

	try {

		std::uniform_int_distribution< uint64_t > distribution(_min, _max);

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_rd != nullptr && m_ibe_mt19937_64 != nullptr) {

			// Initialize seed
			m_ibe_mt19937_64->seed((*m_rd)());

			dice = distribution(*m_ibe_mt19937_64);
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

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_rdeviceRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[RandomGen::_rIbeMt19937_64_rdeviceRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	return dice;
}

uint64_t RandomGen::rDevice() {

	uint64_t dice = 0ull;
	bool ok = false;

	try {

		dice = _rDevice();

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDevice][ErrorSysetm] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDevice][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Conseguiu com o device
	if (ok)
		return dice;

	try {

		dice = _rIbeMt19937_64_chrono();

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDevice][ErrorSysetm] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDevice][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Não conseguiu com o device e nem o chronos
	if (!ok)
		return (uint64_t)std::rand();

	// Conseguiu com o chronos
	return dice;
}

uint64_t RandomGen::rIbeMt19937_64_chrono() {
	
	uint64_t dice = 0ull;
	bool ok = false;

	try {

		dice = _rIbeMt19937_64_chrono();

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_chrono][ErrorSysetm] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_chrono][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Não conseguiu com o chronos
	if (!ok)
		return (uint64_t)std::rand();

	// Conseguiu com o chronos
	return dice;
}

uint64_t RandomGen::rIbeMt19937_64_rdevice() {
	
	uint64_t dice = 0ull;
	bool ok = false;

	try {

		dice = _rIbeMt19937_64_rdevice();

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdevice][ErrorSysetm] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdevice][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Conseguiu com o device
	if (ok)
		return dice;

	try {

		dice = _rIbeMt19937_64_chrono();

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdevice][ErrorSysetm] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdevice][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Não conseguiu com o device e nem o chronos
	if (!ok)
		return (uint64_t)std::rand();

	// Conseguiu com o chronos
	return dice;
}

uint64_t RandomGen::rDeviceRange(uint64_t _min, uint64_t _max) {
	
	uint64_t dice = 0ull;
	bool ok = false;

	try {

		dice = _rDeviceRange(_min, _max);

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDeviceRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDeviceRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Conseguiu com o device
	if (ok)
		return dice;

	try {

		dice = _rIbeMt19937_64_chronoRange(_min, _max);

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDeviceRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rDeviceRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Não conseguiu com o device e nem o chronos
	if (!ok)
		return (uint64_t)(_min + (std::rand() % (_max - _min)));

	// Conseguiu com o chronos
	return dice;
}

uint64_t RandomGen::rIbeMt19937_64_chronoRange(uint64_t _min, uint64_t _max) {
	
	uint64_t dice = 0ull;
	bool ok = false;

	try {

		dice = _rIbeMt19937_64_chronoRange(_min, _max);

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_chronoRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_chronoRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Não conseguiu com o chronos
	if (!ok)
		return (uint64_t)(_min + (std::rand() % (_max - _min)));

	// Conseguiu com o chronos
	return dice;
}

uint64_t RandomGen::rIbeMt19937_64_rdeviceRange(uint64_t _min, uint64_t _max) {
	
	uint64_t dice = 0ull;
	bool ok = false;

	try {

		dice = _rIbeMt19937_64_rdeviceRange(_min, _max);

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdeviceRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdeviceRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Conseguiu com o device
	if (ok)
		return dice;

	try {

		dice = _rIbeMt19937_64_chronoRange(_min, _max);

		ok = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdeviceRange][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[RandomGen::rIbeMt19937_64_rdeviceRange][ErrorSystem][Std] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}

	// Não conseguiu com o device e nem o chronos
	if (!ok)
		return (uint64_t)(_min + (std::rand() % (_max - _min)));

	// Conseguiu com o chronos
	return dice;
}
