// Arquivo cmd_add_part.hpp
// Criado em 31/05/2018 as 11:09 por Acrisio
// Defini��o da classe CmdAddPart

#pragma once
#ifndef _STDA_CMD_ADD_PART_HPP
#define _STDA_CMD_ADD_PART_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddPart : public pangya_db {
		public:
			explicit CmdAddPart(bool _waiter = false);
			CmdAddPart(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, unsigned char _type_iff, bool _waiter = false);
			virtual ~CmdAddPart();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			unsigned char getPurchase();
			void setPurchase(unsigned char _purchase);

			unsigned char getGiftFlag();
			void setGiftFlag(unsigned char _gift_flag);

			unsigned char getTypeIFF();
			void setTypeIFF(unsigned char _type_iff);

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddPart"; };
			virtual std::wstring _wgetName() override { return L"CmdAddPart"; };

		private:
			uint32_t m_uid;
			unsigned char m_purchase;
			unsigned char m_gift_flag;
			unsigned char m_type_iff;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcAddPart";
	};
}

#endif // !_STDA_CMD_ADD_PART_HPP
