// Arquivo logger.cpp
// Criado em 21/11/2020 as 16:00 por Acrisio
// Implementa��o da classe Logger

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <dlfcn.h>
#endif

#include "logger.hpp"

using namespace stdA;

Logger::Logger() : m_pLog(nullptr), m_pResponse(nullptr) {}

Logger::~Logger() {}

void Logger::loadVirtualFunctions(FNPROCSCLOG _scLog, FNPROCRESPONSECALLBACK _responseCallBack) {

#if defined(_WIN32)
	auto hHandle_exe = GetModuleHandleA(0);

	if (m_pLog == nullptr || m_pLog == _scLog) {

		m_pLog = (FNPROCSCLOG)GetProcAddress(hHandle_exe, "scLog");

		if (m_pLog == NULL)
			m_pLog = _scLog; // Padr�o Log
	}

	if (m_pResponse == nullptr || m_pResponse == _responseCallBack) {

		m_pResponse = (FNPROCRESPONSECALLBACK)GetProcAddress(hHandle_exe, "responseCallBack");

		if (m_pResponse == NULL)
			m_pResponse = _responseCallBack; // Padr�o Response Call Back
	}
#elif defined(__linux__)
	auto hHandle_exe = dlopen(nullptr, 0);

	if (m_pLog == nullptr || m_pLog == _scLog) {

		m_pLog = (FNPROCSCLOG)dlsym(hHandle_exe, "scLog");

		if (m_pLog == NULL)
			m_pLog = _scLog; // Padr�o Log
	}

	if (m_pResponse == nullptr || m_pResponse == _responseCallBack) {

		m_pResponse = (FNPROCRESPONSECALLBACK)dlsym(hHandle_exe, "responseCallBack");

		if (m_pResponse == NULL)
			m_pResponse = _responseCallBack; // Padr�o Response Call Back
	}
#endif
}

void Logger::Log(const std::string _log, const eTYPE_LOG _type) {

	if (m_pLog != nullptr)
		m_pLog(_log.c_str(), _type);
}

void Logger::Response(const uint32_t _id, const eTYPE_CALCULATOR_CMD _type, const std::string _response, const eTYPE_RESPONSE _server) {

	if (m_pResponse != nullptr)
		m_pResponse(_id, _type, _response.c_str(), _server);
}

void Logger::setFunctionLog(FNPROCSCLOG _log) {
	m_pLog = _log;
}

void Logger::setFunctionResponse(FNPROCRESPONSECALLBACK _response) {
	m_pResponse = _response;
}
