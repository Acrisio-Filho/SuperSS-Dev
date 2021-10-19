// Arquivo result_set.h
// Criado em 28/01/2018 as 15:09 por Acrisio
// Definição da classe result_set

#pragma once
#ifndef _STDA_RESULT_SET_H
#define _STDA_RESULT_SET_H

#include <cstdint>
#include <stddef.h>

namespace stdA {
    class result_set {
        public:
            enum STATE_TYPE : uint32_t {
                HAVE_DATA,
                _NO_DATA,
				_UPDATE_OR_DELETE,
                _ERROR,
            };

            struct ctx_res {
                char **data;
                size_t cols;
                ctx_res *next;
            };

        public:
            result_set(uint32_t _state);
            result_set(uint32_t _state, size_t _cols, int32_t _lines_affected);
            ~result_set();

            void destroy();

            size_t reserve_cols(size_t _cols);

			ctx_res* getFirstLine();

            void setLinesAffected(int32_t _lines_affected);

            char** getColAt(size_t _index);

            size_t getNumLines();
			size_t getNumCols();

            void setState(uint32_t _state);
            uint32_t getState();

			void addLine();

        protected:
            ctx_res *addLineData();

        protected:
            uint32_t m_state;
            int32_t m_lines_affected;
            
            size_t m_lines;
			size_t m_cols;
            ctx_res *m_data;
            ctx_res *m_curr_data;
    };
}

#endif