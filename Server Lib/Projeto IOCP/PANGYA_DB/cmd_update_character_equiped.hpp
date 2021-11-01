// Arquivo cmd_update_character_equiped.hpp
// Criado em 25/03/2018 as 11:27 por Acrisio
// Defini��o da classe CmdUpdateCharacterEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_CHARACTER_EQUIPED_HPP
#define _STDA_CMD_UPDATE_CHARACTER_EQUIPED_HPP

#include "../PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateCharacterEquiped : public pangya_db {
		public:
			explicit CmdUpdateCharacterEquiped(bool _waiter = false);
			CmdUpdateCharacterEquiped(uint32_t _uid, int32_t _character_id, bool _waiter = false);
			virtual ~CmdUpdateCharacterEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getCharacterID();
			void setCharacterID(int32_t _character_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCharacterEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCharacterEquiped"; };

		private:
			uint32_t m_uid;
			int32_t m_character_id;

			const char* m_szConsulta = "pangya.USP_FLUSH_CHARACTER";
	};
}

#endif // !_STDA_CMD_UPDATE_CHARACTER_EQUIPED_HPP
