// Arquivo cmd_coin_cube_info.hpp
// Criado em 16/10/2020 as 18:48 por Acrisio
// Defini��o da classe CmdCoinCubeInfo

#pragma once
#ifndef _STDA_CMD_COIN_CUBE_INFO_HPP
#define _STDA_CMD_COIN_CUBE_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <map>

namespace stdA {

	class CmdCoinCubeInfo : public pangya_db {

		public:
			CmdCoinCubeInfo(bool _waiter = false);
			virtual ~CmdCoinCubeInfo();

			std::map< unsigned char/*course*/, bool/*active*/ >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdCoinCubeInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCoinCubeInfo"; };

		private:
			std::map< unsigned char/*course*/, bool/*active*/ > m_course_info;

			const char* m_szConsulta = "SELECT course_id, active FROM pangya.pangya_coin_cube_info";
	};
}

#endif // !_STDA_CMD_COIN_CUBE_INFO_HPP
