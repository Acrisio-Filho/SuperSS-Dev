// Arquivo logger.hpp
// Criado em 21/11/2020 as 15:56 por Acrisio
// Defini��o da classe Logger

#pragma once
#ifndef _STDA_LOGGER_HPP
#define _STDA_LOGGER_HPP

#include "../../Projeto IOCP/TYPE/singleton.h"
#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"

#include <string>

namespace stdA {

	class Logger {
		public:
			Logger();
			virtual ~Logger();

			void loadVirtualFunctions(FNPROCSCLOG _scLog, FNPROCRESPONSECALLBACK _responseCallBack);

			void Log(const std::string _log, const eTYPE_LOG _type);
			void Response(const uint32_t _id, const eTYPE_CALCULATOR_CMD _type, const std::string _response, const eTYPE_RESPONSE _server);

			void setFunctionLog(FNPROCSCLOG _log);
			void setFunctionResponse(FNPROCRESPONSECALLBACK _response);

		protected:
			FNPROCSCLOG m_pLog;
			FNPROCRESPONSECALLBACK m_pResponse;
	};

	typedef Singleton< Logger > sLogger;
}

#endif // !_STDA_LOGGER_HPP
