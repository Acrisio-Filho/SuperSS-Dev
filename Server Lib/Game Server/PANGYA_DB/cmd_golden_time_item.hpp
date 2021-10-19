// Arquivo cmd_golden_time_item.hpp
// Criado em 24/10/2020 as 02:44 por Acrisio
// Defini��o da classe CmdGoldenTimeItem

#pragma once
#ifndef _STDA_CMD_GOLDEN_TIME_ITEM_HPP
#define _STDA_CMD_GOLDEN_TIME_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/golden_time_type.hpp"

namespace stdA {

	class CmdGoldenTimeItem : public pangya_db {

		public:
			CmdGoldenTimeItem(uint32_t _id, bool _waiter = false);
			explicit CmdGoldenTimeItem(bool _waiter = false);
			virtual ~CmdGoldenTimeItem();

			uint32_t getId();
			void setId(uint32_t _id);

			std::vector< stItemReward >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdGoldenTimeItem"; };
			virtual std::wstring _wgetName() override { return L"CmdGoldenTimeItem"; };

		private:
			uint32_t m_id;

			std::vector< stItemReward > m_item;

			const char* m_szConsulta = "SELECT typeid, qntd, qntd_time, rate FROM pangya.pangya_golden_time_item WHERE golden_time_id = ";
	};
}

#endif // !_STDA_CMD_GOLDEN_TIME_ITEM_HPP
