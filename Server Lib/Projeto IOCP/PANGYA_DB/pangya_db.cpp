// Arquivo pangya_db.cpp
// Criado em 25/12/2017 por Acrisio
// Implementação da classe pangya_db

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <unistd.h>
#endif

#include "pangya_db.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"

#include "../UTIL/string_util.hpp"

using namespace stdA;

// Teste call db cmd log
#ifdef _DEBUG
#define _TESTCMD_LOG 0
#endif

#if defined(_WIN32) && (_TESTCMD_LOG == 1)
// !@ Teste
#include <fstream>
#include <map>

struct call_db_cmd_st {
public:
	call_db_cmd_st() : m_hMutex(INVALID_HANDLE_VALUE) {

		m_hMutex = CreateMutexA(NULL, FALSE, "xg_CALL_DB_CMD_LOG");

		if (m_hMutex == NULL)
			_smp::message_pool::getInstance().push(new message("[pangya_db::call_db_cmd_st::call_db_cmd_st][Error] fail to create Mutex. Error: " + std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));
	};
	~call_db_cmd_st() {
		if (isValid())
			CloseHandle(m_hMutex);
	};
	std::map< std::string, std::string > loadCmds() {
		
		std::map< std::string, std::string > v_cmds;

		if (!isValid() || !lock())
			return v_cmds;

		std::ifstream in(url_log);

		if (in.is_open()) {

			std::string name, value;

			while (in.good()) {

				in >> name >> value;

				v_cmds.insert(std::make_pair(name, value));
			}

			in.close();
		}

		if (!unlock())
			_smp::message_pool::getInstance().push(new message("[pangya_db::call_db_cmd_st::loadCmds][Error] fail to release Mutex. Error: " + std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));

		return v_cmds;
	};

	void saveCmds(std::map< std::string, std::string >& _cmds) {

		if (_cmds.empty() || !isValid() || !lock())
			return;

		std::ofstream out(url_log);

		if (out.is_open()) {

			for (auto& el : _cmds)
				out << el.first << " " << el.second << std::endl;

			out.close();
		}

		if (!unlock())
			_smp::message_pool::getInstance().push(new message("[pangya_db::call_db_cmd_st::saveCmds][Error] fail to release Mutex. Error: " + std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));
	};

private:
	bool isValid() {
		return m_hMutex != INVALID_HANDLE_VALUE && m_hMutex != NULL;
	};

	bool lock() {

		if (!isValid())
			return false;

		DWORD dwResult = WaitForSingleObject(m_hMutex, INFINITE);

		return dwResult == WAIT_OBJECT_0 ? true : false;
	};

	bool unlock() {

		if (!isValid())
			return false;

		return ReleaseMutex(m_hMutex) == TRUE;
	};

private:
	HANDLE m_hMutex;
	const char url_log[30] = "H:/Server Lib/call_db_cmd.log";
};

// !@ Teste
bool logExecuteCmds(std::string _name) {

	static call_db_cmd_st cdcs;
	// Load
	auto v_cmds = cdcs.loadCmds();

	auto it = v_cmds.find(_name);

	if (it == v_cmds.end()) {

		v_cmds.insert(std::make_pair(_name, "yes"));

		// Save
		cdcs.saveCmds(v_cmds);

		return true; // show log
	}else if (it->second.compare("no") == 0) {

		it->second = "yes";

		// Save
		cdcs.saveCmds(v_cmds);

		return true; // show log
	}

	return false;
}
// !@ Teste
#endif

list_fifo_asyc< exec_query > pangya_db::m_query_pool;
list_async< exec_query* > pangya_db::m_cache_query;

pangya_db::pangya_db(bool _waitable) : m_exception("", 0), m_waitable(_waitable), 
#if defined(_WIN32)
	hEvent(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	hEvent(nullptr)
#endif 
{
	
#if defined(_WIN32)
	if (_waitable)
		if ((hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
			_smp::message_pool::getInstance().push(new message("[pangya_db::pangya_db][Error] Error ao criar evento. ErrorCode: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 52, GetLastError())), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
	if (_waitable) {

		hEvent = new Event(false, 0u);

		if (!hEvent->is_good()) {

			delete hEvent;

			hEvent = nullptr;

			_smp::message_pool::getInstance().push(new message("[pangya_db::pangya_db][Error] Error ao criar evento. ErrorCode: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 52, errno)), CL_FILE_LOG_AND_CONSOLE));
		}
	}
#endif
};

pangya_db::~pangya_db() {
	
#if defined(_WIN32)
	if (hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(hEvent);

	hEvent = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	if (hEvent != nullptr)
		delete hEvent;

	hEvent = nullptr;
#endif
};

inline void pangya_db::exec(database& _db) {
    
    response *r = nullptr;

    try {
		if ((r = prepareConsulta(_db)) != nullptr) {
			for (auto num_result = 0u; num_result < r->getNumResultSet(); ++num_result) {
				if (r->getResultSetAt(num_result) != nullptr && r->getResultSetAt(num_result)->getNumLines() > 0
							&& r->getResultSetAt(num_result)->getState() == result_set::HAVE_DATA) {
					for (auto _result = r->getResultSetAt(num_result)->getFirstLine(); _result != nullptr; _result = _result->next) {
						lineResult(_result, num_result);
					}
				}// só faz esse else se for mandar uma exception

				clear_result(r->getResultSetAt(num_result));
			}

			clear_response(r);
		}
    }catch (exception& e) {
        //UNREFERENCED_PARAMETER(e);

        if (r != nullptr)
            clear_response(r);

        //throw;
		m_exception = e;
		_smp::message_pool::getInstance().push(new message("[pangya_db::" + _getName() + "::exec][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
    }

#if defined(_WIN32) && (_TESTCMD_LOG == 1)
	// !@ Teste
	if (logExecuteCmds(_getName()))
		_smp::message_pool::getInstance().push(new message("[pangya_db::" + _getName() + "::exec][Log] Executado. ------------------->>", CL_FILE_LOG_AND_CONSOLE));
#endif
};

exception& pangya_db::getException() {
	return m_exception;
};

void pangya_db::waitEvent() {

#if defined(_WIN32)
	if (WaitForSingleObject(hEvent, INFINITE) != WAIT_OBJECT_0)
		throw exception("[pangya_db::" + _getName() + "::waitEvent][Error] nao conseguiu esperar pelo evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 53, GetLastError()));
#elif defined(__linux__)
	if (hEvent == nullptr || hEvent->wait(INFINITE) != WAIT_OBJECT_0)
		throw exception("[pangya_db::" + _getName() + "::waitEvent][Error] nao conseguiu esperar pelo evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 53, errno));
#endif
}

void pangya_db::wakeupWaiter() {

#if defined(_WIN32)
	SetEvent(hEvent);
#elif defined(__linux__)
	if (hEvent != nullptr)
		hEvent->set();
#endif
};

bool pangya_db::isWaitable() {
	return m_waitable;
};

inline response* pangya_db::_insert(database& _db, std::string _query) {
	return _insert(_db, MbToWc(_query));
}

inline response* pangya_db::_insert(database& _db, std::wstring _query) {
	
	/*exec_query query(_query, exec_query::_INSERT);

	postAndWaitResponseQuery(query);

	return query.getRes();*/

	return _db.ExecQuery(_query);
}

inline response* pangya_db::_update(database& _db, std::string _query) {
	return _update(_db, MbToWc(_query));
};

inline response* pangya_db::_update(database& _db, std::wstring _query) {
	
	//exec_query query(_query, exec_query::_UPDATE);

	//postAndWaitResponseQuery(query);

	////clear_response(query.getRes());
	//return query.getRes();

	return _db.ExecQuery(_query);
};

inline response* pangya_db::_delete(database& _db, std::string _query) {
	return _delete(_db, MbToWc(_query));
};

inline response* pangya_db::_delete(database& _db, std::wstring _query) {
	
	//exec_query query(_query, exec_query::_DELETE);

	//postAndWaitResponseQuery(query);

	////clear_response(query.getRes());
	//return query.getRes();

	return _db.ExecQuery(_query);
};

inline response* pangya_db::consulta(database& _db, std::string _query) {
	return consulta(_db, MbToWc(_query));
};

inline response* pangya_db::consulta(database& _db, std::wstring _query) {
	
	/*exec_query query(_query, exec_query::_QUERY);

	postAndWaitResponseQuery(query);

	return query.getRes();*/

	return _db.ExecQuery(_db.parseEscapeKeyword(_query));
};

inline response* pangya_db::procedure(database& _db, std::string _name, std::string _params) {
	return procedure(_db, MbToWc(_name), MbToWc(_params));
};

inline response* pangya_db::procedure(database& _db, std::wstring _name, std::wstring _params) {
	
	/*exec_query query(_name, _params, exec_query::_PROCEDURE);

	postAndWaitResponseQuery(query);

	return query.getRes();*/

	return _db.ExecProc(_db.parseEscapeKeyword(_name), _params);
};

inline void pangya_db::postAndWaitResponseQuery(exec_query& _query) {
	DWORD wait = INFINITE;

	//pangya_base_db::m_query_pool.push(&_query); // post query
	m_query_pool.push(&_query);

    while (1) {
        try {
			_query.waitEvent(wait);

			break;
        }catch (exception& e) {
            if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::EXEC_QUERY) {
                if (STDA_ERROR_DECODE(e.getCodeError()) == 7 && wait == INFINITE) {
                    wait = 1000;    // Espera um segundo se não for da próxima dá error
                    continue;
                }

                //pangya_base_db::m_query_pool.remove(&_query);
				m_query_pool.remove(&_query);
                
                throw;
            }else throw;
        }catch (std::exception& e) {
            throw exception("System error: " + std::string(e.what()), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 100, 0));
        }catch (...) {
            throw exception("System error: Desconhecido", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 100, 1));
        }
    }
};

inline void pangya_db::clear_result(result_set*& _rs) {
	
	if (_rs != nullptr)
		delete _rs;

	_rs = nullptr;
};

inline void pangya_db::clear_response(response* _res) {
	
	if (_res != nullptr)
		delete _res;
};

inline void pangya_db::checkColumnNumber(uint32_t _number_cols1, uint32_t _number_cols2) {

	if (_number_cols1 != 0 && _number_cols1 != _number_cols2)
		throw exception("[pangya_db::" + _getName() + "::checkColumnNumber][Error] numero de colunas retornada pela consulta sao diferente do esperado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 2, 0));
};

inline void pangya_db::checkResponse(response* r, std::string _exception_msg) {

	if (r == nullptr || (r->getNumResultSet() <= 0 && r->getRowsAffected() == -1))
		throw exception("[pangya_db::" + _getName() + "::checkResponse][Error] " + _exception_msg, STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 1, 0));
};

inline void pangya_db::checkResponse(response* r, std::wstring _exception_msg) {
	
	if (r == nullptr || (r->getNumResultSet() <= 0 && r->getRowsAffected() == -1))
		throw exception(L"[pangya_db::" + _wgetName() + L"::checkResponse][Error] " + _exception_msg, STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 1, 0));
};

bool pangya_db::compare(exec_query* _query1, exec_query* _query2) {
	return _query1->getQuery().compare(_query2->getQuery()) == 0;
};

bool pangya_db::is_valid_c_string(char* _ptr_c_string) {
	return _ptr_c_string != nullptr && _ptr_c_string[0] != 0;
};
