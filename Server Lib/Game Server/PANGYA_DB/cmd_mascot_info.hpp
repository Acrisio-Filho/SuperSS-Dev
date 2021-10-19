// Arquivo cmd_mascot_info.hpp
// Criado em 18/03/2018 as 21:44 por Acrisio
// Defini��o da classe CmdMascotInfo

#pragma once
#ifndef _STDA_CMD_MASCOT_INFO_HPP
#define _STDA_CMD_MASCOT_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>

namespace stdA {
	class CmdMascotInfo : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,
				ONE,
			};

		public:
			explicit CmdMascotInfo(bool _waiter = false);
			CmdMascotInfo(uint32_t _uid, TYPE _type, int32_t _mascot_id = -1, bool _waiter = false);
			virtual ~CmdMascotInfo();

			std::multimap< int32_t/*ID*/, MascotInfoEx >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

			int32_t getMascotID();
			void setMascotID(int32_t _mascot_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMascotInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdMascotInfo"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			int32_t m_mascot_id;
			std::multimap< int32_t/*ID*/, MascotInfoEx > v_mi;

			const char* m_szConsulta[2] = { "pangya.ProcGetMascotInfo", "pangya.ProcGetMascotInfo_One" };
	};
}

#endif // !_STDA_CMD_MASCOT_INFO_HPP
