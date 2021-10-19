// Arquivo cmd_add_furniture.hpp
// Criado em 1206/2018 as 20:45 por Acrisio
// Defini��o da classe CmdAddFurniture

#pragma once
#ifndef _STDA_CMD_ADD_FURNITURE_HPP
#define _STDA_CMD_ADD_FURNITURE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddFurniture : public pangya_db {
		public:
			explicit CmdAddFurniture(bool _waiter = false);
			CmdAddFurniture(uint32_t _uid, MyRoomItem& _mri, bool _waiter = false);
			virtual ~CmdAddFurniture();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			MyRoomItem& getInfo();
			void setInfo(MyRoomItem& _mri);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddFurniture"; };
			virtual std::wstring _wgetName() override { return L"CmdAddFurniture"; };

		private:
			uint32_t m_uid;
			MyRoomItem m_mri;

			const char* m_szConsulta = "pangya.ProcAddFurniture";
	};
}

#endif // !_STDA_CMD_ADD_FURNITURE_HPP
