// Arquivo Smart Calculator.cpp
// Criado em 15/11/2020 as 17:32 por Acrisio
// Implementa��o da classe SmartCalculator

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "Smart Calculator.hpp"
#include "../UTIL/message_pool.h"
#include "../UTIL/exception.h"

#include "../Server/server.h"
#include "../UTIL/string_util.hpp"

#ifdef _DEBUG
	#include <cstdint>

	#if defined(_WIN32)
		#if INTPTR_MAX == INT64_MAX
			#define PATH_SMART_CALCULATOR_LIB_DLL "../../Smart Calculator lib/x64/Release/SMARTCALCULATORLIB.dll"
		#elif INTPTR_MAX == INT32_MAX
			#define PATH_SMART_CALCULATOR_LIB_DLL "../../Smart Calculator lib/Release/SMARTCALCULATORLIB.dll"
		#else
			#error Unknown pointer size or missing size macros!
		#endif
	#elif defined(__linux__)
		#define PATH_SMART_CALCULATOR_LIB_DLL "SMARTCALCULATORLIB.so"
	#endif
#else
	#if defined(_WIN32)
		#define PATH_SMART_CALCULATOR_LIB_DLL "SMARTCALCULATORLIB.dll"
	#elif defined(__linux__)
		#define PATH_SMART_CALCULATOR_LIB_DLL "SMARTCALCULATORLIB.so"
	#endif
#endif // _DEBUG

#define INIT_CMD_PTR(_ptr) SmartCalculator *smart = reinterpret_cast< SmartCalculator* >( (_ptr) ); \
							if (smart == nullptr || !smart->isLoad()) \
								throw exception("[" + std::string(__FUNCTION__) + "][Error] lib is not loaded.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 3, 0))

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
#define CATCH_CHECK			}catch (exception& e) { \
								LeaveCriticalSection(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); 
#elif defined(__linux__)
#define CATCH_CHECK			}catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); 
#endif

#define END_CHECK			 } \

// Verifica se a Lib est� carregada
#define CHECK_LIB if (!isLoad()) \
						throw exception("[" + std::string(__FUNCTION__) + "][Error] lib is not loaded.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 3, 0));

#define CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type) auto ctx = smart->getPlayerCtx((_uid), (_type)); \
\
											if (ctx == nullptr) { \
\
												_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Error][WARINIG] Player[UID="\
														+ std::to_string((_uid)) + "] don't have context of player.", CL_FILE_LOG_AND_CONSOLE)); \
\
												return; \
											} \

#define CONVERT_ARG_TO_DLL_ARG(_arg) join(const_cast< std::vector< std::string >& >((_arg)), " ").c_str()

using namespace stdA;

EXPORT_LIB void scLog(const char* _log, const eTYPE_LOG _type) {

#ifndef _DEBUG
	if (_type != eTYPE_LOG::TL_LOG)
		_smp::message_pool::getInstance().push(new message(std::string("[SmartCalculator::scLog][Log] ") + _log, CL_FILE_LOG_AND_CONSOLE));
	else
		_smp::message_pool::getInstance().push(new message(std::string("[SmartCalculator::scLog][Log] ") + _log, CL_ONLY_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message(std::string("[SmartCalculator::scLog][Log] ") + _log, CL_FILE_LOG_AND_CONSOLE));
#endif // !_DEBUG
};

extern "C" EXPORT_LIB void responseCallBack(const uint32_t _id, const stdA::eTYPE_CALCULATOR_CMD _type, const char* _response, const stdA::eTYPE_RESPONSE _server) {

	try {

		sSmartCalculator::getInstance().sendReply(_id, _type, _response, _server);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SmartCalculator::responseCallBack][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

SmartCalculator::SmartCalculator() : m_commands(), m_ptr_lib{ 0u }, m_hDll(NULL), m_load(false), m_stopped(false) {

	try {

#if defined(_WIN32)
		InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
		INIT_PTHREAD_MUTEXATTR_RECURSIVE;
		INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
		DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

		// Inicializa os Commandos
		initCommand();

		// Initialize Smart Calculator Lib
		initDllResource();

		m_load = true;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSsytem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

SmartCalculator::~SmartCalculator() {

	clearDllResource();

	TRY_CHECK;
	
	if (!m_commands.empty())
		m_commands.clear();

	// Flag que segura requisi��o externa de recarga
	m_stopped = false;

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

bool SmartCalculator::isLoad() {
	
	bool is_load = false;

	TRY_CHECK;

	is_load = (m_load && m_ptr_lib.isValid() && m_hDll != NULL);

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;

	return is_load;
}

void SmartCalculator::load() {

	if (isLoad())
		clearDllResource();

	initDllResource();
}

void SmartCalculator::close() {

	if (isLoad())
		clearDllResource();
}

bool SmartCalculator::hasStopped(){
	
	bool hasStopped = false;

	TRY_CHECK;

	hasStopped = m_stopped;

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;

	return hasStopped;
}

void SmartCalculator::setStop(bool _stop) {

	TRY_CHECK;

	m_stopped = _stop;

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;
}

void SmartCalculator::initDllResource() {

	TRY_CHECK;

#if defined(_WIN32)
	m_hDll = LoadLibraryA(PATH_SMART_CALCULATOR_LIB_DLL);
#elif defined(__linux__)
	m_hDll = dlopen((std::string(getcwd(nullptr, 0)) + "/" + PATH_SMART_CALCULATOR_LIB_DLL).c_str(), RTLD_LAZY);
#endif

	if (m_hDll == NULL)
		throw exception("[SmartCalculator::initDllResource][Error] nao conseguiu carregar a DLL=" 
				+ std::string(PATH_SMART_CALCULATOR_LIB_DLL)
#if defined(__linux__)
				+ std::string(", Error message: ") + dlerror()
#endif
				, STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 1, 0));

#if defined(_WIN32)
	m_ptr_lib.initSmartCalculatorLib = (FNINITSMARTCALCULATORLIB)GetProcAddress(m_hDll, "initSmartCalculatorLib");

	// Send Command Server
	m_ptr_lib.sendCommandServer = (FNSENDCOMMANDSERVER)GetProcAddress(m_hDll, "sendCommandServer");

	// Player Context
	m_ptr_lib.makePlayerContext = (FNMAKEPLAYERCONTEXT)GetProcAddress(m_hDll, "makePlayerContext");
	m_ptr_lib.getPlayerContext = (FNGETPLAYERCONTEXT)GetProcAddress(m_hDll, "getPlayerContext");
	m_ptr_lib.removeAllPlayerContext = (FNREMOVEALLPLAYERCONTEXT)GetProcAddress(m_hDll, "removeAllPlayerContext");
#elif defined(__linux__)
	m_ptr_lib.initSmartCalculatorLib = (FNINITSMARTCALCULATORLIB)dlsym(m_hDll, "initSmartCalculatorLib");

	// Send Command Server
	m_ptr_lib.sendCommandServer = (FNSENDCOMMANDSERVER)dlsym(m_hDll, "sendCommandServer");

	// Player Context
	m_ptr_lib.makePlayerContext = (FNMAKEPLAYERCONTEXT)dlsym(m_hDll, "makePlayerContext");
	m_ptr_lib.getPlayerContext = (FNGETPLAYERCONTEXT)dlsym(m_hDll, "getPlayerContext");
	m_ptr_lib.removeAllPlayerContext = (FNREMOVEALLPLAYERCONTEXT)dlsym(m_hDll, "removeAllPlayerContext");
#endif

	if (!m_ptr_lib.isValid())
		throw exception("[SmartCalculator::initDllResource][Error] nao conseguiu pegar todas as functions importadas da DLL=" 
				+ std::string(PATH_SMART_CALCULATOR_LIB_DLL), STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 2, 0));

	// Get UID of Server
	uint32_t server_uid = 0;

	// Get Server Base Static
	if (ssv::sv != nullptr)
		server_uid = ssv::sv->getUID();

	// Initialize Smart Calculator Lib | UID do server
	m_ptr_lib.initSmartCalculatorLib(server_uid);

	m_load = true;

	// Flag stopped, coloca para false, por que acabou de carregar
	m_stopped = false;

	// Log
	_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Log] Dll Lib initialized with success.", CL_FILE_LOG_AND_CONSOLE));

	LEAVE_CHECK;
	CATCH_CHECK;
		throw; // Relan�a
	END_CHECK;
}

void SmartCalculator::clearDllResource() {

	TRY_CHECK;

	if (m_hDll != NULL)
#if defined(_WIN32)
		FreeLibrary(m_hDll);
#elif defined(__linux__)
		dlclose(m_hDll);
#endif

	m_hDll = NULL;

	m_ptr_lib.clear();

	m_load = false;

	// Log
	_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Log] Dll Lib clear resource with success.", CL_FILE_LOG_AND_CONSOLE));

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;
}

void SmartCalculator::sendCommandServer(std::string _cmd) {

	if (!isLoad())
		throw exception("[" + std::string(__FUNCTION__) + "][Error] lib is not loaded.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 3, 0));

	if (_cmd.empty())
		return; // Command Empty

	TRY_CHECK;

	// Send command server
	m_ptr_lib.sendCommandServer(_cmd.c_str());

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;
}

void SmartCalculator::checkCommand(const uint32_t _uid, const std::string _cmd, eTYPE_CALCULATOR_CMD _type) {

	try {

		if (_cmd.empty())
			throw exception("[SmartCalculator::checkCommand][Error] Player[UID=" + std::to_string(_uid) 
					+ "] tentou executar comando, mas ele esta vazio.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 1, 0));

		auto args = split(_cmd, " ");

		if (args.size() <= 0)
			throw exception("[SmartCalculator::checkCommand][Error] Player[UID=" + std::to_string(_uid) 
					+ "] tentou executar comando, mas nao tem nenhum argumento. Command(" + _cmd + ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 1, 0));

		auto command = toLowerCase(args.front());

		// Command
		args.erase(args.begin());

		translateCommand(_uid, command, args, _type);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SmartCalculator::checkCommand][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::sendReply(const uint32_t _uid, const eTYPE_CALCULATOR_CMD _type, const std::string _response, const stdA::eTYPE_RESPONSE _server) {

	try {

		if (_server == eTYPE_RESPONSE::PLAYER_RESPONSE) {
			
			responseCallBackPlayer(_uid, _response, _type);
		
		}else if (_server == eTYPE_RESPONSE::SERVER_RESPONSE) {
			
			UNREFERENCED_PARAMETER(_type);
			
			responseCallBackServer(_uid, _response);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SmartCalculator::sendReply][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

stContext* SmartCalculator::makePlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type) {
	CHECK_LIB;
	
	stContext* ctx = nullptr;

	try {

		ctx = m_ptr_lib.makePlayerContext(_uid, _type);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return ctx;
}

stContext* SmartCalculator::getPlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type) {
	CHECK_LIB;

	stContext* ctx = nullptr;

	try {

		ctx = m_ptr_lib.getPlayerContext(_uid, _type);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
	
	return ctx;
}

void SmartCalculator::removeAllPlayerCtx(const uint32_t _uid) {
	CHECK_LIB;

	try {

		m_ptr_lib.removeAllPlayerContext(_uid);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::translateCommand(const uint32_t _uid, const std::string& _cmd, const std::vector< std::string >& _args, const eTYPE_CALCULATOR_CMD _type) {

	TRY_CHECK;

	stKeyCommand key{ _cmd, _type };

	auto cmd = m_commands.find(key);

	if (cmd == m_commands.end())
		throw exception("[SmartCalculator::translateCommand][Error] Player[UID=" + std::to_string(_uid) 
				+ "] tentou executar o comando(" + _cmd + ", TYPE=" + std::to_string((unsigned short)_type) 
				+ "), mas ele nao existe.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 2, 0));

	if (cmd->second == nullptr)
		throw exception("[SmartCalculator::translateCommand][Error] Player[UID=" + std::to_string(_uid)
				+ "] tentou executar o comando(" + _cmd + ", TYPE=" + std::to_string((unsigned short)_type) 
				+ "), mas nao tem um handler para esse comando. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SMART_CALCULATOR, 3, 0));

	cmd->second(this, _type, _uid, _args);

	LEAVE_CHECK;
	CATCH_CHECK;
	END_CHECK;
}

void SmartCalculator::initCommand() {

	// Inicializa os Commandos

	m_commands.insert({
		{
			"ping",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorPing
	});

	m_commands.insert({
		{
			"ping",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorPing
	});

	m_commands.insert({
		{
			"info",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorInfo
	});

	m_commands.insert({
		{
			"info",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorInfo
	});

	m_commands.insert({
		{
			"myinfo",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorMyInfo
	});

	m_commands.insert({
		{
			"myinfo",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorMyInfo
	});

	m_commands.insert({
		{
			"list",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorList
	});

	m_commands.insert({
		{
			"list",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorList
	});

	m_commands.insert({
		{
			"calc",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorCalculate
	});

	m_commands.insert({
		{
			"calc",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorCalculate
	});

	m_commands.insert({
		{
			"expr",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorExpression
	});

	m_commands.insert({
		{
			"expr",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorExpression
	});

	m_commands.insert({
		{
			"m",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorMacro
	});

	m_commands.insert({
		{
			"m",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorMacro
	});

	m_commands.insert({
		{
			"lastr",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorLastResult
	});

	m_commands.insert({
		{
			"lastr",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorLastResult
	});

	m_commands.insert({
		{
			"resolution",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorResolution
	});

	m_commands.insert({
		{
			"resolution",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorResolution
	});

	m_commands.insert({
		{
			"dfav",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorDesvioFavorito
	});

	m_commands.insert({
		{
			"dfav",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorDesvioFavorito
	});

	m_commands.insert({
		{
			"auto_fit",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorAutoFit
	});

	m_commands.insert({
		{
			"auto_fit",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorAutoFit
	});

	m_commands.insert({
		{
			"mycella",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorMycellaDegree
	});

	m_commands.insert({
		{
			"mycella",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdSmartAndStadiumCalculatorMycellaDegree
	});

	// Smart Calculator
	m_commands.insert({
		{
			"club",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorClub
	});

	m_commands.insert({
		{
			"shot",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorShot
	});

	m_commands.insert({
		{
			"ps",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorPowerShot
	});

	m_commands.insert({
		{
			"power",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorPower
	});

	m_commands.insert({
		{
			"ring",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorRing
	});

	m_commands.insert({
		{
			"mascot",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorMascot
	});

	m_commands.insert({
		{
			"card",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorCard
	});

	m_commands.insert({
		{
			"card_ps",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorCardPowerShot
	});

	m_commands.insert({
		{
			"d",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorDistance
	});

	m_commands.insert({
		{
			"h",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorHeight
	});

	m_commands.insert({
		{
			"w",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorWind
	});

	m_commands.insert({
		{
			"a",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorDegree
	});

	m_commands.insert({
		{
			"g",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorGround
	});

	m_commands.insert({
		{
			"s",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorSpin
	});

	m_commands.insert({
		{
			"c",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorCurve
	});

	m_commands.insert({
		{
			"b",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorSlopeBreak
	});

	m_commands.insert({
		{
			"make_slope",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorMakeSlopeBreak
	});

	m_commands.insert({
		{
			"aim",
			eTYPE_CALCULATOR_CMD::SMART_CALCULATOR
		},
		SmartCalculator::cmdSmartCalculatorAimDegree
	});

	// Stadium Calculator
	m_commands.insert({
		{
			"open",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorOpen
	});

	m_commands.insert({
		{
			"shot",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorShot
	});

	m_commands.insert({
		{
			"d",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorDistance
	});

	m_commands.insert({
		{
			"h",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorHeight
	});

	m_commands.insert({
		{
			"w",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorWind
	});

	m_commands.insert({
		{
			"a",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorDegree
	});

	m_commands.insert({
		{
			"g",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorGround
	});

	m_commands.insert({
		{
			"b",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorSlopeBreak
	});

	m_commands.insert({
		{
			"n",
			eTYPE_CALCULATOR_CMD::CALCULATOR_STADIUM
		},
		SmartCalculator::cmdStadiumCalculatorGreenSlope
	});
}

void SmartCalculator::responseCallBackPlayer(const uint32_t _uid, const std::string _response, const eTYPE_CALCULATOR_CMD _type) {

	try {

		if (ssv::sv == nullptr)
			return;

		// Send Reply to top server reply
		ssv::sv->sendSmartCalculatorReplyToPlayer(
			_uid, 
			(_type == eTYPE_CALCULATOR_CMD::SMART_CALCULATOR ? "#SC" : "#CS"), 
			_response
		);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SmartCalculator::responseCallBackPlayer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::responseCallBackServer(const uint32_t _uid, const std::string _response) {

	try {

		auto v_args = split(_response, " ");

		if (v_args.empty())
			return; // Empty;

		if (v_args.size() == 1) {

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Error] Invalid command: " + join(v_args, " "), CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Error] Invalid command: " + join(v_args, " "), CL_ONLY_FILE_LOG));
#endif // _DEBUG

			return;
		}

		auto command = v_args.front(); // Command
		v_args.erase(v_args.begin());

		if (command.compare("chat_discord") == 0) {

			auto value = v_args.front();

#if defined(_WIN32)
			if (_stricmp(value.c_str(), "ON") == 0) {
#elif defined(__linux__)
			if (strcasecmp(value.c_str(), "ON") == 0) {
#endif

				if (ssv::sv != nullptr)
					ssv::sv->setChatDiscord(true);

				// Log
				_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Log] Chat Discord Enable with success.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
			}else if (_stricmp(value.c_str(), "OFF") == 0) {
#elif defined(__linux__)
			}else if (strcasecmp(value.c_str(), "OFF") == 0) {
#endif

				if (ssv::sv != nullptr)
					ssv::sv->setChatDiscord(false);

				// Log
				_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][Log] Chat Discord Disable with success.", CL_FILE_LOG_AND_CONSOLE));
			}

		}else if (command.compare("notice") == 0) {

			auto notice = join(v_args, " ");

			if (notice.empty())
				return;		// Empty notice

			// Envia a notice para todos do server
			if (ssv::sv != nullptr)
				ssv::sv->sendNoticeGMFromDiscordCmd(notice);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[SmartCalculator::responseCallBackServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorPing(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {
		
		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->ping(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorInfo(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {
		
		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->info(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorMyInfo(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {
		
		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->myInfo(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorList(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->list(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorCalculate(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {
		
		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->calcule(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorExpression(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {
		
		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->expression(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorMacro(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {
		
		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->macro(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorLastResult(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->last_result(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorResolution(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->resolution(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorDesvioFavorito(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->desvio_favorito(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorAutoFit(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->auto_fit(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartAndStadiumCalculatorMycellaDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getPlayer()->mycella_degree(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorClub(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->club(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->shot(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorPowerShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->power_shot(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorPower(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->power(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorRing(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->ring(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorMascot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->mascot(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorCard(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->card(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorCardPowerShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->card_power_shot(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorDistance(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->distance(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorHeight(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->height(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorWind(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->wind(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->degree(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorGround(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->ground(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorSpin(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->spin(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorCurve(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->curve(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorSlopeBreak(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->slope_break(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorMakeSlopeBreak(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->make_slope_break(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdSmartCalculatorAimDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getSmartPlayer()->aim_degree(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

// Stadium Calculator
void SmartCalculator::cmdStadiumCalculatorOpen(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->open(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->shot(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorDistance(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->distance(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorHeight(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->height(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorWind(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->wind(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->degree(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorGround(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->ground(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorSlopeBreak(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->slope_break(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void SmartCalculator::cmdStadiumCalculatorGreenSlope(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args) {
	INIT_CMD_PTR(_this);

	try {

		CHECK_AND_INIT_PLAYER_CONTEXT(_uid, _type);

		ctx->getStadiumPlayer()->green_slope(CONVERT_ARG_TO_DLL_ARG(_args));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}
