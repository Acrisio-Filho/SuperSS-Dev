// Arquivo cmd_update_pang.hpp
// Criado em 13/05/2018 as 10:51 por Acrisio
// Defini��o da classe CmdUpdatePang

#pragma once
#ifndef _STDA_CMD_UPDATE_PANG_HPP
#define _STDA_CMD_UPDATE_PANG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdatePang : public pangya_db {
		public:
			enum T_UPDATE_PANG : unsigned char {
				INCREASE,
				DECREASE,
			};

		public:
			explicit CmdUpdatePang(bool _waiter = false);
			CmdUpdatePang(uint32_t _uid, uint64_t _pang, T_UPDATE_PANG _type_update, bool _waiter = false);
			virtual ~CmdUpdatePang();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getPang();
			void setPang(uint64_t _pang);

			T_UPDATE_PANG getTypeUpdate();
			void setTypeUpdate(T_UPDATE_PANG _type_update);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdatePang"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdatePang"; };

		private:
			uint32_t m_uid;
			uint64_t m_pang;
			T_UPDATE_PANG m_type_update;

			const char* m_szConsulta[2] = { "UPDATE pangya.user_info SET pang = pang ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_UPDATE_PANG_HPP
