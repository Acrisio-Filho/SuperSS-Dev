// Arquivo cmd_update_caddie_item.hpp
// Criado em 14/05/2018 as 00:01 por Acrisio
// Defini��o da classe CmdUpdateCaddieItem

#pragma once
#ifndef _STDA_CMD_UPDATE_CADDIE_ITEM_HPP
#define _STDA_CMD_UPDATE_CADDIE_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateCaddieItem : public pangya_db {
		public:
			explicit CmdUpdateCaddieItem(bool _waiter = false);
			CmdUpdateCaddieItem(uint32_t _uid, std::string& _time, CaddieInfoEx& _ci, bool _waiter = false);
			virtual ~CmdUpdateCaddieItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::string& getTime();
			void setTime(std::string& _time);

			CaddieInfoEx& getInfo();
			void setInfo(CaddieInfoEx& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCaddieItem"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCaddieItem"; };

		private:
			uint32_t m_uid;
			std::string m_time;
			CaddieInfoEx m_ci;

			const char* m_szConsulta = "pangya.ProcUpdateCaddieItem";
	};
}

#endif // !_STDA_CMD_UPDATE_CADDIE_ITEM_HPP
