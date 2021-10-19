// Arquivo pangya_db.h
// Criado em 05/12/2017 por Acrisio
// Definição da classe base pangya_base_db
// e dos macros e funçoes de helper do pangya_db

#pragma once
#ifndef _STDA_PANGYA_DB_H
#define _STDA_PANGYA_DB_H

#include "../TYPE/pangya_st.h"

#include "../TYPE/list_fifo.h"
#include "../TYPE/list_async.h"
#include "exec_query.h"

/*#define DB_HOST "localhost"
#define DB_NAME "pangya"
#define USER_NAME "pangya"
#define USER_PASS "pangya"
#define DB_PORT 3307u*/

/*#define CLEAR_MYSQL_RES(_r) if (!(_r).empty()) { \
								mysql_free_result((_r).front()); \
								(_r).erase((_r).begin()); \
								(_r).shrink_to_fit(); \
							} \*/

#define CLEAR_MYSQL_RES(_r) if ((_r) != nullptr) delete (_r); \
							(_r) = nullptr; \

//#define CLEAR_ALL_MYSQL_RES(_r) while (!(_r).empty()) { \
//									mysql_free_result((_r).front()); \
//									(_r).erase((_r).begin()); \
//								} \
//								(_r).shrink_to_fit(); \

#define CLEAR_ALL_MYSQL_RES(_r) if ((_r) != nullptr) delete (_r); \
							(_r) = nullptr; \

//#define BEGIN_RESULT_READ(column, code) if (!r.empty()) { \
//								  MYSQL_ROW row = nullptr; \
//								  ULONGLONG linha = 0; \
//								  size_t num_result = 0; \
//								  while (!r.empty()/*num_result < r.size()*/) { \
//									if ((linha = mysql_num_rows(r.front())) > 0) { \
//										if (mysql_num_fields(r.front()) != (column)) { \
//											CLEAR_ALL_MYSQL_RES(r) \
//											throw exception("Numero de colunas solicitadas sao diferentes do recuperado do banco de dados. BEGIN_RESULT_READ().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, (code), 1)); /*0x40000 Erro ou Aviso pangya_login_db*/ \
//										} \

#define BEGIN_RESULT_READ(column, code) if (r != nullptr && r->getNumResultSet() > 0) { \
											for (auto num_result = 0u; num_result < r->getNumResultSet(); ++num_result) { \
												if (r->getResultSetAt(num_result) != nullptr && r->getResultSetAt(num_result)->getNumLines() > 0 && r->getResultSetAt(num_result)->getState() == result_set::HAVE_DATA) { \
													for (auto _result = r->getResultSetAt(num_result)->getFirstLine(); _result != nullptr; _result = _result->next) { \
														if ((column) != 0 && _result->cols != (column)) { \
															CLEAR_ALL_MYSQL_RES(r); \
															throw exception("Numero de colunas solicitadas sao diferentes do recuperado do banco de dados. BEGIN_RESULT_READ().", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, (code), 1)); \
														} \

//#define END_RESULT_READ(msg, code) }else { \
//							CLEAR_MYSQL_RES(r) \
//							/*throw exception("Nao conseguiu recuperar " + std::string((msg)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, (code), 2)); */\
//						} \
//						CLEAR_MYSQL_RES(r) \
//						/*if (linha > 0) mysql_data_seek(r.front(), 0);*/ \
//						++num_result; \
//					} \
//				}else if (query.getType() == exec_query::_QUERY) \
//					throw exception("Nao conseguiu recuperar " + std::string((msg)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, (code), 3)); \

#define END_RESULT_READ(msg, code) } \
								}else { \
									CLEAR_MYSQL_RES(r->getResultSetAt(num_result)); \
									/*throw exception("Nao conseguiu recuperar " + std::string((msg)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, (code), 2)); */\
								} \
								CLEAR_MYSQL_RES(r->getResultSetAt(num_result)); \
								/*if (linha > 0) mysql_data_seek(r.front(), 0);*/ \
							} \
						}else if (query.getType() == exec_query::_QUERY || query.getType() == exec_query::_PROCEDURE) \
							throw exception("Nao conseguiu recuperar " + std::string((msg)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, (code), 3)); \

//#define MED_RESULT_READ(_query) pangya_base_db::m_mysql_db_query_pool.push((_query)); \
//						DWORD wait_time = INFINITE; \
//						while (1) { \
//							try { \
//								(_query)->waitEvent(wait_time); \
//							}catch (exception& e) { \
//								if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::EXEC_QUERY) { \
//									/*nenhuma db thread disponivel para tratar a query*/ \
//									if (STDA_ERROR_DECODE(e.getCodeError()) == 7 && wait_time == INFINITE) { \
//										wait_time = 1000;/*1 segundo esperar se não for da próxima da erro*/ \
//										continue; \
//									} \
//									pangya_base_db::m_mysql_db_query_pool.remove((_query)); \
//									throw; \
//								}else \
//									throw; \
//							} \
//							\
//							break; \
//						} \
//						std::vector< MYSQL_RES* > r = (_query)->getRes(); \

#define MED_RESULT_READ(_query) pangya_base_db::m_query_pool.push((_query)); \
						DWORD wait_time = INFINITE; \
						while (1) { \
							try { \
								(_query)->waitEvent(wait_time); \
							}catch (exception& e) { \
								if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::EXEC_QUERY) { \
									/*nenhuma db thread disponivel para tratar a query*/ \
									if (STDA_ERROR_DECODE(e.getCodeError()) == 7 && wait_time == INFINITE) { \
										wait_time = 1000;/*1 segundo esperar se não for da próxima da erro*/ \
										continue; \
									} \
									pangya_base_db::m_query_pool.remove((_query)); \
									throw; \
								}else \
									throw; \
							} \
							\
							break; \
						} \
						response *r = (_query)->getRes(); \

// Inteiro
#define IFNULL(_func, _data) ((_data) == nullptr ? 0 : _func((_data)))

namespace stdA {

	class pangya_base_db {
		public:
			pangya_base_db();
			~pangya_base_db();

			static void register_server(ServerInfoEx& si);
			static std::vector< ServerInfo > getServerList();

		public:
			static list_fifo_asyc< exec_query > m_query_pool;
			static list_async< exec_query* > m_cache_query;

			static bool compare(exec_query* _query1, exec_query* _query2);
	};
}

#endif
