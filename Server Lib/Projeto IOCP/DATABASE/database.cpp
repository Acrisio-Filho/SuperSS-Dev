// Arquivo database.cpp
// Criado em 27/01/2018 as 23:13 por Acrisio
// Implementação da classe database

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "database.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <regex>

using namespace stdA;

database::database(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port) :
    m_db_address(_db_address), m_db_name(_db_name), m_user_name(_user_name), m_user_pass(_user_pass), m_db_port(_db_port), 
    m_state(false), m_connected(false) {
        
};

database::database(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port) :
	m_db_address(MbToWc(_db_address)), m_db_name(MbToWc(_db_name)), 
	m_user_name(MbToWc(_user_name)), m_user_pass(MbToWc(_user_pass)), m_db_port(_db_port),
	m_state(false), m_connected(false) {
};

database::~database() {};

bool database::is_connected() {
    return m_connected;
};

bool database::is_valid() {
    return m_state;
};

std::string database::parseEscapeKeyword(std::string _value) {
	return std::regex_replace(_value, std::regex(std::string(DB_ESCAPE_KEYWORD_A) + "([\\w\\.\\-_\\s]+)" + DB_ESCAPE_KEYWORD_A), this->makeEscapeKeyword("$1"));
};

std::wstring database::parseEscapeKeyword(std::wstring _value) {
	return std::regex_replace(_value, std::wregex(std::wstring(DB_ESCAPE_KEYWORD_W) + L"([\\w\\.\\-_\\s]+)" + DB_ESCAPE_KEYWORD_W), this->makeEscapeKeyword(L"$1"));
};

bool database::members_empty() {
    return (m_db_name.empty() || m_db_address.empty() || m_user_name.empty() || m_user_pass.empty());
};
