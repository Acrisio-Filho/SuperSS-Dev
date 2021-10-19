// Arquivo cmd_extend_rental.hpp
// Criado em 10/06/2018 as 16:50 por Acrisio
// Defini��o da classe CmdExtendRental

#pragma once
#ifndef _STDA_CMD_EXTEND_RENTAL_HPP
#define _STDA_CMD_EXTEND_RENTAL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdExtendRental : public pangya_db {
		public:
			explicit CmdExtendRental(bool _waiter = false);
			CmdExtendRental(uint32_t _uid, int32_t _item_id, std::string& _date, bool _waiter = false);
			virtual ~CmdExtendRental();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getItemID();
			void setItemID(int32_t _item_id);

			std::string& getDate();
			void setDate(std::string& _date);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdExtendRental"; };
			virtual std::wstring _wgetName() override { return L"CmdExtendRental"; };

		private:
			uint32_t m_uid;
			int32_t m_item_id;
			std::string m_date;

			const char* m_szConsulta = "pangya.ProcExtendRental";
	};
}

#endif // !_STDA_CMD_EXTEND_RENTAL_HPP
