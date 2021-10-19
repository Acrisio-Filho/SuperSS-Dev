// Arquivo cmd_add_clubset.hpp
// Criado em 19/05/2018 as 14:02 por Acrisio
// Defini��o da classe CmdAddClubSet

#pragma once
#ifndef _STDA_CMD_ADD_CLUBSET_HPP
#define STDA_CMD_ADD_CLUBSET_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddClubSet : public pangya_db {
		public:
			explicit CmdAddClubSet(bool _waiter = false);
			CmdAddClubSet(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddClubSet();

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
			virtual std::string _getName() override { return "CmdAddClubSet"; };
			virtual std::wstring _wgetName() override { return L"CmdAddClubSet"; };

		private:
			uint32_t m_uid;
			unsigned char m_purchase;
			unsigned char m_gift_flag;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcInsertClubSet";
	};
}

#endif // !_STDA_CMD_ADD_CLUBSET_HPP
