// Arquivo cmd_character_info.hpp
// Criado em 18/03/2018 as 20:45 por Acrisio
// Defini��o da classe CmdCharacterInfo

#pragma once
#ifndef _STDA_CMD_CHARACTER_INFO_HPP
#define _STDA_CMD_CHARACTER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>

#ifndef INVALID_ID
#define INVALID_ID -1l
#endif

namespace stdA {
	class CmdCharacterInfo : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,		// TODOS characters
				ONE,		// Um character
			};

		public:
			explicit CmdCharacterInfo(bool _waiter = false);
			CmdCharacterInfo(uint32_t _uid, TYPE _type, int32_t _char_id = INVALID_ID, bool _waiter = false);
			virtual ~CmdCharacterInfo();

			std::multimap< int32_t/*ID*/, CharacterInfo >& getAllInfo();
			CharacterInfo& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getCharID();
			void setCharID(int32_t _char_id);

			TYPE getType();
			void setType(TYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCharacterInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCharacterInfo"; };

		private:
			uint32_t m_uid;
			int32_t m_char_id;
			TYPE m_type;
			std::multimap< int32_t/*ID*/, CharacterInfo > v_ci;

			const char* m_szConsulta[2] = { "pangya.USP_CHAR_EQUIP_LOAD_S4", "pangya.USP_CHAR_EQUIP_LOAD_S4_ONE" };
	};
}

#endif // !_STDA_CMD_CHARACTER_INFO_HPP
