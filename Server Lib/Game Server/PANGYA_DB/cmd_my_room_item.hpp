// Arquivo cmd_my_room_item.hpp
// Criado em 22/03/2018 as 20:39 por Acrisio
// Defini��o da classe CmdMyRoomItem

#pragma once
#ifndef _STDA_CMD_MY_ROOM_ITEM_HPP
#define _STDA_CMD_MY_ROOM_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdMyRoomItem : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,
				ONE,
			};

		public:
			explicit CmdMyRoomItem(bool _waiter = false);
			CmdMyRoomItem(uint32_t _uid, TYPE _type, int32_t _item_id = -1, bool _waiter = false);
			virtual ~CmdMyRoomItem();

			std::vector< MyRoomItem >& getMyRoomItem();
			
			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getItemID();
			void setItemID(int32_t _item_id);

			TYPE getType();
			void setType(TYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMyRoomItem"; };
			virtual std::wstring _wgetName() override { return L"CmdMyRoomItem"; };

		private:
			uint32_t m_uid;
			int32_t m_item_id;
			TYPE m_type;
			std::vector< MyRoomItem > v_mri;

			const char* m_szConsulta[2] = { "pangya.ProcGetRoom", "pangya.ProcGetMyRoom_One" };
	};
}

#endif // !_STDA_CMD_MY_ROOM_ITEM_HPP
