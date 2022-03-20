// Arquivo response.h
// Criado em 28/01/2018 as 14:59 por Acrisio
// Definição da classe response

#pragma once
#ifndef _STDA_RESPONSE_H
#define _STDA_RESPONSE_H

#include "result_set.h"
#include <vector>

namespace stdA {
    class response {
        public:
            response();
            ~response();

            void clear();

            void addResultSet(result_set *_result_set);

            size_t getNumResultSet();

            result_set*& getResultSetAt(size_t _index);

			void setRowsAffected(int64_t _rows_affected);
			int64_t getRowsAffected();

        protected:
            std::vector< result_set* > m_result_set;      // result_set

			int64_t m_rows_affected;
    };
}

#endif