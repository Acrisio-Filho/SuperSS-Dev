// Arquivo result_set.cpp
// Criado em 28/01/2018 as 15:30 por Acrisio
// Implementação da classe result_set

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "result_set.h"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

result_set::result_set(uint32_t _state) 
    : m_state(_state), m_lines_affected(-1), m_data(nullptr), m_curr_data(nullptr), m_lines(0), m_cols(0)
{};

result_set::result_set(uint32_t _state, size_t _cols, int32_t _lines_affected)
    : m_state(_state), m_lines_affected(_lines_affected), m_data(nullptr), m_curr_data(nullptr), m_lines(0), m_cols(_cols) {
};

result_set::~result_set() {
    destroy();
};

void result_set::destroy() {
    if (m_data != nullptr) {
        ctx_res* pNext = nullptr;
        size_t i = 0;

        while ((pNext = m_data)) {
            if (m_data->data != nullptr) {
                for (i = 0; i < m_data->cols; ++i) {
                    if (m_data->data[i] != nullptr)
                        delete[] m_data->data[i];
                }

                delete[] m_data->data;
            }

            m_data = pNext->next;
            
            delete pNext;
        }
    }

	m_data = nullptr;
};

size_t result_set::reserve_cols(size_t _cols) {
    if (_cols > 0) {
        //if (m_data == nullptr)
            addLineData();

        m_cols = m_curr_data->cols = _cols;
        
		if (m_curr_data->data == nullptr)
			m_curr_data->data = new char*[_cols] {0};
    }

	return _cols;
};

result_set::ctx_res* result_set::getFirstLine() {
	return m_data;
};

void result_set::setLinesAffected(int32_t _lines_affected) {
    m_lines_affected = _lines_affected;
};

char** result_set::getColAt(size_t _index) {
    if (m_curr_data == nullptr)
        throw exception("Nao tem nenhum dados reservado, reserve primeiro.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_RESULT_SET, 1, 0));

    if ((int)_index < 0 || _index >= m_curr_data->cols)
        throw exception("Index out of range.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_RESULT_SET, 2, 0));

    return &m_curr_data->data[_index];
};

size_t result_set::getNumLines() {
    return m_lines;
};

size_t result_set::getNumCols() {
	return m_cols;
};

void result_set::setState(uint32_t _state) {
    m_state = _state;
};

uint32_t result_set::getState() {
    return m_state;
};

void stdA::result_set::addLine() {
	if (m_state == HAVE_DATA)
		reserve_cols(m_cols);
};

result_set::ctx_res *result_set::addLineData() {
    
	if (m_data == nullptr) {
        m_data = new ctx_res;
        
        m_curr_data = m_data;

        // Init Dados
        m_curr_data->next = nullptr;
        m_curr_data->cols = 0;
        m_curr_data->data = nullptr;
    }else {
        m_curr_data->next = new ctx_res;

        m_curr_data = m_curr_data->next;

        // Init Dados
        m_curr_data->next = nullptr;
        m_curr_data->cols = 0;
        m_curr_data->data = nullptr;
    }

	++m_lines;

	return m_curr_data;
};
