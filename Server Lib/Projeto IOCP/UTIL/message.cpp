// Arquivo message.cpp
// Criado em 21/05/2017 por Acrisio
// Implementação da classe message

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "message.h"
#include <ctime>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "WinPort.h"
#endif

#include "../../Projeto IOCP/UTIL/string_util.hpp"

// Array of char to string time formated
constexpr size_t MAX_TIME_STRING = 50u;

using namespace stdA;

message::message() {};

message::message(std::string s, size_t _tipo) : m_message(s), m_tipo(_tipo) {
	char time[MAX_TIME_STRING];
	
	SYSTEMTIME st = { 0 };

#if defined(_WIN32)
	::GetLocalTime(&st);
#elif defined(__linux__)
	GetLocalTime(&st);
#endif

#if defined(_WIN32)
	sprintf_s(time, "%.04d-%.02d-%.02d %.02d:%.02d:%.02d.%.03d",
					st.wYear, st.wMonth, st.wDay, 
					st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#elif defined(__linux__)
	sprintf(time, "%.04d-%.02d-%.02d %.02d:%.02d:%.02d.%.03d",
					st.wYear, st.wMonth, st.wDay, 
					st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif

	m_message = std::string(time) + "\t" + m_message;
};

message::message(std::wstring s, size_t _tipo) : m_tipo(_tipo) {
	char time[MAX_TIME_STRING];

	SYSTEMTIME st = { 0 };

#if defined(_WIN32)
	::GetLocalTime(&st);
#elif defined(__linux__)
	GetLocalTime(&st);
#endif

#if defined(_WIN32)
	sprintf_s(time, "%.04d-%.02d-%.02d %.02d:%.02d:%.02d.%.03d",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#elif defined(__linux__)
	sprintf(time, "%.04d-%.02d-%.02d %.02d:%.02d:%.02d.%.03d",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif

	m_message = WcToMb(s);
	//m_message.assign(s.begin(), s.end());	// Convert de WSTRING para STRING

	m_message = std::string(time) + "\t" + m_message;
};

message::~message() {
    if (!m_message.empty()) {
        m_message.clear();
        m_message.shrink_to_fit();
    }
};

void message::append(std::string s) {
    m_message += s;
};

void message::set(std::string s) {
    m_message = s;
};

std::string message::get() {
    return m_message;
};

size_t message::getTipo() {
	return m_tipo;
}