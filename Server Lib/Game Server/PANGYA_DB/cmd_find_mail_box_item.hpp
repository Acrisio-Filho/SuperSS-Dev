// Arquivo cmd_find_mail_box_item.hpp
// Criado em 29/10/2019 ass 21:10 por Acrisio
// Defini��o da classe CmdFindMailBoxItem

#pragma once
#ifndef _STDA_CMD_FIND_MAIL_BOX_ITEM_HPP
#define _STDA_CMD_FIND_MAIL_BOX_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdFindMailBoxItem : public pangya_db {
		public:
			explicit CmdFindMailBoxItem(bool _waiter = false);
			CmdFindMailBoxItem(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
			virtual ~CmdFindMailBoxItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			bool hasFound();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdFindMailBoxItem"; };
			virtual std::wstring _wgetName() override { return L"CmdFindMailBoxItem"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			
			bool m_has_found;		// Encontrou o item
			
			const char* m_consulta = "pangya.ProcFindMailBoxItem";
	};
}

#endif // !_STDA_CMD_FIND_MAIL_BOX_ITEM_HPP
