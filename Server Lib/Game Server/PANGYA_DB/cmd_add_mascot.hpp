// Arquivo cmd_add_mascot.hpp
// Criado em 19/05/2018 as 16:27 por Acrisio
// Defini��o da classe CmdAddMascot

#pragma once
#ifndef _STDA_CMD_ADD_MASCOT_HPP
#define _STDA_CMD_ADD_MASCOT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddMascot : public pangya_db {
		public:
			explicit CmdAddMascot(bool _waiter = false);
			CmdAddMascot(uint32_t _uid, MascotInfoEx& _mi, uint32_t _time, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddMascot();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			MascotInfoEx& getInfo();
			void setInfo(MascotInfoEx& _mi);

			uint32_t getTime();
			void setTime(uint32_t _time);

			unsigned char getGiftFlag();
			void setGiftFlag(unsigned char _gift_flag);

			unsigned char getPurchase();
			void setPurchase(unsigned char _purchase);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddMascot"; };
			virtual std::wstring _wgetName() override { return L"CmdAddMascot"; };

		private:
			uint32_t m_uid;
			unsigned char m_purchase;
			unsigned char m_gift_flag;
			uint32_t m_time;
			MascotInfoEx m_mi;

			const char* m_szConsulta = "pangya.ProcInsertMascot";
	};
}

#endif // !_STDA_CMD_ADD_MASCOT_HPP
