// Arquivo postgresql.cpp
// Criado em 16/07/2021 as 09:23 Acrisio
// Implentação da classe postgresql

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(__linux__)
	#include <codecvt>
	#include <locale>

	#ifndef UNICODE
		#define UNICODE
	#endif
#endif

#include "postgresql.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../TYPE/stda_error.h"
#include <cstdlib>

#include "../UTIL/string_util.hpp"

#include <regex>

#define DB_HOST L"pangya"
#define DB_NAME L"pangya"
#define USER_NAME L"pangya"
#define USER_PASS L"pangya"
#define DB_PORT 5432u           // Essa opção só funciona se o sql server for o pago que o express não aceita o por IP

using namespace stdA;

postgresql::postgresql() : database(DB_HOST, DB_NAME, USER_NAME, USER_PASS, DB_PORT) {

	try {

		init();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[postgresql::postgresql][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = false;
	}
};

postgresql::postgresql(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port)
	: database(_db_address, _db_name, _user_name, _user_pass, _db_port) {
	
	try {

		init();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[postgresql::postgresql][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = false;
	}
}

postgresql::postgresql(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port)
	: database(_db_address, _db_name, _user_name, _user_pass, _db_port) {
	
	try {

		init();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[postgresql::postgresql][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = false;
	}
}

postgresql::~postgresql() {
    destroy();
};

void postgresql::init() {
    
	try {
		RETCODE ret = SQL_ERROR;

		HandleDiagnosticRecord(NULL, SQL_NULL_HANDLE, ALLOC_HANDLE_FAIL_ENV, ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_ctx.hEnv));

		HandleDiagnosticRecord(m_ctx.hEnv, SQL_HANDLE_ENV, SET_ATTR_ENV_FAIL, ret = SQLSetEnvAttr(m_ctx.hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0));

		HandleDiagnosticRecord(m_ctx.hEnv, SQL_HANDLE_ENV, ALLOC_HANDLE_FAIL_DBC, ret = SQLAllocHandle(SQL_HANDLE_DBC, m_ctx.hEnv, &m_ctx.hDbc));

		m_state = true;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(L"[postgresql::init][Log] Inicializou postgresql instancia com sucesso!", CL_FILE_LOG_AND_CONSOLE));
#endif
	
	}catch (exception& e) {

		if (STDA_ERROR_DECODE(e.getCodeError()) == SET_ATTR_ENV_FAIL || STDA_ERROR_DECODE(e.getCodeError()) == ALLOC_HANDLE_FAIL_ENV) {

			if (m_ctx.hEnv != SQL_NULL_HANDLE)
				SQLFreeHandle(SQL_HANDLE_ENV, &m_ctx.hEnv);
		
		}else if (STDA_ERROR_DECODE(e.getCodeError()) == ALLOC_HANDLE_FAIL_DBC) {

			if (m_ctx.hEnv != SQL_NULL_HANDLE)
				SQLFreeHandle(SQL_HANDLE_ENV, &m_ctx.hEnv);

			if (m_ctx.hDbc != SQL_NULL_HANDLE)
				SQLFreeHandle(SQL_HANDLE_DBC, &m_ctx.hDbc);
		}

		// Relança para quem chamou tratar o erro
		throw;
	}
};

void postgresql::destroy() {


    if (is_connected())
        disconnect();

    if (m_ctx.hDbc != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_DBC, m_ctx.hDbc);

    if (m_ctx.hEnv != SQL_NULL_HANDLE)
        SQLFreeHandle(SQL_HANDLE_ENV, m_ctx.hEnv);

    m_state = false;

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message(L"[postgresql::destroy][Log] Destruiu o postgresql instancia.", CL_FILE_LOG_AND_CONSOLE));
#endif
};

bool postgresql::hasGoneAway() {

	if (!m_state || !m_connected || m_ctx.hStmt == SQL_NULL_HANDLE)
		return true;

	if (!SQL_SUCCEEDED(SQLExecDirectA(m_ctx.hStmt, (SQLCHAR*)"SELECT 1", SQL_NTS)))
		return true;

	SQLMoreResults(m_ctx.hStmt);

	return false;
};

void postgresql::connect() {

	try {

		if (!is_valid())
			init();

		if (is_connected())
			throw exception("[postgresql::connect][Error] Ja esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_POSTGRESQL, HAS_CONNECT, 0));

		RETCODE ret = SQL_ERROR;

#if defined(_WIN32)
		std::wstring connect_str = std::wstring(std::wstring(L"DSN=") + m_db_address + L";DATABASE=" + m_db_name + L";UID=" + m_user_name + L";PWD=" + m_user_pass);
#elif defined(__linux__)
		// wstring to u16string, Linux wstring é wchar_t que é char32_t
		std::u16string connect_str = [](std::wstring _el) -> std::u16string {
			return std::u16string(_el.begin(), _el.end());
		}(std::wstring(std::wstring(L"DSN=") + m_db_address + L";DATABASE=" + m_db_name + L";UID=" + m_user_name + L";PWD=" + m_user_pass));
#endif

		HandleDiagnosticRecord(m_ctx.hDbc, SQL_HANDLE_DBC, CONNECT_DRIVER_FAIL, ret = SQLDriverConnect(m_ctx.hDbc, 
#if defined(_WIN32)
			GetDesktopWindow(),
#elif defined(__linux__)
			nullptr,
#endif
			(SQLWCHAR*)connect_str.c_str(),
			SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT));

		m_connected = true;

		HandleDiagnosticRecord(m_ctx.hDbc, SQL_HANDLE_DBC, ALLOC_HANDLE_FAIL_STMT, ret = SQLAllocHandle(SQL_HANDLE_STMT, m_ctx.hDbc, &m_ctx.hStmt));

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(L"[postgresql::connect][Log] Connectou com o HOST=" + m_db_address + L"\tDATABASE=" + m_db_name + L"\tUID=" + m_user_name + L"\tPWD=" + m_user_pass, CL_FILE_LOG_AND_CONSOLE));
#endif
	}catch (exception& e) {

		if (STDA_ERROR_DECODE(e.getCodeError()) == ALLOC_HANDLE_FAIL_STMT) {

			if (m_ctx.hStmt != SQL_NULL_HANDLE)
				SQLFreeHandle(SQL_HANDLE_STMT, &m_ctx.hStmt);

			SQLDisconnect(m_ctx.hDbc);
		}

		m_connected = false;

		// Relança para quem chamou tratar o erro
		throw;
	}
};

void postgresql::connect(std::wstring _db_address, std::wstring _db_name, std::wstring _user_name, std::wstring _user_pass, unsigned short _db_port) {
    
	m_db_address = _db_address;
    m_db_name = _db_name;
    m_user_name = _user_name;
    m_user_pass = _user_pass;
    m_db_port = _db_port;

    connect();
};

void stdA::postgresql::connect(std::string _db_address, std::string _db_name, std::string _user_name, std::string _user_pass, unsigned short _db_port) {
	connect(MbToWc(_db_address), MbToWc(_db_name), MbToWc(_user_name), MbToWc(_user_pass), _db_port);
};

void postgresql::reconnect() {
    disconnect();
    connect();
};

void postgresql::disconnect() {

    if (is_connected()) {

        if (m_ctx.hStmt != SQL_NULL_HANDLE)
            SQLFreeHandle(SQL_HANDLE_STMT, m_ctx.hStmt);

        SQLDisconnect(m_ctx.hDbc);
    }

    m_connected = false;

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message(L"[postgresql::disconnect][Log] Desconnectou do HOST=" + m_db_address, CL_FILE_LOG_AND_CONSOLE));
#endif
};

response* postgresql::ExecQuery(std::wstring _query) {
   
	if (_query.empty())
        throw exception("[postgresql::ExecQuery][Error] _query empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_POSTGRESQL, INVALID_PARAMETER, 0));

	if (!is_valid())
		init();

	if (!is_connected())
		connect();

    RETCODE ret = SQL_ERROR;
    response *res = new response;
    result_set *result = nullptr;

	try {

		HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, EXEC_QUERY_FAIL, ret = SQLExecDirect(m_ctx.hStmt, 
#if defined(_WIN32)
			(SQLWCHAR*)_query.c_str(), 
#elif defined(__linux__)
			(SQLWCHAR*)[](std::wstring& _ws) -> std::u16string {
				return std::u16string(_ws.begin(), _ws.end());
			}(_query).c_str(), 
#endif
			SQL_NTS), _query);

		SQLSMALLINT numResults = 0;
		SQLLEN numRows = 0;
		size_t i;
		SQLLEN szData;
		SQLLEN type_col = 0;
		char **col = nullptr;

		do {

			HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLNumResultCols(m_ctx.hStmt, &numResults), _query);
			HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLRowCount(m_ctx.hStmt, &numRows), _query);           // Linha afetadas, não é a quantidade de linha retornada não

			res->setRowsAffected(numRows);

			if (numResults > 0) {
				ret = SQLFetch(m_ctx.hStmt);

				if (ret != SQL_SUCCESS && ret != SQL_NO_DATA) {
					HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, FETCH_QUERY_FAIL, ret, _query);
				}else if (ret != SQL_NO_DATA) {
					result = new result_set(result_set::HAVE_DATA, numResults, (int)numRows);

					while (ret == SQL_SUCCESS) {

						result->addLine();	// Adiciona linha

						for (i = 1; i <= (size_t)numResults; ++i) {
							HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLColAttribute(m_ctx.hStmt, (SQLUSMALLINT)i, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, &szData), _query);

							// Verifica tipo da coluna
							HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLColAttribute(m_ctx.hStmt, (SQLUSMALLINT)i, SQL_DESC_TYPE, NULL, 0, NULL, &type_col), _query);

							col = result->getColAt(i - 1);
							*col = new char[szData + 1];

							// Estáva C_CHAR
							if ((ret = SQLGetData(m_ctx.hStmt, (SQLUSMALLINT)i, (type_col == SQL_C_BINARY ? SQL_C_BINARY : SQL_C_CHAR), *col, szData + 1, &szData)) != SQL_SUCCESS)
								HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, ret, _query);

							// !@Error quando o valor era null, o SQLGetData retornava nada no buffer e -1 no szData
							// e o cliente pensava que tinha dados por que a variável estava com lixo na memória
							// e pensava que tinha dados.
							// !$Correção Verifica se pegou valor nulo, ele retorna -1 no szData, 
							// deleta o dados alocado e coloca o valor nullptr no ponteiro para o cliente saber
							if (szData <= 0 && *col != nullptr) {
								delete[] *col;
								*col = nullptr;
							}
						}

						// Next Line
						ret = SQLFetch(m_ctx.hStmt);
					}

					res->addResultSet(result);

					result = nullptr;

					if (ret != SQL_SUCCESS && ret != SQL_NO_DATA) {
						HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, FETCH_QUERY_FAIL, ret, _query);
					}
				}else
					res->addResultSet(new result_set(result_set::_NO_DATA, numResults, (int)numRows));
			}else
				res->addResultSet(new result_set(result_set::_UPDATE_OR_DELETE, numResults, (int)numRows));

		} while ((ret = SQLMoreResults(m_ctx.hStmt)) == SQL_SUCCESS);	// Next Result

		if (ret != SQL_SUCCESS && ret != SQL_NO_DATA)
			HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, MORE_RESULTS, ret, _query);
	
	}catch (exception& e) {

		if (STDA_ERROR_DECODE(e.getCodeError()) == GERAL_ERROR)
			clear_stmt(_query);

		// Erro no link de vinculação da conexão com o banco de dados, reconecta
		if (e.getMessageError().find("08S01") != std::string::npos || e.getFullMessageError().find("24000") != std::string::npos)
			reconnect();	// Reconnecta

		// Relança para quem chamou tratar o erro
		throw;
	}

    return res;
};

response* postgresql::ExecQuery(std::string _query) {
	std::wstring tmp = MbToWc(_query);

	return ExecQuery(tmp);
};

response* postgresql::ExecProc(std::wstring _proc_name, std::wstring _proc_params) {
    
	if (_proc_name.empty())
        throw exception("[postgresql::ExecProc][Error] _proc_name empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_POSTGRESQL, INVALID_PARAMETER, 1));

	if (!is_valid())
		init();

	if (!is_connected())
		connect();

    RETCODE ret = SQL_ERROR;
    response *res = new response;
    result_set *result = nullptr;

	std::wstring _query = (L"select * from " + _proc_name + L"(" + _proc_params + L")");

	try {

		HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, EXEC_QUERY_FAIL, ret = SQLExecDirect(m_ctx.hStmt, 
#if defined(_WIN32)
			(SQLWCHAR*)_query.c_str(), 
#elif defined(__linux__)
			(SQLWCHAR*)[](std::wstring& _ws) -> std::u16string {
				return std::u16string(_ws.begin(), _ws.end());
			}(_query).c_str(),
#endif
			SQL_NTS), _query);

		SQLSMALLINT numResults = 0;
		SQLLEN numRows = 0;
		size_t i;
		SQLLEN szData = 0;
		SQLLEN type_col = 0;
		char **col = nullptr;

		do {

			HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLNumResultCols(m_ctx.hStmt, &numResults), _query);
			HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLRowCount(m_ctx.hStmt, &numRows), _query);           // Linha afetadas, não é a quantidade de linha retornada não

			res->setRowsAffected(numRows);

			if (numResults > 0) {
				ret = SQLFetch(m_ctx.hStmt);	// Fetch Line

				if (ret != SQL_SUCCESS && ret != SQL_NO_DATA) {
					HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, FETCH_QUERY_FAIL, ret, _query);
				}else if (ret != SQL_NO_DATA) {
				
					result = new result_set(result_set::HAVE_DATA, numResults, (int)numRows);

					while (ret == SQL_SUCCESS) {

						result->addLine();	// Adiciona linha

						for (i = 1; i <= (size_t)numResults; ++i) {
							HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLColAttribute(m_ctx.hStmt, (SQLUSMALLINT)i, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, &szData), _query);

							// Verifica tipo da coluna
							HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLColAttribute(m_ctx.hStmt, (SQLUSMALLINT)i, SQL_DESC_TYPE, NULL, 0, NULL, &type_col), _query);

							col = result->getColAt(i - 1);
							*col = new char[szData + 1];

							// Estáva C_CHAR
							if ((ret = SQLGetData(m_ctx.hStmt, (SQLUSMALLINT)i, (type_col == SQL_C_BINARY ? SQL_C_BINARY : SQL_C_CHAR), *col, szData + 1, &szData)) != SQL_SUCCESS)
								HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, ret, _query);

							// !@Error quando o valor era null, o SQLGetData retornava nada no buffer e -1 no szData
							// e o cliente pensava que tinha dados por que a variável estava com lixo na memória
							// e pensava que tinha dados.
							// !$Correção Verifica se pegou valor nulo, ele retorna -1 no szData, 
							// deleta o dados alocado e coloca o valor nullptr no ponteiro para o cliente saber
							if (szData <= 0 && *col != nullptr) {
								delete[] *col;
								*col = nullptr;
							}
						}

						// Next Line
						ret = SQLFetch(m_ctx.hStmt);
					}

					res->addResultSet(result);

					result = nullptr;

					if (ret != SQL_SUCCESS && ret != SQL_NO_DATA) {
						HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, FETCH_QUERY_FAIL, ret, _query);
					}
				}else
					res->addResultSet(new result_set(result_set::_NO_DATA, numResults, (int)numRows));
			}else
				res->addResultSet(new result_set(result_set::_UPDATE_OR_DELETE, numResults, (int)numRows));

		} while ((ret = SQLMoreResults(m_ctx.hStmt)) == SQL_SUCCESS);	// Nex Result

		if (ret != SQL_SUCCESS && ret != SQL_NO_DATA)
			HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, MORE_RESULTS, ret, _query);
	
	}catch (exception& e) {

		if (STDA_ERROR_DECODE(e.getCodeError()) == GERAL_ERROR)
			clear_stmt(_query);

		// Erro no link de vinculação da conexão com o banco de dados, reconecta
		if (e.getMessageError().find("08S01") != std::string::npos || e.getFullMessageError().find("24000") != std::string::npos)
			reconnect();	// Reconnecta

		// Relança para quem chamou tratar o erro
		throw;
	}

    return res;
};

response* postgresql::ExecProc(std::string _proc_name, std::string _proc_params) {
	
	std::wstring tmp = MbToWc(_proc_name);
	std::wstring tmp2 = MbToWc(_proc_params);

	return ExecProc(tmp, tmp2);
};

std::string postgresql::makeText(std::string _value) {
	return "'" + _value + "'::text";
};

std::wstring postgresql::makeText(std::wstring _value) {
	return L"'" + _value + L"'::text";
};

std::string postgresql::makeEscapeKeyword(std::string _value) {
	return "\"" + _value + "\"";
};

std::wstring postgresql::makeEscapeKeyword(std::wstring _value) {
	return L"\"" + _value + L"\"";
};

void postgresql::clear_stmt(std::wstring _query) {

	RETCODE ret = SQL_ERROR;

	SQLSMALLINT numResults = 0;
	SQLLEN numRows = 0;

	do {

		HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLNumResultCols(m_ctx.hStmt, &numResults), _query);
		HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, GERAL_ERROR, SQLRowCount(m_ctx.hStmt, &numRows), _query);           // Linha afetadas, não é a quantidade de linha retornada não

		if (numResults > 0) {
			ret = SQLFetch(m_ctx.hStmt);

			if (ret != SQL_SUCCESS && ret != SQL_NO_DATA) {
				HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, FETCH_QUERY_FAIL, ret, _query);
			}else if (ret != SQL_NO_DATA) {

				// Next Line
				while (ret == SQL_SUCCESS)
					ret = SQLFetch(m_ctx.hStmt);

				if (ret != SQL_SUCCESS && ret != SQL_NO_DATA) {
					HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, FETCH_QUERY_FAIL, ret, _query);
				}
			}
		}

	} while ((ret = SQLMoreResults(m_ctx.hStmt)) == SQL_SUCCESS);	// Next Result

	if (ret != SQL_SUCCESS && ret != SQL_NO_DATA)
		HandleDiagnosticRecord(m_ctx.hStmt, SQL_HANDLE_STMT, MORE_RESULTS, ret, _query);
};

void postgresql::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, ERROR_TYPE _error_type, RETCODE RetCode, std::wstring _query) {
   
	// Não precisa tratar a msg nenhum por que não tem, a operação executada foi um sucesso
	if (RetCode == SQL_SUCCESS)
		return;
	
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR		wszMessage2[2000];
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];
    std::wstring tmp;

	if (RetCode == SQL_INVALID_HANDLE)
		throw exception(L"Invalid handle!\n", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_POSTGRESQL, INVALID_HANDLE, 0));

	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
                        (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
#if defined(_WIN32)
		if (wcsncmp(wszState, L"01004", 5)) {
			swprintf_s(wszMessage2, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);

            tmp += wszMessage2;
        }
#elif defined(__linux__)
		if (std::u16string((const char16_t*)wszState).compare(u"01004")/*wcsncmp((const wchar_t*)wszState, L"01004", 5)*/) {
			
			//swprintf((wchar_t*)wszMessage2, sizeof(wszMessage2), L"[%5.5S] %S (%d)\n", (const wchar_t*)wszState, (const wchar_t*)wszMessage, iError);
			// tem que ser assim, por que MS usa char16_t no wchar_t, Linux é char32_t
			std::u16string u16tmp = u"[";

			u16tmp += (char16_t*)wszState;
			u16tmp += u"] ";
			u16tmp += (char16_t*)wszMessage;
			u16tmp += u" (";
			u16tmp += [](const SQLINTEGER& _int) -> std::u16string {
				std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::little_endian>, char16_t> conv;
				return conv.from_bytes(std::to_string(_int));
			}(iError);
			u16tmp += u")\n";

			// u16string to wstring(char32_t)
            tmp += [](std::u16string& _el) -> std::wstring {
				return std::wstring(_el.begin(), _el.end());
			}(u16tmp);
        }
#endif
	}

#ifdef SHOW_SUCCESS_INFO_MSG
	if (RetCode == SQL_SUCCESS_WITH_INFO) {
		if (!tmp.empty())
			_smp::message_pool::getInstance().push(new message(tmp + (_query.empty() ? L"" : std::wstring(L". Query: " + _query)), CL_FILE_LOG_AND_CONSOLE));
	}else if (RetCode != SQL_NO_DATA)	// Teste esse aqui não estava
		throw exception(tmp + (_query.empty() ? L"" : std::wstring(L". Query: " + _query)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_POSTGRESQL, _error_type, (RetCode != -1 ? RetCode : iError)));
#else
	if (RetCode == SQL_SUCCESS_WITH_INFO) {
		if (!tmp.empty())
			_smp::message_pool::getInstance().push(new message(tmp + (_query.empty() ? L"" : std::wstring(L". Query: " + _query)), CL_ONLY_FILE_LOG));
	}else if (RetCode != SQL_NO_DATA)
		throw exception(tmp + (_query.empty() ? L"" : std::wstring(L". Query: " + _query)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_POSTGRESQL, _error_type, (RetCode != -1 ? RetCode : iError)));
#endif
};
