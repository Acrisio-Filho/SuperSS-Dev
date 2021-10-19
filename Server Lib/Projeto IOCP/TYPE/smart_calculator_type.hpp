// Arquivo smart_calculator_type.hpp
// Criado em 17/11/2020 as 12:29 por Acrisio
// Defini��o dos tipos usandos no Smart Calculator System

#pragma once
#ifndef _STDA_SMART_CALCULATOR_TYPE_HPP
#define _STDA_SMART_CALCULATOR_TYPE_HPP

#include "smart_calculator_player_base.hpp"

#include <string>
#include <vector>

namespace stdA {

	enum class eTYPE_LOG : uint32_t {
		TL_LOG_SYSTEM,
		TL_LOG,
		TL_WARNING,
		TL_ERROR
	};

	enum class eTYPE_CALCULATOR_CMD : unsigned char {
		SMART_CALCULATOR,
		CALCULATOR_STADIUM
	};

	enum class eTYPE_RESPONSE : unsigned char {
		PLAYER_RESPONSE,
		SERVER_RESPONSE
	};

	struct stKeyCommand {
	public:
		stKeyCommand(std::string _cmd, eTYPE_CALCULATOR_CMD _type)
			: cmd(_cmd), type(_type) {};

		inline bool operator < (const stKeyCommand& rhs) const {

			if (rhs.cmd.compare(cmd) != 0)
				return rhs.cmd.compare(cmd) < 0;
			else
				return rhs.type < type;
		};

	public:
		std::string cmd;
		eTYPE_CALCULATOR_CMD type;
	};

	struct stKeyPlayer {
	public:
		stKeyPlayer(uint32_t _uid, eTYPE_CALCULATOR_CMD _type)
			: uid(_uid), type(_type) {};

		inline bool operator < (const stKeyPlayer& rhs) const {

			if (rhs.uid != uid)
				return rhs.uid < uid;
			else
				return rhs.type < type;
		};

	public:
		uint32_t uid;
		eTYPE_CALCULATOR_CMD type;
	};

	// Function Pointers
	typedef void(*RESPONSECALLBACK)(const uint32_t _uid, const std::string _response);
	typedef void(*FNTRANSLATECOMMAND)(void* _this, const eTYPE_CALCULATOR_CMD _type, const uint32_t _uid, const std::vector< std::string >& _args);

	// Player Calculator Context
	struct stContext {
	public:
		stContext(ICalculatorPlayer* _player) : player(_player) {};

		ISmartCalculatorPlayer* getSmartPlayer() {
			return reinterpret_cast<ISmartCalculatorPlayer*>(player);
		};

		IStadiumCalculatorPlayer* getStadiumPlayer() {
			return reinterpret_cast<IStadiumCalculatorPlayer*>(player);
		};

		ICalculatorPlayer* getPlayer() {
			return player;
		};

	private:
		ICalculatorPlayer* player;
	};

	// Log and callback function pointers from dynamic library
	typedef void(*FNPROCSCLOG)(const char* _log, const eTYPE_LOG _type);
	typedef void(*FNPROCRESPONSECALLBACK)(const uint32_t _id, const eTYPE_CALCULATOR_CMD _type, const char* _response, const eTYPE_RESPONSE _server);
}

#endif // !_STDA_SMART_CALCULATOR_TYPE_HPP
