// Arquivo compress.h
// Criado em 24/09/2017 por Acrisio
// Definição da classe compress

#pragma once
#ifndef _STDA_COMPRESS_H
#define _STDA_COMPRESS_H

#include "minilzo.h"

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

namespace stdA {
    class compress {
        public:
            compress();
            ~compress();

            void compress_data(unsigned char *_uncompress, size_t _size_uncompress, unsigned char *_compress, size_t *_size_compress);

            void decompress_data(unsigned char *_compress,		/*compress buffer to decompress*/
								 size_t _size_compress,			/*size of compress buffer*/
								 unsigned char *_uncompress,	/*uncompress buffer to put decompress data*/
								 size_t *_size_uncompress,		/*size of decompress data*/
								 size_t _size_decompress		/*size of what equal at _size_uncompress*/);

            int getLastError();

        protected:
            HEAP_ALLOC(m_wrkmem, LZO1X_1_MEM_COMPRESS);
            int m_error;
    };
}

#endif