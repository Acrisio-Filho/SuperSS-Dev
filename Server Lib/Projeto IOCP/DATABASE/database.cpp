// Arquivo database.cpp
// Criado em 27/01/2018 as 23:13 por Acrisio
// Implementação da classe database

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "database.h"
#include "../../Projeto IOCP/UTIL/string_util.hpp"

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

bool database::members_empty() {
    return (m_db_name.empty() || m_db_address.empty() || m_user_name.empty() || m_user_pass.empty());
};
