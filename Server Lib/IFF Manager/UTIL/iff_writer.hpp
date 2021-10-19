// Aquivo iff_writer.hpp
// Criado em 03/06/2019 as 19:44 por Acrisio
// Definição da classe iff_writer

#pragma once
#ifndef _STDA_IFF_WRITER_HPP
#define _STDA_IFF_WRITER_HPP

#include "../../Projeto IOCP/UTIL/iff.h"

namespace stdA {
	class iff_writer : public iff {
		public:
			iff_writer();
			~iff_writer();

			void saveAll();

			void saveBall();
			void saveCaddie();
			void saveCaddieItem();
			void saveCard();
			void saveCharacter();
			void saveClubSet();
			void saveHairStyle();
			void saveItem();
			void saveMascot();
			void savePart();
			void saveAuxPart();
			void saveSetItem();
			void saveSkin();
			void saveCadieMagicBox();
	};
}

#endif // !_STDA_IFF_WRITER_HPP
