// Arquivo Smart Calculator.hpp
// Criado em 15/11/2020 as 17:26 por Acrisio
// Defini��o da classe SmartCalculator

#pragma once
#ifndef _STDA_SMART_CALCULATOR_HPP
#define _STDA_SMART_CALCULATOR_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <pthread.h>
#include <dlfcn.h>
#endif

#include "../../Projeto IOCP/TYPE/singleton.h"
#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"
#include "../../Projeto IOCP/TYPE/smart_calculator_player_base.hpp"

#include <map>
#include <vector>
#include <string>

#if defined(_MSC_VER)
	#define EXPORT_LIB __declspec(dllexport)
	#define IMPORT_LIB __declspec(dllimport)
#elif defined(__GNUC__)
	#define EXPORT_LIB __attribute__((visibility("default")))
	#define IMPORT_LIB
#else
	#define EXPORT_LIB
	#define IMPORT_LIB
#endif

extern "C" EXPORT_LIB void scLog(const char* _log, const stdA::eTYPE_LOG _type);
extern "C" EXPORT_LIB void responseCallBack(const uint32_t _id, const stdA::eTYPE_CALCULATOR_CMD _type, const char* _response, const stdA::eTYPE_RESPONSE _server);

namespace stdA {

	typedef void(*FNINITSMARTCALCULATORLIB)(const uint32_t _server_id);

	// Send Comand Server
	typedef void(*FNSENDCOMMANDSERVER)(const char* _cmd);

	// Player Context
	typedef stContext*(*FNMAKEPLAYERCONTEXT)(uint32_t _uid, eTYPE_CALCULATOR_CMD _type);
	typedef stContext*(*FNGETPLAYERCONTEXT)(uint32_t _uid, eTYPE_CALCULATOR_CMD _type);
	typedef void(*FNREMOVEALLPLAYERCONTEXT)(uint32_t _uid);

	class SmartCalculator {
		public:
			struct stPointersLib {
				public:
					stPointersLib(uint32_t _ul = 0u)
						: initSmartCalculatorLib(nullptr),
						  sendCommandServer(nullptr),
						  makePlayerContext(nullptr),
						  getPlayerContext(nullptr),
						  removeAllPlayerContext(nullptr)
					{};

					void clear() {
						
						initSmartCalculatorLib = nullptr;

						// Send Command Server
						sendCommandServer = nullptr;

						// Player Context
						makePlayerContext = nullptr;
						getPlayerContext = nullptr;
						removeAllPlayerContext = nullptr;
					};

					bool isValid() {
						return initSmartCalculatorLib != nullptr
							&& sendCommandServer != nullptr
							&& makePlayerContext != nullptr
							&& getPlayerContext != nullptr
							&& removeAllPlayerContext != nullptr
							;
					};

				public:
					FNINITSMARTCALCULATORLIB initSmartCalculatorLib;

					// Send Command Server
					FNSENDCOMMANDSERVER sendCommandServer;

					// Player Context
					FNMAKEPLAYERCONTEXT makePlayerContext;
					FNGETPLAYERCONTEXT getPlayerContext;
					FNREMOVEALLPLAYERCONTEXT removeAllPlayerContext;
			};

		public:
			SmartCalculator();
			virtual ~SmartCalculator();

			bool isLoad();

			void load();

			void close();

			// Trava a recarga do Smart Calculator
			bool hasStopped();
			void setStop(bool _stop);

			void sendCommandServer(std::string _cmd);

			void checkCommand(const uint32_t _uid, const std::string _cmd, eTYPE_CALCULATOR_CMD _type);

			void sendReply(const uint32_t _uid, const eTYPE_CALCULATOR_CMD _type, const std::string _response, const stdA::eTYPE_RESPONSE _server);

			stContext* makePlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type);
			stContext* getPlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type);
			void removeAllPlayerCtx(const uint32_t _uid);

		protected:
			void translateCommand(const uint32_t _uid, const std::string& _cmd, const std::vector< std::string >& _args, const eTYPE_CALCULATOR_CMD _type);

			void initCommand();

			void initDllResource();

			void clearDllResource();

		protected:
			void responseCallBackPlayer(const uint32_t _uid, const std::string _response, const eTYPE_CALCULATOR_CMD _type);
			void responseCallBackServer(const uint32_t _uid, const std::string _response);

			// Commandos
			static void cmdSmartAndStadiumCalculatorPing(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorInfo(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorMyInfo(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorList(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorCalculate(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorExpression(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorMacro(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorLastResult(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorResolution(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorDesvioFavorito(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorAutoFit(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartAndStadiumCalculatorMycellaDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);

			// Smart Calculator
			static void cmdSmartCalculatorClub(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorPowerShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorPower(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorRing(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorMascot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorCard(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorCardPowerShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorDistance(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorHeight(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorWind(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorGround(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorSpin(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorCurve(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorSlopeBreak(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorMakeSlopeBreak(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdSmartCalculatorAimDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);

			// Stadium Calculator
			static void cmdStadiumCalculatorOpen(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorShot(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorDistance(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorHeight(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorWind(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorDegree(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorGround(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorSlopeBreak(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);
			static void cmdStadiumCalculatorGreenSlope(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);

		protected:
			std::map< stKeyCommand, FNTRANSLATECOMMAND > m_commands;

			stPointersLib m_ptr_lib;

#if defined(_WIN32)
			HMODULE m_hDll;
#elif defined(__linux__)
			// Linux o de dll(.so) é void*
			void* m_hDll;
#endif

			bool m_load;

			// Flag que n�o deixa ele recarregar at� ela est� desativada
			bool m_stopped;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< SmartCalculator > sSmartCalculator;
}

#endif // !_STDA_SMART_CALCULATOR_HPP
