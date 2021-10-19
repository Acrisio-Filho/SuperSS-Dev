// Arquivo cmd_grand_zodiac_pontos.hpp
// Criado em 25/06/2020 as 15:54 por Acrisio
// Defini��o da classe CmdGrandZodiacPontos

#pragma once
#ifndef _STDA_CMD_GRAND_ZODIAC_PONTOS_HPP
#define _STDA_CMD_GRAND_ZODIAC_PONTOS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdGrandZodiacPontos : public pangya_db {
		public:
			enum eCMD_GRAND_ZODIAC_TYPE : unsigned char {
				CGZT_GET,
				CGZT_UPDATE
			};

		public:
			CmdGrandZodiacPontos(uint32_t _uid, eCMD_GRAND_ZODIAC_TYPE _type, bool _waiter = false);
			CmdGrandZodiacPontos(uint32_t _uid, uint64_t _pontos, eCMD_GRAND_ZODIAC_TYPE _type, bool _waiter = false);
			CmdGrandZodiacPontos(bool _waiter = false);
			virtual ~CmdGrandZodiacPontos();

			uint32_t getUID();
			uint64_t getPontos();
			eCMD_GRAND_ZODIAC_TYPE& getType();

			void setUID(uint32_t _uid);
			void setPontos(uint64_t _pontos);
			void setType(eCMD_GRAND_ZODIAC_TYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// Clas Name
			std::string _getName() override { return "CmdGrandZodiacPontos"; };
			std::wstring _wgetName() override { return L"CmdGrandZodiacPontos"; };

		private:
			uint32_t m_uid;
			uint64_t m_pontos;
			eCMD_GRAND_ZODIAC_TYPE m_type;

			const char* m_szConsulta[3] = { "SELECT pontos FROM pangya.pangya_grand_zodiac_pontos WHERE UID = ", "UPDATE pangya.pangya_grand_zodiac_pontos SET pontos = ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_GRAND_ZODIAC_PONTOS_HPP
