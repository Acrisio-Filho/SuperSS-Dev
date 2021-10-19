// Arquivo cmd_delete_furniture.hpp
// Criado em 16/06/2018 as 13:21 po Acrisio
// Defini��o da classe CmdDeleteFurniture

#pragma once
#ifndef _STDA_CMD_DELETE_FURNITURE_HPP
#define _STDA_CMD_DELETE_FURNITURE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdDeleteFurniture : public pangya_db {
		public:
			explicit CmdDeleteFurniture(bool _waiter = false);
			CmdDeleteFurniture(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdDeleteFurniture();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getID();
			void setID(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteFurniture"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteFurniture"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsula[2] = { "UPDATE pangya.td_room_data UPDATE SET valid = 0 WHERE UID = ", " AND MYROOM_ID = " };
	};
}

#endif // !_STDA_CMD_DELELTE_FURNITURE_HPP
