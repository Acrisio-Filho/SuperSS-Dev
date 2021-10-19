// Arquivo cmd_card_info.hpp
// Criado em 21/03/2018 ass 22:11 por Acrisio
// Defini��o da classe CmdCardInfo

#pragma once
#ifndef _STDA_CMD_CARD_INFO_HPP
#define _STDA_CMD_CARD_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdCardInfo : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,
				ONE,
			};

		public:
			explicit CmdCardInfo(bool _waiter = false);
			CmdCardInfo(uint32_t _uid, TYPE _type, int32_t _card_id = -1, bool _waiter = false);
			virtual ~CmdCardInfo();

			std::vector< CardInfo >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

			int32_t getCardID();
			void setCardID(int32_t _card_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCardInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCardInfo"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			int32_t m_card_id;
			std::vector< CardInfo > v_ci;

			const char* m_szConsulta[2] = { "pangya.ProcGetCardInfo", "pangya.ProcGetCardInfo_One" };
	};
}

#endif // !_STDA_CMD_CARD_INFO_HPP
