// Arquivo cmd_get_gift_clubset.hpp
// Criado em 31/05/2018 as 19:39 por Acrisio
// Defini��o da classe CmdGetGiftClubSet

#pragma once
#ifndef _STDA_CMD_GET_GIFT_CLUBSET_HPP
#define _STDA_CMD_GET_GIFT_CLUBSET_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdGetGiftClubSet : public pangya_db {
		public:
			explicit CmdGetGiftClubSet(bool _waiter = false);
			CmdGetGiftClubSet(uint32_t _uid, WarehouseItemEx& _wi, bool _waiter = false);
			virtual ~CmdGetGiftClubSet();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdGetGiftClubSet"; };
			virtual std::wstring _wgetName() override { return L"CmdGetGiftClubSet"; };

		private:
			uint32_t m_uid;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcGetGiftClubSet";
	};
}

#endif // !_STDA_CMD_GET_GIFT_CLUBSET_HPP
