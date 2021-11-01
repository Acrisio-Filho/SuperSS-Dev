// Arquivo cmd_caddie_info.hpp
// Criado em 18/03/2018 as 21:24 por Acrisio
// Defini��o da classe CmdCaddieInfo

#pragma once
#ifndef _STDA_CMD_CADDIE_INFO_HPP
#define _STDA_CMD_CADDIE_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>

namespace stdA {
	class CmdCaddieInfo : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,
				ONE,
				FERIAS,
			};

		public:
			explicit CmdCaddieInfo(bool _waiter = false);
			CmdCaddieInfo(uint32_t _uid, TYPE _type, int32_t _caddie_id = -1, bool _waiter = false);
			virtual ~CmdCaddieInfo();

			std::multimap< int32_t/*ID*/, CaddieInfoEx >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

			int32_t getCaddieID();
			void setCaddieID(int32_t _caddie_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCaddieInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCaddieInfo"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			int32_t m_caddie_id;
			std::multimap< int32_t/*ID*/, CaddieInfoEx > v_ci;

			const char* m_szConsulta[3] = { "pangya.ProcGetCaddieInfo", "pangya.ProcGetCaddieInfo_One", "pangya.ProcGetCaddieFerias" };
	};
}

#endif // !_STDA_CMD_CADDIE_INFO_HPP
