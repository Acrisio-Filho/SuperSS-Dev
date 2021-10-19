// Arquivo cmd_update_character_cutin_equiped.hpp
// Criado em 25/03/2018 as 12:22 por Acrisio
// Defini��o da classe CmdUpdateCharacterCutinEquiped

#pragma once
#ifndef _STDA_CMD_UPDATE_CHARACTER_CUTIN_EQUIPED_HPP
#define _STDA_CMD_UPDATE_CHARACTER_CUTIN_EQUIPED_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateCharacterCutinEquiped : public pangya_db {
		public:
			explicit CmdUpdateCharacterCutinEquiped(bool _waiter = false);
			CmdUpdateCharacterCutinEquiped(uint32_t _uid, CharacterInfo& _ci, bool _waiter = false);
			virtual ~CmdUpdateCharacterCutinEquiped();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			CharacterInfo& getInfo();
			void setInfo(CharacterInfo& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCharacterCutinEquiped"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCharacterCutinEquiped"; };

		private:
			uint32_t m_uid;
			CharacterInfo m_ci;

			const char* m_szConsulta = "pangya.USP_FLUSHCHARACTERCUTIN";
	};
}

#endif // !_STDA_CMD_UPDATE_CHARACTER_CUTIN_EQUIPED_HPP
