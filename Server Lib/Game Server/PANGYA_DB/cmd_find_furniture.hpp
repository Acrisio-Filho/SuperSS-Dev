// Arquivo cmd_find_furniture.hpp
// Criado em 26/05/2018 as 13:55 por Acrisio
// Defini��o da classe CmdFindFurniture

#pragma once
#ifndef _STDA_CMD_FIND_FURNITURE_HPP
#define _STDA_CMD_FIND_FURNITURE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFindFurniture : public pangya_db {
		public:
			explicit CmdFindFurniture(bool _waiter = false);
			CmdFindFurniture(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
			virtual ~CmdFindFurniture();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			bool hasFound();

			MyRoomItem& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdFindFurniture"; };
			virtual std::wstring _wgetName() override { return L"CmdFindFurniture"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			MyRoomItem m_mri;

			const char* m_szConsulta = "pangya.ProcFindFurniture";
	};
}

#endif // !_STDA_CMD_FIND_FURNITURE_HPP
