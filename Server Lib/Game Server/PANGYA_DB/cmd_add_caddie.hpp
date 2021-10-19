// Arquivo cmd_add_caddie.hpp
// Criado em 25/03/2018 as 18:36 por Acrisio
// Defini��o da classe CmdAddCaddie

#pragma once
#ifndef _STDA_CMD_ADD_CADDIE_HPP
#define _STDA_CMD_ADD_CADDIE_HPP

#include "../../Projeto IOCP/PANGYA_DB/cmd_add_item_base.hpp"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddCaddie : public CmdAddItemBase {
		public:
			explicit CmdAddCaddie(bool _waiter = false);
			CmdAddCaddie(uint32_t _uid, CaddieInfoEx& _ci, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddCaddie();

			CaddieInfoEx& getInfo();
			void setInfo(CaddieInfoEx& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddCaddie"; };
			virtual std::wstring _wgetName() override { return L"CmdAddCaddie"; };

		private:
			CaddieInfoEx m_ci;

			const char* m_szConsulta = "pangya.ProcAddCaddie";
	};
}

#endif // !_STDA_CMD_ADD_CADDIE_HPP
