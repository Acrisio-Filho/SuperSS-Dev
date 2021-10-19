// Arquivo cmd_add_skin.hpp
// Criado em 20/05/2018 as 10:50 por Acrisio
// Defini��o da classe CmdAddSkin

#pragma once
#ifndef _STDA_CMD_ADD_SKIN_HPP
#define _STDA_CMD_ADD_SKIN_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddSkin : public pangya_db {
		public:
			explicit CmdAddSkin(bool _waiter = false);
			CmdAddSkin(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddSkin();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

			unsigned char getGiftFlag();
			void setGiftFlag(unsigned char _gift_flag);

			unsigned char getPurchase();
			void setPurchase(unsigned char _purchase);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddSkin"; };
			virtual std::wstring _wgetName() override { return L"CmdAddSkin"; };

		private:
			uint32_t m_uid;
			unsigned char m_purchase;
			unsigned char m_gift_flag;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcInsertSkin";
	};
}

#endif // !_STDA_CMD_ADD_SKIN_HPP
