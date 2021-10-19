// Arquivo compress.cpp
// Criado em 24/09/2017 por Acrisio
// implementação da classe compress

#include "compress.h"
#include "../UTIL/exception.h"
#include <cmath>
#include "../TYPE/stda_error.h"

using namespace stdA;

compress::compress() : m_error(0) {
    if ((m_error = lzo_init()) != LZO_E_OK)
        throw exception("Error ao initialize o lzo compress, isso nao deveria acontecer poder ser o compilador,\
                         compile sem otimizacao e com -DLZODEBUG para analizes. compress::compress()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 1, m_error));
};

compress::~compress() {};

void compress::compress_data(unsigned char *_uncompress, size_t _size_uncompress, unsigned char *_compress, size_t *_size_compress) {
    if (m_error < 0)
        throw exception("Error nao pode compress por que nao foi initializado corretamente, chame o lzo_init() antes. compress::compress_data()",
                        STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 2, m_error));

    if (_uncompress == nullptr)
        throw exception("Error _uncompress is nullptr. compress::compress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 3, 0));

    if (_compress == nullptr)
        throw exception("Error _compress is nullptr. compress:compress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 3, 0));

    if (_size_compress == nullptr)
        throw exception("Error _size_compress is nullptr. compress::compress::data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 3, 0));

    if (_size_uncompress <= 0)
        throw exception("Error _size_uncompress is less or equal at 0. compress::compress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 4, 0));

    try {
        if ((m_error = lzo1x_1_compress(_uncompress, _size_uncompress, _compress, (lzo_uint*)_size_compress, m_wrkmem)) != LZO_E_OK)
            throw exception("Error ao compress data. compress::compress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 5, m_error));
    }catch (std::exception& e) {
        throw exception(e.what(), STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 5, m_error));
    }
};

void compress::decompress_data(unsigned char *_compress, size_t _size_compress, unsigned char *_uncompress, size_t *_size_uncompress, size_t _size_decompress) {
    if (m_error < 0)
        throw exception("Error nao pode decompress por que nao foi initializado corretamente, chame o lzo_init() antes. compress::decompress_data()",
                        STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 2, m_error));

    if (_uncompress == nullptr)
        throw exception("Error _uncompress is nullptr. compress::decompress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 3, 0));

    if (_compress == nullptr)
        throw exception("Error _compress is nullptr. compress:decompress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 3, 0));

    if (_size_uncompress == nullptr)
        throw exception("Error _size_uncompress is nullptr. compress::decompress::data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 3, 0));

    if (_size_compress <= 0)
        throw exception("Error _size_compress is less or equal at 0. compress::decompress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 4, 0));

    try {
        if ((m_error = lzo1x_decompress(_compress, _size_compress, _uncompress, (lzo_uint*)_size_uncompress, nullptr)) != LZO_E_OK)
            throw exception("Error ao decompress data. compress::decompress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 6, m_error));
    }catch (std::exception& e) {
        throw exception(e.what(), STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 6, m_error));
    }

	if (*_size_uncompress != _size_decompress && abs((int)(*_size_uncompress - _size_decompress)) != 1)
		throw exception("Error size uncompress not match with size decompress.\n_size_uncompress: " + std::to_string(*_size_uncompress) + "\t_size_decompress: " + std::to_string(_size_decompress) + ". compress::decompress_data()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::COMPRESS, 7, 0));
};

int compress::getLastError() {
    return m_error;
};
