// Arquivo mysql_db.cpp
// Criado em 23/07/2017 por Acrisio
// Implementação da classe mysql_db

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "../UTIL/message_pool.h"
#include "mysql_db.h"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"

#include "../UTIL/string_util.hpp"

#include <cstring>

#include <regex>

#define DB_HOST "localhost"
#define DB_NAME "pangya"
#define USER_NAME "pangya"
#define USER_PASS "pangya"
#define DB_PORT 3307u

using namespace stdA;

#pragma comment(lib, PATH_MYSQL_LIB)

mysql_db::mysql_db(unsigned short _db_port) : database(DB_HOST, DB_NAME, USER_NAME, USER_PASS, _db_port) {
    m_state = false;
    m_connected = false;

    init();
};

mysql_db::mysql_db(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port) :
    database(_db_address, _db_name, _user_name, _user_pass, _db_port) {
    
    m_state = false;
    m_connected = false;

    init();
};

mysql_db::mysql_db(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port) :
	database(_db_address, _db_name, _user_name, _user_pass, _db_port) {

	m_state = false;
	m_connected = false;

	init();
};

mysql_db::~mysql_db() {
	destroy();
};

void mysql_db::init() {

    if ((m_mysql = mysql_init(nullptr)) == nullptr)
        throw exception("[mysql_db::init][Error] Nao conseguiu inicializar os dados do mysql. Msg: " + std::string(mysql_error(m_mysql)), 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, ALLOC_HANDLE_FAIL_ENV, mysql_errno(m_mysql)));

	_smp::message_pool::getInstance().push(new message("[mysql_db::init][Log] Inicializou o mysql com sucesso."));

    m_state = true;
}

void mysql_db::destroy() {

	if (is_connected())
		disconnect();

	m_state = false;

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message(L"[mysql_db::destroy][Log] Destruiu o mysql instancia.", CL_FILE_LOG_AND_CONSOLE));
#endif
};

bool mysql_db::hasGoneAway() {

	if (!m_state || !m_connected || m_mysql == nullptr)
		return true;

	int ret = mysql_ping(m_mysql);

	return ret != 0;
};

void mysql_db::connect() {

    if (!is_valid())
        init();

	if (is_connected())
		throw exception("[mysql_db::connect][Log] Ja esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, HAS_CONNECT, 0));
    
    if (!mysql_real_connect(m_mysql, std::string(m_db_address.begin(), m_db_address.end()).c_str(),
									 std::string(m_user_name.begin(), m_user_name.end()).c_str(), 
									 std::string(m_user_pass.begin(), m_user_pass.end()).c_str(), 
									 std::string(m_db_name.begin(), m_db_name.end()).c_str(), m_db_port, nullptr, CLIENT_MULTI_STATEMENTS))
        throw exception("[mysql_db::connect][Error] Nao conseguiu connectar com o banco de dadas: '" + std::string(m_db_name.begin(), m_db_name.end()) 
				+ "'. Msg: " + std::string(mysql_error(m_mysql)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, CONNECT_DRIVER_FAIL, mysql_errno(m_mysql)));

    _smp::message_pool::getInstance().push(new message(L"[mysql_db::connect][Log] Connectou ao banco de dadaos: '" + m_db_name + L"'"));

	if (!mysql_set_character_set(m_mysql, "utf8mb4"))
		_smp::message_pool::getInstance().push(new message(std::string("[mysql_db::connect][Log] New client character set: ") + mysql_character_set_name(m_mysql), CL_FILE_LOG_AND_CONSOLE));

    m_connected = true;
};

void mysql_db::connect(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port) {
	connect(MbToWc(_db_address), MbToWc(_db_name), MbToWc(_user_name), MbToWc(_user_pass), _db_port);
};

void mysql_db::connect(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port) {
	m_db_address = _db_address;
	m_db_name = _db_name;
	m_user_name = _user_name;
	m_user_pass = _user_pass;
	m_db_port = _db_port;

	connect();
};

void mysql_db::reconnect() {

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message(L"[mysql_db::reconnect][Log] Reconnectando com o banco de dadaos: '" + m_db_name + L"'"));
#endif

    disconnect();
    connect();
};

void mysql_db::disconnect() {

    if (m_mysql != nullptr && m_connected)
        mysql_close(m_mysql);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message(L"[mysql_db::disconnect][Log] Desconnectou do banco de dadaos: '" + m_db_name + L"'"));
#endif
    
    m_mysql = nullptr;

	m_state = false;
    m_connected = false;
};

response* mysql_db::ExecQuery(std::string _query) {

	if (!m_state || !m_connected || m_mysql == nullptr)
		throw exception("[mysql_db::ExecQuery][Error] Nao pode executar query, porque nao esta conectado com o banco de dados.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, INVALID_HANDLE, 0)); /*0x1040000 DB ERRO ou AVISO*/

	if (_query.empty())
		throw exception("[mysql_db::ExecQuery][Error] _query empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, INVALID_PARAMETER, 0));

	response *res = new response;
	result_set *result = nullptr;
	MYSQL_RES *r = nullptr;
	MYSQL_ROW row = nullptr;
#if defined(_WIN32)
	errno_t error = 0;
#elif defined(__linux__)
	int error = 0;
#endif

	int64_t numRows;
	short numResults;
#if defined(_WIN32)
	unsigned long *lengths;
#elif defined(__linux__)
	unsigned long *lengths;
#endif
	
	char **col = nullptr;
	size_t i = 0;

	if (mysql_query(m_mysql, _query.c_str()) != 0)
		throw exception("[mysql_db::ExecQuery][Error] Nao conseguiu executar a query: " + _query + ". Msg: " + std::string(mysql_error(m_mysql)), 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, EXEC_QUERY_FAIL, mysql_errno(m_mysql)));

	do {

		numRows = mysql_affected_rows(m_mysql);
		numResults = mysql_field_count(m_mysql);

		res->setRowsAffected(numRows);

		if (numResults > 0) {
			if ((r = mysql_use_result(m_mysql)) != nullptr) { // Antes era mysql_store_result
				row = mysql_fetch_row(r);

				if (row != nullptr) {
					result = new result_set(result_set::HAVE_DATA, numResults, (int)numRows);

					while (row) {
						result->addLine();		// Add Line
						
						lengths = mysql_fetch_lengths(r);

						for (i = 0; i < (size_t)numResults; ++i) {
							col = result->getColAt(i);
							*col = new char[lengths[i] + 1];

#if defined(_WIN32)
							strncpy_s(*col, lengths[i] + 1, row[i], lengths[i]);
#elif defined(__linux__)
							strncpy(*col, row[i], lengths[i]);
#endif

							(*col)[lengths[i]] = '\0';
						}

						// Next Result
						row = mysql_fetch_row(r);
					}

					res->addResultSet(result);

					result = nullptr;
				}else
					res->addResultSet(new result_set(result_set::_NO_DATA, numResults, (int)numRows));

				mysql_free_result(r);
			}else if (mysql_errno(m_mysql) != 0) {

				mysql_free_result(r);

				if (res != nullptr)
					delete res;

				throw exception("[mysql_db::ExecQuery][Error] Nao conseguiu guarda o resultado que recebeu do banco de dados. Msg: " + std::string(mysql_error(m_mysql)), 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, FETCH_QUERY_FAIL, mysql_errno(m_mysql)));
			}
		}
	} while (mysql_more_results(m_mysql) && (error = mysql_next_result(m_mysql)) == 0);

	if (error > 0)
		throw exception("[mysql_db::ExecQuery][Error] Nao conseguiu executar mysql_nex_result. Msg: " + std::string(mysql_error(m_mysql)), 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, MORE_RESULTS, mysql_errno(m_mysql)));

	return res;
};

response* mysql_db::ExecQuery(std::wstring _query) {
	std::string tmp = WcToMb(_query);

	return ExecQuery(tmp);
};

response* mysql_db::ExecProc(std::string _proc_name, std::string _proc_params) {

	if (!m_state || !m_connected || m_mysql == nullptr)
		throw exception("[mysql_db::ExecProc][Error] Nao pode executar query, porque nao esta conectado com o banco de dados.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, INVALID_HANDLE, 0)); /*0x1040000 DB ERRO ou AVISO*/

	if (_proc_name.empty())
		throw exception("[mysql_db::ExecProc][Error] _proc_name empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, INVALID_PARAMETER, 1));

	response *res = new response;
	result_set *result = nullptr;
	MYSQL_RES *r = nullptr;
	MYSQL_ROW row = nullptr;
#if defined(_WIN32)
	errno_t error = 0;
#elif defined(__linux__)
	int error = 0;
#endif

	int64_t numRows;
	short numResults;
#if defined(_WIN32)
	unsigned long *lengths;
#elif defined(__linux__)
	unsigned long *lengths;
#endif
	
	char **col = nullptr;
	size_t i = 0;

	if (mysql_query(m_mysql, ("CALL " + _proc_name + "(" + _proc_params + ")").c_str()) != 0)
		throw exception("[mysql_db::ExecProc][Error] Nao conseguiu executar a query: " + ("CALL " + _proc_name + "(" + _proc_params + ")") + ". Msg: " + std::string(mysql_error(m_mysql)), 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, EXEC_QUERY_FAIL, mysql_errno(m_mysql)));

	do {

		numRows = mysql_affected_rows(m_mysql);
		numResults = mysql_field_count(m_mysql);

		res->setRowsAffected(numRows);

		if (numResults > 0) {
			if ((r = mysql_use_result(m_mysql)) != nullptr) { // Antes era mysql_store_result
				row = mysql_fetch_row(r);

				if (row != nullptr) {
					result = new result_set(result_set::HAVE_DATA, numResults, (int)numRows);

					while (row) {
						result->addLine();		// Add Line
						
						lengths = mysql_fetch_lengths(r);

						for (i = 0; i < (size_t)numResults; ++i) {
							col = result->getColAt(i);
							*col = new char[lengths[i] + 1];

#if defined(_WIN32)
							strncpy_s(*col, lengths[i] + 1, row[i], lengths[i]);
#elif defined(__linux__)
							strncpy(*col, row[i], lengths[i]);
#endif
							
							(*col)[lengths[i]] = '\0';
						}

						// Next Result
						row = mysql_fetch_row(r);
					}

					res->addResultSet(result);

					result = nullptr;
				}else
					res->addResultSet(new result_set(result_set::_NO_DATA, numResults, (int)numRows));

				mysql_free_result(r);
			}else if (mysql_errno(m_mysql) != 0) {

				mysql_free_result(r);

				if (res != nullptr)
					delete res;

				throw exception("[mysql_db::ExecProc][Error] Nao conseguiu guarda o resultado que recebeu do banco de dados. Msg: " + std::string(mysql_error(m_mysql)), 
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, FETCH_QUERY_FAIL, mysql_errno(m_mysql)));
			}
		}
	} while (mysql_more_results(m_mysql) && (error = mysql_next_result(m_mysql)) == 0);

	if (error > 0)
		throw exception("[mysql_db::ExecProc][Error] Nao conseguiu executar mysql_nex_result. Msg: " + std::string(mysql_error(m_mysql)), 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::_MYSQL, MORE_RESULTS, mysql_errno(m_mysql)));

	return res;
};

response* mysql_db::ExecProc(std::wstring _proc_name, std::wstring _proc_params) {
	
	std::string tmp = WcToMb(_proc_name);
	std::string tmp2 = WcToMb(_proc_params);

	return ExecProc(tmp, tmp2);
};

std::string mysql_db::makeText(std::string _value) {
	return "'" + std::regex_replace(_value, std::regex("\\\\"), "\\\\\\") + "'";
};

std::wstring mysql_db::makeText(std::wstring _value) {
	return L"'" + std::regex_replace(_value, std::wregex(L"\\\\"), L"\\\\\\") + L"'";
};

std::string mysql_db::makeEscapeKeyword(std::string _value) {
	return "`" + _value + "`";
};

std::wstring mysql_db::makeEscapeKeyword(std::wstring _value) {
	return L"`" + _value + L"`";
};