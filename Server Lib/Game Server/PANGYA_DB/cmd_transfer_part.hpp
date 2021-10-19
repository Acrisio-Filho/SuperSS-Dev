// Arquivo cmd_transfer_part.hpp
// Criado em 10/06/2018 as 11:37 por Acrisio
// Defini��o da classe CmdTransferPart

#pragma once
#ifndef _STDA_CMD_TRANSFER_PART_HPP
#define _STDA_CMD_TRANSFER_PART_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdTransferPart : public pangya_db {
		public:
			explicit CmdTransferPart(bool _waiter = false);
			CmdTransferPart(uint32_t _uid_sell, uint32_t _uid_buy, int32_t _item_id, unsigned char _type_iff, bool _waiter = false);
			virtual ~CmdTransferPart();

			uint32_t getUIDSell();
			void setUIDSell(uint32_t _uid_sell);

			uint32_t getUIDBuy();
			void setUIDBuy(uint32_t _uid_buy);

			int32_t getItemID();
			void setItemID(int32_t _item_id);

			unsigned char getTypeIFF();
			void setTypeIFF(unsigned char _type_iff);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdTransferPart"; };
			virtual std::wstring _wgetName() override { return L"CmdTransferPart"; };

		private:
			uint32_t m_uid_sell;
			uint32_t m_uid_buy;
			int32_t m_item_id;
			unsigned char m_type_iff;

			const char* m_szConsulta = "pangya.ProcTransferPart";
	};
}

#endif // !_STDA_CMD_TRANSFER_PART_HPP
